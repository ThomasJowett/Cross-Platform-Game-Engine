#include "stdafx.h"
#include "SceneSerializer.h"

#include "TinyXml2/tinyxml2.h"
#include "Scene/Entity.h"
#include "Components/Components.h"
#include "Core/Application.h"

void Encode(tinyxml2::XMLElement* pElement, const Vector2f& vec2)
{
	pElement->SetAttribute("X", vec2.x);
	pElement->SetAttribute("Y", vec2.y);
}

void Decode(tinyxml2::XMLElement* pElement, Vector2f& vec2)
{
	if (pElement)
	{
		const char* xChar = pElement->Attribute("X");
		const char* yChar = pElement->Attribute("Y");
		if (xChar) vec2.x = atof(xChar);
		if (yChar) vec2.y = atof(yChar);
	}
}

void Encode(tinyxml2::XMLElement* pElement, const Vector3f& vec3)
{
	pElement->SetAttribute("X", vec3.x);
	pElement->SetAttribute("Y", vec3.y);
	pElement->SetAttribute("Z", vec3.z);
}

void Decode(tinyxml2::XMLElement* pElement, Vector3f& vec3)
{
	if (pElement)
	{
		const char* xChar = pElement->Attribute("X");
		const char* yChar = pElement->Attribute("Y");
		const char* zChar = pElement->Attribute("Z");
		if (xChar) vec3.x = atof(xChar);
		if (yChar) vec3.y = atof(yChar);
		if (zChar) vec3.y = atof(zChar);
	}
}

void Encode(tinyxml2::XMLElement* pElement, const Colour& colour)
{
	pElement->SetAttribute("R", colour.r);
	pElement->SetAttribute("G", colour.g);
	pElement->SetAttribute("B", colour.b);
	pElement->SetAttribute("A", colour.a);

}

void Decode(tinyxml2::XMLElement* pElement, Colour& colour)
{
	if (pElement)
	{
		const char* rChar = pElement->Attribute("R");
		const char* gChar = pElement->Attribute("G");
		const char* bChar = pElement->Attribute("B");
		const char* aChar = pElement->Attribute("A");
		colour.r = atof(rChar);
		colour.g = atof(gChar);
		colour.b = atof(bChar);
		colour.a = atof(aChar);
	}
}

void Encode(tinyxml2::XMLElement* pElement, const Material& material)
{
	std::string relativePath = FileUtils::relativePath(material.GetFilepath(), Application::GetOpenDocumentDirectory()).string();
	pElement->SetAttribute("Filepath", relativePath.c_str());
	material.SaveMaterial();
}

SceneSerializer::SceneSerializer(Scene* scene)
	:m_Scene(scene)
{
}

