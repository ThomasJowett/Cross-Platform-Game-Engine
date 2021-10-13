#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"
#include "ImGui/ImGuiFileEdit.h"

#include "Viewers/ViewerManager.h"

#include <cstring>

PropertiesPanel::PropertiesPanel(bool* show, HierarchyPanel* hierarchyPanel)
	:Layer("Properties"), m_Show(show), m_HierarchyPanel(hierarchyPanel)
{
}

void PropertiesPanel::OnAttach()
{
}

void PropertiesPanel::OnFixedUpdate()
{
}

void PropertiesPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_TOOLS" Properties", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		Entity entity = m_HierarchyPanel->GetSelectedEntity();
		if (entity)
		{
			ImGui::BeginGroup();
			DrawComponents(entity);
			ImGui::Separator();
			DrawAddComponent(entity);
			ImVec2 available = ImGui::GetContentRegionAvail();
			ImGui::Dummy(available);
			ImGui::EndGroup();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
				{
					std::filesystem::path* file = (std::filesystem::path*)payload->Data;

					if (file->extension() == ".staticmesh" && !entity.HasComponent<StaticMeshComponent>())
					{
						//TODO: Store the material in the mesh file
						Mesh mesh(*file);

						Material material(Shader::Create("NormalMap"));

						material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory() / "resources" / "UVChecker.png"), 0);

						entity.AddComponent<StaticMeshComponent>(mesh, material);
					}

					CLIENT_DEBUG(file->string());
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
	ImGui::End();
}

