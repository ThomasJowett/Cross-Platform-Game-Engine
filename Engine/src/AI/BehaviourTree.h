// BrainTree - A C++ behaviour tree single header library.
// Copyright 2015-2018 Par Arvidsson. All rights reserved.
// Licensed under the MIT license (https://github.com/arvidsson/BrainTree/blob/master/LICENSE).

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Core/core.h"
#include "Logging/Logger.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"

#include <iostream>

namespace BehaviourTree
{
class Node
{
public:
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};

	virtual ~Node() = default;

	virtual Status update(float deltaTime) = 0;
	virtual void initialize() {}
	virtual void terminate(Status s) {}

	Status tick(float deltaTime)
	{
		if (m_Status != Status::Running) {
			initialize();
		}

		m_Status = update(deltaTime);

		if (m_Status != Status::Running) {
			terminate(m_Status);
		}

		return m_Status;
	}

	bool isSuccess() const { return m_Status == Status::Success; }
	bool isFailure() const { return m_Status == Status::Failure; }
	bool isRunning() const { return m_Status == Status::Running; }
	bool isTerminated() const { return isSuccess() || isFailure(); }

	void reset() { m_Status = Status::Invalid; }

	Vector2f GetEditorPosition() { return m_EditorPosition; }
	void SetEditorPosition(Vector2f editorPosition) { m_EditorPosition = editorPosition; }
private:
	Status m_Status = Status::Invalid;

	Vector2f m_EditorPosition;
};

//--------------------------------------------------------------------------------------------------------------------

class Composite : public Node
{
public:
	Composite() = default;
	~Composite() override = default;

	void addChild(Ref<Node> child) { m_Children.push_back(child); }
	bool hasChildren() const { return !m_Children.empty(); }

	std::vector<Ref<Node>>::iterator begin() { return m_Children.begin(); }
	std::vector<Ref<Node>>::iterator end() { return m_Children.end(); }
	std::vector<Ref<Node>>::reverse_iterator rbegin() { return m_Children.rbegin(); }
	std::vector<Ref<Node>>::reverse_iterator rend() { return m_Children.rend(); }

	std::vector<Ref<Node>>::const_iterator begin() const { return m_Children.begin(); }
	std::vector<Ref<Node>>::const_iterator end() const { return m_Children.end(); }
	std::vector<Ref<Node>>::const_reverse_iterator rbegin() const { return m_Children.rbegin(); }
	std::vector<Ref<Node>>::const_reverse_iterator rend() const { return m_Children.rend(); }

protected:
	std::vector<Ref<Node>> m_Children;
};

//--------------------------------------------------------------------------------------------------------------------

class Blackboard
{
public:
	//BOOL
	void setBool(std::string const& key, bool value) { m_Bools[key] = value; }
	bool getBool(std::string const& key)
	{
		if (m_Bools.find(key) == m_Bools.end()) {
			m_Bools[key] = false;
		}
		return m_Bools[key];
	}
	bool hasBool(std::string const& key) const { return m_Bools.find(key) != m_Bools.end(); }

	//INT
	void setInt(std::string const& key, int value) { m_Ints[key] = value; }
	int getInt(std::string const& key)
	{
		if (m_Ints.find(key) == m_Ints.end()) {
			m_Ints[key] = 0;
		}
		return m_Ints[key];
	}
	bool hasInt(std::string const& key) const { return m_Ints.find(key) != m_Ints.end(); }

	//FLOAT
	void setFloat(std::string const& key, float value) { m_Floats[key] = value; }
	float getFloat(std::string const& key)
	{
		if (m_Floats.find(key) == m_Floats.end()) {
			m_Floats[key] = 0.0f;
		}
		return m_Floats[key];
	}
	bool hasFloat(std::string const& key) const { return m_Floats.find(key) != m_Floats.end(); }

	//DOUBLE
	void setDouble(std::string const& key, double value) { m_Doubles[key] = value; }
	double getDouble(std::string const& key)
	{
		if (m_Doubles.find(key) == m_Doubles.end()) {
			m_Doubles[key] = 0.0f;
		}
		return m_Doubles[key];
	}
	bool hasDouble(std::string const& key) const { return m_Doubles.find(key) != m_Doubles.end(); }

