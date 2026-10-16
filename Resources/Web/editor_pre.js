// Browser side of the web Editor: project folders, saving back to disk, persistent settings and downloads.
// Linked with --pre-js, so FS and IDBFS are in scope. Called from Editor/src/FileSystem/WebProject.cpp.

var AttercopWeb = {
	persistentDir: '/persistent',
	projectsDir: '/projects',

	// In-memory project root -> FileSystemDirectoryHandle (missing for read-only projects)
	handles: {},
	// In-memory project root -> { files: Map(relative path -> 'size:mtime'), dirs: Set(relative path) }
	records: {},

	// The project opened most recently
	currentRoot: '',
	// Files saved outside the project: in-memory path -> { handle, stamp }
	externalFiles: {},
	externalCount: 0,

	syncing: false,
	syncRequested: false,
	persisting: false,
	syncErrors: 0,

	// Copies a string into wasm memory for C++, which frees it
	toC(str) {
		const bytes = new TextEncoder().encode(str);
		const ptr = _malloc(bytes.length + 1);
		HEAPU8.set(bytes, ptr);
		HEAPU8[ptr + bytes.length] = 0;
		return ptr;
	},

	canWriteToDisk() {
		return typeof window.showDirectoryPicker === 'function';
	},

	// --- Loading overlay (from shell.html) ---

	showProgress(text, fraction) {
		const loading = document.getElementById('loading');
		if (!loading)
			return;
		loading.style.display = 'flex';
		document.getElementById('status').textContent = text;
		document.getElementById('progress').style.width = (100 * Math.max(0, Math.min(1, fraction))) + '%';
	},

	hideProgress() {
		const loading = document.getElementById('loading');
		if (loading)
			loading.style.display = 'none';
		if (Module.canvas)
			Module.canvas.focus();
	},

	// --- Paths ---

	join(...parts) {
		return parts.filter(p => p.length).join('/').replace(/\/+/g, '/');
	},

	isSkipped(name) {
		return name.startsWith('.');
	},

	clearRoot(root) {
		if (!FS.analyzePath(root).exists)
			return;
		const remove = (path) => {
			for (const name of FS.readdir(path)) {
				if (name === '.' || name === '..')
					continue;
				const child = this.join(path, name);
				if (FS.isDir(FS.stat(child).mode))
					remove(child);
				else
					FS.unlink(child);
			}
			FS.rmdir(path);
		};
		remove(root);
	},

	// --- Change records ---

	stamp(path) {
		const stat = FS.stat(path);
		return stat.size + ':' + stat.mtime.getTime();
	},

	walk(root) {
		const files = new Map();
		const dirs = new Set();
		const visit = (relative) => {
			const absolute = this.join(root, relative);
			for (const name of FS.readdir(absolute)) {
				if (name === '.' || name === '..' || this.isSkipped(name))
					continue;
				const childRelative = relative ? relative + '/' + name : name;
				const childAbsolute = this.join(root, childRelative);
				if (FS.isDir(FS.stat(childAbsolute).mode)) {
					dirs.add(childRelative);
					visit(childRelative);
				} else {
					files.set(childRelative, this.stamp(childAbsolute));
				}
			}
		};
		if (FS.analyzePath(root).exists)
			visit('');
		return { files, dirs };
	},

	findChanges(root) {
		const record = this.records[root];
		const current = this.walk(root);
		const written = [];
		const removedFiles = [];
		const removedDirs = [];
		for (const [path, stamp] of current.files)
			if (record.files.get(path) !== stamp)
				written.push(path);
		for (const path of record.files.keys())
			if (!current.files.has(path))
				removedFiles.push(path);
		for (const path of record.dirs)
			if (!current.dirs.has(path))
				removedDirs.push(path);
		const addedDirs = [...current.dirs].filter(path => !record.dirs.has(path));
		return { current, written, removedFiles, removedDirs, addedDirs };
	},

	hasUnsavedChanges() {
		if (this.syncing || this.syncRequested)
			return true;
		// Includes read-only projects, whose changes can only be kept by downloading them
		for (const root in this.records) {
			const changes = this.findChanges(root);
			if (changes.written.length || changes.removedFiles.length || changes.removedDirs.length || changes.addedDirs.length)
				return true;
		}
		return false;
	},

	// --- Copying a folder into the in-memory filesystem ---

	async copyDirectoryHandle(handle, root) {
		this.showProgress('Reading ' + handle.name + '…', 0);
		const entries = [];
		const list = async (dirHandle, relative) => {
			for await (const entry of dirHandle.values()) {
				if (this.isSkipped(entry.name))
					continue;
				const childRelative = relative ? relative + '/' + entry.name : entry.name;
				if (entry.kind === 'directory') {
					entries.push({ relative: childRelative, directory: true });
					await list(entry, childRelative);
				} else {
					entries.push({ relative: childRelative, handle: entry });
				}
			}
		};
		await list(handle, '');

		this.clearRoot(root);
		FS.mkdirTree(root);
		const files = entries.filter(e => !e.directory);
		let copied = 0;
		for (const entry of entries) {
			const path = this.join(root, entry.relative);
			if (entry.directory) {
				FS.mkdirTree(path);
				continue;
			}
			const file = await entry.handle.getFile();
			FS.writeFile(path, new Uint8Array(await file.arrayBuffer()));
			++copied;
			this.showProgress('Loading ' + handle.name + ' (' + copied + '/' + files.length + ')', copied / Math.max(1, files.length));
		}
		this.hideProgress();
	},

	async copyFileList(fileList, root) {
		this.clearRoot(root);
		FS.mkdirTree(root);
		let copied = 0;
		for (const file of fileList) {
			// webkitRelativePath starts with the chosen folder's own name
			const relative = file.webkitRelativePath.split('/').slice(1);
			if (relative.some(part => this.isSkipped(part)))
				continue;
			const path = this.join(root, relative.join('/'));
			FS.mkdirTree(path.substring(0, path.lastIndexOf('/')));
			FS.writeFile(path, new Uint8Array(await file.arrayBuffer()));
			++copied;
			this.showProgress('Loading (' + copied + '/' + fileList.length + ')', copied / Math.max(1, fileList.length));
		}
		this.hideProgress();
	},

	findProjectFile(root) {
		const projects = FS.readdir(root).filter(name => name.endsWith('.proj')).sort();
		if (projects.length > 1)
			console.warn('More than one .proj file in ' + root + ', opening ' + projects[0]);
		return projects.length ? this.join(root, projects[0]) : '';
	},

	async openHandle(handle) {
		const root = this.join(this.projectsDir, handle.name);
		// Save anything pending, then stop syncing this root while it's replaced
		if (this.handles[root])
			await this.flush();
		delete this.handles[root];
		delete this.records[root];
		await this.copyDirectoryHandle(handle, root);
		this.handles[root] = handle;
		this.records[root] = this.walk(root);
		this.currentRoot = root;
		await this.rememberProject(handle);
		return this.findProjectFile(root);
	},

	// Lets the user pick a project folder and copies it in; returns the .proj path, or '' if cancelled
	async openProjectFolder() {
		try {
			if (this.canWriteToDisk()) {
				const handle = await window.showDirectoryPicker({ id: 'attercop-project', mode: 'readwrite' });
				const projectFile = await this.openHandle(handle);
				if (!projectFile)
					console.error('No .proj file found in ' + handle.name);
				return projectFile;
			}
			return await this.openReadOnlyFolder();
		} catch (err) {
			this.hideProgress();
			if (err.name !== 'AbortError')
				console.error('Could not open project folder:', err);
			return '';
		}
	},

	openReadOnlyFolder() {
		return new Promise((resolve) => {
			const input = document.createElement('input');
			input.type = 'file';
			input.webkitdirectory = true;
			input.addEventListener('change', async () => {
				const files = [...input.files];
				if (!files.length)
					return resolve('');
				const name = files[0].webkitRelativePath.split('/')[0];
				const root = this.join(this.projectsDir, name);
				delete this.handles[root];
				delete this.records[root];
				await this.copyFileList(files, root);
				this.records[root] = this.walk(root);
				this.currentRoot = root;
				resolve(this.findProjectFile(root));
			});
			input.addEventListener('cancel', () => resolve(''));
			input.click();
		});
	},

	// Lets the user pick an empty folder for a new project; returns its in-memory root, or '' if cancelled
	async chooseNewProjectFolder() {
		try {
			const handle = await window.showDirectoryPicker({ id: 'attercop-project', mode: 'readwrite' });
			for await (const entry of handle.values()) {
				if (!this.isSkipped(entry.name)) {
					console.error(handle.name + ' is not empty, choose an empty folder for a new project');
					return '';
				}
			}
			const root = this.join(this.projectsDir, handle.name);
			if (this.handles[root])
				await this.flush();
			delete this.handles[root];
			this.clearRoot(root);
			FS.mkdirTree(root);
			this.handles[root] = handle;
			this.records[root] = { files: new Map(), dirs: new Set() };
			this.currentRoot = root;
			await this.rememberProject(handle);
			return root;
		} catch (err) {
			if (err.name !== 'AbortError')
				console.error('Could not choose a project folder:', err);
			return '';
		}
	},

	// --- Writing changes back to disk ---

	async directoryHandleFor(root, relativeDir, create) {
		let dir = this.handles[root];
		if (!relativeDir)
			return dir;
		for (const part of relativeDir.split('/'))
			dir = await dir.getDirectoryHandle(part, { create });
		return dir;
	},

	async syncRoot(root) {
		const changes = this.findChanges(root);
		const record = this.records[root];
		const split = (path) => {
			const slash = path.lastIndexOf('/');
			return slash < 0 ? ['', path] : [path.substring(0, slash), path.substring(slash + 1)];
		};

		for (const path of changes.addedDirs) {
			await this.directoryHandleFor(root, path, true);
			record.dirs.add(path);
		}
		for (const path of changes.written) {
			const absolute = this.join(root, path);
			// Read before awaiting, so the bytes match the stamp being recorded
			const stamp = this.stamp(absolute);
			const bytes = FS.readFile(absolute);
			const [dirPath, name] = split(path);
			const dir = await this.directoryHandleFor(root, dirPath, true);
			const fileHandle = await dir.getFileHandle(name, { create: true });
			const writable = await fileHandle.createWritable();
			await writable.write(bytes);
			await writable.close();
			record.files.set(path, stamp);
		}
		for (const path of changes.removedFiles) {
			const [dirPath, name] = split(path);
			try {
				const dir = await this.directoryHandleFor(root, dirPath, false);
				await dir.removeEntry(name);
			} catch (err) {
				if (err.name !== 'NotFoundError')
					throw err;
			}
			record.files.delete(path);
		}
		// Deepest first, so a parent is only removed after its children
		for (const path of changes.removedDirs.sort((a, b) => b.length - a.length)) {
			const [dirPath, name] = split(path);
			try {
				const dir = await this.directoryHandleFor(root, dirPath, false);
				await dir.removeEntry(name, { recursive: true });
			} catch (err) {
				if (err.name !== 'NotFoundError')
					throw err;
			}
			record.dirs.delete(path);
		}
		return changes.written.length + changes.removedFiles.length + changes.removedDirs.length;
	},

	async syncExternalFiles() {
		for (const path in this.externalFiles) {
			const file = this.externalFiles[path];
			if (!FS.analyzePath(path).exists)
				continue;
			const stamp = this.stamp(path);
			if (stamp === file.stamp)
				continue;
			const bytes = FS.readFile(path);
			const writable = await file.handle.createWritable();
			await writable.write(bytes);
			await writable.close();
			file.stamp = stamp;
			console.log('Saved ' + file.handle.name);
		}
	},

	// Starts writing changed project files back to disk; doesn't wait for it to finish
	sync() {
		if (this.syncing) {
			this.syncRequested = true;
			return;
		}
		const roots = Object.keys(this.handles);
		if (!roots.length && !Object.keys(this.externalFiles).length)
			return;
		this.syncing = true;
		this.syncRequested = false;
		(async () => {
			for (const root of roots) {
				try {
					const count = await this.syncRoot(root);
					if (count)
						console.log('Saved ' + count + ' change(s) to ' + this.handles[root].name);
					this.syncErrors = 0;
				} catch (err) {
					// Logged once per run of failures, not on every timer tick
					if (this.syncErrors++ === 0)
						console.error('Could not save changes to ' + this.handles[root].name + ':', err);
				}
			}
			try {
				await this.syncExternalFiles();
			} catch (err) {
				console.error('Could not save a file outside the project:', err);
			}
			this.syncing = false;
			if (this.syncRequested)
				this.sync();
		})();
	},

	// Saves now and waits until everything has been written
	async flush() {
		this.sync();
		while (this.syncing)
			await new Promise(resolve => setTimeout(resolve, 50));
	},

	isReadOnly(root) {
		return this.records[root] !== undefined && this.handles[root] === undefined;
	},

	// --- Settings, kept in IndexedDB through IDBFS ---

	mountPersistent() {
		FS.mkdirTree(this.persistentDir);
		FS.mount(IDBFS, {}, this.persistentDir);
		addRunDependency('attercop-persistent');
		FS.syncfs(true, (err) => {
			if (err)
				console.error('Could not load saved settings:', err);
			removeRunDependency('attercop-persistent');
		});
	},

	persist() {
		if (this.persisting)
			return;
		this.persisting = true;
		FS.syncfs(false, (err) => {
			if (err)
				console.error('Could not save settings:', err);
			this.persisting = false;
		});
	},

	// --- Recent projects: folder handles kept in IndexedDB ---

	openDatabase() {
		return new Promise((resolve, reject) => {
			const request = indexedDB.open('AttercopProjects', 1);
			request.onupgradeneeded = () => request.result.createObjectStore('handles');
			request.onsuccess = () => resolve(request.result);
			request.onerror = () => reject(request.error);
		});
	},

	async withStore(mode, action) {
		const db = await this.openDatabase();
		try {
			return await new Promise((resolve, reject) => {
				const transaction = db.transaction('handles', mode);
				const request = action(transaction.objectStore('handles'));
				transaction.oncomplete = () => resolve(request ? request.result : undefined);
				transaction.onerror = () => reject(transaction.error);
			});
		} finally {
			db.close();
		}
	},

	async rememberProject(handle) {
		try {
			await this.withStore('readwrite', store => store.put({ handle, opened: Date.now() }, handle.name));
		} catch (err) {
			console.warn('Could not remember project ' + handle.name + ':', err);
		}
	},

	async recentProjects() {
		if (!this.canWriteToDisk())
			return [];
		try {
			const entries = await this.withStore('readonly', store => store.getAll());
			const keys = await this.withStore('readonly', store => store.getAllKeys());
			return keys.map((name, i) => ({ name, opened: entries[i].opened }))
				.sort((a, b) => b.opened - a.opened)
				.map(entry => entry.name);
		} catch (err) {
			console.warn('Could not read recent projects:', err);
			return [];
		}
	},

	// Reopens a recent project; must run from a click, since it may ask for permission
	async openRecent(name) {
		try {
			const entry = await this.withStore('readonly', store => store.get(name));
			if (!entry)
				return '';
			const handle = entry.handle;
			if (await handle.queryPermission({ mode: 'readwrite' }) !== 'granted'
				&& await handle.requestPermission({ mode: 'readwrite' }) !== 'granted')
				return '';
			return await this.openHandle(handle);
		} catch (err) {
			this.hideProgress();
			if (err.name === 'NotFoundError') {
				console.error(name + ' no longer exists, removing it from recent projects');
				await this.withStore('readwrite', store => store.delete(name));
			} else if (err.name !== 'AbortError') {
				console.error('Could not open ' + name + ':', err);
			}
			return '';
		}
	},

	// --- Downloads and fetches ---

	download(path, name) {
		const bytes = FS.readFile(path);
		const url = URL.createObjectURL(new Blob([bytes]));
		const link = document.createElement('a');
		link.href = url;
		link.download = name;
		document.body.appendChild(link);
		link.click();
		link.remove();
		setTimeout(() => URL.revokeObjectURL(url), 10000);
	},

	async fetchToFile(url, path) {
		try {
			const response = await fetch(url);
			if (!response.ok)
				throw new Error(response.status + ' ' + response.statusText);
			FS.mkdirTree(path.substring(0, path.lastIndexOf('/')));
			FS.writeFile(path, new Uint8Array(await response.arrayBuffer()));
			return true;
		} catch (err) {
			console.error('Could not fetch ' + url + ':', err);
			return false;
		}
	},

	// Picks where to save a file; inside the open project that's its in-memory path, otherwise a temporary
	// path that's copied to the chosen file on sync. Returns '' if cancelled.
	async chooseSavePath(extension) {
		const suggestedName = extension ? 'Untitled' + extension : undefined;
		const root = this.currentRoot;
		try {
			if (typeof window.showSaveFilePicker !== 'function') {
				if (!root)
					return '';
				const relative = window.prompt('Save as (a path inside the project)', suggestedName || '');
				if (!relative)
					return '';
				const path = this.join(root, relative);
				FS.mkdirTree(path.substring(0, path.lastIndexOf('/')));
				return path;
			}

			const options = { suggestedName };
			if (this.handles[root])
				options.startIn = this.handles[root];
			const fileHandle = await window.showSaveFilePicker(options);

			if (this.handles[root]) {
				const parts = await this.handles[root].resolve(fileHandle);
				if (parts) {
					const path = this.join(root, parts.join('/'));
					FS.mkdirTree(path.substring(0, path.lastIndexOf('/')));
					return path;
				}
			}

			const dir = this.join('/tmp/saved', String(++this.externalCount));
			FS.mkdirTree(dir);
			const path = this.join(dir, fileHandle.name);
			this.externalFiles[path] = { handle: fileHandle, stamp: null };
			return path;
		} catch (err) {
			if (err.name !== 'AbortError')
				console.error('Could not choose where to save:', err);
			return '';
		}
	},
};

Module.preRun = Module.preRun || [];
Module.preRun.push(() => AttercopWeb.mountPersistent());

// Tab icon; added here rather than in shell.html, which exported games share
(() => {
	const icon = document.createElement('link');
	icon.rel = 'icon';
	icon.type = 'image/png';
	icon.href = 'favicon.png';
	document.head.appendChild(icon);
})();

window.addEventListener('beforeunload', (event) => {
	if (AttercopWeb.hasUnsavedChanges()) {
		event.preventDefault();
		event.returnValue = 'You have unsaved changes';
	}
});
