#pragma once

#include "HierarchyPanel.h"
#include "ImGui/ImGuiTilemapEditor.h"

#include "Engine.h"

#include "IconsFontAwesome5.h"

class PropertiesPanel :
	public Layer
{
public:
	explicit PropertiesPanel(bool* show, HierarchyPanel* hierarchyPanel, Ref<TilemapEditor> tilemapEditor);
	~PropertiesPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
private:
	void DrawComponents(Entity entity);
	void DrawAddComponent(Entity entity);

	template<typename T>
	static void AddComponentMenuItem(const char* name, Entity entity)
	{
		if (ImGui::MenuItem(name, nullptr, nullptr, !entity.HasComponent<T>()))
		{
			entity.AddComponent<T>();
		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const char* name, Entity entity, UIFunction uifunction, bool deletable = true)
	{
		if (entity.HasComponent<T>())
		{
			bool deleteComponent = false;

			if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding, "%s", name))
			{
				if (deletable)
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete"))
						{
							deleteComponent = true;
						}

						ImGui::EndPopup();
					}
				}

				auto& component = entity.GetComponent<T>();

				uifunction(component);

				ImGui::TreePop();
			}

			if (deleteComponent)
				entity.RemoveComponent<T>();
		}
	}

private:
	bool* m_Show;

	HierarchyPanel* m_HierarchyPanel;
	Ref<TilemapEditor> m_TilemapEditor;
};