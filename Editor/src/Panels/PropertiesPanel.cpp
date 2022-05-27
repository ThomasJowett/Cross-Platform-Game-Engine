#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "Fonts/IconsMaterialDesignIcons.h"
#include "Fonts/Fonts.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"
#include "ImGui/ImGuiFileEdit.h"
#include "ImGui/ImGuiTextureEdit.h"
#include "ImGui/ImGuiMaterialEdit.h"

#include "Viewers/ViewerManager.h"
#include "FileSystem/Directory.h"

PropertiesPanel::PropertiesPanel(bool* show, HierarchyPanel* hierarchyPanel, Ref<TilemapEditor> tilemapEditor)
	:Layer("Properties"), m_Show(show), m_HierarchyPanel(hierarchyPanel), m_TilemapEditor(tilemapEditor)
{
}

void PropertiesPanel::OnAttach()
{
}

void PropertiesPanel::OnUpdate(float deltaTime)
{
	Entity entity = m_HierarchyPanel->GetSelectedEntity();
	if (entity && entity.HasComponent<PrimitiveComponent>())
	{
		StaticMeshComponent& staticMeshComp = m_HierarchyPanel->GetSelectedEntity().GetOrAddComponent<StaticMeshComponent>();
		PrimitiveComponent& primitiveComp = m_HierarchyPanel->GetSelectedEntity().GetComponent<PrimitiveComponent>();

		if (primitiveComp.needsUpdating)
		{
			switch (primitiveComp.type)
			{
			case PrimitiveComponent::Shape::Cube:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateCube(primitiveComp.cubeWidth, primitiveComp.cubeHeight, primitiveComp.cubeDepth), "Cube");
				break;
			case PrimitiveComponent::Shape::Sphere:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateSphere(primitiveComp.sphereRadius, primitiveComp.sphereLongitudeLines, primitiveComp.sphereLatitudeLines), "Sphere");
				break;
			case PrimitiveComponent::Shape::Plane:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateGrid(primitiveComp.planeWidth, primitiveComp.planeLength, primitiveComp.planeLengthLines, primitiveComp.planeWidthLines, primitiveComp.planeTileU, primitiveComp.planeTileV), "Plane");
				break;
			case PrimitiveComponent::Shape::Cylinder:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateCylinder(primitiveComp.cylinderBottomRadius, primitiveComp.cylinderTopRadius, primitiveComp.cylinderHeight, primitiveComp.cylinderSliceCount, primitiveComp.cylinderStackCount), "Cylinder");
				break;
			case PrimitiveComponent::Shape::Cone:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateCylinder(primitiveComp.coneBottomRadius, 0.00001f, primitiveComp.coneHeight, primitiveComp.coneSliceCount, primitiveComp.coneStackCount), "Cone");
				break;
			case PrimitiveComponent::Shape::Torus:
				staticMeshComp.mesh.LoadModel(GeometryGenerator::CreateTorus(primitiveComp.torusOuterRadius, primitiveComp.torusInnerRadius, primitiveComp.torusSliceCount), "Torus");
				break;
			default:
				break;
			}
			primitiveComp.needsUpdating = false;
		}
	}
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

					if (ViewerManager::GetFileType(*file) == FileType::MESH && !entity.HasComponent<StaticMeshComponent>())
					{
						//TODO: Store the material in the mesh file
						Mesh mesh(*file);

						Material material("Standard", Colours::WHITE);

						material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory() / "resources" / "UVChecker.png"), 0);

						entity.AddComponent<StaticMeshComponent>(mesh, material);
					}
					else if (file->extension() == ".lua" && !entity.HasComponent<LuaScriptComponent>())
					{
						entity.AddComponent<LuaScriptComponent>(*file);
					}

					CLIENT_DEBUG(file->string());
				}
				ImGui::EndDragDropTarget();
			}
		}
		else if (SceneManager::IsSceneLoaded())
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
	//Name------------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<NameComponent>())
	{
		auto& name = entity.GetComponent<NameComponent>().name;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::strncpy(buffer, name.c_str(), sizeof(buffer));

		if (ImGui::InputText("Entity Name##", buffer, sizeof(buffer)))
		{
			name = std::string(buffer);
			SceneManager::CurrentScene()->MakeDirty();
		}
	}

	//Transform------------------------------------------------------------------------------------------------------------
	DrawComponent<TransformComponent>(ICON_MDI_AXIS_ARROW" Transform", entity, [&](auto& transform)
		{
			if (ImGui::Transform(transform.position, transform.rotation, transform.scale))
			{
				if (entity.HasComponent<RigidBody2DComponent>())
				{
					entity.GetComponent<RigidBody2DComponent>().SetTransform(transform.position, transform.rotation.z);
				}
			}
		}, false);

	//Sprite--------------------------------------------------------------------------------------------------------------
	DrawComponent<SpriteComponent>(ICON_FA_IMAGE" Sprite", entity, [&](auto& sprite)
		{
			float* tint[4] = { &sprite.tint.r, &sprite.tint.g, &sprite.tint.b, &sprite.tint.a };
			float* tilingFactor = &sprite.tilingFactor;

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (ImGui::Button("Pixel Perfect"))
			{
				if (sprite.texture)
				{
					entity.GetTransform().scale.x = sprite.texture->GetWidth();
					entity.GetTransform().scale.y = sprite.texture->GetHeight();
				}
			}
			ImGui::Tooltip("Set Scale to pixel perfect scaling");

			if (ImGui::Texture2DEdit("Texture", sprite.texture))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (ImGui::DragFloat("Tiling Factor", tilingFactor, 0.1f, 0.0f, 100.0f))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}
		});

	//Animated Sprite--------------------------------------------------------------------------------------------------------------
	DrawComponent<AnimatedSpriteComponent>(ICON_FA_IMAGE" Animated Sprite", entity, [&](auto& sprite)
		{
			float* tint[4] = { &sprite.tint.r, &sprite.tint.g, &sprite.tint.b, &sprite.tint.a };

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (ImGui::Button("Pixel Perfect"))
			{
				if (sprite.tileset)
				{
					entity.GetTransform().scale.x = sprite.tileset->GetSubTexture()->GetSpriteWidth();
					entity.GetTransform().scale.y = sprite.tileset->GetSubTexture()->GetSpriteHeight();
				}
			}
			ImGui::Tooltip("Set Scale to pixel perfect scaling");
			std::string tilesetName;
			if (sprite.tileset)
				tilesetName = sprite.tileset->GetFilepath().filename().string();

			if (ImGui::BeginCombo("Tileset", tilesetName.c_str()))
			{
				for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::TILESET)))
				{
					const bool is_selected = false;
					if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
					{
						if (!sprite.tileset)
							sprite.tileset = CreateRef<Tileset>();
						sprite.tileset->Load(file);
						SceneManager::CurrentScene()->MakeDirty();
					}
					ImGui::Tooltip(file.string().c_str());
				}
				ImGui::EndCombo();
			}
			if (sprite.tileset)
			{
				ImGui::SameLine();

				if (ImGui::Button(ICON_FA_PEN_SQUARE"##AnimatedSprite"))
				{
					ViewerManager::OpenViewer(sprite.tileset->GetFilepath());
				}
				ImGui::Tooltip("Edit Tileset");
			}

			if (sprite.tileset)
			{
				if (ImGui::BeginCombo("Animation", sprite.tileset->GetCurrentAnimation().c_str()))
				{
					for (auto& [name, animation] : sprite.tileset->GetAnimations())
					{
						if (ImGui::Selectable(name.c_str()))
						{
							sprite.SelectAnimation(name);
							SceneManager::CurrentScene()->MakeDirty();
						}
					}
					ImGui::EndCombo();
				}
			}
		});

	//Static Mesh------------------------------------------------------------------------------------------------------------
	DrawComponent<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity, [](auto& staticMesh)
		{
			if (ImGui::FileEdit("Static Mesh", staticMesh.mesh.GetFilepath(), FileType::MESH))
				staticMesh.mesh.LoadModel();
			if (ImGui::MaterialEdit("Material", staticMesh.material))
				SceneManager::CurrentScene()->MakeDirty();
		});

	//Camera------------------------------------------------------------------------------------------------------------
	DrawComponent<CameraComponent>(ICON_FA_VIDEO" Camera", entity, [](auto& cameraComp)
		{
			auto& camera = cameraComp.Camera;

			ImGui::Checkbox("Primary", &cameraComp.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);

			SceneCamera::ProjectionType projectionType = camera.GetProjectionType();
			if (ImGui::Combo("Projection", (int*)&projectionType,
				"Perspective\0"
				"Orthographic"))
			{
				camera.SetProjection(projectionType);
				SceneManager::CurrentScene()->MakeDirty();
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
	DrawComponent<PrimitiveComponent>(ICON_FA_SHAPES" Primitive", entity, [=](auto& primitive)
		{
			if (ImGui::Combo("Shape", (int*)&primitive.type,
				"Cube\0"
				"Sphere\0"
				"Plane\0"
				"Cylinder\0"
				"Cone\0"
				"Torus\0"))
			{
				primitive.needsUpdating = true;
				SceneManager::CurrentScene()->MakeDirty();
			}

			switch (primitive.type)
			{
				int tempInt;
			case PrimitiveComponent::Shape::Cube:
				if (ImGui::DragFloat("Width##cube", &primitive.cubeWidth, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##cube", &primitive.cubeHeight, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Depth##cube", &primitive.cubeDepth, 0.1f, 0.0f))
					primitive.needsUpdating = true;
				break;
			case PrimitiveComponent::Shape::Sphere:
				if (ImGui::DragFloat("Radius##Sphere", &primitive.sphereRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.sphereLongitudeLines;
				if (ImGui::DragInt("Longitude Lines##Sphere", &tempInt, 1.0f, 3, 600))
				{
					primitive.sphereLongitudeLines = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.sphereLatitudeLines;
				if (ImGui::DragInt("Latitude Lines##Sphere", &tempInt, 1.0f, 2, 600))
				{
					primitive.sphereLatitudeLines = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Plane:
				if (ImGui::DragFloat("Width##Plane", &primitive.planeWidth, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Length##Plane", &primitive.planeLength, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.planeWidthLines;
				if (ImGui::DragInt("Width Lines##Plane", &tempInt, 1.0f, 2, 1000))
				{
					primitive.planeWidthLines = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.planeLengthLines;
				if (ImGui::DragInt("Length Lines##Plane", &tempInt, 1.0f, 2, 1000))
				{
					primitive.planeLengthLines = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Tile U##Plane", &primitive.planeTileU, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Tile V##Plane", &primitive.planeTileV, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Cylinder:
				if (ImGui::DragFloat("Bottom Radius##Cylinder", &primitive.cylinderBottomRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Top Radius##Cylinder", &primitive.cylinderTopRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##Cylinder", &primitive.cylinderHeight, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.cylinderSliceCount;
				if (ImGui::DragInt("Slice Count##Cylinder", &tempInt, 1.0f, 3, 600))
				{
					primitive.cylinderSliceCount = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.cylinderStackCount;
				if (ImGui::DragInt("Stack Count##Cylinder", &tempInt, 1.0f, 1, 600))
				{
					primitive.cylinderStackCount = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Cone:
				if (ImGui::DragFloat("Bottom Radius##Cone", &primitive.coneBottomRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Height##Cone", &primitive.coneHeight, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.coneSliceCount;
				if (ImGui::DragInt("Slice Count##Cone", &tempInt, 1.0f, 3, 600))
				{
					primitive.coneSliceCount = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.coneStackCount;
				if (ImGui::DragInt("Stack Count##Cone", &tempInt, 1.0f, 1, 600))
				{
					primitive.coneStackCount = tempInt;
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				break;
			case PrimitiveComponent::Shape::Torus:
				if (ImGui::DragFloat("Outer Radius##Torus", &primitive.torusOuterRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				if (ImGui::DragFloat("Inner Radius##Torus", &primitive.torusInnerRadius, 0.1f, 0.0f))
				{
					primitive.needsUpdating = true;
					SceneManager::CurrentScene()->MakeDirty();
				}
				tempInt = primitive.torusSliceCount;
				if (ImGui::DragInt("Slice Count##Torus", &tempInt, 1.0f, 3, 600))
				{
					primitive.torusSliceCount = tempInt;
					primitive.needsUpdating = true;
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
			if (ImGui::Combo("Body Type", (int*)&rigidBody2D.type,
				"Static\0"
				"Kinematic\0"
				"Dynamic\0"))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

			if (rigidBody2D.type == RigidBody2DComponent::BodyType::DYNAMIC)
			{
				ImGui::Checkbox("Fixed Rotation", &rigidBody2D.fixedRotation);
				ImGui::DragFloat("Gravity Scale", &rigidBody2D.gravityScale, 0.01f, -1.0f, 2.0f);
				ImGui::DragFloat("Angular Damping", &rigidBody2D.angularDamping, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Linear Damping", &rigidBody2D.linearDamping, 0.01f, 0.0f, 1.0f);
			}
		});

	//Box Collider 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<BoxCollider2DComponent>(ICON_FA_VECTOR_SQUARE" Box Collider 2D", entity, [](auto& boxCollider2D)
		{
			Vector2f& offset = boxCollider2D.offset;
			ImGui::Text("Offset");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			float width = ImGui::GetContentRegionAvail().x;

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

			if (ImGui::DragFloat("Density", &boxCollider2D.density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &boxCollider2D.friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &boxCollider2D.restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();
		});

	//Circle Collider 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<CircleCollider2DComponent>(ICON_MDI_CIRCLE_OUTLINE" Circle Collider 2D", entity, [](auto& circleCollider2D)
		{
			Vector2f& offset = circleCollider2D.offset;
			ImGui::Text("Offset");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			float width = ImGui::GetContentRegionAvail().x;

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

			if (ImGui::DragFloat("Radius", &circleCollider2D.radius, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Density", &circleCollider2D.density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &circleCollider2D.friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &circleCollider2D.restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();
		});

	// Polygon Collider 2D ------------------------------------------------------------------------------------------------------------
	DrawComponent<PolygonCollider2DComponent>(ICON_FA_DRAW_POLYGON" Polygon Collider 2D", entity, [](auto& polygonCollider2D)
		{
			Vector2f& offset = polygonCollider2D.offset;
			ImGui::Text("Offset");
			ImGui::TextColored({ 245,0,0,255 }, "X");
			ImGui::SameLine();

			float width = ImGui::GetContentRegionAvail().x;

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

			if (ImGui::TreeNode("Vertices"))
			{
				if (ImGui::Button(ICON_FA_PLUS"##AddVertex"))
				{
					polygonCollider2D.vertices.push_back(Vector2f());
					SceneManager::CurrentScene()->MakeDirty();
				}
				size_t i = 0;
				while (i < polygonCollider2D.vertices.size())
				{
					std::string deleteStr = ICON_FA_TRASH_ALT "##" + std::to_string(i);
					if (ImGui::Button(deleteStr.c_str()))
					{
						polygonCollider2D.vertices.erase(polygonCollider2D.vertices.begin() + i);
						continue;
					}

					ImGui::SameLine();

					ImGui::TextColored({ 245,0,0,255 }, "X");
					ImGui::SameLine();

					float width = ImGui::GetContentRegionAvail().x;

					ImGui::SetNextItemWidth(width / 2 - 20);
					std::string xStr = "##X" + std::to_string(i);
					if (ImGui::DragFloat(xStr.c_str(), &polygonCollider2D.vertices[i].x, 0.1f))
						SceneManager::CurrentScene()->MakeDirty();
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						polygonCollider2D.vertices[i].x = 0.0f;
						SceneManager::CurrentScene()->MakeDirty();
					}

					ImGui::SameLine();
					ImGui::TextColored({ 0,245,0,255 }, "Y");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(width / 2 - 20);
					std::string yStr = "##Y" + std::to_string(i);
					if (ImGui::DragFloat(yStr.c_str(), &polygonCollider2D.vertices[i].y, 0.1f))
						SceneManager::CurrentScene()->MakeDirty();
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						polygonCollider2D.vertices[i].y = 0.0f;
						SceneManager::CurrentScene()->MakeDirty();
					}

					i++;
				}
				ImGui::TreePop();
			}

			if (ImGui::DragFloat("Density", &polygonCollider2D.density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &polygonCollider2D.friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &polygonCollider2D.restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (polygonCollider2D.vertices.size() < 3)
			{
				Colour textColour(Colours::YELLOW);
				ImGui::TextColored(ImVec4(textColour.r, textColour.g, textColour.b, textColour.a), "No Polygon Collider Created!");
			}
		});

	// Circle Renderer------------------------------------------------------------------------------------------------------------------
	DrawComponent<CircleRendererComponent>(ICON_FA_CIRCLE" Circle Renderer", entity, [](auto& circleRenderer)
		{
			float* colour[4] = { &circleRenderer.colour.r, &circleRenderer.colour.g, &circleRenderer.colour.b, &circleRenderer.colour.a };
			ImGui::ColorEdit4("Colour", colour[0]);
			ImGui::DragFloat("Thickness", &circleRenderer.thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &circleRenderer.fade, 0.00025f, 0.0f, 1.0f);
		});

	// Tilemap ------------------------------------------------------------------------------------------------------------------------
	DrawComponent<TilemapComponent>(ICON_FA_BORDER_ALL" Tilemap", entity, [=](auto& tilemap)
		{
			m_TilemapEditor->OnImGuiRender(tilemap);
		});

	//Native Script------------------------------------------------------------------------------------------------------------
	DrawComponent<NativeScriptComponent>(ICON_FA_FILE_CODE" Native Script", entity, [](auto& script)
		{
			ImGui::Text("%s", script.Name.c_str());
		});

	// Lua Script ---------------------------------------------------------------------------------------------------------------------
	DrawComponent<LuaScriptComponent>(ICON_FA_FILE_CODE" Lua Script", entity, [](auto& luaScript)
		{
			if (ImGui::BeginCombo("##luaScript", luaScript.absoluteFilepath.filename().string().c_str()))
			{
				for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::SCRIPT)))
				{
					const bool is_selected = false;
					if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
					{
						luaScript.absoluteFilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / file);
						SceneManager::CurrentScene()->MakeDirty();
						break;
					}
					ImGui::Tooltip(file.string().c_str());
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PEN_SQUARE"##LuaScript"))
			{
				ViewerManager::OpenViewer(luaScript.absoluteFilepath);
			}
			ImGui::Tooltip("Edit script");
		});
}

void PropertiesPanel::DrawAddComponent(Entity entity)
{
	ImGui::Dummy({ 0,0 });
	float width = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine((width / 2.0f) - (ImGui::CalcTextSize("Add Component").x / 2.0f));
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Components");
	}

	if (ImGui::BeginPopup("Components"))
	{
		AddComponentMenuItem<CameraComponent>(ICON_FA_VIDEO" Camera", entity);
		AddComponentMenuItem<SpriteComponent>(ICON_FA_IMAGE" Sprite", entity);
		AddComponentMenuItem<AnimatedSpriteComponent>(ICON_FA_IMAGE" Animated Sprite", entity);
		AddComponentMenuItem<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity);
		AddComponentMenuItem<PrimitiveComponent>(ICON_FA_SHAPES" Primitive", entity);
		AddComponentMenuItem<RigidBody2DComponent>(ICON_FA_BASEBALL_BALL" Rigid Body 2D", entity);
		AddComponentMenuItem<BoxCollider2DComponent>(ICON_FA_VECTOR_SQUARE" Box Collider 2D", entity);
		AddComponentMenuItem<CircleCollider2DComponent>(ICON_MDI_CIRCLE_OUTLINE" Circle Collider 2D", entity);
		AddComponentMenuItem<PolygonCollider2DComponent>(ICON_FA_DRAW_POLYGON" Polygon Collider 2D", entity);
		AddComponentMenuItem<CircleRendererComponent>(ICON_FA_CIRCLE" Circle Renderer", entity);
		AddComponentMenuItem<TilemapComponent>(ICON_FA_BORDER_ALL" Tilemap", entity);

		if (ImGui::BeginMenu(ICON_FA_FILE_CODE" Native Script", !entity.HasComponent<NativeScriptComponent>() && Factory<ScriptableEntity>::GetMap()->size() > 0))
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

		std::vector<std::filesystem::path> scripts = Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::SCRIPT));

		if (ImGui::BeginMenu(ICON_FA_FILE_CODE" Lua Script", !entity.HasComponent<LuaScriptComponent>() && scripts.size() > 0))
		{
			for (std::filesystem::path& file : scripts)
			{
				if (ImGui::MenuItem(file.filename().string().c_str()))
				{
					entity.AddComponent<LuaScriptComponent>(file);
					break;
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
