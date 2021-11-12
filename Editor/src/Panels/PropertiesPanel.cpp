#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"
#include "ImGui/ImGuiFileEdit.h"

#include "Viewers/ViewerManager.h"

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
		else if(SceneManager::IsSceneLoaded())
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, SceneManager::CurrentScene()->GetSceneName().c_str(), sizeof(buffer));

			if (ImGui::InputText("Scene Name", buffer, sizeof(buffer)))
			{
				SceneManager::CurrentScene()->SetSceneName(buffer);
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
		std::strncpy(buffer, tag.c_str(), sizeof(buffer));

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
			if(ImGui::FileEdit("Material", sprite.material.GetFilepath(), FileType::MATERIAL))
				sprite.material.LoadMaterial();
			
			if (ImGui::DragFloat("Tiling Factor", tilingFactor, 0.1f, 0.0f, 100.0f))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}
		});

	//Static Mesh------------------------------------------------------------------------------------------------------------
	DrawComponent<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity, [](auto& staticMesh)
		{
			if (ImGui::FileEdit("Static Mesh", staticMesh.Geometry.GetFilepath(), FileType::MESH))
				staticMesh.Geometry.LoadModel();
			if (ImGui::FileEdit("Material", staticMesh.material.GetFilepath(), FileType::MATERIAL))
				staticMesh.material.LoadMaterial();
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

	//Rigid Body 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<RigidBody2DComponent>(ICON_FA_BASEBALL_BALL" Rigid Body 2D", entity, [](auto& rigidBody2D)
		{
			const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };

			const char* currentBodyTypeString = bodyTypeStrings[(int)rigidBody2D.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];

					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						rigidBody2D.Type = (RigidBody2DComponent::BodyType)i;
						SceneManager::CurrentScene()->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (rigidBody2D.Type == RigidBody2DComponent::BodyType::DYNAMIC)
			{
				ImGui::Checkbox("Fixed Rotation", &rigidBody2D.FixedRotation);
				ImGui::DragFloat("Gravity Scale", &rigidBody2D.GravityScale, 0.01f, -1.0f, 2.0f);
				ImGui::DragFloat("Angular Damping", &rigidBody2D.AngularDamping, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Linear Damping", &rigidBody2D.LinearDamping, 0.01f, 0.0f, 1.0f);
			}
		});

	//Box Collider 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<BoxCollider2DComponent>(ICON_FA_VECTOR_SQUARE" Box Collider 2D", entity, [](auto& boxCollider2D)
		{
			Vector2f& offset = boxCollider2D.Offset;
			ImGui::Text("Offset");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			float width = ImGui::GetContentRegionAvailWidth();

			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##offsetX", &offset.x, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				offset.x = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			ImGui::SameLine();
			ImGui::TextColored({ 0,245,0,255 }, "Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##offsetY", &offset.y, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				offset.y = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			Vector2f& size = boxCollider2D.Size;
			ImGui::Text("Size");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##sizeX", &size.x, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				size.x = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			ImGui::SameLine();
			ImGui::TextColored({ 0,245,0,255 }, "Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##sizeY", &size.y, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				size.y = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (ImGui::DragFloat("Density", &boxCollider2D.Density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &boxCollider2D.Friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &boxCollider2D.Restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();
		});

	//Circle Collider 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<CircleCollider2DComponent>(ICON_FA_CIRCLE_NOTCH" Circle Collider 2D", entity, [](auto& circleCollider2D)
		{
			Vector2f& offset = circleCollider2D.Offset;
			ImGui::Text("Offset");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			float width = ImGui::GetContentRegionAvailWidth();

			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##offsetX", &offset.x, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				offset.x = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			ImGui::SameLine();
			ImGui::TextColored({ 0,245,0,255 }, "Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width / 2 - 20);
			if (ImGui::DragFloat("##offsetY", &offset.y, 0.1f))
				SceneManager::CurrentScene()->MakeDirty();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				offset.y = 0.0f;
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (ImGui::DragFloat("Radius", &circleCollider2D.Radius, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Density", &circleCollider2D.Density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &circleCollider2D.Friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &circleCollider2D.Restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();
		});

	// Circle Renderer------------------------------------------------------------------------------------------------------------------
	DrawComponent<CircleRendererComponent>(ICON_FA_CIRCLE" Circle Renderer", entity, [](auto& circleRenderer)
		{
			float* colour[4] = { &circleRenderer.Colour.r, &circleRenderer.Colour.g, &circleRenderer.Colour.b, &circleRenderer.Colour.a };
			ImGui::ColorEdit4("Colour", colour[0]);
			ImGui::DragFloat("Thickness", &circleRenderer.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &circleRenderer.Fade, 0.00025f, 0.0f, 1.0f);
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
		AddComponentMenuItem<RigidBody2DComponent>("Rigid Body 2D", entity);
		AddComponentMenuItem<BoxCollider2DComponent>("Box Collider 2D", entity);
		AddComponentMenuItem<CircleCollider2DComponent>("Circle Collider 2D", entity);
		AddComponentMenuItem<CircleRendererComponent>("Circle Renderer", entity);

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
