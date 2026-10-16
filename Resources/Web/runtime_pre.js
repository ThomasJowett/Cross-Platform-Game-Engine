// Fetches an exported game's files from next to the page before main runs. Linked with --pre-js.
Module.preRun = Module.preRun || [];
Module.preRun.push(() => {
	FS.mkdirTree('/game');
	for (const name of ['game.meta', 'assets.pak']) {
		const dependency = 'game-' + name;
		addRunDependency(dependency);
		fetch(name)
			.then(response => {
				if (!response.ok)
					throw new Error(response.status + ' ' + response.statusText);
				return response.arrayBuffer();
			})
			.then(buffer => FS.writeFile('/game/' + name, new Uint8Array(buffer)))
			.catch(err => {
				console.error('Could not load ' + name + ':', err);
				if (typeof showError === 'function')
					showError('Could not load ' + name + ', see the browser console for details.');
			})
			.finally(() => removeRunDependency(dependency));
	}
});
