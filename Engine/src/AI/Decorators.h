#pragma once
#include "BehaviorTree.h"

namespace BehaviorTree
{

	// The Blackboard Bool decorator returns the value of a blackboard bool
	class BlackboardBool : public Decorator
	{
	public:
		BlackboardBool(Ref<Blackboard> blackboard, std::string blackboardkey, bool isSet)
			:mBlackboardKey(blackboardkey), mIsSet(isSet),
			Decorator(blackboard) {}

		Status update(float deltaTime) override
		{
			if (!(m_Blackboard->getBool(mBlackboardKey) ^ mIsSet))
				return m_Child->tick(deltaTime);

			return Status::Failure;
		}
	private:
		std::string mBlackboardKey;
		bool mIsSet;
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Blackboard Compare decorator return whether two blackboard keys are equal
	class BlackboardCompare : public Decorator
	{
	public:
		BlackboardCompare(Ref<Blackboard> blackboard, std::string blackboardkey_1, std::string blackboardkey_2, bool isEqual)
			:mBBKey_1(blackboardkey_1), mBBKey_2(blackboardkey_2), mIsEqual(isEqual),
			Decorator(blackboard) {}

		Status update(float deltaTime) override
		{
			if (!((m_Blackboard->getBool(mBBKey_1) == m_Blackboard->getBool(mBBKey_1)) ^ mIsEqual))
				return m_Child->tick(deltaTime);

			return Status::Failure;
		}
	private:
		std::string mBBKey_1;
		std::string mBBKey_2;
		bool mIsEqual;
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Succeeder decorator returns success, regardless of what happens to the child.
	class Succeeder : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			m_Child->tick(deltaTime);
			return Status::Success;
		}
	};

	// The Failer decorator returns failure, regardless of what happens to the child.
	class Failer : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			m_Child->tick(deltaTime);
			return Status::Failure;
		}
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
	// If the child runs, the Inverter returns the status that it is running too.
	class Inverter : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			auto s = m_Child->tick(deltaTime);

			if (s == Status::Success) {
				return Status::Failure;
			}
			else if (s == Status::Failure) {
				return Status::Success;
			}

			return s;
		}
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Repeater decorator repeats infinitely or to a limit until the child returns success.
	class Repeater : public Decorator
	{
	public:
		Repeater(int limit = 0) : limit(limit) {}

		void initialize() override
		{
			counter = 0;
		}

		Status update(float deltaTime) override
		{
			m_Child->tick(deltaTime);

			if (limit > 0 && ++counter == limit) {
				return Status::Success;
			}

			return Status::Running;
		}

	protected:
		int limit;
		int counter = 0;
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The UntilSuccess decorator repeats until the child returns success and then returns success.
	class UntilSuccess : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			while (true) {
				auto status = m_Child->tick(deltaTime);

				if (status == Status::Success) {
					return Status::Success;
				}
			}
		}
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The UntilFailure decorator repeats until the child returns fail and then returns success.
	class UntilFailure : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			while (true) {
				auto status = m_Child->tick(deltaTime);

				if (status == Status::Failure) {
					return Status::Success;
				}
			}
		}
	};

}