void PropertiesPanel::DrawComponents(Entity entity)
{
	//Tag------------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, tag.c_str(), sizeof(buffer));

		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
			SceneManager::CurrentScene()->MakeDirty();
		}
	}

	//Transform------------------------------------------------------------------------------------------------------------
	DrawComponent<TransformComponent>("Transform", entity, [](auto& transform)
		{
			ImGui::Transform(transform.Position, transform.Rotation, transform.Scale);
		}, false);


	//Sprite--------------------------------------------------------------------------------------------------------------
	DrawComponent<SpriteComponent>(ICON_FA_IMAGE" Sprite", entity, [](auto& sprite)
		{
			float* tint[4] = { &sprite.Tint.r, &sprite.Tint.g, &sprite.Tint.b, &sprite.Tint.a };
			float* tilingFactor = &sprite.TilingFactor;

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}
			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
				{
					std::filesystem::path* file = (std::filesystem::path*)payload->Data;

					if (ViewerManager::GetFileType(*file) == FileType::IMAGE)
					{
						sprite.Texture = Texture2D::Create(*file);
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::DragFloat("Tiling Factor", tilingFactor, 0.1f, 0.0f, 100.0f))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}
		});

	//Static Mesh------------------------------------------------------------------------------------------------------------
	DrawComponent<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity, [](auto& staticMesh)
		{
			if (ImGui::FileEdit("Static Mesh", staticMesh.Geometry.GetFilepath(), L"Static Mesh (.staticMesh)\0*.staticMesh\0"))
				staticMesh.Geometry.LoadModel();
			//if (ImGui::FileEdit("Material", staticMesh.material.GetFilepath(), L"Material (.material)\0*.material\0"))
			//	staticMesh.material.LoadMaterial();
		});

	//Native Script------------------------------------------------------------------------------------------------------------
	DrawComponent<NativeScriptComponent>(ICON_FA_FILE_CODE" Native Script", entity, [](auto& script)
		{
			ImGui::Text("%s", script.Name.c_str());
		});

	//Camera------------------------------------------------------------------------------------------------------------
	DrawComponent<CameraComponent>(ICON_FA_VIDEO" Camera", entity, [](auto& cameraComp)
		{
			auto& camera = cameraComp.Camera;

			ImGui::Checkbox("Primary", &cameraComp.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };

			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjection((SceneCamera::ProjectionType)i);
						SceneManager::CurrentScene()->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}


			switch (camera.GetProjectionType())
			{
			case SceneCamera::ProjectionType::perspective:
			{
				float fov = (float)RadToDeg(camera.GetVerticalFov());
				if (ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 180.0f))
				{
					camera.SetVerticalFov((float)DegToRad(fov));
					SceneManager::CurrentScene()->MakeDirty();
				}

				float nearClip = camera.GetPerspectiveNear();
				if (ImGui::DragFloat("Near Clip##Perspective", &nearClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveNear(nearClip);
					SceneManager::CurrentScene()->MakeDirty();
				}

				float farClip = camera.GetPerspectiveFar();
				if (ImGui::DragFloat("Far Clip##Perspective", &farClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveFar(farClip);
					SceneManager::CurrentScene()->MakeDirty();
				}

				camera.RecalculateProjection();
				break;
			}
			case SceneCamera::ProjectionType::orthographic:
			{
				float size = camera.GetOrthoSize();
				if (ImGui::DragFloat("Size", &size, 0.1f, 0.0f, 100.0f))
				{
					camera.SetOrthoSize(size);
					SceneManager::CurrentScene()->MakeDirty();
				}

				float nearClip = camera.GetOrthoNear();
				if (ImGui::DragFloat("Near Clip##Orthographic", &nearClip))
				{
					camera.SetOrthoNear(nearClip);
					SceneManager::CurrentScene()->MakeDirty();
				}

				float farClip = camera.GetOrthoFar();
				if (ImGui::DragFloat("Far Clip##Orthographic", &farClip))
				{
					camera.SetOrthoFar(farClip);
					SceneManager::CurrentScene()->MakeDirty();
				}

				camera.RecalculateProjection();
				break;
			}
			}
		});

	//Primitive--------------------------------------------------------------------------------------------------------------
	DrawComponent<PrimitiveComponent>(ICON_FA_SHAPES" Primitive", entity, [](auto& primitive)
		{
			const char* shapeTypeStrings[] = { "Cube", "Sphere", "Plane", "Cylinder", "Cone", "Torus" };

			const char* currentShapeTypeString = shapeTypeStrings[(int)primitive.Type];

			if (ImGui::BeginCombo("Shape", currentShapeTypeString))
			{
				for (int i = 0; i < 6; i++)
				{
					bool isSelected = currentShapeTypeString == shapeTypeStrings[i];

					if (ImGui::Selectable(shapeTypeStrings[i], isSelected))
					{
						currentShapeTypeString = shapeTypeStrings[i];
						primitive.Type = (PrimitiveComponent::Shape)i;
						primitive.NeedsUpdating = true;
						SceneManager::CurrentScene()->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			switch (primitive.Type)
			{
				int tempInt;
			case PrimitiveComponent::Shape::Cube:
				if (ImGui::DragFloat("Width##cube", &primitive.CubeWidth, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##cube", &primitive.CubeHeight, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Depth##cube", &primitive.CubeDepth, 0.1f, 0.0f))
					primitive.NeedsUpdating = true;
				break;
			case PrimitiveComponent::Shape::Sphere:
				if (ImGui::DragFloat("Radius##Sphere", &primitive.SphereRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.SphereLongitudeLines;
				if (ImGui::DragInt("Longitude Lines##Sphere", &tempInt, 1.0f, 3, 600))
				{
					primitive.SphereLongitudeLines = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.SphereLatitudeLines;
				if (ImGui::DragInt("Latitude Lines##Sphere", &tempInt, 1.0f, 2, 600))
				{
					primitive.SphereLatitudeLines = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Plane:
				if (ImGui::DragFloat("Width##Plane", &primitive.PlaneWidth, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Length##Plane", &primitive.PlaneLength, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.PlaneWidthLines;
				if (ImGui::DragInt("Width Lines##Plane", &tempInt, 1.0f, 2, 1000))
				{
					primitive.PlaneWidthLines = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.PlaneLengthLines;
				if (ImGui::DragInt("Length Lines##Plane", &tempInt, 1.0f, 2, 1000))
				{
					primitive.PlaneLengthLines = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Tile U##Plane", &primitive.PlaneTileU, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Tile V##Plane", &primitive.PlaneTileV, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Cylinder:
				if (ImGui::DragFloat("Bottom Radius##Cylinder", &primitive.CylinderBottomRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Top Radius##Cylinder", &primitive.CylinderTopRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##Cylinder", &primitive.CylinderHeight, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.CylinderSliceCount;
				if (ImGui::DragInt("Slice Count##Cylinder", &tempInt, 1.0f, 3, 600))
				{
					primitive.CylinderSliceCount = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.CylinderStackCount;
				if (ImGui::DragInt("Stack Count##Cylinder", &tempInt, 1.0f, 1, 600))
				{
					primitive.CylinderStackCount = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Cone:
				if (ImGui::DragFloat("Bottom Radius##Cone", &primitive.ConeBottomRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##Cone", &primitive.ConeHeight, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.ConeSliceCount;
				if (ImGui::DragInt("Slice Count##Cone", &tempInt, 1.0f, 3, 600))
				{
					primitive.ConeSliceCount = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.ConeStackCount;
				if (ImGui::DragInt("Stack Count##Cone", &tempInt, 1.0f, 1, 600))
				{
					primitive.ConeStackCount = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Torus:
				if (ImGui::DragFloat("Outer Radius##Torus", &primitive.TorusOuterRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Inner Radius##Torus", &primitive.TorusInnerRadius, 0.1f, 0.0f))
				{
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.TorusSliceCount;
				if (ImGui::DragInt("Slice Count##Torus", &tempInt, 1.0f, 3, 600))
				{
					primitive.TorusSliceCount = tempInt;
					primitive.NeedsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			default:
				break;
			}
		});
}

void PropertiesPanel::DrawAddComponent(Entity entity)
{
	ImGui::Dummy({ 0,0 });
	float width = ImGui::GetContentRegionAvailWidth();
	ImGui::SameLine((width / 2.0f) - (ImGui::CalcTextSize("Add Component").x / 2.0f));
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Components");
	}

	if (ImGui::BeginPopup("Components"))
	{
		AddComponentMenuItem<CameraComponent>("Camera", entity);
		AddComponentMenuItem<SpriteComponent>("Sprite", entity);
		AddComponentMenuItem<StaticMeshComponent>("Static Mesh", entity);
		AddComponentMenuItem<PrimitiveComponent>("Primitive", entity);

		if (ImGui::BeginMenu("Native Script", !entity.HasComponent<NativeScriptComponent>() && Factory<ScriptableEntity>::GetMap()->size() > 0))
		{
			for (auto&& [key, value] : *Factory<ScriptableEntity>::GetMap())
			{
				if (ImGui::MenuItem(key.c_str()))
				{
					entity.AddComponent<NativeScriptComponent>().Bind(key);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
