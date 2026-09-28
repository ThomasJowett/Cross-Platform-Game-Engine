#include "BehaviourTreeSerializer.h"

#include "BehaviourTree.h"
#include "Decorators.h"
#include "Tasks.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"
#include "Logging/Instrumentor.h"
#include "Utilities/SerializationUtils.h"
#include "Utilities/FileUtils.h"
#include "Scene/AssetManager.h"

namespace BehaviourTree
{
namespace
{
const char* const c_ValueTypeNames[] = { "Bool", "Int", "Float", "Double", "String", "Vec2", "Vec3" };

SetBlackboard::ValueType ValueTypeFromName(const char* name)
{
	for (int i = 0; i < (int)std::size(c_ValueTypeNames); ++i)
		if (name && strcmp(name, c_ValueTypeNames[i]) == 0)
			return (SetBlackboard::ValueType)i;
	return SetBlackboard::ValueType::Bool;
}
}

void Serializer::SerializeNode(tinyxml2::XMLElement* pElement, const Ref<Node> node)
{
	if (!node)
		return;

	tinyxml2::XMLElement* pNode = nullptr;

	auto SerializeCompositeNode = [&](const char* name, Ref<Composite> composite)
		{
			pNode = pElement->InsertNewChildElement(name);
			for (const auto& child : *composite)
			{
				SerializeNode(pNode, child);
			}
		};

	auto SerializeDecorator = [&](const char* name, Ref<Decorator> decorator)
		{
			pNode = pElement->InsertNewChildElement(name);
			SerializeNode(pNode, decorator->getChild());
		};

	// Composites -----------------------------------------
	if (Ref<StatefulSelector> statefulSelector = std::dynamic_pointer_cast<StatefulSelector>(node)) {
		SerializeCompositeNode("StatefulSelector", statefulSelector);
	}
	else if (Ref<MemSequence> sequence = std::dynamic_pointer_cast<MemSequence>(node)) {
		SerializeCompositeNode("MemSequence", sequence);
	}
	else if (Ref<ParallelSequence> sequence = std::dynamic_pointer_cast<ParallelSequence>(node)) {
		SerializeCompositeNode("ParallelSequence", sequence);
		if (sequence->usesSuccessFailPolicy()) {
			pNode->SetAttribute("SuccessOnAll", sequence->successOnAll());
			pNode->SetAttribute("FailOnAll", sequence->failOnAll());
		}
		else {
			pNode->SetAttribute("MinSuccess", sequence->getMinSuccess());
			pNode->SetAttribute("MinFail", sequence->getMinFail());
		}
	}
	else if (Ref<Sequence> sequence = std::dynamic_pointer_cast<Sequence>(node)) {
		SerializeCompositeNode("Sequence", sequence);
	}
	else if (Ref<Selector> selector = std::dynamic_pointer_cast<Selector>(node)) {
		SerializeCompositeNode("Selector", selector);
	}

	// Decorators -----------------------------------------
	else if (Ref<BlackboardBool> decorator = std::dynamic_pointer_cast<BlackboardBool>(node)) {
		SerializeDecorator("BlackboardBoolDecorator", decorator);
		pNode->SetAttribute("Key", decorator->getKey().c_str());
		pNode->SetAttribute("IsSet", decorator->getIsSet());
	}
	else if (Ref<BlackboardCompare> decorator = std::dynamic_pointer_cast<BlackboardCompare>(node)) {
		SerializeDecorator("BlackboardCompareDecorator", decorator);
		pNode->SetAttribute("Key1", decorator->getKey1().c_str());
		pNode->SetAttribute("Key2", decorator->getKey2().c_str());
		pNode->SetAttribute("IsEqual", decorator->getIsEqual());
	}
	else if (Ref<Succeeder> decorator = std::dynamic_pointer_cast<Succeeder>(node)) {
		SerializeDecorator("SucceederDecorator", decorator);
	}
	else if (Ref<Failer> decorator = std::dynamic_pointer_cast<Failer>(node)) {
		SerializeDecorator("FailerDecorator", decorator);
	}
	else if (Ref<Inverter> decorator = std::dynamic_pointer_cast<Inverter>(node)) {
		SerializeDecorator("InverterDecorator", decorator);
	}
	else if (Ref<Repeater> decorator = std::dynamic_pointer_cast<Repeater>(node)) {
		SerializeDecorator("RepeaterDecorator", decorator);
		pNode->SetAttribute("Limit", decorator->getLimit());
	}
	else if (Ref<UntilSuccess> decorator = std::dynamic_pointer_cast<UntilSuccess>(node)) {
		SerializeDecorator("UntilSuccessDecorator", decorator);
	}
	else if (Ref<UntilFailure> decorator = std::dynamic_pointer_cast<UntilFailure>(node)) {
		SerializeDecorator("UntilFailureDecorator", decorator);
	}

	// Tasks -----------------------------------------
	else if (Ref<Wait> wait = std::dynamic_pointer_cast<Wait>(node)) {
		pNode = pElement->InsertNewChildElement("Wait");
		pNode->SetAttribute("WaitTime", wait->getWaitTime());
	}
	else if (Ref<CustomTask> customTask = std::dynamic_pointer_cast<CustomTask>(node)) {
		pNode = pElement->InsertNewChildElement("CustomTask");
		SerializationUtils::Encode(pNode, customTask->getScriptPath());
	}
	else if (Ref<RandomWait> randomWait = std::dynamic_pointer_cast<RandomWait>(node)) {
		pNode = pElement->InsertNewChildElement("RandomWait");
		pNode->SetAttribute("MinTime", randomWait->getMinTime());
		pNode->SetAttribute("MaxTime", randomWait->getMaxTime());
	}
	else if (Ref<SetBlackboard> setBlackboard = std::dynamic_pointer_cast<SetBlackboard>(node)) {
		pNode = pElement->InsertNewChildElement("SetBlackboard");
		const SetBlackboard::Value& value = setBlackboard->getValue();
		pNode->SetAttribute("Key", setBlackboard->getKey().c_str());
		pNode->SetAttribute("Type", c_ValueTypeNames[(int)value.type]);
		switch (value.type)
		{
		case SetBlackboard::ValueType::Bool:	pNode->SetAttribute("Value", value.boolValue); break;
		case SetBlackboard::ValueType::Int:		pNode->SetAttribute("Value", value.intValue); break;
		case SetBlackboard::ValueType::Float:
		case SetBlackboard::ValueType::Double:	pNode->SetAttribute("Value", value.numberValue); break;
		case SetBlackboard::ValueType::String:	pNode->SetAttribute("Value", value.stringValue.c_str()); break;
		case SetBlackboard::ValueType::Vec2:	SerializationUtils::Encode(pNode->InsertNewChildElement("Vector"), Vector2f(value.vectorValue.x, value.vectorValue.y)); break;
		case SetBlackboard::ValueType::Vec3:	SerializationUtils::Encode(pNode->InsertNewChildElement("Vector"), value.vectorValue); break;
		}
	}
	else if (Ref<EmitSignal> emitSignal = std::dynamic_pointer_cast<EmitSignal>(node)) {
		pNode = pElement->InsertNewChildElement("EmitSignal");
		pNode->SetAttribute("Signal", emitSignal->getSignalName().c_str());
	}

	if (pNode) {
		Vector2f editorPosition = node->GetEditorPosition();
		pNode->SetAttribute("x", editorPosition.x);
		pNode->SetAttribute("y", editorPosition.y);
	}
	else {
		ENGINE_ERROR("Unknown behaviour tree node, not serialized");
	}
}

Ref<Node> Serializer::DeserializeNode(tinyxml2::XMLElement* pElement, BehaviourTree* behaviourTree)
{
	Vector2f position;

	pElement->QueryFloatAttribute("x", &position.x);
	pElement->QueryFloatAttribute("y", &position.y);

	auto DeserializeCompositeNode = [&](tinyxml2::XMLElement* pElement, Ref<Composite> composite)
		{
			composite->SetEditorPosition(position);
			tinyxml2::XMLElement* pChildElement = pElement->FirstChildElement();
			while (pChildElement) {
				if (Ref<Node> child = DeserializeNode(pChildElement, behaviourTree))
					composite->addChild(child);
				pChildElement = pChildElement->NextSiblingElement();
			}
		};

	auto DeserializeDecorator = [&](tinyxml2::XMLElement* pElement, Ref<Decorator> decorator)
		{
			decorator->SetEditorPosition(position);
			tinyxml2::XMLElement* child = pElement->FirstChildElement();
			if (child) {
				decorator->setChild(DeserializeNode(child, behaviourTree));
				if (child->NextSiblingElement())
					ENGINE_ERROR("Decorator can only have one child!");
			}
			else
				ENGINE_WARN("Decorator has no child node");
		};

	std::string name = pElement->Name();

	// Composites -----------------------------------------
	if (name == "Sequence")
	{
		Ref<Sequence> sequence = CreateRef<Sequence>();
		DeserializeCompositeNode(pElement, sequence);
		return sequence;
	}
	else if (name == "Selector")
	{
		Ref<Selector> selector = CreateRef<Selector>();
		DeserializeCompositeNode(pElement, selector);
		return selector;
	}
	else if (name == "StatefulSelector")
	{
		Ref<StatefulSelector> selector = CreateRef<StatefulSelector>();
		DeserializeCompositeNode(pElement, selector);
		return selector;
	}
	else if (name == "MemSequence")
	{
		Ref<MemSequence> sequence = CreateRef<MemSequence>();
		DeserializeCompositeNode(pElement, sequence);
		return sequence;
	}
	else if (name == "ParallelSequence")
	{
		Ref<ParallelSequence> sequence;
		if (pElement->Attribute("MinSuccess") || pElement->Attribute("MinFail"))
			sequence = CreateRef<ParallelSequence>(pElement->IntAttribute("MinSuccess", 1), pElement->IntAttribute("MinFail", 1));
		else
			sequence = CreateRef<ParallelSequence>(pElement->BoolAttribute("SuccessOnAll", true), pElement->BoolAttribute("FailOnAll", true));
		DeserializeCompositeNode(pElement, sequence);
		return sequence;
	}
	// Decorators -----------------------------------------
	else if (name == "BlackboardBoolDecorator")
	{
		const char* key = pElement->Attribute("Key");
		bool isSet = pElement->BoolAttribute("IsSet", true);

		Ref<BlackboardBool> decorator = CreateRef<BlackboardBool>(behaviourTree->getBlackboard(), key ? key : "key", isSet);
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "BlackboardCompareDecorator")
	{
		const char* key1 = pElement->Attribute("Key1");
		const char* key2 = pElement->Attribute("Key2");
		bool isEqual = pElement->BoolAttribute("IsEqual", true);
		Ref<BlackboardCompare> decorator = CreateRef<BlackboardCompare>(behaviourTree->getBlackboard(), key1 ? key1 : "key1", key2 ? key2 : "key2", isEqual);
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "SucceederDecorator")
	{
		Ref<Succeeder> decorator = CreateRef<Succeeder>();
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "FailerDecorator")
	{
		Ref<Failer> decorator = CreateRef<Failer>();
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "InverterDecorator")
	{
		Ref<Inverter> decorator = CreateRef<Inverter>();
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "RepeaterDecorator")
	{
		Ref<Repeater> decorator = CreateRef<Repeater>(pElement->IntAttribute("Limit", 0));
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "UntilSuccessDecorator")
	{
		Ref<UntilSuccess> decorator = CreateRef<UntilSuccess>();
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	else if (name == "UntilFailureDecorator")
	{
		Ref<UntilFailure> decorator = CreateRef<UntilFailure>();
		DeserializeDecorator(pElement, decorator);
		return decorator;
	}
	// Tasks -----------------------------------------
	else if (name == "Wait")
	{
		float waitTime = pElement->FloatAttribute("WaitTime", 1.0f);
		Ref<Wait> wait = CreateRef<Wait>(behaviourTree, waitTime);
		wait->SetEditorPosition(position);
		return wait;
	}
	else if (name == "CustomTask")
	{
		std::filesystem::path filepath;
		SerializationUtils::Decode(pElement, filepath);
		Ref<CustomTask> customTask = CreateRef<CustomTask>(behaviourTree, filepath);
		customTask->SetEditorPosition(position);
		return customTask;
	}
	else if (name == "RandomWait")
	{
		Ref<RandomWait> randomWait = CreateRef<RandomWait>(behaviourTree, pElement->FloatAttribute("MinTime", 0.5f), pElement->FloatAttribute("MaxTime", 1.5f));
		randomWait->SetEditorPosition(position);
		return randomWait;
	}
	else if (name == "SetBlackboard")
	{
		SetBlackboard::Value value;
		value.type = ValueTypeFromName(pElement->Attribute("Type"));
		switch (value.type)
		{
		case SetBlackboard::ValueType::Bool:	value.boolValue = pElement->BoolAttribute("Value"); break;
		case SetBlackboard::ValueType::Int:		value.intValue = pElement->IntAttribute("Value"); break;
		case SetBlackboard::ValueType::Float:
		case SetBlackboard::ValueType::Double:	value.numberValue = pElement->DoubleAttribute("Value"); break;
		case SetBlackboard::ValueType::String:
			if (const char* stringValue = pElement->Attribute("Value"))
				value.stringValue = stringValue;
			break;
		case SetBlackboard::ValueType::Vec2:
		{
			Vector2f vector;
			SerializationUtils::Decode(pElement->FirstChildElement("Vector"), vector);
			value.vectorValue = Vector3f(vector.x, vector.y, 0.0f);
			break;
		}
		case SetBlackboard::ValueType::Vec3:	SerializationUtils::Decode(pElement->FirstChildElement("Vector"), value.vectorValue); break;
		}

		const char* key = pElement->Attribute("Key");
		Ref<SetBlackboard> setBlackboard = CreateRef<SetBlackboard>(behaviourTree, behaviourTree->getBlackboard(), key ? key : "", value);
		setBlackboard->SetEditorPosition(position);
		return setBlackboard;
	}
	else if (name == "EmitSignal")
	{
		const char* signalName = pElement->Attribute("Signal");
		Ref<EmitSignal> emitSignal = CreateRef<EmitSignal>(behaviourTree, signalName ? signalName : "");
		emitSignal->SetEditorPosition(position);
		return emitSignal;
	}

	else
	{
		ENGINE_ERROR("Unknown behaviour tree node {0}", name);
	}

	return nullptr;
}

bool Serializer::Serialize(const std::filesystem::path& filepath, BehaviourTree* behaviourTree)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("BehaviourTree");

	pRoot->SetAttribute("EngineVersion", VERSION);

	doc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement* pBlackboard = pRoot->InsertNewChildElement("Blackboard");

	Ref<Blackboard> blackboard = behaviourTree->getBlackboard();


	for (auto iter = blackboard->getBoolsBegin(); iter != blackboard->getBoolsEnd(); ++iter) {
		auto pBool = pBlackboard->InsertNewChildElement("Bool");
		pBool->SetAttribute("Key", iter->first.c_str());
		pBool->SetAttribute("Value", iter->second);
	}

	for (auto iter = blackboard->getIntsBegin(); iter != blackboard->getIntsEnd(); ++iter) {
		auto pInt = pBlackboard->InsertNewChildElement("Int");
		pInt->SetAttribute("Key", iter->first.c_str());
		pInt->SetAttribute("Value", iter->second);
	}

	for (auto iter = blackboard->getFloatsBegin(); iter != blackboard->getFloatsEnd(); ++iter) {
		auto pFloat = pBlackboard->InsertNewChildElement("Float");
		pFloat->SetAttribute("Key", iter->first.c_str());
		pFloat->SetAttribute("Value", iter->second);
	}

	for (auto iter = blackboard->getDoublesBegin(); iter != blackboard->getDoublesEnd(); ++iter) {
		auto pDouble = pBlackboard->InsertNewChildElement("Double");
		pDouble->SetAttribute("Key", iter->first.c_str());
		pDouble->SetAttribute("Value", iter->second);
	}

	for (auto iter = blackboard->getStringsBegin(); iter != blackboard->getStringsEnd(); ++iter) {
		auto pString = pBlackboard->InsertNewChildElement("String");
		pString->SetAttribute("Key", iter->first.c_str());
		pString->SetAttribute("Value", iter->second.c_str());
	}

	for (auto iter = blackboard->getVector2sBegin(); iter != blackboard->getVector2sEnd(); ++iter) {
		auto pVec2 = pBlackboard->InsertNewChildElement("Vec2");
		pVec2->SetAttribute("Key", iter->first.c_str());
		SerializationUtils::Encode(pVec2, iter->second);
	}

	for (auto iter = blackboard->getVector3sBegin(); iter != blackboard->getVector3sEnd(); ++iter) {
		auto pVec3 = pBlackboard->InsertNewChildElement("Vec3");
		pVec3->SetAttribute("Key", iter->first.c_str());
		SerializationUtils::Encode(pVec3, iter->second);
	}

	tinyxml2::XMLElement* pEntry = pRoot->InsertNewChildElement("Root");
	pEntry->SetAttribute("x", behaviourTree->GetEditorPosition().x);
	pEntry->SetAttribute("y", behaviourTree->GetEditorPosition().y);

	const Ref<Node> rootNode = behaviourTree->getRoot();

	if (rootNode) {
		SerializeNode(pEntry, rootNode);
	}

	if (!behaviourTree->getUnattached().empty()) {
		tinyxml2::XMLElement* pUnattached = pRoot->InsertNewChildElement("Unattached");
		for (const Ref<Node>& node : behaviourTree->getUnattached())
			SerializeNode(pUnattached, node);
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());

	return error == tinyxml2::XML_SUCCESS;
}

Ref<BehaviourTree> Serializer::Load(const std::filesystem::path& filepath)
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

Ref<BehaviourTree> Serializer::Deserialize(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		return LoadXML(&doc);
	}

