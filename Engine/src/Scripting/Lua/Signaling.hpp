#pragma once
#include <functional>
#include <string>
#include <Scene/Entity.h>
#include <sol/sol.hpp>

class SignalBus {
public:
	using Callback = std::function<void(Entity sender, sol::table data)>;

	void Connect(const std::string& signalName, Entity listener, Callback callback);

	void Emit(const std::string& signalName, Entity sender, sol::table data);

	void Shutdown();

private:
	struct Subscriber {
		Entity listener;
		Callback callback;
	};

	std::unordered_map<std::string, std::vector<Subscriber>> m_Subscribers;
};
