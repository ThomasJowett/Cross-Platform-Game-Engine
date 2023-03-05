#include "stdafx.h"
#include "BehaviourTreeSerializer.h"

#include "BehaviorTree.h"
#include "Decorators.h"
#include "Tasks.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"
#include "Logging/Instrumentor.h"

namespace BehaviourTree
{
void Serializer::SerializeNode(tinyxml2::XMLElement* pElement, const Ref<Node> node)
{
	Vector2f editorPosition = node->GetEditorPosition();
	pElement->SetAttribute("x", editorPosition.x);
	pElement->SetAttribute("y", editorPosition.y);

	auto SerializeCompositeNode = [&](tinyxml2::XMLElement* pElement, Ref<Composite> composite)
	{
		for (const auto child : *composite)
		{
			SerializeNode(pElement, child);
		}
	};

	// Composites -----------------------------------------
	if (Ref<StatefulSelector> statefulSelector = std::dynamic_pointer_cast<StatefulSelector>(node)) {
		tinyxml2::XMLElement* pSelector = pElement->InsertNewChildElement("StatefulSelector");
		SerializeCompositeNode(pSelector, statefulSelector);
	}
	else if (Ref<MemSequence> sequence = std::dynamic_pointer_cast<MemSequence>(node)) {
		tinyxml2::XMLElement* pSequence = pElement->InsertNewChildElement("MemSequence");
		SerializeCompositeNode(pSequence, sequence);
	}
	else if (Ref<ParallelSequence> sequence = std::dynamic_pointer_cast<ParallelSequence>(node)) {
		tinyxml2::XMLElement* pSequence = pElement->InsertNewChildElement("ParallelSequence");
		SerializeCompositeNode(pSequence, sequence);
	}
	else if (Ref<Sequence> sequence = std::dynamic_pointer_cast<Sequence>(node)) {
		tinyxml2::XMLElement* pSequence = pElement->InsertNewChildElement("Sequence");
		SerializeCompositeNode(pSequence, sequence);
	}
	else if (Ref<Selector> selector = std::dynamic_pointer_cast<Selector>(node)) {
		tinyxml2::XMLElement* pSelector = pElement->InsertNewChildElement("Selector");
		SerializeCompositeNode(pSelector, selector);
	}

	// Decorators -----------------------------------------
	else if (Ref<BlackboardBool> decorator = std::dynamic_pointer_cast<BlackboardBool>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("BlackboardBoolDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<BlackboardCompare> decorator = std::dynamic_pointer_cast<BlackboardCompare>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("BlackboardCompareDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<Succeeder> decorator = std::dynamic_pointer_cast<Succeeder>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("SucceederDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<Failer> decorator = std::dynamic_pointer_cast<Failer>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("FailerDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<Inverter> decorator = std::dynamic_pointer_cast<Inverter>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("InverterDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<Repeater> decorator = std::dynamic_pointer_cast<Repeater>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("RepeaterDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<UntilSuccess> decorator = std::dynamic_pointer_cast<UntilSuccess>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("UntilSuccessDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}
	else if (Ref<UntilFailure> decorator = std::dynamic_pointer_cast<UntilFailure>(node)) {
		tinyxml2::XMLElement* pDecorator = pElement->InsertNewChildElement("UntilFailureDecorator");
		SerializeNode(pDecorator, decorator->getChild());
	}

	// Tasks -----------------------------------------
	else if (Ref<Wait> wait = std::dynamic_pointer_cast<Wait>(node)) {
		tinyxml2::XMLElement* pWait = pElement->InsertNewChildElement("Wait");
		pWait->SetAttribute("WaitTime", wait->getWaitTime());
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
			composite->addChild(DeserializeNode(pChildElement, behaviourTree));
			pChildElement = pChildElement->NextSiblingElement();
		}
	};

	auto DeserializeDecorator = [&](tinyxml2::XMLElement* pElement, Ref<Decorator> decorator)
	{
		decorator->SetEditorPosition(position);
		tinyxml2::XMLElement* child = pElement->FirstChildElement();
		if (child)
			decorator->setChild(DeserializeNode(child, behaviourTree));
		else
			ENGINE_ERROR("Decorator must have child node!");

		if (child->NextSibling())
			ENGINE_ERROR("Decorator can only have one child!");
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
	if (name == "MemSequence")
	{
		Ref<MemSequence> sequence = CreateRef<MemSequence>();
		DeserializeCompositeNode(pElement, sequence);
		return sequence;
	}
	if (name == "ParallelSequence")
	{
		Ref<ParallelSequence> sequence = CreateRef<ParallelSequence>();
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
	else if (name == "InverterDecorator")
	{
		Ref<Repeater> decorator = CreateRef<Repeater>();
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

	else
	{
		ENGINE_ERROR("Unkown behaviour tree node");
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
		auto pDouble = pBlackboard->InsertNewChildElement("Doubles");
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
		pVec2->SetAttribute("Value", iter->second);
	}

	for (auto iter = blackboard->getVector3sBegin(); iter != blackboard->getVector3sEnd(); ++iter) {
		auto pVec3 = pBlackboard->InsertNewChildElement("Vec3");
		pVec3->SetAttribute("Key", iter->first.c_str());
		pVec3->SetAttribute("Value", iter->second);
	}

	tinyxml2::XMLElement* pEntry = pRoot->InsertNewChildElement("Root");

	const Ref<Node> rootNode = behaviourTree->getRoot();

	if (rootNode) {
		SerializeNode(pEntry, rootNode);
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());

	return error == tinyxml2::XML_SUCCESS;
}

Ref<BehaviourTree> Serializer::Deserialize(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		Ref<BehaviourTree> behaviourTree = CreateRef<BehaviourTree>();
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("BehaviourTree");

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
				if(key) blackboard->setBool(key, value);
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
				std::string value = pString->Attribute("Value");
				if (key) blackboard->setString(key, value);
				pString = pString->NextSiblingElement("String");
			}

			tinyxml2::XMLElement* pVec2 = pBlackboardElement->FirstChildElement("Vec2");
			while (pVec2) {
				const char* key = pString->Attribute("Key");
				float x = pVec2->FloatAttribute("x");
				float y = pVec2->FloatAttribute("y");
				if (key) blackboard->setVector2(key, Vector2f(x, y));
				pVec2 = pVec2->NextSiblingElement("Vec2");
			}

			tinyxml2::XMLElement* pVec3 = pBlackboardElement->FirstChildElement("Vec3");
			while (pVec3) {
				const char* key = pString->Attribute("Key");
				float x = pVec3->FloatAttribute("x");
				float y = pVec3->FloatAttribute("y");
				float z = pVec3->FloatAttribute("z");
				if (key) blackboard->setVector3(key, Vector3f(x, y, z));
				pVec3 = pVec3->NextSiblingElement("Vec3");
			}
		}

		// Root Node
		tinyxml2::XMLElement* pRootElement = pRoot->FirstChildElement("Root");

		if (pRootElement) {
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
		return behaviourTree;
	}

	ENGINE_ERROR("could not load behaviour tree {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
	return nullptr;
}
}


