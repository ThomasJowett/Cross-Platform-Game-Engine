#include "stdafx.h"
#include "BehaviourTreeSerializer.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"

BehaviourTree::Serializer::Serializer(BehaviourTree* behaviourTree)
	:m_BehaviourTree(behaviourTree)
{
}

bool BehaviourTree::Serializer::Serialize(const std::filesystem::path& filepath) const
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("BehaviourTree");

	pRoot->SetAttribute("EngineVersion", VERSION);

	doc.InsertFirstChild(pRoot);

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());

	return error == tinyxml2::XML_SUCCESS;
}

bool BehaviourTree::Serializer::Deserialize(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("BehaviourTree");

		// Version
		const char* version = pRoot->Attribute("EngineVersion");

		if (version)
			if (atoi(version) != VERSION)
				ENGINE_WARN("Loading scene created with a different version of the engine");

		return true;
	}
	
	ENGINE_ERROR("could not load behaviour tree {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
	return false;
}

