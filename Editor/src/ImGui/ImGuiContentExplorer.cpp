#include "ImGuiContentExplorer.h"

#include "ImGui/imgui.h"

#include <Windows.h>

#include "Renderer/Texture.h"
#include "FileSystem/Directory.h"

struct History
{
protected:
	std::vector<std::filesystem::path> paths;
	int currentPathIndex = 0;
public:
	inline bool CanGoBack()
	{
		return currentPathIndex > 0;
	}
	inline bool CanGoForward()
	{
		return currentPathIndex >= 0 && currentPathIndex < (int)paths.size() - 1;
	}

	bool GoBack()
	{
		if (CanGoBack())
		{
			--currentPathIndex;

			return true;
		}
		return false;
	}

	bool GoForward()
	{
		if (CanGoForward())
		{
			++currentPathIndex;
			return true;
		}
		return false;
	}

	bool IsValid() const { return (currentPathIndex >= 0 && currentPathIndex < (int)paths.size() && paths.size() > 0); }
	const std::filesystem::path* GetCurrentFolder() const { return IsValid() ? &paths[currentPathIndex] : new std::filesystem::path(); }

	bool SwitchTo(const std::filesystem::path fi)
	{
		if (fi.string().length() == 0)
			return false;
		if (currentPathIndex >= 0 && paths.size() > 0)
		{
			const std::filesystem::path lastPath = paths[currentPathIndex];
			if (lastPath == fi)
				return false;
		}
		paths.push_back(fi);
		currentPathIndex = paths.size() - 1;
		return true;
	}
};

struct Internal
{
	std::vector<std::filesystem::path> dirs, files;
	std::filesystem::path currentPath;
	std::vector<std::string> currentSplitPath;

	Sorting sortingMode = Sorting::ALPHABETIC;
	History history;


	bool editLocationCheckButtonPressed = false;
	bool forceRescan = true;

	int totalNumBrowsingEntries;
	int numBrowsingColumns;
	int numBrowsingEntriesPerColumn;
public:
	std::filesystem::path GetPathForSplitPathIndex(int index)
	{
		std::string path;
		for (int i = 0; i <= index; i++)
		{
			path += currentSplitPath[i];
			if (i != index)
				path += '\\';
		}

		return std::filesystem::path(path);
	}

	void CalculateBrowsingDataTableSizes(const ImVec2& childWindowSize = ImVec2(-1, -1))
	{
		int approxNumEntriesPerColumn = 20;
		if (childWindowSize.y > 0)
		{
			int numLinesThatFit = childWindowSize.y / ImGui::GetTextLineHeightWithSpacing();
			if (numLinesThatFit <= 0)
				numLinesThatFit = 1;
			approxNumEntriesPerColumn = numLinesThatFit;
		}
		numBrowsingColumns = totalNumBrowsingEntries / approxNumEntriesPerColumn;

		if (numBrowsingColumns <= 0)
		{
			numBrowsingColumns = 1;
			numBrowsingEntriesPerColumn = approxNumEntriesPerColumn;
			return;
		}

		if (totalNumBrowsingEntries % approxNumEntriesPerColumn > (approxNumEntriesPerColumn / 2))
			++numBrowsingColumns;

		int maxNumBrowsingColumns = (childWindowSize.x > 0) ? (childWindowSize.x / 100) : 6;

		if (maxNumBrowsingColumns < 1)
			maxNumBrowsingColumns = 1;

		if (numBrowsingColumns > maxNumBrowsingColumns)
			numBrowsingColumns = maxNumBrowsingColumns;

		numBrowsingEntriesPerColumn = totalNumBrowsingEntries / numBrowsingColumns;

		if (totalNumBrowsingEntries % numBrowsingColumns != 0)
			++numBrowsingEntriesPerColumn;
	}
};

ImGuiContentExplorer::ImGuiContentExplorer(bool* show)
	:m_Show(show), Layer("ContentExplorer")
{
	m_Internal = new Internal();
}

void ImGuiContentExplorer::OnAttach()
{
	m_TextureLibrary.Load("resources/Icons/Folder.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Right.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Left.png");
	m_TextureLibrary.Load("resources/Icons/Arrow_Up.png");
}