	//STRING
	void setString(std::string const& key, std::string_view value) { m_Strings[key] = value; }
	std::string getString(std::string const& key)
	{
		if (m_Strings.find(key) == m_Strings.end()) {
			m_Strings[key] = "";
		}
		return m_Strings[key];
	}
	bool hasString(std::string const& key) const { return m_Strings.find(key) != m_Strings.end(); }

	//VECTOR2D
	void setVector2(std::string const& key, Vector2f value) { m_Vector2s[key] = value; }
	Vector2f getVector2(std::string const& key)
	{
		if (m_Vector2s.find(key) == m_Vector2s.end()) {
			m_Vector2s[key] = Vector2f();
		}
		return m_Vector2s[key];
	}
	bool hasVector2D(std::string const& key) const { return m_Vector2s.find(key) != m_Vector2s.end(); }

	//VECTOR3D
	void setVector3(std::string const& key, Vector3f value) { m_Vector3fs[key] = value; }
	Vector3f getVector3(std::string const& key)
	{
		if (m_Vector3fs.find(key) == m_Vector3fs.end()) {
			m_Vector3fs[key] = Vector3f();
		}
		return m_Vector3fs[key];
	}
	bool hasVector3(std::string const& key) const { return m_Vector3fs.find(key) != m_Vector3fs.end(); }

	std::unordered_map<std::string, bool>::iterator getBoolsBegin() { return m_Bools.begin(); }
	std::unordered_map<std::string, bool>::iterator getBoolsEnd() { return m_Bools.end(); }

	std::unordered_map<std::string, int>::iterator getIntsBegin() { return m_Ints.begin(); }
	std::unordered_map<std::string, int>::iterator getIntsEnd() { return m_Ints.end(); }

	std::unordered_map<std::string, float>::iterator getFloatsBegin() { return m_Floats.begin(); }
	std::unordered_map<std::string, float>::iterator getFloatsEnd() { return m_Floats.end(); }

	std::unordered_map<std::string, double>::iterator getDoublesBegin() { return m_Doubles.begin(); }
	std::unordered_map<std::string, double>::iterator getDoublesEnd() { return m_Doubles.end(); }

	std::unordered_map<std::string, std::string>::iterator getStringsBegin() { return m_Strings.begin(); }
	std::unordered_map<std::string, std::string>::iterator getStringsEnd() { return m_Strings.end(); }

	std::unordered_map<std::string, Vector2f>::iterator getVector2sBegin() { return m_Vector2s.begin(); }
	std::unordered_map<std::string, Vector2f>::iterator getVector2sEnd() { return m_Vector2s.end(); }

	std::unordered_map<std::string, Vector3f>::iterator getVector3sBegin() { return m_Vector3fs.begin(); }
	std::unordered_map<std::string, Vector3f>::iterator getVector3sEnd() { return m_Vector3fs.end(); }

private:
	std::unordered_map<std::string, bool> m_Bools;
	std::unordered_map<std::string, int> m_Ints;
	std::unordered_map<std::string, float> m_Floats;
	std::unordered_map<std::string, double> m_Doubles;
	std::unordered_map<std::string, std::string> m_Strings;
	std::unordered_map<std::string, Vector2f> m_Vector2s;
	std::unordered_map<std::string, Vector3f> m_Vector3fs;
};

//--------------------------------------------------------------------------------------------------------------------

class Decorator : public Node
{
public:
	Decorator() = default;
	~Decorator() override = default;

	void setChild(Ref<Node> node) { m_Child = node; }
	const Ref<Node> getChild() const { return m_Child; }
	bool hasChild() const { return m_Child != nullptr; }

protected:
	Ref<Node> m_Child = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------

class BehaviourTree : public Node
{
public:
	BehaviourTree() : m_Blackboard(CreateRef<Blackboard>()) {}
	explicit BehaviourTree(const Ref<Node>& rootNode) : BehaviourTree() { m_Root = rootNode; }

