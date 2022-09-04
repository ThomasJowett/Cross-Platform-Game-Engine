#include "stdafx.h"
#include "SceneSerializer.h"

#include "Scene/Entity.h"
#include "Components/Components.h"
#include "Core/Application.h"
#include "Core/Version.h"
#include "Utilities/SerializationUtils.h"
#include "AssetManager.h"

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
			Entity entity = { entityID, m_Scene };

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

/* ------------------------------------------------------------------------------------------------------------------ */

void SceneSerializer::SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity, tinyxml2::XMLElement* pParentNode)
{
	PROFILE_FUNCTION();

	pElement->SetAttribute("Name", entity.GetName().c_str());
	pElement->SetAttribute("ID", entity.GetID());

	TransformComponent const& transformcomp = entity.GetTransform();

	tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Transform");
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Position"), transformcomp.position);
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Rotation"), transformcomp.rotation);
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Scale"), transformcomp.scale);

	if (entity.HasComponent<CameraComponent>())
	{
		CameraComponent const& component = entity.GetComponent<CameraComponent>();

		tinyxml2::XMLElement* pCameraElement = pElement->InsertNewChildElement("Camera");

		pCameraElement->SetAttribute("Primary", component.Primary);
		pCameraElement->SetAttribute("FixedAspectRatio", component.FixedAspectRatio);

		tinyxml2::XMLElement* pSceneCameraElement = pCameraElement->InsertNewChildElement("SceneCamera");

		pSceneCameraElement->SetAttribute("ProjectionType", (int)component.Camera.GetProjectionType());
		pSceneCameraElement->SetAttribute("OrthoSize", component.Camera.GetOrthoSize());
		pSceneCameraElement->SetAttribute("OrthoNear", component.Camera.GetOrthoNear());
		pSceneCameraElement->SetAttribute("OrthoFar", component.Camera.GetOrthoFar());
		pSceneCameraElement->SetAttribute("PerspectiveNear", component.Camera.GetPerspectiveNear());
		pSceneCameraElement->SetAttribute("PerspectiveFar", component.Camera.GetPerspectiveFar());
		pSceneCameraElement->SetAttribute("FOV", component.Camera.GetVerticalFov());
		pSceneCameraElement->SetAttribute("AspectRatio", component.Camera.GetAspectRatio());
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

		if (component.tileset)
			SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tileset"), component.tileset->GetFilepath());

		SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tint"), component.tint);

		pAnimatedSpriteElement->SetAttribute("CurrentAnimation", component.GetCurrentAnimationName().c_str());
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent const& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pStaticMeshElement = pElement->InsertNewChildElement("StaticMesh");

		if (component.mesh)
			SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("Mesh"), component.mesh->GetFilepath());

		if (component.material)
			SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("Material"), component.material->GetFilepath());
	}

	if (entity.HasComponent<PrimitiveComponent>())
	{
		PrimitiveComponent const& component = entity.GetComponent<PrimitiveComponent>();

		tinyxml2::XMLElement* pPrimitiveElement = pElement->InsertNewChildElement("Primitive");

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

		if(component.physicsMaterial)
			SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("PhysicsMaterial"), component.physicsMaterial->GetFilepath());

		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Offset"), component.offset);
		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Size"), component.size);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent const& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pCircleColliderElement = pElement->InsertNewChildElement("CircleCollider2D");

		pCircleColliderElement->SetAttribute("Density", component.density);
		pCircleColliderElement->SetAttribute("Friction", component.friction);
		pCircleColliderElement->SetAttribute("Restitution", component.restitution);

		pCircleColliderElement->SetAttribute("Radius", component.radius);
		SerializationUtils::Encode(pCircleColliderElement->InsertNewChildElement("Offset"), component.offset);
	}

	if (entity.HasComponent<PolygonCollider2DComponent>())
	{
		PolygonCollider2DComponent const& component = entity.GetComponent<PolygonCollider2DComponent>();

		tinyxml2::XMLElement* pPolygonColliderElement = pElement->InsertNewChildElement("PolygonCollider2D");

		pPolygonColliderElement->SetAttribute("Density", component.density);
		pPolygonColliderElement->SetAttribute("Friction", component.friction);
		pPolygonColliderElement->SetAttribute("Restitution", component.restitution);

		SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Offset"), component.offset);

		for (Vector2f const& vertex : component.vertices)
		{
			SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Vertex"), vertex);
		}
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

		SerializationUtils::Encode(pLuaScriptElement, component.absoluteFilepath);
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
		Uuid uuid(std::stoull(uuidChar));
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
		TransformComponent& transformComp = entity.GetTransform();
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Position"), transformComp.position);
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Rotation"), transformComp.rotation);
		SerializationUtils::Decode(pTransformComponentElement->FirstChildElement("Scale"), transformComp.scale);
	}

	// Camera -------------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pCameraComponentElement = pEntityElement->FirstChildElement("Camera"))
	{
		CameraComponent& component = entity.AddComponent<CameraComponent>();

		pCameraComponentElement->QueryBoolAttribute("Primary", &component.Primary);
		pCameraComponentElement->QueryBoolAttribute("FixedAspectRatio", &component.FixedAspectRatio);

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
			pSceneCameraElement->QueryFloatAttribute("AspectRatio", &sceneCamera.m_AspectRatio);

			sceneCamera.SetProjection(projectionType);
		}

		component.Camera = sceneCamera;
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

		if (tinyxml2::XMLElement const* pTilesetElement = pAnimatedSpriteComponentElement->FirstChildElement("Tileset"))
		{
			const char* tilesetChar = pTilesetElement->Attribute("Filepath");

			if (tilesetChar)
			{
				if (std::filesystem::path tilesetFilepath = SerializationUtils::AbsolutePath(tilesetChar);
					!tilesetFilepath.empty())
				{
					component.tileset = AssetManager::GetTileset(tilesetFilepath);
				}
			}
		}

		if (const char* currentAnimation = pAnimatedSpriteComponentElement->Attribute("CurrentAnimation"))
			component.SelectAnimation(currentAnimation);
	}

	// Static Mesh -------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pStaticMeshComponentElement = pEntityElement->FirstChildElement("StaticMesh"))
	{
		StaticMeshComponent& component = entity.AddComponent<StaticMeshComponent>();

		if (tinyxml2::XMLElement const* pMeshElement = pStaticMeshComponentElement->FirstChildElement("Mesh"))
		{
			const char* meshFilepathChar = pMeshElement->Attribute("Filepath");

			if (meshFilepathChar)
			{
				if (std::filesystem::path meshFilepath = SerializationUtils::AbsolutePath(meshFilepathChar);
					!meshFilepath.empty())
				{
					component.mesh = AssetManager::GetMesh(meshFilepath);
				}
			}
		}

		if (tinyxml2::XMLElement const* pMaterialElement = pStaticMeshComponentElement->FirstChildElement("Material"))
		{
			if (const char* materialFilepathChar = pMaterialElement->Attribute("Filepath"))
			{
				if (std::string materialFilepathStr(materialFilepathChar);
					!materialFilepathStr.empty())
				{
					std::filesystem::path materialFilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / materialFilepathStr);
					component.material = AssetManager::GetMaterial(materialFilepath);
				}
			}
		}
	}

	// Primitive -------------------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pPrimitiveComponentElement = pEntityElement->FirstChildElement("Primitive"))
	{
		PrimitiveComponent::Shape type =
			(PrimitiveComponent::Shape)pPrimitiveComponentElement->IntAttribute("Type", (int)PrimitiveComponent::Shape::Cube);

		switch (type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			if (tinyxml2::XMLElement const* pCubeElement = pPrimitiveComponentElement->FirstChildElement("Cube"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cube);

				pCubeElement->QueryFloatAttribute("Width", &component.cubeWidth);
				pCubeElement->QueryFloatAttribute("Height", &component.cubeHeight);
				pCubeElement->QueryFloatAttribute("Depth", &component.cubeDepth);
			}
			break;
		}
		case PrimitiveComponent::Shape::Sphere:
		{
			if (tinyxml2::XMLElement const* pSphereElement = pPrimitiveComponentElement->FirstChildElement("Sphere"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Sphere);

				pSphereElement->QueryFloatAttribute("Radius", &component.sphereRadius);
				pSphereElement->QueryUnsignedAttribute("LongitudeLines", &component.sphereLongitudeLines);
				pSphereElement->QueryUnsignedAttribute("LatitudeLines", &component.sphereLatitudeLines);
			}
			break;
		}
		case PrimitiveComponent::Shape::Plane:
		{
			if (tinyxml2::XMLElement const* pPlaneElement = pPrimitiveComponentElement->FirstChildElement("Plane"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Plane);

				pPlaneElement->QueryFloatAttribute("Width", &component.planeWidth);
				pPlaneElement->QueryFloatAttribute("Length", &component.planeLength);
				pPlaneElement->QueryUnsignedAttribute("WidthLines", &component.planeWidthLines);
				pPlaneElement->QueryUnsignedAttribute("LengthLines", &component.planeLengthLines);
				pPlaneElement->QueryFloatAttribute("TileU", &component.planeTileU);
				pPlaneElement->QueryFloatAttribute("TileV", &component.planeTileV);
			}
			break;
		}
		case PrimitiveComponent::Shape::Cylinder:
		{
			if (tinyxml2::XMLElement const* pCylinderElement = pPrimitiveComponentElement->FirstChildElement("Cylinder"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cylinder);

				pCylinderElement->QueryFloatAttribute("BottomRadius", &component.cylinderBottomRadius);
				pCylinderElement->QueryFloatAttribute("TopRadius", &component.cylinderTopRadius);
				pCylinderElement->QueryFloatAttribute("Height", &component.cylinderHeight);
				pCylinderElement->QueryUnsignedAttribute("SliceCount", &component.cylinderSliceCount);
				pCylinderElement->QueryUnsignedAttribute("StackCount", &component.cylinderStackCount);
			}
			break;
		}
		case PrimitiveComponent::Shape::Cone:
		{
			if (tinyxml2::XMLElement const* pConeElement = pPrimitiveComponentElement->FirstChildElement("Cone"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cone);

				pConeElement->QueryFloatAttribute("BottomRadius", &component.coneBottomRadius);
				pConeElement->QueryFloatAttribute("Height", &component.coneHeight);
				pConeElement->QueryUnsignedAttribute("SliceCount", &component.coneSliceCount);
				pConeElement->QueryUnsignedAttribute("StackCount", &component.coneStackCount);
			}
			break;
		}
		case PrimitiveComponent::Shape::Torus:
		{
			if (tinyxml2::XMLElement const* pTorusElement = pPrimitiveComponentElement->FirstChildElement("Torus"))
			{
				PrimitiveComponent& component = entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Torus);

				pTorusElement->QueryFloatAttribute("OuterRadius", &component.torusOuterRadius);
				pTorusElement->QueryFloatAttribute("InnerRadius", &component.torusInnerRadius);
				pTorusElement->QueryUnsignedAttribute("SliceCount", &component.torusSliceCount);
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
			std::filesystem::path tilesetfilepath = SerializationUtils::AbsolutePath(tilesetChar);
			if (!tilesetfilepath.empty())
			{
				component.tileset = AssetManager::GetTileset(tilesetfilepath);
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

		pTilemapComponentElement->QueryUnsignedAttribute("TilesWide", &component.tilesWide);
		pTilemapComponentElement->QueryUnsignedAttribute("TilesHigh", &component.tilesHigh);

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
			component.physicsMaterial = AssetManager::GetPhysicsMaterial(physicsMaterial);
		}
		else
			component.physicsMaterial = nullptr;

		SerializationUtils::Decode(pBoxColliderComponentElement->FirstChildElement("Offset"), component.offset);
		SerializationUtils::Decode(pBoxColliderComponentElement->FirstChildElement("Size"), component.size);
	}

	// CircleCollider2D -----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pCircleCollider2DComponentElement = pEntityElement->FirstChildElement("CircleCollider2D"))
	{
		CircleCollider2DComponent& component = entity.AddComponent<CircleCollider2DComponent>();

		pCircleCollider2DComponentElement->QueryFloatAttribute("Density", &component.density);
		pCircleCollider2DComponentElement->QueryFloatAttribute("Friction", &component.friction);
		pCircleCollider2DComponentElement->QueryFloatAttribute("Restitution", &component.restitution);

		SerializationUtils::Decode(pCircleCollider2DComponentElement->FirstChildElement("Offset"), component.offset);
	}

	// PolygonCollider2D --------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement* pPolygonCollider2DComponentElement = pEntityElement->FirstChildElement("PolygonCollider2D"))
	{
		PolygonCollider2DComponent& component = entity.AddComponent<PolygonCollider2DComponent>();

		pPolygonCollider2DComponentElement->QueryFloatAttribute("Density", &component.density);
		pPolygonCollider2DComponentElement->QueryFloatAttribute("Friction", &component.friction);
		pPolygonCollider2DComponentElement->QueryFloatAttribute("Restitution", &component.restitution);
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

	// Behaviour Tree ----------------------------------------------------------------------------------------------
	if (tinyxml2::XMLElement const* pBehaviourTreeComponentElement = pEntityElement->FirstChildElement("BehaviourTree"))
	{
		BehaviourTreeComponent& component = entity.AddComponent<BehaviourTreeComponent>();
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

		SerializationUtils::Decode(pLuaScriptComponentElement, component.absoluteFilepath);
	}

	// Hierarachy ---------------------------------------------------------------------------------------------------
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