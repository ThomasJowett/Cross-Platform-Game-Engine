#pragma once
#include "BehaviourTree.h"

namespace BehaviourTree
{
	// The Blackboard Bool decorator returns the value of a blackboard bool
	class BlackboardBool : public Decorator
	{
	public:
		BlackboardBool(Ref<Blackboard> blackboard, std::string const& blackboardkey, bool isSet)
			:m_Blackboard(blackboard), mBlackboardKey(blackboardkey), mIsSet(isSet) {}

		Status update(float deltaTime) override
		{
			if (!m_Child)
				return Status::Failure;

			if (!(m_Blackboard->getBool(mBlackboardKey) != mIsSet))
				return m_Child->tick(deltaTime);

			m_Child->halt();
			return Status::Failure;
		}
		const std::string& getKey() const { return mBlackboardKey; }
		bool getIsSet() const { return mIsSet; }

	private:
		Ref<Blackboard> m_Blackboard = nullptr;
		std::string mBlackboardKey;
		bool mIsSet;
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Blackboard Compare decorator return whether two blackboard keys are equal
	class BlackboardCompare : public Decorator
	{
	public:
		BlackboardCompare(Ref<Blackboard> blackboard, std::string const& blackboardkey_1, std::string const& blackboardkey_2, bool isEqual)
			:m_Blackboard(blackboard), mBBKey_1(blackboardkey_1), mBBKey_2(blackboardkey_2), mIsEqual(isEqual) {}

		Status update(float deltaTime) override
		{
			if (!m_Child)
				return Status::Failure;

			if (!((m_Blackboard->getBool(mBBKey_1) == m_Blackboard->getBool(mBBKey_2)) != mIsEqual))
				return m_Child->tick(deltaTime);

			m_Child->halt();
			return Status::Failure;
		}
		const std::string& getKey1() const { return mBBKey_1; }
		const std::string& getKey2() const { return mBBKey_2; }
		bool getIsEqual() const { return mIsEqual; }

	private:
		Ref<Blackboard> m_Blackboard = nullptr;
		std::string mBBKey_1;
		std::string mBBKey_2;
		bool mIsEqual;
	};

	//--------------------------------------------------------------------------------------------------------------------

	// The Succeeder decorator returns success once the child finishes, regardless of its result.
	class Succeeder : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			if (!m_Child)
				return Status::Failure;

			if (m_Child->tick(deltaTime) == Status::Running)
				return Status::Running;
			return Status::Success;
		}
	};

	// The Failer decorator returns failure once the child finishes, regardless of its result.
	class Failer : public Decorator
	{
	public:
		Status update(float deltaTime) override
		{
			if (!m_Child)
				return Status::Failure;

			if (m_Child->tick(deltaTime) == Status::Running)
				return Status::Running;
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
			if (!m_Child)
				return Status::Failure;

			Status s = m_Child->tick(deltaTime);

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
		explicit Repeater(int limit = 0) : limit(limit) {}

		void initialize() override
		{
			counter = 0;
		}

		Status update(float deltaTime) override
		{
			if (!m_Child)
				return Status::Failure;

			m_Child->tick(deltaTime);

			if (limit > 0 && ++counter == limit) {
				m_Child->halt();
				return Status::Success;
			}

			return Status::Running;
		}

		int getLimit() const { return limit; }

	private:
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
			if (!m_Child)
				return Status::Failure;

			while (true) {
				Status status = m_Child->tick(deltaTime);

				if (status == Status::Success) {
					return Status::Success;
				}
				if (status == Status::Running) {
					return Status::Running;
				}
				if (status == Status::Invalid) {
					return Status::Failure;
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
			if (!m_Child)
				return Status::Failure;

			while (true) {
				Status status = m_Child->tick(deltaTime);

				if (status == Status::Failure) {
					return Status::Success;
				}
				if (status == Status::Running) {
					return Status::Running;
				}
				if (status == Status::Invalid) {
					return Status::Failure;
				}
			}
		}
	};
}