void ImGuiContentExplorer::OnImGuiRender()
{
	if (*m_Show)
	{
		static char inputBuffer[1024] = "";

		if (m_Internal->forceRescan)
		{
			m_Internal->forceRescan = false;

			if (m_Internal->currentPath == "")
			{
				m_Internal->currentPath = std::filesystem::absolute(".");
			}

			//set the input buffer as the current path
			memset(inputBuffer, 0, sizeof(inputBuffer));
			for (int i = 0; i < m_Internal->currentPath.string().length(); i++)
			{
				inputBuffer[i] = m_Internal->currentPath.string()[i];
			}

			m_Internal->dirs = Directory::GetDirectories(m_Internal->currentPath, m_Internal->sortingMode);
			m_Internal->files = Directory::GetFiles(m_Internal->currentPath, m_Internal->sortingMode);
		}
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Content Explorer", m_Show))
		{
			const ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 dummyButtonColour(0.0f, 0.0f, 0.0f, 0.5f);

			//----------------------------------------------------------------------------------------------------
			// History ('<', '>', '^')
			bool historyBackClicked = false;
			bool historyForwardClicked = false;
			bool levelUpClicked = false;



			ImGui::PushID("historyDirectoriesID");
			{

				const bool historyCanGoBack = m_Internal->history.CanGoBack();
				const bool historyCanGoForward = m_Internal->history.CanGoForward();

				if (!historyCanGoBack)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
				}
				historyBackClicked = ImGui::ImageButton(m_TextureLibrary.Get("resources/Icons/Arrow_Left.png"), { 16,16 }, 0, ImGui::GetStyle().Colors[2]);

				if (!historyCanGoBack) {
					ImGui::PopStyleColor(3);
				}

				if (!historyCanGoForward)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
				}
				ImGui::SameLine();
				historyForwardClicked = ImGui::ImageButton(m_TextureLibrary.Get("resources/Icons/Arrow_Right.png"), { 16,16 }, 0, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);

				if (!historyCanGoForward) {
					ImGui::PopStyleColor(3);
				}
			}
			ImGui::PopID();

			if (historyBackClicked || historyForwardClicked)
			{
				if (historyBackClicked)
				{
					if (m_Internal->history.GoBack())
						m_Internal->forceRescan = true;
				}
				else if (historyForwardClicked)
				{
					if (m_Internal->history.GoForward())
						m_Internal->forceRescan = true;
				}

				m_Internal->currentPath = *m_Internal->history.GetCurrentFolder();
			}
			//----------------------------------------------------------------------------------------------------
			// Manual Location text entry
			const std::filesystem::path* fi = m_Internal->history.GetCurrentFolder();

			// Edit Location CheckButton
			bool editlocationInputTextReturnPressed = false;
			{
				bool mustValidateInputPath = false;
				ImGui::PushStyleColor(ImGuiCol_Button, m_Internal->editLocationCheckButtonPressed ? dummyButtonColour : style.Colors[ImGuiCol_Button]);
				ImGui::SameLine();
				if (ImGui::ImageButton(m_TextureLibrary.Get("resources/Icons/Folder.png"), { 16,16 }, 0, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]))
				{
					m_Internal->editLocationCheckButtonPressed = !m_Internal->editLocationCheckButtonPressed;
					if (m_Internal->editLocationCheckButtonPressed)
					{
						ImGui::SetKeyboardFocusHere();
					}
				}

				ImGui::PopStyleColor();


				if (m_Internal->editLocationCheckButtonPressed)
				{
					ImGui::SameLine();
					editlocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::SetKeyboardFocusHere();
					if (editlocationInputTextReturnPressed)
					{
						mustValidateInputPath = true;
					}
					else ImGui::Separator();
				}

				if (mustValidateInputPath)
				{
					try
					{
						if (std::filesystem::exists(inputBuffer))
						{
							m_Internal->currentPath = inputBuffer;

							if (std::filesystem::is_regular_file(inputBuffer))
							{
								m_Internal->currentPath._Remove_filename_and_separator();
							}
							m_Internal->currentSplitPath = SplitString(m_Internal->currentPath.string(), '\\');
							m_Internal->editLocationCheckButtonPressed = false;
							m_Internal->forceRescan = true;
							m_Internal->history.SwitchTo(m_Internal->currentPath);
						}
					}
					catch (const std::exception& e)
					{
						CLIENT_ERROR(e.what());
					}
				}
			}

			//----------------------------------------------------------------------------------------------------
			// Split path control
			if (!m_Internal->editLocationCheckButtonPressed && !editlocationInputTextReturnPressed)
			{
				bool mustSwitchSplitPath = false;

				//Split Path
				// tab:
				{
					const int numTabs = (int)m_Internal->currentSplitPath.size();

					int pushpop = 0;

					for (int t = 0; t < numTabs; t++)
					{
						if (t == numTabs - 1)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
						}

						ImGui::PushID(t);
						ImGui::SameLine();

						const bool pressed = ImGui::Button(m_Internal->currentSplitPath[t].c_str());
						if (t != numTabs - 1)
						{
							ImGui::SameLine();
							ImGui::Text(">");
						}
						ImGui::PopID();

						if (pressed)
						{
							if (t == numTabs - 1)
							{
								m_Internal->editLocationCheckButtonPressed = true;
								ImGui::PopStyleColor(3);
								continue;
							}

							m_Internal->history.SwitchTo(m_Internal->GetPathForSplitPathIndex(t));
							m_Internal->forceRescan = true;

							m_Internal->currentPath = *m_Internal->history.GetCurrentFolder();
						}
						if (t == numTabs - 1) {
							ImGui::PopStyleColor(3);
						}
					}

					m_Internal->currentSplitPath = SplitString(m_Internal->currentPath.string(), '\\');

					ImGui::Separator();
				}
			}

			//----------------------------------------------------------------------------------------------------
			// MAIN BROWSING WINDOW
			//----------------------------------------------------------------------------------------------------
			{
				if (ImGui::BeginChild("BrowsingFrame"))
				{
					m_Internal->CalculateBrowsingDataTableSizes(ImGui::GetWindowSize());

					ImGui::Columns(m_Internal->numBrowsingColumns);

					static int id;
					ImGui::PushID(&id);
					int cntEntries = 0;
					//Directories -------------------------------------------------------------------------------
					if (m_Internal->dirs.size() > 0)
					{
						for (int i = 0; i < m_Internal->dirs.size(); i++)
						{
							std::string dirName = SplitString(m_Internal->dirs[i].string(), '\\').back();
							if (ImGui::SmallButton(dirName.c_str()))
							{
								//change dirctory on click
								m_Internal->history.SwitchTo(m_Internal->dirs[i]);
								m_Internal->currentPath = *m_Internal->history.GetCurrentFolder();
								m_Internal->forceRescan = true;
							}
							++cntEntries;
							//TODO:: switch on view zoom
							if (cntEntries == m_Internal->numBrowsingEntriesPerColumn)
							{
								cntEntries = 0;
								ImGui::NextColumn();
							}
						}
					}

					//Files -----------------------------------------------------------------------------------
					if (m_Internal->files.size() > 0)
					{
						for (int i = 0; i < m_Internal->files.size(); i++)
						{
							if (ImGui::SmallButton(m_Internal->files[i].filename().string().c_str()))
							{
								//open file on click

							}

						}
					}

					ImGui::PopID();
				}
				ImGui::EndChild();
			}
		}
		ImGui::End();
	}
}

std::string ImGuiContentExplorer::FileDialog(const wchar_t* title, const wchar_t* filter)
{
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Text Files\0*.txt\0Any File\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = L"Select a file path";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:		ENGINE_ERROR("CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE:		ENGINE_ERROR("CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION:		ENGINE_ERROR("CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE:		ENGINE_ERROR("CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE:		ENGINE_ERROR("CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE:		ENGINE_ERROR("CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE:		ENGINE_ERROR("CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE:		ENGINE_ERROR("CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE:			ENGINE_ERROR("CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK:				ENGINE_ERROR("CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE:			ENGINE_ERROR("CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE:			ENGINE_ERROR("CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL:		ENGINE_ERROR("FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME:		ENGINE_ERROR("FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE:		ENGINE_ERROR("FNERR_SUBCLASSFAILURE"); break;
		default: ENGINE_INFO("File open cancelled.");
		}
	}

	std::wstring ws(filename);
	return std::string(ws.begin(), ws.end());
}