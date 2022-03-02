#include "stdafx.h"
#include "SceneSerializer.h"

#include "TinyXml2/tinyxml2.h"
#include "Scene/Entity.h"
#include "Components/Components.h"
#include "Core/Application.h"
#include "Core/Version.h"
#include "Utilities/SerializationUtils.h"

/* ------------------------------------------------------------------------------------------------------------------ */

SceneSerializer::SceneSerializer(Scene* scene)
	:m_Scene(scene)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneSerializer::Serialize(const std::filesystem::path& filepath) const
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Scene");

	pRoot->SetAttribute("Name", m_Scene->GetSceneName().c_str());

	pRoot->SetAttribute("EngineVersion", VERSION);

	doc.InsertFirstChild(pRoot);

	m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene };

			if (!entity)
				return;

			SerializeEntity(pRoot->InsertNewChildElement("Entity"), entity);
			return;
		});

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
{
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

		// Version
		const char* version = pRoot->Attribute("EngineVersion");

		if (version)
			if (atoi(version) != VERSION)
				ENGINE_WARN("Loading scene created with a different version of the engine");

		// Entities
		tinyxml2::XMLElement* pEntityElement = pRoot->FirstChildElement("Entity");



		while (pEntityElement)
		{
			DeserializeEntity(m_Scene, pEntityElement);

			pEntityElement = pEntityElement->NextSiblingElement("Entity");
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

void SceneSerializer::SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity)
{
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
			pSpriteElement->SetAttribute("Texture", SerializationUtils::RelativePath(component.texture->GetFilepath()).c_str());

		SerializationUtils::Encode(pSpriteElement->InsertNewChildElement("Tint"), component.tint);
	}

	if (entity.HasComponent<AnimatedSpriteComponent>())
	{
		AnimatedSpriteComponent& component = entity.GetComponent<AnimatedSpriteComponent>();

		tinyxml2::XMLElement* pAnimatedSpriteElement = pElement->InsertNewChildElement("AnimatedSprite");
		tinyxml2::XMLElement* pAnimatorElement = pAnimatedSpriteElement->InsertNewChildElement("Animator");

		if (component.animator.GetSpriteSheet()->GetTexture())
			pAnimatorElement->SetAttribute("Texture",
				SerializationUtils::RelativePath(component.animator.GetSpriteSheet()->GetTexture()->GetFilepath()).c_str());

		pAnimatorElement->SetAttribute("SpriteWidth", component.animator.GetSpriteSheet()->GetSpriteWidth());
		pAnimatorElement->SetAttribute("SpriteHeight", component.animator.GetSpriteSheet()->GetSpriteHeight());

		for (Animation& animation : component.animator.GetAnimations())
		{
			tinyxml2::XMLElement* pAnimationElement = pAnimatorElement->InsertNewChildElement("Animation");
			pAnimationElement->SetAttribute("Name", animation.GetName().c_str());
			pAnimationElement->SetAttribute("StartFrame", animation.GetStartFrame());
			pAnimationElement->SetAttribute("FrameCount", animation.GetFrameCount());
			pAnimationElement->SetAttribute("FrameTime", animation.GetFrameTime());
		}

		SerializationUtils::Encode(pAnimatedSpriteElement->InsertNewChildElement("Tint"), component.tint);
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pStaticMeshElement = pElement->InsertNewChildElement("StaticMesh");
		std::filesystem::path relativepath;
		if (!component.mesh.GetFilepath().empty())
		{
			relativepath = FileUtils::RelativePath(component.mesh.GetFilepath(), Application::GetOpenDocumentDirectory());
		}
		pStaticMeshElement->InsertNewChildElement("Mesh")->SetAttribute("Filepath", relativepath.string().c_str());

		SerializationUtils::Encode(pStaticMeshElement->InsertNewChildElement("Material"), component.material);
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

		std::string relativePath = FileUtils::RelativePath(component.tilemap.GetFilepath(), Application::GetOpenDocumentDirectory()).string();

		pTilemapElement->SetAttribute("Filepath", relativePath.c_str());

		component.tilemap.Save();
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

		pBoxColliderElement->SetAttribute("Density", component.Density);
		pBoxColliderElement->SetAttribute("Friction", component.Friction);
		pBoxColliderElement->SetAttribute("Restitution", component.Restitution);

		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Offset"), component.Offset);
		SerializationUtils::Encode(pBoxColliderElement->InsertNewChildElement("Size"), component.Size);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pCircleColliderElement = pElement->InsertNewChildElement("CircleCollider2D");

		pCircleColliderElement->SetAttribute("Density", component.Density);
		pCircleColliderElement->SetAttribute("Friction", component.Friction);
		pCircleColliderElement->SetAttribute("Restitution", component.Restitution);

		pCircleColliderElement->SetAttribute("Radius", component.Radius);
		SerializationUtils::Encode(pCircleColliderElement->InsertNewChildElement("Offset"), component.Offset);
	}

	if (entity.HasComponent<CircleRendererComponent>())
	{
		CircleRendererComponent& component = entity.GetComponent<CircleRendererComponent>();

		tinyxml2::XMLElement* pCircleRendererElement = pElement->InsertNewChildElement("CircleRenderer");

		pCircleRendererElement->SetAttribute("Radius", component.Radius);
		pCircleRendererElement->SetAttribute("Thickness", component.Thickness);
		pCircleRendererElement->SetAttribute("Fade", component.Fade);

		SerializationUtils::Encode(pCircleRendererElement->InsertNewChildElement("Colour"), component.colour);
	}
}

