#include "Signaling.hpp"

void SignalBus::Connect(const std::string& signalName, Entity listener, Callback callback)
{
	m_Subscribers[signalName].emplace_back(Subscriber{ listener, std::move(callback) });
}

void SignalBus::Emit(const std::string& signalName, Entity sender, sol::table data)
{
	auto it = m_Subscribers.find(signalName);
	if (it != m_Subscribers.end())
	{
		for (const auto& subscriber : it->second)
		{
			if (!subscriber.listener.IsSceneValid())
				continue;
			subscriber.callback(sender, data);
		}
	}
}
