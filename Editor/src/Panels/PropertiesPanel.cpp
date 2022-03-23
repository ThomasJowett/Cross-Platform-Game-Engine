#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"
#include "ImGui/ImGuiFileEdit.h"
#include "ImGui/ImGuiTextureEdit.h"

#include "Viewers/ViewerManager.h"
#include "FileSystem/Directory.h"

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

						Material material(Shader::Create("Standard"));

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

		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			name = std::string(buffer);
			SceneManager::CurrentScene()->MakeDirty();
		}
	}

	//Transform------------------------------------------------------------------------------------------------------------
	DrawComponent<TransformComponent>("Transform", entity, [](auto& transform)
		{
			ImGui::Transform(transform.position, transform.rotation, transform.scale);
		}, false);


	//Sprite--------------------------------------------------------------------------------------------------------------
	DrawComponent<SpriteComponent>(ICON_FA_IMAGE" Sprite", entity, [](auto& sprite)
		{
			float* tint[4] = { &sprite.tint.r, &sprite.tint.g, &sprite.tint.b, &sprite.tint.a };
			float* tilingFactor = &sprite.tilingFactor;

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

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
	DrawComponent<AnimatedSpriteComponent>(ICON_FA_IMAGE" Animated Sprite", entity, [](auto& sprite)
		{
			float* tint[4] = { &sprite.tint.r, &sprite.tint.g, &sprite.tint.b, &sprite.tint.a };

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::CurrentScene()->MakeDirty();
			}

			// Sprite Sheet
			if (ImGui::Texture2DEdit("Sprite Sheet", sprite.animator.GetSpriteSheet()->GetTexture()))
			{
				SceneManager::CurrentScene()->MakeDirty();
				sprite.animator.GetSpriteSheet()->RecalculateCellsDimensions();
			}

			// Sprite Size
			int spriteSize[2] = { (int)sprite.animator.GetSpriteSheet()->GetSpriteWidth(), (int)sprite.animator.GetSpriteSheet()->GetSpriteHeight() };
			if (ImGui::InputInt2("Sprite Size", spriteSize))
			{
				sprite.animator.GetSpriteSheet()->SetSpriteDimensions(spriteSize[0], spriteSize[1]);
			}

			if (ImGui::Button(ICON_FA_PLUS"## Add animation"))
			{
				sprite.animator.AddAnimation();
			}
			ImGui::Tooltip("Add animation");

			ImGuiTableFlags table_flags =
				ImGuiTableFlags_Resizable;
			if (ImGui::BeginTable("Animations", 4))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Start Frame");
				ImGui::TableSetupColumn("Frame Count");
				ImGui::TableSetupColumn("Frame Time (ms)");
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();

				static char inputBuffer[1024] = "";

				int index = 0;
				for (Animation& animation : sprite.animator.GetAnimations())
				{
					memset(inputBuffer, 0, sizeof(inputBuffer));
					for (int i = 0; i < animation.GetName().length(); i++)
					{
						inputBuffer[i] = animation.GetName()[i];
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string nameStr = "##name" + std::to_string(index);
					ImGui::InputText(nameStr.c_str(), inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_AutoSelectAll);
					animation.SetName(inputBuffer);

					int frameStart = (int)animation.GetStartFrame();
					int frameCount = (int)animation.GetFrameCount();
					float frameTime = animation.GetFrameTime();

					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string starFrameStr = "##startFrame" + std::to_string(index);
					if (ImGui::DragInt(starFrameStr.c_str(), &frameStart))
					{
						if (frameStart < 0)
							frameStart = 0;
						animation.SetStartFrame((uint32_t)frameStart);
					}

					ImGui::TableSetColumnIndex(2);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string frameCountStr = "##frameCount" + std::to_string(index);
					if (ImGui::DragInt(frameCountStr.c_str(), &frameCount))
					{
						if (frameCount < 0)
							frameCount = 0;
						animation.SetFrameCount((uint32_t)frameCount);
					}

					ImGui::TableSetColumnIndex(3);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string frameTimeStr = "##frameTime" + std::to_string(index);
					if (ImGui::DragFloat(frameTimeStr.c_str(), &frameTime, 0.001f, 0.0f, 10.0f, "% .3f"))
						animation.SetFrameTime(frameTime);

					index++;
				}
				ImGui::EndTable();
			}
		});

	//Static Mesh------------------------------------------------------------------------------------------------------------
	DrawComponent<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity, [](auto& staticMesh)
		{
			if (ImGui::FileEdit("Static Mesh", staticMesh.mesh.GetFilepath(), FileType::MESH))
				staticMesh.mesh.LoadModel();
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

			const char* currentShapeTypeString = shapeTypeStrings[(int)primitive.type];

			if (ImGui::BeginCombo("Shape", currentShapeTypeString))
			{
				for (int i = 0; i < 6; i++)
				{
					bool isSelected = currentShapeTypeString == shapeTypeStrings[i];

					if (ImGui::Selectable(shapeTypeStrings[i], isSelected))
					{
						currentShapeTypeString = shapeTypeStrings[i];
						primitive.type = (PrimitiveComponent::Shape)i;
						primitive.needsUpdating = true;
						SceneManager::CurrentScene()->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
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
			const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };

			const char* currentBodyTypeString = bodyTypeStrings[(int)rigidBody2D.type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];

					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						rigidBody2D.type = (RigidBody2DComponent::BodyType)i;
						SceneManager::CurrentScene()->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
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
			Vector2f& offset = boxCollider2D.Offset;
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

			if (ImGui::DragFloat("Density", &boxCollider2D.Density, 0.01f, 0.0f, 10.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Friction", &boxCollider2D.Friction, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();

			if (ImGui::DragFloat("Restitution", &boxCollider2D.Restitution, 0.001f, 0.0f, 1.0f))
				SceneManager::CurrentScene()->MakeDirty();
		});

	//Circle Collider 2D--------------------------------------------------------------------------------------------------------------
	DrawComponent<CircleCollider2DComponent>(ICON_MD_PANORAMA_FISHEYE" Circle Collider 2D", entity, [](auto& circleCollider2D)
		{
			Vector2f& offset = circleCollider2D.Offset;
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
			float* colour[4] = { &circleRenderer.colour.r, &circleRenderer.colour.g, &circleRenderer.colour.b, &circleRenderer.colour.a };
			ImGui::ColorEdit4("Colour", colour[0]);
			ImGui::DragFloat("Thickness", &circleRenderer.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &circleRenderer.Fade, 0.00025f, 0.0f, 1.0f);
		});

	// Lua Script ---------------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<LuaScriptComponent>())
	{
		LuaScriptComponent& scriptComp = entity.GetComponent<LuaScriptComponent>();

		std::string scriptName = scriptComp.filepath.filename().string();
		scriptName = scriptName.substr(0, scriptName.find_last_of('.'));

		scriptName = std::string(ICON_FA_FILE_CODE) + " " + scriptName;

		DrawComponent<LuaScriptComponent>(scriptName.c_str(), entity, [](auto& luaScript)
			{

			});
	}
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
		AddComponentMenuItem<CameraComponent>("Camera", entity);
		AddComponentMenuItem<SpriteComponent>("Sprite", entity);
		AddComponentMenuItem<AnimatedSpriteComponent>("Animated Sprite", entity);
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

		if (ImGui::BeginMenu("Lua Script", !entity.HasComponent<LuaScriptComponent>()))
		{
			for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::SCRIPT)))
			{
				const bool is_selected = false;
				if (ImGui::MenuItem(file.filename().string().c_str(), is_selected))
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