	ENGINE_ERROR("could not load behaviour tree {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
	return nullptr;
}

Ref<BehaviourTree> Serializer::Deserialize(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		return LoadXML(&doc);
	}
	ENGINE_ERROR("could not load behaviour tree {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());

	return nullptr;
}

Ref<BehaviourTree> Serializer::LoadXML(tinyxml2::XMLDocument* doc)
{
	PROFILE_FUNCTION();

	Ref<BehaviourTree> behaviourTree = CreateRef<BehaviourTree>();
	tinyxml2::XMLElement* pRoot = doc->FirstChildElement("BehaviourTree");
	if (!pRoot) {
		ENGINE_ERROR("Not a behaviour tree file");
		return nullptr;
	}

	// Version
	if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
		ENGINE_WARN("Loading behaviour tree created with a different version of the engine");

	// Blackboard
	tinyxml2::XMLElement* pBlackboardElement = pRoot->FirstChildElement("Blackboard");

	if (pBlackboardElement) {
		Ref<Blackboard> blackboard = behaviourTree->getBlackboard();

		tinyxml2::XMLElement* pBool = pBlackboardElement->FirstChildElement("Bool");
		while (pBool)
		{
			const char* key = pBool->Attribute("Key");
			bool value = pBool->BoolAttribute("Value");
			if (key) blackboard->setBool(key, value);
			pBool = pBool->NextSiblingElement("Bool");
		}

		tinyxml2::XMLElement* pInt = pBlackboardElement->FirstChildElement("Int");
		while (pInt)
		{
			const char* key = pInt->Attribute("Key");
			int value = pInt->IntAttribute("Value");
			if (key) blackboard->setInt(key, value);
			pInt = pInt->NextSiblingElement("Int");
		}

		tinyxml2::XMLElement* pFloat = pBlackboardElement->FirstChildElement("Float");
		while (pFloat)
		{
			const char* key = pFloat->Attribute("Key");
			float value = pFloat->FloatAttribute("Value");
			if (key) blackboard->setFloat(key, value);
			pFloat = pFloat->NextSiblingElement("Float");
		}

		tinyxml2::XMLElement* pDouble = pBlackboardElement->FirstChildElement("Double");
		while (pDouble) {
			const char* key = pDouble->Attribute("Key");
			double value = pDouble->DoubleAttribute("Value");
			if (key) blackboard->setDouble(key, value);
			pDouble = pDouble->NextSiblingElement("Double");
		}

		tinyxml2::XMLElement* pString = pBlackboardElement->FirstChildElement("String");
		while (pString) {
			const char* key = pString->Attribute("Key");
			const char* value = pString->Attribute("Value");
			if (key) blackboard->setString(key, value ? value : "");
			pString = pString->NextSiblingElement("String");
		}

		tinyxml2::XMLElement* pVec2 = pBlackboardElement->FirstChildElement("Vec2");
		while (pVec2) {
			const char* key = pVec2->Attribute("Key");
			Vector2f value;
			SerializationUtils::Decode(pVec2, value);
			if (key) blackboard->setVector2(key, value);
			pVec2 = pVec2->NextSiblingElement("Vec2");
		}

		tinyxml2::XMLElement* pVec3 = pBlackboardElement->FirstChildElement("Vec3");
		while (pVec3) {
			const char* key = pVec3->Attribute("Key");
			Vector3f value;
			SerializationUtils::Decode(pVec3, value);
			if (key) blackboard->setVector3(key, value);
			pVec3 = pVec3->NextSiblingElement("Vec3");
		}
	}

	// Root Node
	tinyxml2::XMLElement* pRootElement = pRoot->FirstChildElement("Root");

	if (pRootElement) {
		Vector2f rootPosition;
		pRootElement->QueryFloatAttribute("x", &rootPosition.x);
		pRootElement->QueryFloatAttribute("y", &rootPosition.y);
		behaviourTree->SetEditorPosition(rootPosition);

		tinyxml2::XMLElement* pEntryElement = pRootElement->FirstChildElement();
		if (pEntryElement) {
			// Nodes are recursively deserialized
			Ref<Node> firstNode = DeserializeNode(pEntryElement, behaviourTree.get());
			if (firstNode)
				behaviourTree->setRoot(firstNode);
		}
	}
	else {
		ENGINE_ERROR("Behaviour tree must have a root node!");
		return nullptr;
	}

	if (tinyxml2::XMLElement* pUnattached = pRoot->FirstChildElement("Unattached")) {
		for (tinyxml2::XMLElement* pNode = pUnattached->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
			if (Ref<Node> node = DeserializeNode(pNode, behaviourTree.get()))
				behaviourTree->addUnattached(node);
		}
	}
	return behaviourTree;
}
}