bool SceneSerializer::Serialize(const std::filesystem::path& filepath) const
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Scene");

	pRoot->SetAttribute("Name", m_Scene->GetSceneName().c_str());

	doc.InsertFirstChild(pRoot);


	m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene };

			if (!entity)
				return;

			SerializeEntity(pRoot->InsertNewChildElement("Entity"), entity);
		});

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		m_Scene->SetFilepath(filepath);

		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");

		const char* sceneName = pRoot->Attribute("Name");

		m_Scene->SetSceneName(sceneName);

		//Entities
		tinyxml2::XMLElement* pEntityElement = pRoot->FirstChildElement("Entity");

		while (pEntityElement)
		{
			Entity entity;
			const char* entityName = pEntityElement->Attribute("Name");
			const char* uuidChar = pEntityElement->Attribute("ID");

			if (uuidChar != nullptr)
			{
				Uuid uuid(std::stoull(uuidChar));
				entity = m_Scene->CreateEntity(uuid, entityName);
			}
			else
			{
				entity = m_Scene->CreateEntity(entityName);
			}

			// Transform ----------------------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pTransformElement = pEntityElement->FirstChildElement("Transform");

			if (pTransformElement)
			{
				TransformComponent& transformComp = entity.GetTransform();
				Decode(pTransformElement->FirstChildElement("Position"), transformComp.Position);
				Decode(pTransformElement->FirstChildElement("Rotation"), transformComp.Rotation);
				Decode(pTransformElement->FirstChildElement("Scale"), transformComp.Scale);
			}

			// Camera -------------------------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pCamera = pEntityElement->FirstChildElement("Camera");

			if (pCamera)
			{
				const char* primaryChar = pCamera->Attribute("Primary");
				bool isPrimary = !strcmp(primaryChar, "true");

				const char* fixedAspectRatioChar = pCamera->Attribute("FixedAspectRatio");
				bool isFixedAspectRatio = !strcmp(fixedAspectRatioChar, "true");

				SceneCamera sceneCamera;
				tinyxml2::XMLElement* pSceneCameraElement = pCamera->FirstChildElement("SceneCamera");

				if (pSceneCameraElement)
				{
					sceneCamera.SetProjection((SceneCamera::ProjectionType)atoi(pSceneCameraElement->Attribute("ProjectionType")));
					sceneCamera.SetOrthoSize(atof(pSceneCameraElement->Attribute("OrthoSize")));
					sceneCamera.SetOrthoNear(atof(pSceneCameraElement->Attribute("OrthoNear")));
					sceneCamera.SetOrthoFar(atof(pSceneCameraElement->Attribute("OrthoFar")));
					sceneCamera.SetPerspectiveNear(atof(pSceneCameraElement->Attribute("PerspectiveNear")));
					sceneCamera.SetPerspectiveFar(atof(pSceneCameraElement->Attribute("PerspectiveFar")));
					sceneCamera.SetVerticalFov(atof(pSceneCameraElement->Attribute("FOV")));
					sceneCamera.SetAspectRatio(atof(pSceneCameraElement->Attribute("AspectRatio")));
				}

				entity.AddComponent<CameraComponent>(sceneCamera, isPrimary, isFixedAspectRatio);
			}

			// Sprite -------------------------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pSpriteElement = pEntityElement->FirstChildElement("Sprite");

			if (pSpriteElement)
			{
				Colour tint;
				Decode(pSpriteElement->FirstChildElement("Tint"), tint);
				entity.AddComponent<SpriteComponent>(tint, atof(pSpriteElement->Attribute("TilingFactor")));
				//TODO: load material
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
					std::string meshFilepathStr(meshFilepathChar);
					if (!meshFilepathStr.empty())
					{
						std::filesystem::path meshfilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / meshFilepathStr);
						mesh.LoadModel(meshfilepath);
					}
				}

				tinyxml2::XMLElement* pMaterialElement = pStaticMeshElement->FirstChildElement("Material");

				if (pMaterialElement)
				{
					const char* materialFilepathChar = pMeshElement->Attribute("Filepath");
					std::string materialFilepathStr(materialFilepathChar);

					if (!materialFilepathStr.empty())
					{
						std::filesystem::path materailfilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / materialFilepathStr);
						material.LoadMaterial(materailfilepath);
					}
				}
				entity.AddComponent<StaticMeshComponent>(mesh, material);
			}

			// Native Script -----------------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pNativeScriptElement = pEntityElement->FirstChildElement("NativeScript");

			if (pNativeScriptElement)
			{
				const char* nativeScriptName = pNativeScriptElement->Attribute("Name");
				entity.AddComponent<NativeScriptComponent>(nativeScriptName);
			}

			// Primitive -------------------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pPrimitiveElement = pEntityElement->FirstChildElement("Primitive");

			if (pPrimitiveElement)
			{
				PrimitiveComponent::Shape type = (PrimitiveComponent::Shape)atoi(pPrimitiveElement->Attribute("Type"));

				switch (type)
				{
				case PrimitiveComponent::Shape::Cube:
				{
					tinyxml2::XMLElement* pCubeElement = pPrimitiveElement->FirstChildElement("Cube");

					if (pCubeElement)
					{
						entity.AddComponent<PrimitiveComponent>(
							(float)atof(pCubeElement->Attribute("Width")),
							(float)atof(pCubeElement->Attribute("Height")),
							(float)atof(pCubeElement->Attribute("Depth")));
					}
					break;
				}
				case PrimitiveComponent::Shape::Sphere:
				{
					tinyxml2::XMLElement* pSphereElement = pPrimitiveElement->FirstChildElement("Sphere");

					if (pSphereElement)
					{
						entity.AddComponent<PrimitiveComponent>(
							(float)atof(pSphereElement->Attribute("Radius")),
							(uint32_t)atoi(pSphereElement->Attribute("LongitudeLines")),
							(uint32_t)atoi(pSphereElement->Attribute("LatitudeLines")));
					}
					break;
				}
				case PrimitiveComponent::Shape::Plane:
				{
					tinyxml2::XMLElement* pPlaneElement = pPrimitiveElement->FirstChildElement("Plane");

					if (pPlaneElement)
					{
						entity.AddComponent<PrimitiveComponent>(
							(float)atof(pPlaneElement->Attribute("Width")),
							(float)atof(pPlaneElement->Attribute("Length")),
							(uint32_t)atoi(pPlaneElement->Attribute("WidthLines")),
							(uint32_t)atoi(pPlaneElement->Attribute("LengthLines")),
							(float)atof(pPlaneElement->Attribute("TileU")),
							(float)atof(pPlaneElement->Attribute("TileV")));
					}
					break;
				}
				case PrimitiveComponent::Shape::Cylinder:
				{
					tinyxml2::XMLElement* pCylinderElement = pPrimitiveElement->FirstChildElement("Cylinder");

					if (pCylinderElement)
					{
						entity.AddComponent<PrimitiveComponent>(
							(float)atof(pCylinderElement->Attribute("BottomRadius")),
							(float)atof(pCylinderElement->Attribute("TopRadius")),
							(float)atof(pCylinderElement->Attribute("Height")),
							(uint32_t)atoi(pCylinderElement->Attribute("SliceCount")),
							(uint32_t)atoi(pCylinderElement->Attribute("StackCount")));
					}
					break;
				}
				case PrimitiveComponent::Shape::Cone:
				{
					tinyxml2::XMLElement* pConeElement = pPrimitiveElement->FirstChildElement("Cone");

					if (pConeElement)
					{
						entity.AddComponent<PrimitiveComponent>(
							(float)atof(pConeElement->Attribute("BottomRadius")),
							(float)atof(pConeElement->Attribute("Height")),
							(uint32_t)atoi(pConeElement->Attribute("SliceCount")),
							(uint32_t)atoi(pConeElement->Attribute("StackCount")));
					}
					break;
				}
				case PrimitiveComponent::Shape::Torus:
				{
					tinyxml2::XMLElement* pTorusElement = pPrimitiveElement->FirstChildElement("Torus");

					if (pTorusElement)
					{
						float outerRadius = 1.0f, innerRadius = 0.4f;
						uint32_t sliceCount = 32;

						const char* outerRadiusChar = pTorusElement->Attribute("OuterRadius");
						const char* innerRadiusChar = pTorusElement->Attribute("InnerRadius");
						const char* sliceCountChar = pTorusElement->Attribute("SliceCount");

						if (outerRadiusChar) outerRadius = atof(outerRadiusChar);
						if (innerRadiusChar) innerRadius = atof(innerRadiusChar);
						if (sliceCountChar) sliceCount = atoi(sliceCountChar);

						entity.AddComponent<PrimitiveComponent>(outerRadius, innerRadius, sliceCount);
					}
				}
				break;
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
				//TODO
				entity.AddComponent<RigidBody2DComponent>();
			}

			// BoxCollider2D -----------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pBoxCollider2DElement = pEntityElement->FirstChildElement("BoxCollider2D");

			if (pBoxCollider2DElement)
			{
				//TODO
				entity.AddComponent<BoxCollider2DComponent>();
			}

			// CircleCollider2D -----------------------------------------------------------------------------------------------
			tinyxml2::XMLElement* pCircleCollider2DElement = pEntityElement->FirstChildElement("CircleCollider2D");

			if (pCircleCollider2DElement)
			{
				//TODO
				entity.AddComponent<CircleCollider2DComponent>();
			}

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

void SceneSerializer::SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity) const
{
	pElement->SetAttribute("Name", entity.GetTag().Tag.c_str());
	pElement->SetAttribute("ID", entity.GetID().ID);

	TransformComponent& transformcomp = entity.GetTransform();

	tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Transform");
	Encode(pTransformElement->InsertNewChildElement("Position"), transformcomp.Position);
	Encode(pTransformElement->InsertNewChildElement("Rotation"), transformcomp.Rotation);
	Encode(pTransformElement->InsertNewChildElement("Scale"), transformcomp.Scale);

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
		pSpriteElement->SetAttribute("TilingFactor", component.TilingFactor);

		Encode(pSpriteElement->InsertNewChildElement("Tint"), component.Tint);
		Encode(pSpriteElement->InsertNewChildElement("Material"), component.material);
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pStaticMeshElement = pElement->InsertNewChildElement("StaticMesh");
		std::filesystem::path relativepath;
		if (!component.Geometry.GetFilepath().empty())
		{
			relativepath = FileUtils::relativePath(component.Geometry.GetFilepath(), Application::GetOpenDocumentDirectory());
		}
		pStaticMeshElement->InsertNewChildElement("Mesh")->SetAttribute("Filepath", relativepath.string().c_str());

		Encode(pStaticMeshElement->InsertNewChildElement("Material"), component.material);
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

		pPrimitiveElement->SetAttribute("Type", (int)component.Type);

		switch (component.Type)
		{
		case PrimitiveComponent::Shape::Cube:
		{
			tinyxml2::XMLElement* pCubeElement = pPrimitiveElement->InsertNewChildElement("Cube");

			pCubeElement->SetAttribute("Width", component.CubeWidth);
			pCubeElement->SetAttribute("Height", component.CubeHeight);
			pCubeElement->SetAttribute("Depth", component.CubeDepth);
			break;
		}
		case PrimitiveComponent::Shape::Sphere:
		{
			tinyxml2::XMLElement* pSphereElement = pPrimitiveElement->InsertNewChildElement("Sphere");

			pSphereElement->SetAttribute("Radius", component.SphereRadius);
			pSphereElement->SetAttribute("LongitudeLines", component.SphereLongitudeLines);
			pSphereElement->SetAttribute("LatitudeLines", component.SphereLatitudeLines);
			break;
		}
		case PrimitiveComponent::Shape::Plane:
		{
			tinyxml2::XMLElement* pPlaneElement = pPrimitiveElement->InsertNewChildElement("Plane");

			pPlaneElement->SetAttribute("Width", component.PlaneWidth);
			pPlaneElement->SetAttribute("Length", component.PlaneLength);
			pPlaneElement->SetAttribute("WidthLines", component.PlaneWidthLines);
			pPlaneElement->SetAttribute("LengthLines", component.PlaneLengthLines);
			pPlaneElement->SetAttribute("TileU", component.PlaneTileU);
			pPlaneElement->SetAttribute("TileV", component.PlaneTileV);
			break;
		}
		case PrimitiveComponent::Shape::Cylinder:
		{
			tinyxml2::XMLElement* pCylinderElement = pPrimitiveElement->InsertNewChildElement("Cylinder");

			pCylinderElement->SetAttribute("BottomRadius", component.CylinderBottomRadius);
			pCylinderElement->SetAttribute("TopRadius", component.CylinderTopRadius);
			pCylinderElement->SetAttribute("Height", component.CylinderHeight);
			pCylinderElement->SetAttribute("SliceCount", component.CylinderSliceCount);
			pCylinderElement->SetAttribute("StackCount", component.CylinderStackCount);
			break;
		}
		case PrimitiveComponent::Shape::Cone:
		{
			tinyxml2::XMLElement* pConeElement = pPrimitiveElement->InsertNewChildElement("Cone");

			pConeElement->SetAttribute("BottomRadius", component.ConeBottomRadius);
			pConeElement->SetAttribute("Height", component.ConeHeight);
			pConeElement->SetAttribute("SliceCount", component.ConeSliceCount);
			pConeElement->SetAttribute("StackCount", component.ConeStackCount);
			break;
		}
		case PrimitiveComponent::Shape::Torus:
		{
			tinyxml2::XMLElement* pTorusElement = pPrimitiveElement->InsertNewChildElement("Torus");

			pTorusElement->SetAttribute("OuterRadius", component.TorusOuterRadius);
			pTorusElement->SetAttribute("InnerRadius", component.TorusInnerRadius);
			pTorusElement->SetAttribute("SliceCount", component.TorusSliceCount);
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

		std::string relativePath = FileUtils::relativePath(component.tilemap.GetFilepath(), Application::GetOpenDocumentDirectory()).string();

		pTilemapElement->SetAttribute("Filepath", relativePath.c_str());

		component.tilemap.Save();
	}

	if (entity.HasComponent<RigidBody2DComponent>())
	{
		RigidBody2DComponent& component = entity.GetComponent<RigidBody2DComponent>();

		tinyxml2::XMLElement* pRigidBody2DElement = pElement->InsertNewChildElement("RigidBody2D");

		pRigidBody2DElement->SetAttribute("BodyType", (int)component.Type);
		pRigidBody2DElement->SetAttribute("FixedRotation", component.FixedRotation);
		pRigidBody2DElement->SetAttribute("GravityScale", component.GravityScale);
		pRigidBody2DElement->SetAttribute("AngularDamping", component.AngularDamping);
		pRigidBody2DElement->SetAttribute("LinearDamping", component.LinearDamping);
	}

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		BoxCollider2DComponent& component = entity.GetComponent<BoxCollider2DComponent>();

		tinyxml2::XMLElement* pBoxColliderElement = pElement->InsertNewChildElement("BoxCollider2D");

		pBoxColliderElement->SetAttribute("Density", component.Density);
		pBoxColliderElement->SetAttribute("Friction", component.Friction);
		pBoxColliderElement->SetAttribute("Restitution", component.Restitution);

		Encode(pBoxColliderElement->InsertNewChildElement("Offset"), component.Offset);
		Encode(pBoxColliderElement->InsertNewChildElement("Size"), component.Size);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pCircleColliderElement = pElement->InsertNewChildElement("CircleCollider2D");

		pCircleColliderElement->SetAttribute("Density", component.Density);
		pCircleColliderElement->SetAttribute("Friction", component.Friction);
		pCircleColliderElement->SetAttribute("Restitution", component.Restitution);

		pCircleColliderElement->SetAttribute("Radius", component.Radius);
	}
}
