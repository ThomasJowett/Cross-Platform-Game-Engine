#include "stdafx.h"
#include "SceneSerializer.h"

#include "TinyXml2/tinyxml2.h"
#include "Scene/Entity.h"
#include "Components/Components.h"

void Encode(tinyxml2::XMLElement* pElement, Vector2f& vec2)
{
	pElement->SetAttribute("X", vec2.x);
	pElement->SetAttribute("Y", vec2.y);
}

void Encode(tinyxml2::XMLElement* pElement, Vector3f& vec3)
{
	pElement->SetAttribute("X", vec3.x);
	pElement->SetAttribute("Y", vec3.y);
	pElement->SetAttribute("Z", vec3.z);
}

SceneSerializer::SceneSerializer(Scene* scene)
	:m_Scene(scene)
{
}

bool SceneSerializer::Serialize(const std::filesystem::path& filepath)
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

		return true;
	}
	else
	{
		ENGINE_ERROR("Could not load scene {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
}

void SceneSerializer::SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity)
{
	pElement->SetAttribute("Name", entity.GetTag().Tag.c_str());
	pElement->SetAttribute("ID", entity.GetID().ID);

	TransformComponent & transformcomp = entity.GetTransform();

	tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Transform");

	tinyxml2::XMLElement* pPosition = pTransformElement->InsertNewChildElement("Position");
	Encode(pPosition, transformcomp.Position);

	tinyxml2::XMLElement* pRotation = pTransformElement->InsertNewChildElement("Rotation");
	Encode(pRotation, transformcomp.Rotation);

	tinyxml2::XMLElement* pScale = pTransformElement->InsertNewChildElement("Scale");
	Encode(pScale, transformcomp.Scale);

	if (entity.HasComponent<CameraComponent>())
	{
		CameraComponent& component = entity.GetComponent<CameraComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Camera");
	}

	if (entity.HasComponent<SpriteComponent>())
	{
		SpriteComponent& component = entity.GetComponent<SpriteComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Sprite");
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
		StaticMeshComponent& component = entity.GetComponent<StaticMeshComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("StaticMesh");
	}

	if (entity.HasComponent<NativeScriptComponent>())
	{
		NativeScriptComponent& component = entity.GetComponent<NativeScriptComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("NativeScript");
	}

	if (entity.HasComponent<PrimitiveComponent>())
	{
		PrimitiveComponent& component = entity.GetComponent<PrimitiveComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Primitive");
	}

	if (entity.HasComponent<TilemapComponent>())
	{
		TilemapComponent& component = entity.GetComponent<TilemapComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("Tilemap");
	}

	if (entity.HasComponent<RigidBody2DComponent>())
	{
		RigidBody2DComponent& component = entity.GetComponent<RigidBody2DComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("RigidBody2D");
	}

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		BoxCollider2DComponent& component = entity.GetComponent<BoxCollider2DComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("BoxCollider2D");
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		CircleCollider2DComponent& component = entity.GetComponent<CircleCollider2DComponent>();

		tinyxml2::XMLElement* pTransformElement = pElement->InsertNewChildElement("CircleCollider2D");
	}
}
