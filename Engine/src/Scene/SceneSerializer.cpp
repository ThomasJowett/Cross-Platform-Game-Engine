#include "stdafx.h"
#include "SceneSerializer.h"

SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	:m_Scene(scene)
{
}

bool SceneSerializer::Serialize(const std::filesystem::path& filepath)
{
	//m_Scene->GetRegistry().each([&](auto entityID)
	//	{
	//		Entity entity = { entityID, m_Scene.get() };
	//
	//		if (!entity)
	//			return;
	//
	//
	//	});
	return false;
}
