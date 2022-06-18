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

	pRoot->SetAttribute("Name", m_Scene->GetSceneName().c_str());

	pRoot->SetAttribute("EngineVersion", VERSION);

	SerializationUtils::Encode(pRoot->InsertNewChildElement("Gravity"), m_Scene->GetGravity());

	doc.InsertFirstChild(pRoot);

	m_Scene->m_Registry.each([this, &pRoot](auto entityID)
		{
			Entity entity = { entityID, m_Scene };

			if (!entity)
				return;


			if (HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>(); 
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

		const char* sceneName = pRoot->Attribute("Name");

		if (sceneName)
			m_Scene->SetSceneName(sceneName);

		Vector2f gravity = m_Scene->GetGravity();
		SerializationUtils::Decode(pRoot->FirstChildElement("Gravity"), gravity);
		m_Scene->SetGravity(gravity);

		// Version
		const char* version = pRoot->Attribute("EngineVersion");

		if (version)
			if (atoi(version) != VERSION)
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

	TransformComponent& transformcomp = entity.GetTransform();

	tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Transform");
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Position"), transformcomp.position);
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Rotation"), transformcomp.rotation);
	SerializationUtils::Encode(pTransformElement->InsertNewChildElement("Scale"), transformcomp.scale);

	if (entity.HasComponent<CameraComponent>())
	{
		CameraComponent& component = entity.GetComponent<CameraComponent>();

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
		SpriteComponent& component = entity.GetComponent<SpriteComponent>();

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
		AnimatedSpriteComponent& component = entity.GetComponent<AnimatedSpriteComponent>();

		tinyxml2::XMLElement* pAnimatedSpriteElement = pElement->InsertNewChildElement("AnimatedSprite");

		if (component.tileset)
			SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tileset"), component.tileset->GetFilepath());

		SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tint"), component.tint);

		pAnimatedSpriteElement->SetAttribute("CurrentAnimation", component.GetCurrentAnimationName().c_str());
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pStaticMeshElement = pElement->InsertNewChildElement("StaticMesh");

		if(component.mesh)
			SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("Mesh"), component.mesh->GetFilepath());

		if(component.material)
			SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("Material"), component.material->GetFilepath());
	}

	if (entity.HasComponent<NativeScriptComponent>())
	{
		NativeScriptComponent& component = entity.GetComponent<NativeScriptComponent>();

		tinyxml2::XMLElement* pNativeScriptElement = pElement->InsertNewChildElement("NativeScript");

		pNativeScriptElement->SetAttribute("Name", component.Name.c_str());
	}

	if (entity.HasComponent<PrimitiveComponent>())
	{
		PrimitiveComponent& component = entity.GetComponent<PrimitiveComponent>();

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
				csv.seekp(-1, csv.cur);
				csv << "\0";
			}
			csv << std::endl;
		}

		pTilemapElement->SetText(csv.str().c_str());
	}

	if (entity.HasComponent<RigidBody2DComponent>())
	{
		RigidBody2DComponent& component = entity.GetComponent<RigidBody2DComponent>();

		tinyxml2::XMLElement* pRigidBody2DElement = pElement->InsertNewChildElement("RigidBody2D");

		pRigidBody2DElement->SetAttribute("BodyType", (int)component.type);
		pRigidBody2DElement->SetAttribute("FixedRotation", component.fixedRotation);
		pRigidBody2DElement->SetAttribute("GravityScale", component.gravityScale);
		pRigidBody2DElement->SetAttribute("AngularDamping", component.angularDamping);
		pRigidBody2DElement->SetAttribute("LinearDamping", component.linearDamping);
	}

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		BoxCollider2DComponent& component = entity.GetComponent<BoxCollider2DComponent>();

		tinyxml2::XMLElement* pBoxColliderElement = pElement->InsertNewChildElement("BoxCollider2D");

		pBoxColliderElement->SetAttribute("Density", component.density);
		pBoxColliderElement->SetAttribute("Friction", component.friction);
		pBoxColliderElement->SetAttribute("Restitution", component.restitution);

		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Offset"), component.offset);
		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Size"), component.Size);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pCircleColliderElement = pElement->InsertNewChildElement("CircleCollider2D");

		pCircleColliderElement->SetAttribute("Density", component.density);
		pCircleColliderElement->SetAttribute("Friction", component.friction);
		pCircleColliderElement->SetAttribute("Restitution", component.restitution);

		pCircleColliderElement->SetAttribute("Radius", component.radius);
		SerializationUtils::Encode(pCircleColliderElement->InsertNewChildElement("Offset"), component.offset);
	}

	if (entity.HasComponent<PolygonCollider2DComponent>())
	{
		PolygonCollider2DComponent& component = entity.GetComponent<PolygonCollider2DComponent>();

		tinyxml2::XMLElement* pPolygonColliderElement = pElement->InsertNewChildElement("PolygonCollider2D");

		pPolygonColliderElement->SetAttribute("Density", component.density);
		pPolygonColliderElement->SetAttribute("Friction", component.friction);
		pPolygonColliderElement->SetAttribute("Restitution", component.restitution);

		SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Offset"), component.offset);

		for (Vector2f& vertex : component.vertices)
		{
			SerializationUtils::Encode(pPolygonColliderElement->InsertNewChildElement("Vertex"), vertex);
		}
	}

	if (entity.HasComponent<CircleRendererComponent>())
	{
		CircleRendererComponent& component = entity.GetComponent<CircleRendererComponent>();

		tinyxml2::XMLElement* pCircleRendererElement = pElement->InsertNewChildElement("CircleRenderer");

		pCircleRendererElement->SetAttribute("Radius", component.radius);
		pCircleRendererElement->SetAttribute("Thickness", component.thickness);
		pCircleRendererElement->SetAttribute("Fade", component.fade);

		SerializationUtils::Encode(pCircleRendererElement->InsertNewChildElement("Colour"), component.colour);
	}

	if (entity.HasComponent<BehaviourTreeComponent>())
	{
		BehaviourTreeComponent& component = entity.GetComponent<BehaviourTreeComponent>();

		tinyxml2::XMLElement* pBehaviourTreeElement = pElement->InsertNewChildElement("BehaviourTree");
	}

	if (entity.HasComponent<StateMachineComponent>())
	{
		StateMachineComponent& component = entity.GetComponent<StateMachineComponent>();
		tinyxml2::XMLElement* pStateMachineElement = pElement->InsertNewChildElement("StateMachine");
	}

	if (entity.HasComponent<LuaScriptComponent>())
	{
		LuaScriptComponent& component = entity.GetComponent<LuaScriptComponent>();

		tinyxml2::XMLElement* pLuaScriptElement = pElement->InsertNewChildElement("LuaScript");

		SerializationUtils::Encode(pLuaScriptElement, component.absoluteFilepath);
	}

	if (entity.HasComponent<HierarchyComponent>())
	{
		HierarchyComponent& component = entity.GetComponent<HierarchyComponent>();

		if (component.firstChild != entt::null)
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
	const char* uuidChar = pEntityElement->Attribute("ID");

	if (uuidChar && entityName && !resetUuid)
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

	tinyxml2::XMLElement* pComponentElement;

	// Transform ----------------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("Transform");

	if (pComponentElement)
	{
		TransformComponent& transformComp = entity.GetTransform();
		SerializationUtils::Decode(pComponentElement->FirstChildElement("Position"), transformComp.position);
		SerializationUtils::Decode(pComponentElement->FirstChildElement("Rotation"), transformComp.rotation);
		SerializationUtils::Decode(pComponentElement->FirstChildElement("Scale"), transformComp.scale);
	}

	// Camera -------------------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("Camera");

	if (pComponentElement)
	{
		CameraComponent& component = entity.AddComponent<CameraComponent>();

		pComponentElement->QueryBoolAttribute("Primary", &component.Primary);
		pComponentElement->QueryBoolAttribute("FixedAspectRatio", &component.FixedAspectRatio);

		SceneCamera sceneCamera;
		tinyxml2::XMLElement* pSceneCameraElement = pComponentElement->FirstChildElement("SceneCamera");

		if (pSceneCameraElement)
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
	pComponentElement = pEntityElement->FirstChildElement("Sprite");

	if (pComponentElement)
	{
		SpriteComponent& component = entity.AddComponent<SpriteComponent>();

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Tint"), component.tint);
		pComponentElement->QueryFloatAttribute("Tilingfactor", &component.tilingFactor);

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Texture"), component.texture);
	}

	// Animated Sprite ---------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("AnimatedSprite");

	if (pComponentElement)
	{
		AnimatedSpriteComponent& component = entity.AddComponent<AnimatedSpriteComponent>();

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Tint"), component.tint);

		tinyxml2::XMLElement* pTilesetElement = pComponentElement->FirstChildElement("Tileset");

		if (pTilesetElement)
		{
			const char* tilesetChar = pTilesetElement->Attribute("Filepath");

			if (tilesetChar)
			{
				std::filesystem::path tilesetFilepath = SerializationUtils::AbsolutePath(tilesetChar);
				if (!tilesetFilepath.empty())
				{
					component.tileset = AssetManager::GetTileset(tilesetFilepath);
				}
			}
		}

		const char* currentAnimation = pComponentElement->Attribute("CurrentAnimation");
		if (currentAnimation)
			component.SelectAnimation(currentAnimation);
	}

	// Static Mesh -------------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("StaticMesh");

	if (pComponentElement)
	{
		StaticMeshComponent& component = entity.AddComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pMeshElement = pComponentElement->FirstChildElement("Mesh");

		if (pMeshElement)
		{
			const char* meshFilepathChar = pMeshElement->Attribute("Filepath");

			if (meshFilepathChar)
			{
				std::filesystem::path meshFilepath = SerializationUtils::AbsolutePath(meshFilepathChar);
				if (!meshFilepath.empty())
				{
					component.mesh = AssetManager::GetMesh(meshFilepath);
				}
			}
		}

		tinyxml2::XMLElement* pMaterialElement = pComponentElement->FirstChildElement("Material");

		if (pMaterialElement)
		{
			const char* materialFilepathChar = pMaterialElement->Attribute("Filepath");

			if (materialFilepathChar)
			{
				std::string materialFilepathStr(materialFilepathChar);

				if (!materialFilepathStr.empty())
				{
					std::filesystem::path materialFilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / materialFilepathStr);
					component.material = AssetManager::GetMaterial(materialFilepath);
				}
			}
		}
	}

	// Native Script -----------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("NativeScript");

	if (pComponentElement)
	{
		const char* nativeScriptName = pComponentElement->Attribute("Name");
		if (nativeScriptName)
			entity.AddComponent<NativeScriptComponent>(nativeScriptName);
	}

	// Primitive -------------------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("Primitive");

	if (pComponentElement)
	{
		PrimitiveComponent::Shape type =
			(PrimitiveComponent::Shape)pComponentElement->IntAttribute("Type", (int)PrimitiveComponent::Shape::Cube);

		switch (type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			tinyxml2::XMLElement* pCubeElement = pComponentElement->FirstChildElement("Cube");

			if (pCubeElement)
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
			tinyxml2::XMLElement* pSphereElement = pComponentElement->FirstChildElement("Sphere");

			if (pSphereElement)
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
			tinyxml2::XMLElement* pPlaneElement = pComponentElement->FirstChildElement("Plane");

			if (pPlaneElement)
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
			tinyxml2::XMLElement* pCylinderElement = pComponentElement->FirstChildElement("Cylinder");

			if (pCylinderElement)
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
			tinyxml2::XMLElement* pConeElement = pComponentElement->FirstChildElement("Cone");

			if (pConeElement)
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
			tinyxml2::XMLElement* pTorusElement = pComponentElement->FirstChildElement("Torus");

			if (pTorusElement)
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
	pComponentElement = pEntityElement->FirstChildElement("Tilemap");

	if (pComponentElement)
	{
		TilemapComponent& component = entity.AddComponent<TilemapComponent>();
		const char* tilesetChar = pComponentElement->Attribute("Filepath");

		if (tilesetChar)
		{
			std::filesystem::path tilesetfilepath = SerializationUtils::AbsolutePath(tilesetChar);
			if (!tilesetfilepath.empty())
			{
				component.tileset = AssetManager::GetTileset(tilesetfilepath);
			}
		}

		const char* orientationchar = pComponentElement->Attribute("Orientation");

		if (orientationchar != nullptr)
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

		pComponentElement->QueryUnsignedAttribute("TilesWide", &component.tilesWide);
		pComponentElement->QueryUnsignedAttribute("TilesHigh", &component.tilesHigh);

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Tint"), component.tint);

		const char* text = pComponentElement->GetText();

		if (text)
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

	// RigidBody2D -----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("RigidBody2D");

	if (pComponentElement)
	{
		RigidBody2DComponent& component = entity.AddComponent<RigidBody2DComponent>();

		component.type = (RigidBody2DComponent::BodyType)pComponentElement->IntAttribute("BodyType", (int)component.type);
		pComponentElement->QueryBoolAttribute("FixedRotation", &component.fixedRotation);
		pComponentElement->QueryFloatAttribute("GravityScale", &component.gravityScale);
		pComponentElement->QueryFloatAttribute("AngularDamping", &component.angularDamping);
		pComponentElement->QueryFloatAttribute("LinearDamping", &component.linearDamping);
	}

	// BoxCollider2D -----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("BoxCollider2D");

	if (pComponentElement)
	{
		BoxCollider2DComponent& component = entity.AddComponent<BoxCollider2DComponent>();

		pComponentElement->QueryFloatAttribute("Density", &component.density);
		pComponentElement->QueryFloatAttribute("Friction", &component.friction);
		pComponentElement->QueryFloatAttribute("Restitution", &component.restitution);

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Offset"), component.offset);
		SerializationUtils::Decode(pComponentElement->FirstChildElement("Size"), component.Size);
	}

	// CircleCollider2D -----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("CircleCollider2D");

	if (pComponentElement)
	{
		CircleCollider2DComponent& component = entity.AddComponent<CircleCollider2DComponent>();

		pComponentElement->QueryFloatAttribute("Density", &component.density);
		pComponentElement->QueryFloatAttribute("Friction", &component.friction);
		pComponentElement->QueryFloatAttribute("Restitution", &component.restitution);

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Offset"), component.offset);
	}

	// PolygonCollider2D --------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("PolygonCollider2D");

	if (pComponentElement)
	{
		PolygonCollider2DComponent& component = entity.AddComponent<PolygonCollider2DComponent>();

		pComponentElement->QueryFloatAttribute("Density", &component.density);
		pComponentElement->QueryFloatAttribute("Friction", &component.friction);
		pComponentElement->QueryFloatAttribute("Restitution", &component.restitution);
		SerializationUtils::Decode(pComponentElement->FirstChildElement("Offset"), component.offset);

		tinyxml2::XMLElement* pVertexElement = pComponentElement->FirstChildElement("Vertex");

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
	pComponentElement = pEntityElement->FirstChildElement("CircleRenderer");

	if (pComponentElement)
	{
		CircleRendererComponent& component = entity.AddComponent<CircleRendererComponent>();

		pComponentElement->QueryFloatAttribute("Radius", &component.radius);
		pComponentElement->QueryFloatAttribute("Thickness", &component.thickness);
		pComponentElement->QueryFloatAttribute("Fade", &component.fade);

		SerializationUtils::Decode(pComponentElement->FirstChildElement("Colour"), component.colour);
	}

	// Behaviour Tree ----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("BehaviourTree");

	if (pComponentElement)
	{
		BehaviourTreeComponent& component = entity.AddComponent<BehaviourTreeComponent>();
	}

	// State Machine ----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("StateMachine");

	if (pComponentElement)
	{
		StateMachineComponent& component = entity.AddComponent<StateMachineComponent>();
	}

	// LuaScript -----------------------------------------------------------------------------------------------
	pComponentElement = pEntityElement->FirstChildElement("LuaScript");

	if (pComponentElement)
	{
		LuaScriptComponent& component = entity.AddComponent<LuaScriptComponent>();

		SerializationUtils::Decode(pComponentElement, component.absoluteFilepath);
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
