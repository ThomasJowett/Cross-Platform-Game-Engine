#include "StateMachineSerializer.h"

#include "StateMachine.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Application.h"
#include "Core/Version.h"
#include "Logging/Instrumentor.h"
#include "Utilities/SerializationUtils.h"
#include "Utilities/FileUtils.h"
#include "Scene/AssetManager.h"

bool StateMachineSerializer::Serialize(const std::filesystem::path& filepath, StateMachine* stateMachine)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("StateMachine");

	pRoot->SetAttribute("EngineVersion", VERSION);
	pRoot->SetAttribute("InitialState", stateMachine->GetInitialState().c_str());

	doc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement* pEntry = pRoot->InsertNewChildElement("Entry");
	pEntry->SetAttribute("x", stateMachine->GetEditorPosition().x);
	pEntry->SetAttribute("y", stateMachine->GetEditorPosition().y);

	for (const Ref<State>& state : stateMachine->GetStates())
	{
		tinyxml2::XMLElement* pState = pRoot->InsertNewChildElement("State");
		pState->SetAttribute("Name", state->GetName().c_str());
		if (Ref<LuaState> luaState = std::dynamic_pointer_cast<LuaState>(state))
			SerializationUtils::Encode(pState, luaState->GetScriptPath());
		pState->SetAttribute("x", state->GetEditorPosition().x);
		pState->SetAttribute("y", state->GetEditorPosition().y);
	}

	for (const StateMachine::Transition& transition : stateMachine->GetTransitions())
	{
		tinyxml2::XMLElement* pTransition = pRoot->InsertNewChildElement("Transition");
		pTransition->SetAttribute("From", transition.from.c_str());
		pTransition->SetAttribute("To", transition.to.c_str());
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());

	return error == tinyxml2::XML_SUCCESS;
}

Ref<StateMachine> StateMachineSerializer::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	std::filesystem::path relativePath = filepath.is_absolute() ? FileUtils::RelativePath(filepath, Application::GetOpenDocumentDirectory()) : filepath;

	if (AssetManager::HasBundle())
	{
		std::vector<uint8_t> data;
		if (AssetManager::GetFileData(relativePath, data))
			return Deserialize(relativePath, data);
	}

	return Deserialize(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
}

Ref<StateMachine> StateMachineSerializer::Deserialize(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		return LoadXML(&doc);
	}

	ENGINE_ERROR("could not load state machine {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
	return nullptr;
}

Ref<StateMachine> StateMachineSerializer::Deserialize(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		return LoadXML(&doc);
	}
	ENGINE_ERROR("could not load state machine {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());

	return nullptr;
}

Ref<StateMachine> StateMachineSerializer::LoadXML(tinyxml2::XMLDocument* doc)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLElement* pRoot = doc->FirstChildElement("StateMachine");
	if (!pRoot) {
		ENGINE_ERROR("Not a state machine file");
		return nullptr;
	}

	if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
		ENGINE_WARN("Loading state machine created with a different version of the engine");

	Ref<StateMachine> stateMachine = CreateRef<StateMachine>();

	if (const char* initialState = pRoot->Attribute("InitialState"))
		stateMachine->SetInitialState(initialState);

	if (tinyxml2::XMLElement* pEntry = pRoot->FirstChildElement("Entry"))
	{
		Vector2f position;
		pEntry->QueryFloatAttribute("x", &position.x);
		pEntry->QueryFloatAttribute("y", &position.y);
		stateMachine->SetEditorPosition(position);
	}

	for (tinyxml2::XMLElement* pState = pRoot->FirstChildElement("State"); pState; pState = pState->NextSiblingElement("State"))
	{
		const char* name = pState->Attribute("Name");
		if (!name || !*name)
		{
			ENGINE_ERROR("State machine state has no name, skipped");
			continue;
		}

		std::filesystem::path scriptPath;
		SerializationUtils::Decode(pState, scriptPath);

		Vector2f position;
		pState->QueryFloatAttribute("x", &position.x);
		pState->QueryFloatAttribute("y", &position.y);

		Ref<LuaState> state = CreateRef<LuaState>(name, scriptPath);
		state->SetEditorPosition(position);
		stateMachine->AddState(state);
	}

	for (tinyxml2::XMLElement* pTransition = pRoot->FirstChildElement("Transition"); pTransition; pTransition = pTransition->NextSiblingElement("Transition"))
	{
		const char* from = pTransition->Attribute("From");
		const char* to = pTransition->Attribute("To");
		if (from && to)
			stateMachine->AddTransition(from, to);
	}

	return stateMachine;
}
