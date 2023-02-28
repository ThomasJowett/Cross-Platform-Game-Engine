#include "SpriteSheetView.h"

#include "IconsFontAwesome6.h"
#include "FileSystem/FileDialog.h"
#include "MainDockSpace.h"
#include "ImGui/ImGuiTextureEdit.h"

#include "Engine.h"

SpriteSheetView::SpriteSheetView(bool* show, std::filesystem::path filepath)
	:View("SpriteSheetView"), m_Show(show), m_Filepath(filepath)
{
}

void SpriteSheetView::OnAttach()
{
	m_WindowName = ICON_FA_PHOTO_FILM + std::string(" " + m_Filepath.filename().string());

	m_SpriteSheet = AssetManager::GetAsset<SpriteSheet>(m_Filepath);
	m_LocalSpriteSheet = CreateRef<SpriteSheet>(*m_SpriteSheet);
	m_LocalSpriteSheet->SetSubTexture(CreateRef<SubTexture2D>(*m_LocalSpriteSheet->GetSubTexture()));

	m_PreviewSprite.spriteSheet = m_LocalSpriteSheet;

	m_SelectedFrames.resize((size_t)(m_LocalSpriteSheet->GetSubTexture()->GetCellsTall()));
	for (size_t i = 0; i < m_LocalSpriteSheet->GetSubTexture()->GetCellsTall(); i++)
	{
		m_SelectedFrames[i].resize((size_t)(m_LocalSpriteSheet->GetSubTexture()->GetCellsWide()));
	}

	if (m_LocalSpriteSheet->GetSubTexture()->GetSpriteWidth() <= 32 || m_LocalSpriteSheet->GetSubTexture()->GetSpriteHeight() <= 32)
		m_Zoom = 2.0f;
	if (m_LocalSpriteSheet->GetSubTexture()->GetSpriteWidth() <= 16 || m_LocalSpriteSheet->GetSubTexture()->GetSpriteHeight() <= 16)
		m_Zoom = 4.0f;

	if (!m_LocalSpriteSheet->GetAnimations().empty()) {
		m_PreviewSprite.animation = m_LocalSpriteSheet->GetAnimations().begin()->first;
	}
	GetListOfAnimations();
}

void SpriteSheetView::OnUpdate(float deltaTime)
{
	m_PreviewSprite.Animate(deltaTime);
}

