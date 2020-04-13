#include "ImGuiContentExplorer.h"

#include "ImGui/imgui.h"

#include <Windows.h>

#include "Renderer/Texture.h"
#include "FileSystem/Directory.h"

struct History
{
protected:
	std::vector<std::filesystem::path> info;
	int currentInfoIndex;
public:
	inline bool CanGoBack()
	{
		return currentInfoIndex > 0;
	}
	inline bool CanGoForward()
	{
		return currentInfoIndex >= 0 && currentInfoIndex < (int)info.size() - 1;
	}

	bool IsValid() const { return (currentInfoIndex >= 0 && currentInfoIndex < (int)info.size()); }
	const std::filesystem::path* GetCurrentFolder() const { return IsValid() ? &info[currentInfoIndex] : NULL; }


};

struct Internal
{
	std::filesystem::path currentPath;
	std::vector<std::string> currentSplitPath;

	Sorting sortingMode = Sorting::ALPHABETIC;
	History history;

	const char* editLocationInputText = "";

	bool editLocationCheckButtonPressed = false;
	bool forceRescan = true;
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
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}

				if (!historyCanGoForward)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dummyButtonColour);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, dummyButtonColour);
				}
				ImGui::SameLine();
				historyForwardClicked = ImGui::ImageButton(m_TextureLibrary.Get("resources/Icons/Arrow_Right.png"), { 16,16 }, 0, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);

				if (!historyCanGoBack) {
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}
			}
			ImGui::PopID();

			if (historyBackClicked || historyForwardClicked)
			{
				//if (historyBackClicked) {}
				//else if (historyForwardClicked) {}
				//
				//m_Internal->currentPath =  m_Internal->history.GetCurrentFolder();
				//m_Internal->editLocationInputText = m_Internal->currentPath.remove_filename().string();
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
						m_Internal->editLocationInputText = m_Internal->currentPath.remove_filename().string().c_str();
						ImGui::SetKeyboardFocusHere();
					}
				}

				ImGui::PopStyleColor();

				static char inputBuffer[1024] = "";

				if (m_Internal->editLocationCheckButtonPressed)
				{
					ImGui::SameLine();
					editlocationInputTextReturnPressed = ImGui::InputText("##EditLocationInputText", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
					if (editlocationInputTextReturnPressed)
					{
						mustValidateInputPath = true;
						m_Internal->editLocationInputText = inputBuffer;
					}
					else ImGui::Separator();
				}

				if (mustValidateInputPath)
				{
					if (std::filesystem::exists(m_Internal->editLocationInputText))
					{
						m_Internal->currentPath = m_Internal->editLocationInputText;
						m_Internal->currentSplitPath = SplitString(m_Internal->currentPath.string(), '\\');
						m_Internal->editLocationCheckButtonPressed = false;
						m_Internal->forceRescan = true;
					}
					else
					{
						memset(inputBuffer, 0, sizeof(inputBuffer));
						for (int i = 0; i < m_Internal->currentPath.string().length(); i++)
						{
							inputBuffer[i] = m_Internal->currentPath.string()[i];
						}
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

						}
						if (t == numTabs - 1) {
							ImGui::PopStyleColor();
							ImGui::PopStyleColor();
							ImGui::PopStyleColor();
						}
					}

					ImGui::Separator();
				}
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