Entity SceneSerializer::DeserializeEntity(Scene* scene, tinyxml2::XMLElement* pEntityElement)
{
	Entity entity;
	if (!pEntityElement)
		return entity;
	const char* entityName = pEntityElement->Attribute("Name");
	const char* uuidChar = pEntityElement->Attribute("ID");

	if (uuidChar && entityName)
	{
		Uuid uuid(std::stoull(uuidChar));
		entity = scene->CreateEntity(uuid, entityName);
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
	tinyxml2::XMLElement* pTransformElement = pEntityElement->FirstChildElement("Transform");

	if (pTransformElement)
	{
		TransformComponent& transformComp = entity.GetTransform();
		SerializationUtils::Decode(pTransformElement->FirstChildElement("Position"), transformComp.position);
		SerializationUtils::Decode(pTransformElement->FirstChildElement("Rotation"), transformComp.rotation);
		SerializationUtils::Decode(pTransformElement->FirstChildElement("Scale"), transformComp.scale);
	}

	// Camera -------------------------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pCamera = pEntityElement->FirstChildElement("Camera");

	if (pCamera)
	{
		CameraComponent& component = entity.AddComponent<CameraComponent>();

		pCamera->QueryBoolAttribute("Primary", &component.Primary);
		pCamera->QueryBoolAttribute("FixedAspectRatio", &component.FixedAspectRatio);

		SceneCamera sceneCamera;
		tinyxml2::XMLElement* pSceneCameraElement = pCamera->FirstChildElement("SceneCamera");

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
	tinyxml2::XMLElement* pSpriteElement = pEntityElement->FirstChildElement("Sprite");

	if (pSpriteElement)
	{
		SpriteComponent& component = entity.AddComponent<SpriteComponent>();

		SerializationUtils::Decode(pSpriteElement->FirstChildElement("Tint"), component.tint);
		pSpriteElement->QueryFloatAttribute("Tilingfactor", &component.tilingFactor);

		const char* textureRelativePath = pSpriteElement->Attribute("Texture");
		if (textureRelativePath != nullptr)
			component.texture = Texture2D::Create(SerializationUtils::AbsolutePath(textureRelativePath));
	}

	// Static Mesh -------------------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pStaticMeshElement = pEntityElement->FirstChildElement("StaticMesh");

	if (pStaticMeshElement)
	{
		tinyxml2::XMLElement* pMeshElement = pStaticMeshElement->FirstChildElement("Mesh");
		Mesh mesh;
		Material material;

		if (pMeshElement)
		{
			const char* meshFilepathChar = pMeshElement->Attribute("Filepath");

			if (meshFilepathChar)
			{
				std::string meshFilepathStr(meshFilepathChar);
				if (!meshFilepathStr.empty())
				{
					std::filesystem::path meshfilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / meshFilepathStr);
					mesh.LoadModel(meshfilepath);
				}
			}
		}

		SerializationUtils::Decode(pStaticMeshElement->FirstChildElement("Material"), material);

		entity.AddComponent<StaticMeshComponent>(mesh, material);
	}

	// Native Script -----------------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pNativeScriptElement = pEntityElement->FirstChildElement("NativeScript");

	if (pNativeScriptElement)
	{
		const char* nativeScriptName = pNativeScriptElement->Attribute("Name");
		if (nativeScriptName)
			entity.AddComponent<NativeScriptComponent>(nativeScriptName);
	}

	// Primitive -------------------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pPrimitiveElement = pEntityElement->FirstChildElement("Primitive");

	if (pPrimitiveElement)
	{
		PrimitiveComponent::Shape type =
			(PrimitiveComponent::Shape)pPrimitiveElement->IntAttribute("Type", (int)PrimitiveComponent::Shape::Cube);

		switch (type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			tinyxml2::XMLElement* pCubeElement = pPrimitiveElement->FirstChildElement("Cube");

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
			tinyxml2::XMLElement* pSphereElement = pPrimitiveElement->FirstChildElement("Sphere");

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
			tinyxml2::XMLElement* pPlaneElement = pPrimitiveElement->FirstChildElement("Plane");

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
			tinyxml2::XMLElement* pCylinderElement = pPrimitiveElement->FirstChildElement("Cylinder");

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
			tinyxml2::XMLElement* pConeElement = pPrimitiveElement->FirstChildElement("Cone");

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
			tinyxml2::XMLElement* pTorusElement = pPrimitiveElement->FirstChildElement("Torus");

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
	tinyxml2::XMLElement* pTilemapElement = pEntityElement->FirstChildElement("Tilemap");

	if (pTilemapElement)
	{
		Tilemap tilemap;
		const char* tilemapChar = pTilemapElement->Attribute("Filepath");
		if (tilemapChar)
		{
			std::string tilemapPath(tilemapChar);
			if (!tilemapPath.empty())
			{
				std::filesystem::path tilemapfilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / tilemapPath);
				tilemap.Load(tilemapfilepath);
			}
		}
		entity.AddComponent<TilemapComponent>(tilemap);
	}

	// RigidBody2D -----------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pRigidBody2DElement = pEntityElement->FirstChildElement("RigidBody2D");

	if (pRigidBody2DElement)
	{
		RigidBody2DComponent& component = entity.AddComponent<RigidBody2DComponent>();

		component.type = (RigidBody2DComponent::BodyType)pRigidBody2DElement->IntAttribute("BodyType", (int)component.type);
		pRigidBody2DElement->QueryBoolAttribute("FixedRotation", &component.fixedRotation);
		pRigidBody2DElement->QueryFloatAttribute("GravityScale", &component.gravityScale);
		pRigidBody2DElement->QueryFloatAttribute("AngularDamping", &component.angularDamping);
		pRigidBody2DElement->QueryFloatAttribute("LinearDamping", &component.linearDamping);
	}

	// BoxCollider2D -----------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pBoxCollider2DElement = pEntityElement->FirstChildElement("BoxCollider2D");

	if (pBoxCollider2DElement)
	{
		BoxCollider2DComponent& component = entity.AddComponent<BoxCollider2DComponent>();

		pBoxCollider2DElement->QueryFloatAttribute("Density", &component.Density);
		pBoxCollider2DElement->QueryFloatAttribute("Friction", &component.Friction);
		pBoxCollider2DElement->QueryFloatAttribute("Restitution", &component.Restitution);

		SerializationUtils::Decode(pBoxCollider2DElement->FirstChildElement("Offset"), component.Offset);
		SerializationUtils::Decode(pBoxCollider2DElement->FirstChildElement("Size"), component.Size);
	}

	// CircleCollider2D -----------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pCircleCollider2DElement = pEntityElement->FirstChildElement("CircleCollider2D");

	if (pCircleCollider2DElement)
	{
		CircleCollider2DComponent& component = entity.AddComponent<CircleCollider2DComponent>();

		pCircleCollider2DElement->QueryFloatAttribute("Density", &component.Density);
		pCircleCollider2DElement->QueryFloatAttribute("Friction", &component.Friction);
		pCircleCollider2DElement->QueryFloatAttribute("Restitution", &component.Restitution);

		SerializationUtils::Decode(pCircleCollider2DElement->FirstChildElement("Offset"), component.Offset);
	}

	// CircleRenderer -----------------------------------------------------------------------------------------------
	tinyxml2::XMLElement* pCircleRendererElement = pEntityElement->FirstChildElement("CircleRenderer");

	if (pCircleRendererElement)
	{
		CircleRendererComponent& component = entity.AddComponent<CircleRendererComponent>();

		pCircleRendererElement->QueryFloatAttribute("Radius", &component.Radius);
		pCircleRendererElement->QueryFloatAttribute("Thickness", &component.Thickness);
		pCircleRendererElement->QueryFloatAttribute("Fade", &component.Fade);

		SerializationUtils::Decode(pCircleRendererElement->FirstChildElement("Colour"), component.colour);
	}

	return entity;
}