void SpriteSheetView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_Dirty)
		{
			ImGui::OpenPopup("Save Prompt");
		}

		if (ImGui::BeginPopupModal("Save Prompt"))
		{
			ImGui::TextUnformatted("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				m_Dirty = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*m_Show = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(680, 580), ImGuiCond_FirstUseEver);

	ImVec2 displaySize;
	if (m_LocalSpriteSheet->GetSubTexture()->GetTexture())
	{
		displaySize = ImVec2((float)m_LocalSpriteSheet->GetSubTexture()->GetTexture()->GetWidth() * m_Zoom,
			(float)m_LocalSpriteSheet->GetSubTexture()->GetTexture()->GetHeight() * m_Zoom);
	}
	else
	{
		displaySize = ImVec2(100.0f, 100.0f);
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save", "Ctrl + S"))
				{
					Save();
				}
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
				{
					SaveAs();
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable;

		if (ImGui::BeginTable("##TopLevel", 2, table_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Texture2DEdit("Texture", m_LocalSpriteSheet->GetSubTexture()->GetTexture()))
			{
				m_Dirty = true;
				m_LocalSpriteSheet->GetSubTexture()->RecalculateCellsDimensions();
			}

			int spriteSize[2] = { (int)m_LocalSpriteSheet->GetSubTexture()->GetSpriteWidth(), (int)m_LocalSpriteSheet->GetSubTexture()->GetSpriteHeight() };
			if (ImGui::InputInt2("Sprite Size", spriteSize))
			{
				m_LocalSpriteSheet->GetSubTexture()->SetSpriteDimensions(spriteSize[0], spriteSize[1]);
				m_Dirty = true;
			}

			int cellsWide = m_LocalSpriteSheet->GetSubTexture()->GetCellsWide();
			int cellsTall = m_LocalSpriteSheet->GetSubTexture()->GetCellsTall();

			if (ImGui::InputInt("Cells Wide", &cellsWide))
			{
				m_LocalSpriteSheet->GetSubTexture()->SetCellDimensions(cellsWide, cellsTall);
				m_Dirty = true;
			}
			if (ImGui::InputInt("Cells Tall", &cellsTall))
			{
				m_LocalSpriteSheet->GetSubTexture()->SetCellDimensions(cellsWide, cellsTall);
				m_Dirty = true;
			}

			ImGui::Separator();

			float ratio = ImGui::GetContentRegionAvail().x / spriteSize[0];

			m_LocalSpriteSheet->GetSubTexture()->SetCurrentCell(m_PreviewSprite.currentFrame);
			ImGui::Image(m_LocalSpriteSheet->GetSubTexture(),
				ImVec2(ImGui::GetContentRegionAvail().x, spriteSize[1] * ratio));


			if (ImGui::TreeNodeEx("Animations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Button(ICON_FA_PLUS"## Add animation"))
				{
					m_LocalSpriteSheet->AddAnimation("New Animation", 0, 3, 0.1f);
					GetListOfAnimations();
					m_PreviewSprite.animation = "New Animation";
					m_Dirty = true;
				}
				ImGui::Tooltip("Add animation");

				if (ImGui::BeginTable("Animations", 6, table_flags | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti))
				{
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 24.0f);
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, 0.0f, 0);
					ImGui::TableSetupColumn("Start Frame", ImGuiTableColumnFlags_None, 0.0f, 1);
					ImGui::TableSetupColumn("Frame Count", ImGuiTableColumnFlags_None, 0.0f, 2);
					ImGui::TableSetupColumn("Frame Time (s)", ImGuiTableColumnFlags_None, 0.0f, 3);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 24.0f);
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableHeadersRow();

					m_ActiveIndex = -1;

					std::string deletedAnimation;

					if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
					{
						if (sort_specs->SpecsDirty)
						{
							for (int n = 0; n < sort_specs->SpecsCount; n++)
							{
								const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[n];
								if (sort_spec->SortDirection == ImGuiSortDirection_Ascending)
								{

									switch (sort_spec->ColumnUserID)
									{
									case 0: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareName);				break;
									case 1: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareStartFrame); break;
									case 2: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareFrameCount); break;
									case 3: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareHoldTime);		break;
									default:
										break;
									}
								}
								else if (sort_spec->SortDirection == ImGuiSortDirection_Descending)
								{
									switch (sort_spec->ColumnUserID)
									{
									case 0: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareNameReverse);				break;
									case 1: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareStartFrameReverse);	break;
									case 2: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareFrameCountReverse);	break;
									case 3: std::sort(m_AnimationsSorted.begin(), m_AnimationsSorted.end(), CompareHoldTimeReverse);		break;
									default:
										break;
									}
								}
							}
							sort_specs->SpecsDirty = false;
						}
					}

					for (int i = 0; i < m_AnimationsSorted.size(); ++i)
					{
						if (m_AnimationsSorted[i].first == m_PreviewSprite.animation)
						{
							m_SelectedAnimation = i;
							break;
						}
					}

					int index = 0;
					for (auto&& [name, animation] : m_AnimationsSorted)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string radioBtnId = "##selected" + std::to_string(index);
						if (ImGui::RadioButton(radioBtnId.c_str(), &m_SelectedAnimation, index))
							m_PreviewSprite.animation = name;
						memset(m_InputBuffer, 0, sizeof(m_InputBuffer));
						for (int i = 0; i < name.length(); i++)
						{
							m_InputBuffer[i] = name[i];
						}
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						std::string nameStr = "##name" + std::to_string(index);

						ImGui::InputText(nameStr.c_str(), m_InputBuffer, sizeof(m_InputBuffer),
							ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

						if (ImGui::IsItemActive() && ImGui::IsWindowFocused())
						{
							m_ActiveIndex = index;
						}

						if (m_ActiveIndex == index && !ImGui::IsItemActive() && ImGui::IsWindowFocused())
						{
							if (m_PreviewSprite.animation == name)
								m_PreviewSprite.animation = m_InputBuffer;
							m_LocalSpriteSheet->RenameAnimation(name, m_InputBuffer);
							GetListOfAnimations();
							m_Dirty = true;
							m_ActiveIndex = -1;
						}

						int frameStart = (int)animation->GetStartFrame();
						int frameCount = (int)animation->GetFrameCount();
						float frameTime = animation->GetHoldTime();

						ImGui::TableSetColumnIndex(2);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						std::string starFrameStr = "##startFrame" + std::to_string(index);
						if (ImGui::DragInt(starFrameStr.c_str(), &frameStart, 1.0f, 0, m_LocalSpriteSheet->GetSubTexture()->GetNumberOfCells()))
						{
							if (frameStart < 0)
								frameStart = 0;
							m_LocalSpriteSheet->GetAnimations().at(name).SetStartFrame((uint32_t)frameStart);
							m_Dirty = true;
						}

						ImGui::TableSetColumnIndex(3);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						std::string frameCountStr = "##frameCount" + std::to_string(index);
						if (ImGui::DragInt(frameCountStr.c_str(), &frameCount, 1.0f, 1, m_LocalSpriteSheet->GetSubTexture()->GetNumberOfCells()))
						{
							if (frameCount < 0)
								frameCount = 0;
							m_LocalSpriteSheet->GetAnimations().at(name).SetFrameCount((uint32_t)frameCount);
							m_Dirty = true;
						}

						ImGui::TableSetColumnIndex(4);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						std::string frameTimeStr = "##frameTime" + std::to_string(index);
						if (ImGui::DragFloat(frameTimeStr.c_str(), &frameTime, 0.001f, 0.001f, 10.0f, "% .3f"))
						{
							m_LocalSpriteSheet->GetAnimations().at(name).SetHoldTime(frameTime);
							m_Dirty = true;
						}

						ImGui::TableSetColumnIndex(5);
						std::string deleteStr = ICON_FA_TRASH_CAN"##" + name + std::to_string(index);
						if (ImGui::Button(deleteStr.c_str()))
							deletedAnimation = name;
						ImGui::Tooltip("Delete Animation");

						index++;
					}
					ImGui::EndTable();

					if (!deletedAnimation.empty())
					{
						m_LocalSpriteSheet->RemoveAnimation(deletedAnimation);
						GetListOfAnimations();
						if (m_PreviewSprite.animation == deletedAnimation)
							m_PreviewSprite.animation = m_AnimationsSorted.empty() ? "" : m_AnimationsSorted[m_SelectedAnimation].first;
						m_Dirty = true;
					}
				}
				ImGui::TreePop();
			}



			ImGui::TableSetColumnIndex(1);

			ImGui::SliderFloat("Zoom", &m_Zoom, 0.25f, 4.0f, "%.2f");

			if (m_LocalSpriteSheet->GetSubTexture()->GetTexture())
			{
				ImVec2 tileSize((float)m_LocalSpriteSheet->GetSubTexture()->GetSpriteWidth() * m_Zoom,
					(float)m_LocalSpriteSheet->GetSubTexture()->GetSpriteHeight() * m_Zoom);
				ImGuiWindowFlags window_flags_image = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::BeginChild("Tileset Texture",
					ImVec2(ImGui::GetContentRegionAvail().x - 5.0f, std::min(displaySize.y + 5.0f, ImGui::GetContentRegionAvail().y - 5.0f)),
					false, window_flags_image);
				const ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::Image(m_LocalSpriteSheet->GetSubTexture()->GetTexture(), displaySize);
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				const ImU32 lineColour = ImColor(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
				const ImU32 selectionColour = ImColor(ImVec4(0.1f, 0.1f, 0.5f, 0.5f));
				const ImU32 selectionColourOutline = ImColor(ImVec4(0.2f, 0.2f, 0.7f, 0.7f));

				//Horizontal Lines
				for (uint32_t i = 0; i <= m_LocalSpriteSheet->GetSubTexture()->GetCellsTall(); i++)
				{
					float y = tileSize.y * (float)i;
					draw_list->AddLine(ImVec2(p.x, p.y + y), ImVec2(p.x + displaySize.x, p.y + y), lineColour);
				}
				//Vertical Lines
				for (uint32_t i = 0; i <= m_LocalSpriteSheet->GetSubTexture()->GetCellsWide(); i++)
				{
					float x = tileSize.x * (float)i;
					draw_list->AddLine(ImVec2(p.x + x, p.y), ImVec2(p.x + x, p.y + displaySize.y), lineColour);

					if (i != m_LocalSpriteSheet->GetSubTexture()->GetCellsWide()
						&& tileSize.y > 20.0f)
					{
						for (uint32_t j = 0; j < m_LocalSpriteSheet->GetSubTexture()->GetCellsTall(); j++)
						{
							float y = tileSize.y * (float)j;
							std::string number = std::to_string(i + (j * m_LocalSpriteSheet->GetSubTexture()->GetCellsWide()));
							draw_list->AddText(ImVec2(p.x + x + 2, p.y + y + 2), lineColour, number.c_str());
						}
					}
				}

				if (!m_PreviewSprite.animation.empty())
				{
					Animation* currentAnimation = m_LocalSpriteSheet->GetAnimation(m_PreviewSprite.animation);
					if (currentAnimation) {
						for (int i = currentAnimation->GetStartFrame(); i < (int)(currentAnimation->GetEndFrame()); ++i)
						{
							div_t div = std::div(i, (int)m_LocalSpriteSheet->GetSubTexture()->GetCellsWide());
							uint32_t cellX = div.rem;
							uint32_t cellY = div.quot;
							ImVec2 topLeft(cellX * tileSize.x + p.x, cellY * tileSize.y + p.y);
							ImVec2 bottomRight(cellX * tileSize.x + p.x + tileSize.x, cellY * tileSize.y + p.y + tileSize.y);
							draw_list->AddRectFilled(topLeft, bottomRight, selectionColour);
							draw_list->AddRect(topLeft, bottomRight, selectionColourOutline);
						}
					}
				}
				ImGui::EndChild();
			}
			else
			{
				ImGui::Dummy(displaySize);
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void SpriteSheetView::Save()
{
	m_LocalSpriteSheet->Save();
	m_SpriteSheet->Load(m_Filepath);
	m_Dirty = false;
}

void SpriteSheetView::SaveAs()
{
	auto ext = m_LocalSpriteSheet->GetFilepath().extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_LocalSpriteSheet->GetFilepath().extension().string()));
	if (dialogPath)
	{
		std::filesystem::path filepath = dialogPath.value();
		if (!filepath.has_extension())
			filepath.replace_extension(ext);
		m_LocalSpriteSheet->SaveAs(filepath);
		m_Dirty = false;
	}
}

bool SpriteSheetView::CompareName(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.first < second.first;
}
bool SpriteSheetView::CompareStartFrame(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetStartFrame() < second.second->GetStartFrame();
}
bool SpriteSheetView::CompareFrameCount(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetFrameCount() < second.second->GetFrameCount();
}
bool SpriteSheetView::CompareHoldTime(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetHoldTime() < second.second->GetHoldTime();
}

bool SpriteSheetView::CompareNameReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.first > second.first;
}
bool SpriteSheetView::CompareStartFrameReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetStartFrame() > second.second->GetStartFrame();
}
bool SpriteSheetView::CompareFrameCountReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetFrameCount() > second.second->GetFrameCount();
}
bool SpriteSheetView::CompareHoldTimeReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second)
{
	return first.second->GetHoldTime() > second.second->GetHoldTime();
}

void SpriteSheetView::GetListOfAnimations()
{
	m_AnimationsSorted.clear();
	for (auto&& [name, animation] : m_LocalSpriteSheet->GetAnimations())
	{
		m_AnimationsSorted.emplace_back(name, &animation);
	}
	while (m_SelectedAnimation >= m_AnimationsSorted.size() && m_SelectedAnimation > 0)
		m_SelectedAnimation--;
}
