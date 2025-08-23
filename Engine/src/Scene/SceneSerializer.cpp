#include "stdafx.h"
#include "SceneSerializer.h"

#include "Scene/Entity.h"
#include "Components.h"
#include "Core/Application.h"
#include "Core/Version.h"
#include "Utilities/SerializationUtils.h"
#include "AssetManager.h"

#include "TinyXml2/tinyxml2.h"

/* ------------------------------------------------------------------------------------------------------------------ */

SceneSerializer::SceneSerializer(Scene* scene)
	:m_Scene(scene)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneSerializer::Serialize(const std::filesystem::path& filepath) const
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Scene");

	pRoot->SetAttribute("EngineVersion", VERSION);

	SerializationUtils::Encode(pRoot->InsertNewChildElement("Gravity"), m_Scene->GetGravity());

	doc.InsertFirstChild(pRoot);

	m_Scene->m_Registry.each([this, &pRoot](auto entityID)
		{
			Entity entity = { entityID, m_Scene};

			if (!entity)
				return;

			if (HierarchyComponent const* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();
				hierarchyComp != nullptr && hierarchyComp->parent != entt::null)
				return;

			SerializeEntity(pRoot->InsertNewChildElement("Entity"), entity, pRoot);
			return;
		});

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		m_Scene->SetFilepath(filepath);

		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");

		if (!pRoot)
		{
			ENGINE_ERROR("Not a valid scene file. Could not find Scene node");
			return false;
		}

		Vector2f gravity = m_Scene->GetGravity();
		SerializationUtils::Decode(pRoot->FirstChildElement("Gravity"), gravity);
		m_Scene->SetGravity(gravity);

		// Version

		if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
			ENGINE_WARN("Loading scene created with a different version of the engine");

		// Entities
		tinyxml2::XMLElement* pEntityElement = pRoot->LastChildElement("Entity");

		while (pEntityElement)
		{
			DeserializeEntity(m_Scene, pEntityElement);
			pEntityElement = pEntityElement->PreviousSiblingElement("Entity");
		}

		return true;
	}
	else
	{
		ENGINE_ERROR("Could not load scene {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
}

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;

	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		m_Scene->SetFilepath(filepath);
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");

		if (!pRoot)
		{
			ENGINE_ERROR("Not a valid scene file. Could not find Scene node");
			return false;
		}

		Vector2f gravity = m_Scene->GetGravity();
		SerializationUtils::Decode(pRoot->FirstChildElement("Gravity"), gravity);
		m_Scene->SetGravity(gravity);

		// Version

		if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
			ENGINE_WARN("Loading scene created with a different version of the engine");

		// Entities
		tinyxml2::XMLElement* pEntityElement = pRoot->LastChildElement("Entity");

		while (pEntityElement)
		{
			DeserializeEntity(m_Scene, pEntityElement);
			pEntityElement = pEntityElement->PreviousSiblingElement("Entity");
		}

		return true;
	}
	else
	{
		ENGINE_ERROR("Could not load scene {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SceneSerializer::SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity, tinyxml2::XMLElement* pParentNode)
{
	PROFILE_FUNCTION();

	pElement->SetAttribute("Name", entity.GetName().c_str());
	pElement->SetAttribute("ID", entity.GetID().to_string().c_str());

	if (TransformComponent* transformcomp = entity.TryGetComponent<TransformComponent>())
	{
		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Transform");
		SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Position"), transformcomp->position);
		SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Rotation"), transformcomp->rotation);
		SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Scale"), transformcomp->scale);
	}

	if (entity.HasComponent<CameraComponent>())
	{
		CameraComponent const& component = entity.GetComponent<CameraComponent>();

		tinyxml2::XMLElement* pCameraElement = pElement->InsertNewChildElement("Camera");

		pCameraElement->SetAttribute("Primary", component.primary);
		pCameraElement->SetAttribute("FixedAspectRatio", component.fixedAspectRatio);

		tinyxml2::XMLElement* pSceneCameraElement = pCameraElement->InsertNewChildElement("SceneCamera");

		pSceneCameraElement->SetAttribute("ProjectionType", (int)component.camera.GetProjectionType());
		pSceneCameraElement->SetAttribute("OrthoSize", component.camera.GetOrthoSize());
		pSceneCameraElement->SetAttribute("OrthoNear", component.camera.GetOrthoNear());
		pSceneCameraElement->SetAttribute("OrthoFar", component.camera.GetOrthoFar());
		pSceneCameraElement->SetAttribute("PerspectiveNear", component.camera.GetPerspectiveNear());
		pSceneCameraElement->SetAttribute("PerspectiveFar", component.camera.GetPerspectiveFar());
		pSceneCameraElement->SetAttribute("FOV", component.camera.GetVerticalFov());
		if (component.fixedAspectRatio)
			pSceneCameraElement->SetAttribute("AspectRatio", component.camera.GetAspectRatio());
	}

	if (entity.HasComponent<SpriteComponent>())
	{
		SpriteComponent const& component = entity.GetComponent<SpriteComponent>();

		tinyxml2::XMLElement* pSpriteElement = pElement->InsertNewChildElement("Sprite");
		pSpriteElement->SetAttribute("TilingFactor", component.tilingFactor);
		if (component.texture)
		{
			SerializationUtils::Encode(pSpriteElement->InsertNewChildElement("Texture"), component.texture);
		}

		SerializationUtils::Encode(pSpriteElement->InsertNewChildElement("Tint"), component.tint);
	}

	if (entity.HasComponent<AnimatedSpriteComponent>())
	{
		AnimatedSpriteComponent const& component = entity.GetComponent<AnimatedSpriteComponent>();

		tinyxml2::XMLElement* pAnimatedSpriteElement = pElement->InsertNewChildElement("AnimatedSprite");

		if (component.spriteSheet)
			SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("SpriteSheet"), component.spriteSheet->GetFilepath());

		SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tint"), component.tint);

		pAnimatedSpriteElement->SetAttribute("Animation", component.animation.c_str());
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent const& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pStaticMeshElement = pElement->InsertNewChildElement("StaticMesh");

		if (component.mesh)
			SerializationUtils::Encode(pStaticMeshElement, component.mesh->GetFilepath());

		for (const auto& materialOverride : component.materialOverrides) {
			if(materialOverride != Material::GetDefaultMaterial())
				SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("MaterialOverride"), materialOverride->GetFilepath());
		}
	}

	if (entity.HasComponent<PrimitiveComponent>())
	{
		PrimitiveComponent const& component = entity.GetComponent<PrimitiveComponent>();

		tinyxml2::XMLElement* pPrimitiveElement = pElement->InsertNewChildElement("Primitive");

		if (component.material != Material::GetDefaultMaterial())
			SerializationUtils::Encode(pPrimitiveElement->InsertNewChildElement("Material"), component.material->GetFilepath());

		pPrimitiveElement->SetAttribute("Type", (int)component.type);

		switch (component.type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			tinyxml2::XMLElement* pCubeElement = pPrimitiveElement->InsertNewChildElement("Cube");

			pCubeElement->SetAttribute("Width", component.cubeWidth);
			pCubeElement->SetAttribute("Height", component.cubeHeight);
			pCubeElement->SetAttribute("Depth", component.cubeDepth);
			break;
		}
		case PrimitiveComponent::Shape::Sphere:
		{
			tinyxml2::XMLElement* pSphereElement = pPrimitiveElement->InsertNewChildElement("Sphere");

			pSphereElement->SetAttribute("Radius", component.sphereRadius);
			pSphereElement->SetAttribute("LongitudeLines", component.sphereLongitudeLines);
			pSphereElement->SetAttribute("LatitudeLines", component.sphereLatitudeLines);
			break;
		}
		case PrimitiveComponent::Shape::Plane:
		{
			tinyxml2::XMLElement* pPlaneElement = pPrimitiveElement->InsertNewChildElement("Plane");

			pPlaneElement->SetAttribute("Width", component.planeWidth);
			pPlaneElement->SetAttribute("Length", component.planeLength);
			pPlaneElement->SetAttribute("WidthLines", component.planeWidthLines);
			pPlaneElement->SetAttribute("LengthLines", component.planeLengthLines);
			pPlaneElement->SetAttribute("TileU", component.planeTileU);
			pPlaneElement->SetAttribute("TileV", component.planeTileV);
			break;
		}
		case PrimitiveComponent::Shape::Cylinder:
		{
			tinyxml2::XMLElement* pCylinderElement = pPrimitiveElement->InsertNewChildElement("Cylinder");

			pCylinderElement->SetAttribute("BottomRadius", component.cylinderBottomRadius);
			pCylinderElement->SetAttribute("TopRadius", component.cylinderTopRadius);
			pCylinderElement->SetAttribute("Height", component.cylinderHeight);
			pCylinderElement->SetAttribute("SliceCount", component.cylinderSliceCount);
			pCylinderElement->SetAttribute("StackCount", component.cylinderStackCount);
			break;
		}
		case PrimitiveComponent::Shape::Cone:
		{
			tinyxml2::XMLElement* pConeElement = pPrimitiveElement->InsertNewChildElement("Cone");

			pConeElement->SetAttribute("BottomRadius", component.coneBottomRadius);
			pConeElement->SetAttribute("Height", component.coneHeight);
			pConeElement->SetAttribute("SliceCount", component.coneSliceCount);
			pConeElement->SetAttribute("StackCount", component.coneStackCount);
			break;
		}
		case PrimitiveComponent::Shape::Torus:
		{
			tinyxml2::XMLElement* pTorusElement = pPrimitiveElement->InsertNewChildElement("Torus");

			pTorusElement->SetAttribute("OuterRadius", component.torusOuterRadius);
			pTorusElement->SetAttribute("InnerRadius", component.torusInnerRadius);
			pTorusElement->SetAttribute("SliceCount", component.torusSliceCount);
			break;
		}
		default:
			break;
		}
	}

	if (entity.HasComponent<TilemapComponent>())
	{
		TilemapComponent& component = entity.GetComponent<TilemapComponent>();

		tinyxml2::XMLElement* pTilemapElement = pElement->InsertNewChildElement("Tilemap");

		if (component.tileset)
			SerializationUtils::Encode(pTilemapElement, component.tileset->GetFilepath());

		pTilemapElement->SetAttribute("TilesWide", component.tilesWide);
		pTilemapElement->SetAttribute("TilesHigh", component.tilesHigh);
		pTilemapElement->SetAttribute("TileWidth", component.tileWidth);
		pTilemapElement->SetAttribute("TileHeight", component.tileHeight);

		SerializationUtils::Encode(pTilemapElement->InsertNewChildElement("Tint"), component.tint);

		switch (component.orientation)
		{
		case TilemapComponent::Orientation::orthogonal:
			pTilemapElement->SetAttribute("Orientation", "Orthogonal");
			break;
		case TilemapComponent::Orientation::isometric:
			pTilemapElement->SetAttribute("Orientation", "Isometric");
			break;
		case TilemapComponent::Orientation::staggered:
			pTilemapElement->SetAttribute("Orientation", "Staggered");
			break;
		case TilemapComponent::Orientation::hexagonal:
			pTilemapElement->SetAttribute("Orientation", "Hexagonal");
			break;
		default:
			break;
		}

		pTilemapElement->SetAttribute("IsTrigger", component.isTrigger);

		std::stringstream csv;

		csv << std::endl;

		for (size_t y = 0; y < component.tilesHigh; y++)
		{
			for (size_t x = 0; x < component.tilesWide; x++)
			{
				csv << component.tiles[y][x] << ',';
			}

			if (y == component.tilesHigh - 1)
			{
				csv.seekp(-1, std::stringstream::cur);
				csv << "\0";
			}
			csv << std::endl;
		}

		pTilemapElement->SetText(csv.str().c_str());
	}

	if (TextComponent* component = entity.TryGetComponent<TextComponent>())
	{
		tinyxml2::XMLElement* pTextElement = pElement->InsertNewChildElement("Text");
		pTextElement->SetText(component->text.c_str());
		if (component->font != Font::GetDefaultFont())
			SerializationUtils::Encode(pTextElement->InsertNewChildElement("Font"), component->font->GetFilepath());

		pTextElement->SetAttribute("MaxWidth", component->maxWidth);

		SerializationUtils::Encode(pTextElement->InsertNewChildElement("Colour"), component->colour);
	}

	if (entity.HasComponent<RigidBody2DComponent>())
	{
		RigidBody2DComponent const& component = entity.GetComponent<RigidBody2DComponent>();

		tinyxml2::XMLElement* pRigidBody2DElement = pElement->InsertNewChildElement("RigidBody2D");

		pRigidBody2DElement->SetAttribute("BodyType", (int)component.type);
		pRigidBody2DElement->SetAttribute("FixedRotation", component.fixedRotation);
		pRigidBody2DElement->SetAttribute("GravityScale", component.gravityScale);
		pRigidBody2DElement->SetAttribute("AngularDamping", component.angularDamping);
		pRigidBody2DElement->SetAttribute("LinearDamping", component.linearDamping);
	}

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		BoxCollider2DComponent const& component = entity.GetComponent<BoxCollider2DComponent>();

		tinyxml2::XMLElement* pBoxColliderElement = pElement->InsertNewChildElement("BoxCollider2D");

		if (component.physicsMaterial)
			SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("PhysicsMaterial"), component.physicsMaterial->GetFilepath());

		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Offset"), component.offset);
		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Size"), component.size);

		pBoxColliderElement->SetAttribute("IsTrigger", component.isTrigger);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent const& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pCircleColliderElement = pElement->InsertNewChildElement("CircleCollider2D");

		if (component.physicsMaterial)
			SerializationUtils::Encode(pCircleColliderElement->InsertNewChildElement("PhysicsMaterial"), component.physicsMaterial->GetFilepath());

		pCircleColliderElement->SetAttribute("Radius", component.radius);
		SerializationUtils::Encode(pCircleColliderElement->InsertNewChildElement("Offset"), component.offset);
		pCircleColliderElement->SetAttribute("IsTrigger", component.isTrigger);
	}

	if (entity.HasComponent<PolygonCollider2DComponent>())
	{
		PolygonCollider2DComponent const& component = entity.GetComponent<PolygonCollider2DComponent>();

		tinyxml2::XMLElement* pPolygonColliderElement = pElement->InsertNewChildElement("PolygonCollider2D");

		if (component.physicsMaterial)
			SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("PhysicsMaterial"), component.physicsMaterial->GetFilepath());

		SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Offset"), component.offset);

		for (Vector2f const& vertex : component.vertices)
		{
			SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Vertex"), vertex);
		}

		pPolygonColliderElement->SetAttribute("IsTrigger", component.isTrigger);
	}

	if (entity.HasComponent<CapsuleCollider2DComponent>())
	{
		CapsuleCollider2DComponent const& component = entity.GetComponent<CapsuleCollider2DComponent>();

		tinyxml2::XMLElement* pCapsuleColliderElement = pElement->InsertNewChildElement("CapsuleCollider2D");

		if (component.physicsMaterial)
			SerializationUtils::Encode(pCapsuleColliderElement->InsertNewChildElement("PhysicsMaterial"), component.physicsMaterial->GetFilepath());

		pCapsuleColliderElement->SetAttribute("Direction", (int)component.direction);
		pCapsuleColliderElement->SetAttribute("Radius", component.radius);
		pCapsuleColliderElement->SetAttribute("Height", component.height);

		SerializationUtils::Encode(pCapsuleColliderElement->InsertNewChildElement("Offset"), component.offset);
		pCapsuleColliderElement->SetAttribute("IsTrigger", component.isTrigger);
	}

	if (entity.HasComponent<WeldJoint2DComponent>())
	{
		WeldJoint2DComponent const& component = entity.GetComponent<WeldJoint2DComponent>();
		tinyxml2::XMLElement* pWeldJointElement = pElement->InsertNewChildElement("WeldJoint2D");
		pWeldJointElement->SetAttribute("CollideConnected", component.collideConnected);
		pWeldJointElement->SetAttribute("Damping", component.damping);
		pWeldJointElement->SetAttribute("Stiffness", component.stiffness);
	}

	if (entity.HasComponent<CircleRendererComponent>())
	{
		CircleRendererComponent const& component = entity.GetComponent<CircleRendererComponent>();

		tinyxml2::XMLElement* pCircleRendererElement = pElement->InsertNewChildElement("CircleRenderer");

		pCircleRendererElement->SetAttribute("Radius", component.radius);
		pCircleRendererElement->SetAttribute("Thickness", component.thickness);
		pCircleRendererElement->SetAttribute("Fade", component.fade);

		SerializationUtils::Encode(pCircleRendererElement->InsertNewChildElement("Colour"), component.colour);
	}

	if (entity.HasComponent<BehaviourTreeComponent>())
	{
		BehaviourTreeComponent const& component = entity.GetComponent<BehaviourTreeComponent>();

		tinyxml2::XMLElement* pBehaviourTreeElement = pElement->InsertNewChildElement("BehaviourTree");
		SerializationUtils::Encode(pBehaviourTreeElement, component.filepath);
	}

	if (entity.HasComponent<StateMachineComponent>())
	{
		StateMachineComponent const& component = entity.GetComponent<StateMachineComponent>();
		tinyxml2::XMLElement* pStateMachineElement = pElement->InsertNewChildElement("StateMachine");
	}

	if (entity.HasComponent<LuaScriptComponent>())
	{
		LuaScriptComponent const& component = entity.GetComponent<LuaScriptComponent>();

		tinyxml2::XMLElement* pLuaScriptElement = pElement->InsertNewChildElement("LuaScript");

		if (component.script)
			SerializationUtils::Encode(pLuaScriptElement, component.script->GetFilepath());
	}

	if (entity.HasComponent<PointLightComponent>())
	{
		PointLightComponent const& component = entity.GetComponent<PointLightComponent>();

		tinyxml2::XMLElement* pPointLightElement = pElement->InsertNewChildElement("PointLight");

		pPointLightElement->SetAttribute("Range", component.range);
		pPointLightElement->SetAttribute("Attenuation", component.attenuation);
		pPointLightElement->SetAttribute("CastShadows", component.castsShadows);

		SerializationUtils::Encode(pPointLightElement->InsertNewChildElement("Colour"), component.colour);
	}

	if (BillboardComponent* component = entity.TryGetComponent<BillboardComponent>())
	{
		tinyxml2::XMLElement* pBillboardElement = pElement->InsertNewChildElement("Billboard");

		pBillboardElement->SetAttribute("Orientation", (int)component->orientation);
		pBillboardElement->SetAttribute("Position", (int)component->position);
		if (component->position == BillboardComponent::Position::Camera)
			SerializationUtils::Encode(pBillboardElement->InsertNewChildElement("ScreenPosition"), component->screenPosition);
	}

	if (CanvasComponent* component = entity.TryGetComponent<CanvasComponent>())
	{
		tinyxml2::XMLElement* pCanvasElement = pElement->InsertNewChildElement("Canvas");

		pCanvasElement->SetAttribute("PixelPerUnit", component->pixelPerUnit);
	}

	if (WidgetComponent* component = entity.TryGetComponent<WidgetComponent>())
	{
		tinyxml2::XMLElement* pWidgetElement = pElement->InsertNewChildElement("Widget");

		pWidgetElement->SetAttribute("Disabled", component->disabled);
		pWidgetElement->SetAttribute("FixedWidth", component->fixedWidth);
		pWidgetElement->SetAttribute("FixedHeight", component->fixedHeight);
		pWidgetElement->SetAttribute("AnchorLeft", component->anchorLeft);
		pWidgetElement->SetAttribute("AnchorRight", component->anchorRight);
		pWidgetElement->SetAttribute("AnchorTop", component->anchorTop);
		pWidgetElement->SetAttribute("AnchorBottom", component->anchorBottom);
		pWidgetElement->SetAttribute("MarginLeft", component->marginLeft);
		pWidgetElement->SetAttribute("MarginRight", component->marginRight);
		pWidgetElement->SetAttribute("MarginTop", component->marginTop);
		pWidgetElement->SetAttribute("MarginBottom", component->marginBottom);

		SerializationUtils::Encode(pWidgetElement->InsertNewChildElement("Position"), component->position);
		SerializationUtils::Encode(pWidgetElement->InsertNewChildElement("Size"), component->size);

	}

	if (ButtonComponent* component = entity.TryGetComponent<ButtonComponent>())
	{
		tinyxml2::XMLElement* pButtonElement = pElement->InsertNewChildElement("Button");

		if (component->normalTexture)
		{
			SerializationUtils::Encode(pButtonElement->InsertNewChildElement("NormalTexture"), component->normalTexture);
		}
		if (component->hoveredTexture)
		{
			SerializationUtils::Encode(pButtonElement->InsertNewChildElement("HoveredTexture"), component->hoveredTexture);
		}
		if (component->clickedTexture)
		{
			SerializationUtils::Encode(pButtonElement->InsertNewChildElement("ClickedTexture"), component->clickedTexture);
		}
		if (component->disabledTexture)
		{
			SerializationUtils::Encode(pButtonElement->InsertNewChildElement("DisabledTexture"), component->disabledTexture);
		}

		SerializationUtils::Encode(pButtonElement->InsertNewChildElement("NormalTint"), component->normalTint);
		SerializationUtils::Encode(pButtonElement->InsertNewChildElement("HoveredTint"), component->hoveredTint);
		SerializationUtils::Encode(pButtonElement->InsertNewChildElement("ClickedTint"), component->clickedTint);
		SerializationUtils::Encode(pButtonElement->InsertNewChildElement("disabledTint"), component->disabledTint);
	}

	if (AudioSourceComponent* component = entity.TryGetComponent<AudioSourceComponent>())
	{
		tinyxml2::XMLElement* pAudioSourceElement = pElement->InsertNewChildElement("AudioSource");
		if (component->audioClip)
			SerializationUtils::Encode(pAudioSourceElement, component->audioClip->GetFilepath());
		pAudioSourceElement->SetAttribute("Volume", component->volume);
		pAudioSourceElement->SetAttribute("Pitch", component->pitch);
		pAudioSourceElement->SetAttribute("Loop", component->loop);
		pAudioSourceElement->SetAttribute("MinDistance", component->minDistance);
		pAudioSourceElement->SetAttribute("MaxDistance", component->maxDistance);
		pAudioSourceElement->SetAttribute("Rolloff", component->rolloff);
		pAudioSourceElement->SetAttribute("Stream", component->stream);
		pAudioSourceElement->SetAttribute("PlayOnStart", component->playOnStart);
	}

	if (AudioListenerComponent* component = entity.TryGetComponent<AudioListenerComponent>())
	{
		tinyxml2::XMLElement* pAudioListenerElement = pElement->InsertNewChildElement("AudioListener");
		pAudioListenerElement->SetAttribute("Primary", component->primary);
	}

	if (entity.HasComponent<HierarchyComponent>())
	{
		if (HierarchyComponent const& component = entity.GetComponent<HierarchyComponent>();
			component.firstChild != entt::null)
		{
			Entity child = { component.firstChild, entity.GetScene() };

			SerializeEntity(pElement->InsertNewChildElement("Entity"), child, pElement);
		}

		if (pParentNode != nullptr)
		{
			entt::entity next = entity.GetComponent<HierarchyComponent>().nextSibling;
			if (next != entt::null)
			{
				Entity siblingEntity = { next, entity.GetScene() };
				SerializeEntity(pParentNode->InsertNewChildElement("Entity"), siblingEntity, pParentNode);
				return;
			}
		}
	}
}