	Status update(float deltaTime) override
	{
		if (m_Root)
			return m_Root->tick(deltaTime);
		return Status::Invalid;
	}

	Ref<Blackboard> getBlackboard() const { return m_Blackboard; }

	void setRoot(const Ref<Node> node) { m_Root = node; }
	const Ref<Node> getRoot() { return m_Root; }

private:
	Ref<Node> m_Root = nullptr;
	Ref<Blackboard> m_Blackboard = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------

class Leaf : public Node
{
public:
	~Leaf() override = default;
	explicit Leaf(BehaviourTree* behaviourTree) : m_BehaviourTree(behaviourTree) {}

	Status update(float deltaTime) override = 0;

protected:
	BehaviourTree* m_BehaviourTree;
};

//--------------------------------------------------------------------------------------------------------------------

template <class Parent>
class DecoratorBuilder;

template <class Parent>
class CompositeBuilder
{
public:
	CompositeBuilder(Parent* parent, Composite* node) : m_Parent(parent), m_Node(node) {}

	template <class NodeType, typename... Args>
	CompositeBuilder<Parent> leaf(Args... args)
	{
		auto child = std::make_shared<NodeType>((args)...);
		m_Node->addChild(child);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<CompositeBuilder<Parent>> composite(Args... args)
	{
		auto child = std::make_shared<CompositeType>((args)...);
		m_Node->addChild(child);
		return CompositeBuilder<CompositeBuilder<Parent>>(this, (CompositeType*)child.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<CompositeBuilder<Parent>> decorator(Args... args)
	{
		auto child = std::make_shared<DecoratorType>((args)...);
		m_Node->addChild(child);
		return DecoratorBuilder<CompositeBuilder<Parent>>(this, (DecoratorType*)child.get());
	}

	Parent& end()
	{
		return *m_Parent;
	}

private:
	Parent* m_Parent;
	Composite* m_Node;
};

//--------------------------------------------------------------------------------------------------------------------

template <class Parent>
class DecoratorBuilder
{
public:
	DecoratorBuilder(Parent* parent, Decorator* node) : m_Parent(parent), m_Node(node) {}

	template <class NodeType, typename... Args>
	DecoratorBuilder<Parent> leaf(Args... args)
	{
		auto child = std::make_shared<NodeType>((args)...);
		m_Node->setChild(child);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<DecoratorBuilder<Parent>> composite(Args... args)
	{
		auto child = std::make_shared<CompositeType>((args)...);
		m_Node->setChild(child);
		return CompositeBuilder<DecoratorBuilder<Parent>>(this, (CompositeType*)child.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<DecoratorBuilder<Parent>> decorator(Args... args)
	{
		auto child = std::make_shared<DecoratorType>((args)...);
		m_Node->setChild(child);
		return DecoratorBuilder<DecoratorBuilder<Parent>>(this, (DecoratorType*)child.get());
	}

	Parent& end()
	{
		return *m_Parent;
	}

private:
	Parent* m_Parent;
	Decorator* m_Node;
};

//--------------------------------------------------------------------------------------------------------------------

class Builder
{
public:
	template <class NodeType, typename... Args>
	Builder leaf(Args... args)
	{
		m_Root = std::make_shared<NodeType>((args)...);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<Builder> composite(Args... args)
	{
		m_Root = std::make_shared<CompositeType>((args)...);
		return CompositeBuilder<Builder>(this, (CompositeType*)m_Root.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<Builder> decorator(Args... args)
	{
		m_Root = std::make_shared<DecoratorType>((args)...);
		return DecoratorBuilder<Builder>(this, (DecoratorType*)m_Root.get());
	}

	Ref<Node> build() const
	{
		ASSERT(m_Root != nullptr, "The Behaviour Tree is empty!");
		auto tree = std::make_shared<BehaviourTree>();
		tree->setRoot(m_Root);
		return tree;
	}

private:
	Ref<Node> m_Root;
};

//--------------------------------------------------------------------------------------------------------------------

// The Selector composite ticks each child node in order.
// If a child succeeds or runs, the selector returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children fails, only then does the selector fail.
class Selector : public Composite
{
public:

	Status update(float deltaTime) override
	{
		ASSERT(hasChildren(), "Composite has no children");

		for (Ref<Node> child : m_Children)
		{
			Node::Status status = child->tick(deltaTime);

			if (status != Status::Failure)
			{
				return status;
			}
		}

		return Status::Failure;
	}
};

//--------------------------------------------------------------------------------------------------------------------

// The Sequence composite ticks each child node in order.
// If a child fails or runs, the sequence returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children succeeds, only then does the sequence succeed.
class Sequence : public Composite
{
public:

	Status update(float deltaTime) override
	{
		ASSERT(hasChildren(), "Composite has no children");

		for (Ref<Node> child : m_Children)
		{
			Node::Status status = child->tick(deltaTime);

			if (status != Status::Success) {
				return status;
			}
		}

		return Status::Success;
	}
};

//--------------------------------------------------------------------------------------------------------------------

// The StatefulSelector composite ticks each child node in order, and remembers what child it previously tried to tick.
// If a child succeeds or runs, the stateful selector returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children fails, only then does the stateful selector fail.
class StatefulSelector : public Composite
{
public:
	void initialize() override
	{
		it = m_Children.begin();
	}

	Status update(float deltaTime) override
	{
		ASSERT(hasChildren(), "Composite has no children");

		while (it != m_Children.end()) {
			auto status = (*it)->tick(deltaTime);

			if (status != Status::Failure) {
				return status;
			}

			it++;
		}

		it = m_Children.begin();
		return Status::Failure;
	}
private:
	std::vector<Ref<Node>>::iterator it = m_Children.begin();
};

//--------------------------------------------------------------------------------------------------------------------

// The MemSequence composite ticks each child node in order, and remembers what child it previously tried to tick.
// If a child fails or runs, the stateful sequence returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children succeeds, only then does the stateful sequence succeed.
class MemSequence : public Composite
{
public:
	void initialize() override
	{
		it = m_Children.begin();
	}

	Status update(float deltaTime) override
	{
		ASSERT(hasChildren(), "Composite has no children");

		while (it != m_Children.end()) {
			auto status = (*it)->tick(deltaTime);

			if (status != Status::Success) {
				return status;
			}

			it++;
		}

		it = m_Children.begin();
		return Status::Success;
	}
private:
	std::vector<Ref<Node>>::iterator it = m_Children.begin();
};

//--------------------------------------------------------------------------------------------------------------------

class ParallelSequence : public Composite
{
public:
	ParallelSequence(bool successOnAll = true, bool failOnAll = true) : m_UseSuccessFailPolicy(true), m_SuccessOnAll(successOnAll), m_FailOnAll(failOnAll) {}
	ParallelSequence(int minSuccess, int minFail) : m_MinSuccess(minSuccess), m_MinFail(minFail) {}

	Status update(float deltaTime) override
	{
		ASSERT(hasChildren(), "Composite has no children");

		size_t minimumSuccess = m_MinSuccess;
		size_t minimumFail = m_MinFail;

		if (m_UseSuccessFailPolicy) {
			if (m_SuccessOnAll) {
				minimumSuccess = m_Children.size();
			}
			else {
				minimumSuccess = 1;
			}

			if (m_FailOnAll) {
				minimumFail = m_Children.size();
			}
			else {
				minimumFail = 1;
			}
		}

		int total_success = 0;
		int total_fail = 0;

		for (auto& child : m_Children) {
			auto status = child->tick(deltaTime);
			if (status == Status::Success) {
				total_success++;
			}
			if (status == Status::Failure) {
				total_fail++;
			}
		}

		if (total_success >= minimumSuccess) {
			return Status::Success;
		}
		if (total_fail >= minimumFail) {
			return Status::Failure;
		}

		return Status::Running;
	}

private:
	bool m_UseSuccessFailPolicy = false;
	bool m_SuccessOnAll = true;
	bool m_FailOnAll = true;
	size_t m_MinSuccess = 0;
	size_t m_MinFail = 0;
};
} // namespace BehaviourTree
