#pragma once

#include "HierarchyPanel.h"
#include "ImGui/ImGuiTilemapEditor.h"
#include "Asset/PhysicsMaterial.h"
#include "History/HistoryCommands.h"
#include "Scene/Components.h"
#include "Interfaces/IUndoable.h"

#include "IconsFontAwesome6.h"

#include "imgui/imgui.h"

class PropertiesPanel :
	public Layer, public IUndoable
{
public:
	explicit PropertiesPanel(bool* show, Ref<HierarchyPanel> hierarchyPanel, Ref<TilemapEditor> tilemapEditor);
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
			Ref<AddComponentCommand<T>> addComponentCommand = CreateRef<AddComponentCommand<T>>(entity);
			entity.AddComponent<T>();
			HistoryManager::AddHistoryRecord(addComponentCommand);
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
						if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete"))
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

			if (deleteComponent) {
				Ref<RemoveComponentCommand<T>> removeComponentCommand = CreateRef<RemoveComponentCommand<T>>(entity);
				entity.RemoveComponent<T>();
				HistoryManager::AddHistoryRecord(removeComponentCommand);
			}
		}
	}

	// Inherited via IUndoable
	virtual void Undo(int asteps) override;
	virtual void Redo(int asteps) override;
	virtual bool CanUndo() const override;
	virtual bool CanRedo() const override;

private:
	bool* m_Show;

	Ref<HierarchyPanel> m_HierarchyPanel;
	Ref<TilemapEditor> m_TilemapEditor;

	Ref<Material> m_DefaultMaterial;
	Ref<PhysicsMaterial> m_DefaultPhysMaterial;

	Ref<EditComponentCommand<NameComponent>> m_EditNameComponent;
	std::pair<bool, Ref<EditComponentCommand<TransformComponent>>> m_EditTransformComponent;
	std::pair<bool, Ref<EditComponentCommand<WidgetComponent>>> m_EditWidgetComponent;
	std::pair<bool, Ref<EditComponentCommand<SpriteComponent>>> m_EditSpriteCommand;
	std::pair<bool, Ref<EditComponentCommand<AnimatedSpriteComponent>>> m_EditAnimatedSpriteCommand;
	std::pair<bool, Ref<EditComponentCommand<CircleRendererComponent>>> m_EditCircleRendererCommand;
	std::pair<bool, Ref<EditComponentCommand<TilemapComponent>>> m_EditTilemapCommand;
	std::pair<bool, Ref<EditComponentCommand<StaticMeshComponent>>> m_EditStaticMeshCommand;
	std::pair<bool, Ref<EditComponentCommand<CameraComponent>>> m_EditCameraCommand;
	std::pair<bool, Ref<EditComponentCommand<PrimitiveComponent>>> m_EditPrimitiveCommand;
	std::pair<bool, Ref<EditComponentCommand<TextComponent>>> m_EditTextCommand;
	std::pair<bool, Ref<EditComponentCommand<RigidBody2DComponent>>> m_EditRigidBody2DCommand;
	std::pair<bool, Ref<EditComponentCommand<BoxCollider2DComponent>>> m_EditBoxCollider2DCommand;
	std::pair<bool, Ref<EditComponentCommand<CircleCollider2DComponent>>> m_EditCircleCollider2DCommand;
	std::pair<bool, Ref<EditComponentCommand<PolygonCollider2DComponent>>> m_EditPolygonCollider2DCommand;
	std::pair<bool, Ref<EditComponentCommand<CapsuleCollider2DComponent> >> m_EditCapsuleCollider2DCommand;
	std::pair<bool, Ref<EditComponentCommand<BehaviourTreeComponent>>> m_EditBehaviourTreeCommand;
	std::pair<bool, Ref<EditComponentCommand<StateMachineComponent>>> m_EditStateMachineCommand;
	std::pair<bool, Ref<EditComponentCommand<BillboardComponent>>> m_EditBillboardCommand;
	std::pair<bool, Ref<EditComponentCommand<PointLightComponent>>> m_EditPontLightCommand;
};