Entity SceneSerializer::DeserializeEntity(Scene* scene, tinyxml2::XMLElement* pEntityElement, bool resetUuid)
{
	PROFILE_FUNCTION();

	Entity entity;
	if (!pEntityElement)
		return entity;
	const char* entityName = pEntityElement->Attribute("Name");

	if (const char* uuidChar = pEntityElement->Attribute("ID");
		uuidChar && entityName && !resetUuid)
	{
		Uuid uuid(uuidChar);
		entity = scene->CreateEntity(uuid, entityName);
	}
	else if (entityName && resetUuid)
	{
		entity = scene->CreateEntity(Uuid(), entityName);
	}
	else if (entityName)
	{
		ENGINE_WARN("Entity ID not found in file");
		entity = scene->CreateEntity(entityName);
	}
	else
	{
		ENGINE_WARN("Entity name not found in file");
		entity = scene->CreateEntity();
	}

	// Transform ----------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pTransformComponentElement = pEntityElement->FirstChildElement("Transform"))
	{
		TransformComponent& transformComp = entity.GetOrAddComponent<TransformComponent>();
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Position"), transformComp.position);
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Rotation"), transformComp.rotation);
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Scale"), transformComp.scale);
	}

	// Camera -------------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pCameraComponentElement = pEntityElement->FirstChildElement("Camera"))
	{
		CameraComponent& component = entity.AddComponent<CameraComponent>();

		pCameraComponentElement->QueryBoolAttribute("Primary", &component.primary);
		pCameraComponentElement->QueryBoolAttribute("FixedAspectRatio", &component.fixedAspectRatio);

		SceneCamera sceneCamera;

		if (tinyxml2::XMLElement const* pSceneCameraElement = pCameraComponentElement->FirstChildElement("SceneCamera"))
		{
			SceneCamera::ProjectionType projectionType =
				(SceneCamera::ProjectionType)pSceneCameraElement->IntAttribute("ProjectionType", (int)sceneCamera.GetProjectionType());

			pSceneCameraElement->QueryFloatAttribute("OrthoSize", &sceneCamera.m_OrthographicSize);
			pSceneCameraElement->QueryFloatAttribute("OrthoNear", &sceneCamera.m_OrthoGraphicNear);
			pSceneCameraElement->QueryFloatAttribute("OrthoFar", &sceneCamera.m_OrthoGraphicFar);
			pSceneCameraElement->QueryFloatAttribute("PerspectiveNear", &sceneCamera.m_PerspectiveNear);
			pSceneCameraElement->QueryFloatAttribute("PerspectiveFar", &sceneCamera.m_PerspectiveFar);
			pSceneCameraElement->QueryFloatAttribute("FOV", &sceneCamera.m_Fov);
			if (component.fixedAspectRatio)
				pSceneCameraElement->QueryFloatAttribute("AspectRatio", &sceneCamera.m_AspectRatio);

			sceneCamera.SetProjection(projectionType);
		}

		component.camera = sceneCamera;
	}

	// Sprite -------------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pSpriteComponentElement = pEntityElement->FirstChildElement("Sprite"))
	{
		SpriteComponent& component = entity.AddComponent<SpriteComponent>();

		SerializationUtils::Decode(pSpriteComponentElement->FirstChildElement("Tint"), component.tint);
		pSpriteComponentElement->QueryFloatAttribute("Tilingfactor", &component.tilingFactor);

		SerializationUtils::Decode(pSpriteComponentElement->FirstChildElement("Texture"), component.texture);
	}

	// Animated Sprite ---------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pAnimatedSpriteComponentElement = pEntityElement->FirstChildElement("AnimatedSprite"))
	{
		AnimatedSpriteComponent& component = entity.AddComponent<AnimatedSpriteComponent>();

		SerializationUtils::Decode(pAnimatedSpriteComponentElement->FirstChildElement("Tint"), component.tint);

		if (tinyxml2::XMLElement const* pTilesetElement = pAnimatedSpriteComponentElement->FirstChildElement("SpriteSheet"))
		{
			const char* tilesetChar = pTilesetElement->Attribute("Filepath");

			if (tilesetChar)
			{
				if (std::filesystem::path tilesetFilepath = tilesetChar;
					!tilesetFilepath.empty())
				{
					component.spriteSheet = AssetManager::GetAsset<SpriteSheet>(tilesetFilepath);
				}
			}
		}

		const char* animation = pAnimatedSpriteComponentElement->Attribute("Animation");
		if (animation)
		{
			component.animation = animation;
			if (component.spriteSheet) {
				Animation* animationRef = component.spriteSheet->GetAnimation(animation);
				if(animationRef)
					component.currentFrame = animationRef->GetStartFrame();

			}
		}
	}

	// Static Mesh -------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pStaticMeshComponentElement = pEntityElement->FirstChildElement("StaticMesh"))
	{
		StaticMeshComponent& component = entity.AddComponent<StaticMeshComponent>();

		const char* meshFilepathChar = pStaticMeshComponentElement->Attribute("Filepath");

		if (meshFilepathChar)
		{
			if (std::filesystem::path meshFilepath = meshFilepathChar;
				!meshFilepath.empty())
			{
				component.SetMesh(AssetManager::GetAsset<StaticMesh>(meshFilepath));
			}
		}

		uint32_t materialIndex = 0;
		for (tinyxml2::XMLElement const* pMaterialElement = pStaticMeshComponentElement->FirstChildElement("MaterialOverride");
			pMaterialElement; pMaterialElement = pMaterialElement->NextSiblingElement("MaterialOverride"))
		{
			if (const char* materialFilepathChar = pMaterialElement->Attribute("Filepath"))
			{
				component.materialOverrides.at(materialIndex) = AssetManager::GetAsset<Material>(materialFilepathChar);
			}
			materialIndex++;
		}
	}

	// Primitive -------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pPrimitiveComponentElement = pEntityElement->FirstChildElement("Primitive"))
	{
		PrimitiveComponent::Shape type =
			(PrimitiveComponent::Shape)pPrimitiveComponentElement->IntAttribute("Type", (int)PrimitiveComponent::Shape::Cube);

		std::filesystem::path materialPath;
		SerializationUtils::Decode(pPrimitiveComponentElement->FirstChildElement("Material"), materialPath);

		switch (type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			if (tinyxml2::XMLElement const* pCubeElement = pPrimitiveComponentElement->FirstChildElement("Cube"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pCubeElement->QueryFloatAttribute("Width", &component.cubeWidth);
				pCubeElement->QueryFloatAttribute("Height", &component.cubeHeight);
				pCubeElement->QueryFloatAttribute("Depth", &component.cubeDepth);

				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetCube(component.cubeWidth, component.cubeDepth, component.cubeHeight);
			}
			break;
		}
		case PrimitiveComponent::Shape::Sphere:
		{
			if (tinyxml2::XMLElement const* pSphereElement = pPrimitiveComponentElement->FirstChildElement("Sphere"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pSphereElement->QueryFloatAttribute("Radius", &component.sphereRadius);
				pSphereElement->QueryUnsignedAttribute("LongitudeLines", &component.sphereLongitudeLines);
				pSphereElement->QueryUnsignedAttribute("LatitudeLines", &component.sphereLatitudeLines);
				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetSphere(component.sphereRadius, component.sphereLongitudeLines, component.sphereLatitudeLines);
			}
			break;
		}
		case PrimitiveComponent::Shape::Plane:
		{
			if (tinyxml2::XMLElement const* pPlaneElement = pPrimitiveComponentElement->FirstChildElement("Plane"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pPlaneElement->QueryFloatAttribute("Width", &component.planeWidth);
				pPlaneElement->QueryFloatAttribute("Length", &component.planeLength);
				pPlaneElement->QueryUnsignedAttribute("WidthLines", &component.planeWidthLines);
				pPlaneElement->QueryUnsignedAttribute("LengthLines", &component.planeLengthLines);
				pPlaneElement->QueryFloatAttribute("TileU", &component.planeTileU);
				pPlaneElement->QueryFloatAttribute("TileV", &component.planeTileV);
				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetPlane(component.planeWidth, component.planeLength, component.planeWidthLines, component.planeLengthLines, component.planeTileU, component.planeTileV);
			}
			break;
		}
		case PrimitiveComponent::Shape::Cylinder:
		{
			if (tinyxml2::XMLElement const* pCylinderElement = pPrimitiveComponentElement->FirstChildElement("Cylinder"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pCylinderElement->QueryFloatAttribute("BottomRadius", &component.cylinderBottomRadius);
				pCylinderElement->QueryFloatAttribute("TopRadius", &component.cylinderTopRadius);
				pCylinderElement->QueryFloatAttribute("Height", &component.cylinderHeight);
				pCylinderElement->QueryUnsignedAttribute("SliceCount", &component.cylinderSliceCount);
				pCylinderElement->QueryUnsignedAttribute("StackCount", &component.cylinderStackCount);
				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetCylinder(component.cylinderBottomRadius, component.cylinderTopRadius, component.cylinderHeight, component.cylinderSliceCount, component.cylinderStackCount);
			}
			break;
		}
		case PrimitiveComponent::Shape::Cone:
		{
			if (tinyxml2::XMLElement const* pConeElement = pPrimitiveComponentElement->FirstChildElement("Cone"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pConeElement->QueryFloatAttribute("BottomRadius", &component.coneBottomRadius);
				pConeElement->QueryFloatAttribute("Height", &component.coneHeight);
				pConeElement->QueryUnsignedAttribute("SliceCount", &component.coneSliceCount);
				pConeElement->QueryUnsignedAttribute("StackCount", &component.coneStackCount);
				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetCone(component.coneBottomRadius, component.coneHeight, component.coneSliceCount, component.coneStackCount);
			}
			break;
		}
		case PrimitiveComponent::Shape::Torus:
		{
			if (tinyxml2::XMLElement const* pTorusElement = pPrimitiveComponentElement->FirstChildElement("Torus"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>();

				pTorusElement->QueryFloatAttribute("OuterRadius", &component.torusOuterRadius);
				pTorusElement->QueryFloatAttribute("InnerRadius", &component.torusInnerRadius);
				pTorusElement->QueryUnsignedAttribute("SliceCount", &component.torusSliceCount);
				if (!materialPath.empty()) component.material = AssetManager::GetAsset<Material>(materialPath);
				component.SetTorus(component.torusOuterRadius, component.torusInnerRadius, component.torusSliceCount);
			}
			break;
		}
		default:
			break;
		}
	}

	// Tilemap ------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pTilemapComponentElement = pEntityElement->FirstChildElement("Tilemap"))
	{
		TilemapComponent& component = entity.AddComponent<TilemapComponent>();

		if (const char* tilesetChar = pTilemapComponentElement->Attribute("Filepath"))
		{
			std::filesystem::path tilesetfilepath = tilesetChar;
			if (!tilesetfilepath.empty())
			{
				component.tileset = AssetManager::GetAsset<Tileset>(tilesetfilepath);
			}
		}

		if (const char* orientationchar = pTilemapComponentElement->Attribute("Orientation"); orientationchar != nullptr)
		{
			std::string orientation(orientationchar);

			if (orientation == "Orthogonal")
				component.orientation = TilemapComponent::Orientation::orthogonal;
			else if (orientation == "Isometric")
				component.orientation = TilemapComponent::Orientation::isometric;
			else if (orientation == "Staggered")
				component.orientation = TilemapComponent::Orientation::staggered;
			else if (orientation == "Hexagonal")
				component.orientation = TilemapComponent::Orientation::hexagonal;
		}

		component.isTrigger = pTilemapComponentElement->BoolAttribute("IsTrigger", false);

		pTilemapComponentElement->QueryUnsignedAttribute("TilesWide", &component.tilesWide);
		pTilemapComponentElement->QueryUnsignedAttribute("TilesHigh", &component.tilesHigh);
		pTilemapComponentElement->QueryUnsignedAttribute("TileWidth", &component.tileWidth);
		pTilemapComponentElement->QueryUnsignedAttribute("TileHeight", &component.tileHeight);

		SerializationUtils::Decode(pTilemapComponentElement->FirstChildElement("Tint"), component.tint);

		if (const char* text = pTilemapComponentElement->GetText())
		{
			std::vector<std::string> seperatedData = SplitString(text, ',');

			ASSERT((uint32_t)seperatedData.size() == component.tilesWide * component.tilesHigh, "Data not the correct length");
			component.tiles.resize(component.tilesHigh);

			for (uint32_t i = 0; i < component.tilesHigh; i++)
			{
				component.tiles[i].resize(component.tilesWide);
				for (uint32_t j = 0; j < component.tilesWide; j++)
				{
					component.tiles[i][j] = (uint32_t)atoi(seperatedData[(i * (static_cast<size_t>(component.tilesWide))) + j].c_str());
				}
			}
		}

		component.Rebuild();
	}

	// RigidBody2D ------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pRigidBody2DComponentElement = pEntityElement->FirstChildElement("RigidBody2D"))
	{
		RigidBody2DComponent& component = entity.AddComponent<RigidBody2DComponent>();

		component.type = (RigidBody2DComponent::BodyType)pRigidBody2DComponentElement->IntAttribute("BodyType", (int)component.type);
		pRigidBody2DComponentElement->QueryBoolAttribute("FixedRotation", &component.fixedRotation);
		pRigidBody2DComponentElement->QueryFloatAttribute("GravityScale", &component.gravityScale);
		pRigidBody2DComponentElement->QueryFloatAttribute("AngularDamping", &component.angularDamping);
		pRigidBody2DComponentElement->QueryFloatAttribute("LinearDamping", &component.linearDamping);
	}

	// BoxCollider2D -----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pBoxColliderComponentElement = pEntityElement->FirstChildElement("BoxCollider2D"))
	{
		BoxCollider2DComponent& component = entity.AddComponent<BoxCollider2DComponent>();

		std::filesystem::path physicsMaterial;
		SerializationUtils::Decode(pBoxColliderComponentElement->FirstChildElement("PhysicsMaterial"), physicsMaterial);

		if (!physicsMaterial.empty())
		{
			component.physicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(physicsMaterial);
		}
		else
			component.physicsMaterial = nullptr;

		SerializationUtils::Decode(pBoxColliderComponentElement->FirstChildElement("Offset"), component.offset);
		SerializationUtils::Decode(pBoxColliderComponentElement->FirstChildElement("Size"), component.size);

		component.isTrigger = pBoxColliderComponentElement->BoolAttribute("IsTrigger", false);
	}

	// CircleCollider2D -----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pCircleCollider2DComponentElement = pEntityElement->FirstChildElement("CircleCollider2D"))
	{
		CircleCollider2DComponent& component = entity.AddComponent<CircleCollider2DComponent>();
		std::filesystem::path physicsMaterial;
		SerializationUtils::Decode(pCircleCollider2DComponentElement->FirstChildElement("PhysicsMaterial"), physicsMaterial);

		if (!physicsMaterial.empty())
		{
			component.physicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(physicsMaterial);
		}
		else
			component.physicsMaterial = nullptr;

		pCircleCollider2DComponentElement->QueryFloatAttribute("Radius", &component.radius);
		SerializationUtils::Decode(pCircleCollider2DComponentElement->FirstChildElement("Offset"), component.offset);
		component.isTrigger = pCircleCollider2DComponentElement->BoolAttribute("IsTrigger", false);
	}

	// PolygonCollider2D --------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pPolygonCollider2DComponentElement = pEntityElement->FirstChildElement("PolygonCollider2D"))
	{
		PolygonCollider2DComponent& component = entity.AddComponent<PolygonCollider2DComponent>();

		std::filesystem::path physicsMaterial;
		SerializationUtils::Decode(pPolygonCollider2DComponentElement->FirstChildElement("PhysicsMaterial"), physicsMaterial);

		if (!physicsMaterial.empty())
		{
			component.physicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(physicsMaterial);
		}
		else
			component.physicsMaterial = nullptr;
		SerializationUtils::Decode(pPolygonCollider2DComponentElement->FirstChildElement("Offset"), component.offset);

		tinyxml2::XMLElement* pVertexElement = pPolygonCollider2DComponentElement->FirstChildElement("Vertex");

		component.vertices.clear();
		while (pVertexElement)
		{
			Vector2f vertex;
			SerializationUtils::Decode(pVertexElement, vertex);
			component.vertices.push_back(vertex);
			pVertexElement = pVertexElement->NextSiblingElement("Vertex");
		}

		component.isTrigger = pPolygonCollider2DComponentElement->BoolAttribute("IsTrigger", false);
	}

	// CapsuleCollider2D --------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pCapsuleColliderComponentElement = pEntityElement->FirstChildElement("CapsuleCollider2D"))
	{
		CapsuleCollider2DComponent& component = entity.AddComponent<CapsuleCollider2DComponent>();

		std::filesystem::path physicsMaterial;
		SerializationUtils::Decode(pCapsuleColliderComponentElement->FirstChildElement("PhysicsMaterial"), physicsMaterial);

		if (!physicsMaterial.empty())
		{
			component.physicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(physicsMaterial);
		}
		else
			component.physicsMaterial = nullptr;
		SerializationUtils::Decode(pCapsuleColliderComponentElement->FirstChildElement("Offset"), component.offset);
		pCapsuleColliderComponentElement->QueryFloatAttribute("Radius", &component.radius);
		pCapsuleColliderComponentElement->QueryFloatAttribute("Height", &component.height);

		component.direction = (CapsuleCollider2DComponent::Direction)pCapsuleColliderComponentElement->IntAttribute("Direction", (int)component.direction);

		component.isTrigger = pCapsuleColliderComponentElement->BoolAttribute("IsTrigger", false);
	}

	// WeldJoint2D ---------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pWeldJoint2DColliderComponentElement = pEntityElement->FirstChildElement("WeldJoint2D"))
	{
		WeldJoint2DComponent& component = entity.AddComponent<WeldJoint2DComponent>();
		pWeldJoint2DColliderComponentElement->QueryBoolAttribute("CollideConnected", &component.collideConnected);
		pWeldJoint2DColliderComponentElement->QueryFloatAttribute("Damping", &component.damping);
		pWeldJoint2DColliderComponentElement->QueryFloatAttribute("Stiffness", &component.stiffness);
	}

	// CircleRenderer -----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pCircleRendererComponentElement = pEntityElement->FirstChildElement("CircleRenderer"))
	{
		CircleRendererComponent& component = entity.AddComponent<CircleRendererComponent>();

		pCircleRendererComponentElement->QueryFloatAttribute("Radius", &component.radius);
		pCircleRendererComponentElement->QueryFloatAttribute("Thickness", &component.thickness);
		pCircleRendererComponentElement->QueryFloatAttribute("Fade", &component.fade);

		SerializationUtils::Decode(pCircleRendererComponentElement->FirstChildElement("Colour"), component.colour);
	}

	// Text --------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pTextComponentElement = pEntityElement->FirstChildElement("Text"))
	{
		TextComponent& component = entity.AddComponent<TextComponent>();

		if (const char* text = pTextComponentElement->GetText())
		{
			component.text = text;
		}
		pTextComponentElement->QueryFloatAttribute("MaxWidth", &component.maxWidth);
		std::filesystem::path fontPath;
		SerializationUtils::Decode(pTextComponentElement->FirstChildElement("Font"), fontPath);
		if (!fontPath.empty())
			component.font = AssetManager::GetAsset<Font>(fontPath);
		else
			component.font = Font::GetDefaultFont();
		SerializationUtils::Decode(pTextComponentElement->FirstChildElement("Colour"), component.colour);
	}

	// Behaviour Tree ----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pBehaviourTreeComponentElement = pEntityElement->FirstChildElement("BehaviourTree"))
	{
		BehaviourTreeComponent& component = entity.AddComponent<BehaviourTreeComponent>();

		SerializationUtils::Decode(pBehaviourTreeComponentElement, component.filepath);
		if(!component.filepath.empty())
			component.behaviourTree = BehaviourTree::Serializer::Deserialize(component.filepath);
	}

	// State Machine -----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pStateMachineComponentElement = pEntityElement->FirstChildElement("StateMachine"))
	{
		StateMachineComponent& component = entity.AddComponent<StateMachineComponent>();
	}

	// LuaScript ---------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pLuaScriptComponentElement = pEntityElement->FirstChildElement("LuaScript"))
	{
		LuaScriptComponent& component = entity.AddComponent<LuaScriptComponent>();
		std::filesystem::path scriptPath;
		SerializationUtils::Decode(pLuaScriptComponentElement, scriptPath);
		if (!scriptPath.empty())
			component.script = AssetManager::GetAsset<LuaScript>(scriptPath);
		else
			component.script = nullptr;
	}

	// Point Light --------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pPointLightComponentElement = pEntityElement->FirstChildElement("PointLight"))
	{
		PointLightComponent& component = entity.AddComponent<PointLightComponent>();

		pPointLightComponentElement->QueryFloatAttribute("Range", &component.range);
		pPointLightComponentElement->QueryFloatAttribute("Attenuation", &component.attenuation);
		pPointLightComponentElement->QueryBoolAttribute("CastShadows", &component.castsShadows);

		SerializationUtils::Decode(pPointLightComponentElement->FirstChildElement("Colour"), component.colour);
	}

	// Billboard ---------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pBillboardComponentElement = pEntityElement->FirstChildElement("Billboard"))
	{
		BillboardComponent& component = entity.AddComponent<BillboardComponent>();

		component.orientation = (BillboardComponent::Orientation)pBillboardComponentElement->IntAttribute("Orientation", (int)component.orientation);
		component.position = (BillboardComponent::Position)pBillboardComponentElement->IntAttribute("Position", (int)component.position);
		if (component.position == BillboardComponent::Position::Camera)
			SerializationUtils::Decode(pBillboardComponentElement->FirstChildElement("ScreenPosition"), component.screenPosition);
	}

	// Canvas ----------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pCanvasComponent = pEntityElement->FirstChildElement("Canvas"))
	{
		CanvasComponent& component = entity.AddComponent<CanvasComponent>();

		component.pixelPerUnit = pCanvasComponent->FloatAttribute("PixelPerUnit", 1.0f);
	}

	// Widget ----------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pWidgetComponent = pEntityElement->FirstChildElement("Widget"))
	{
		WidgetComponent& component = entity.AddComponent<WidgetComponent>();

		pWidgetComponent->QueryBoolAttribute("Disabled", &component.disabled);

		pWidgetComponent->QueryBoolAttribute("FixedWidth", &component.fixedWidth);
		pWidgetComponent->QueryBoolAttribute("FixedHeight", &component.fixedHeight);

		pWidgetComponent->QueryFloatAttribute("AnchorLeft", &component.anchorLeft);
		pWidgetComponent->QueryFloatAttribute("AnchorRight", &component.anchorRight);
		pWidgetComponent->QueryFloatAttribute("AnchorTop", &component.anchorTop);
		pWidgetComponent->QueryFloatAttribute("AnchorBottom", &component.anchorBottom);

		pWidgetComponent->QueryFloatAttribute("MarginLeft", &component.marginLeft);
		pWidgetComponent->QueryFloatAttribute("MarginRight", &component.marginRight);
		pWidgetComponent->QueryFloatAttribute("MarginTop", &component.marginTop);
		pWidgetComponent->QueryFloatAttribute("MarginBottom", &component.marginBottom);
		
		pWidgetComponent->QueryFloatAttribute("rotation", &component.rotation);
		SerializationUtils::Decode(pWidgetComponent->FirstChildElement("Position"), component.position);
		SerializationUtils::Decode(pWidgetComponent->FirstChildElement("Size"), component.size);
	}

	// Button ----------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pButtonComponent = pEntityElement->FirstChildElement("Button"))
	{
		ButtonComponent& component = entity.AddComponent<ButtonComponent>();
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("NormalTexture"), component.normalTexture);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("HoveredTexture"), component.hoveredTexture);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("ClickedTexture"), component.clickedTexture);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("DisabledTexture"), component.disabledTexture);

		SerializationUtils::Decode(pButtonComponent->FirstChildElement("NormalTint"), component.normalTint);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("HoveredTint"), component.hoveredTint);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("ClickedTint"), component.clickedTint);
		SerializationUtils::Decode(pButtonComponent->FirstChildElement("DisabledTint"), component.disabledTint);
	}

	// AudioSource ----------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pAudioSourceComponent = pEntityElement->FirstChildElement("AudioSource"))
	{
		AudioSourceComponent& component = entity.AddComponent<AudioSourceComponent>();
		std::filesystem::path audioClipPath;
		SerializationUtils::Decode(pAudioSourceComponent, audioClipPath);
		if (!audioClipPath.empty())
			component.audioClip = AssetManager::GetAsset<AudioClip>(audioClipPath);
		pAudioSourceComponent->QueryFloatAttribute("Volume", &component.volume);
		pAudioSourceComponent->QueryFloatAttribute("Pitch", &component.pitch);
		pAudioSourceComponent->QueryBoolAttribute("Loop", &component.loop);
		pAudioSourceComponent->QueryFloatAttribute("MinDistance", &component.minDistance);
		pAudioSourceComponent->QueryFloatAttribute("MaxDistance", &component.maxDistance);
		pAudioSourceComponent->QueryFloatAttribute("Rolloff", &component.rolloff);
		pAudioSourceComponent->QueryBoolAttribute("Stream", &component.stream);
		pAudioSourceComponent->QueryBoolAttribute("PlayOnStart", &component.playOnStart);
	}

	// AudioListener ----------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pAudioListenerComponent = pEntityElement->FirstChildElement("AudioListener"))
	{
		AudioListenerComponent& component = entity.AddComponent<AudioListenerComponent>();
		pAudioListenerComponent->QueryBoolAttribute("Primary", &component.primary);
	}

	// Hierarchy --------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pChildElement = pEntityElement->LastChildElement("Entity"))
	{
		entity.AddComponent<HierarchyComponent>();

		Entity previousChild;

		while (pChildElement)
		{
			Entity childEntity = DeserializeEntity(scene, pChildElement, resetUuid);
			HierarchyComponent& childHierarchyComp = childEntity.GetOrAddComponent<HierarchyComponent>();
			childHierarchyComp.parent = entity.GetHandle();

			childHierarchyComp.nextSibling = previousChild.GetHandle();
			if (previousChild.GetHandle() != entt::null)
				previousChild.GetComponent<HierarchyComponent>().previousSibling = childEntity.GetHandle();
			previousChild = childEntity;

			entity.GetComponent<HierarchyComponent>().firstChild = childEntity.GetHandle();
			pChildElement = pChildElement->PreviousSiblingElement("Entity");
		}
	}

	return entity;
}

std::string SceneSerializer::SerializeEntity(Entity entity)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pEntityElement = doc.NewElement("Entity");
	doc.InsertFirstChild(pEntityElement);
	SceneSerializer::SerializeEntity(pEntityElement, entity);
	tinyxml2::XMLPrinter printer;
	doc.Accept(&printer);
	return printer.CStr();
}

Entity SceneSerializer::DeserializeEntity(Scene* scene, const std::string& prefab, bool resetUuid)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.Parse(prefab.c_str());

	if (error == tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pEntityElement = doc.FirstChildElement("Entity");
		if (pEntityElement)
			return SceneSerializer::DeserializeEntity(scene, pEntityElement, resetUuid);
	}
	return Entity();
}
