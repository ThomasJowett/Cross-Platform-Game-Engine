#pragma once

#include <filesystem>

#include "Core/Layer.h"
#include "Events/ApplicationEvent.h"
#include "Logging/Instrumentor.h"
#include "Scripting/Lua/LuaErrorEvent.h"

// Process exit codes reported by SceneTest
enum class SceneTestResult
{
	Passed = 0,
	Failed = 1,
	ErrorCountMismatch = 2,
	NoVerdict = 3,
	TimedOut = 4
};

struct SceneTestOptions
{
	std::filesystem::path scene;
	uint64_t maxFrames = 600;
	int expectedErrors = 0;
	bool requireVerdict = false;
	bool display = false;
};

// Loads a scene, plays it, and records Test.* calls and Lua errors until the run ends
class SceneTestLayer : public Layer
{
public:
	explicit SceneTestLayer(const SceneTestOptions& options);

	virtual void OnAttach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& event) override;

	// Classifies the run, call once it has ended
	SceneTestResult GetResult() const;

	// Registers the Lua Test table
	static void BindTestTable(SceneTestLayer* layer);

private:
	bool OnLuaError(LuaErrorEvent& event);
	bool OnQuitRequested(AppQuitRequestedEvent& event);

	void Expect(bool condition, const std::string& message);
	void Pass(const std::string& message);
	void Fail(const std::string& message);

	SceneTestOptions m_Options;
	uint64_t m_Frames = 0;
	int m_LuaErrors = 0;
	int m_FailedExpectations = 0;
	bool m_SceneLoaded = false;
	bool m_Passed = false;
	bool m_Failed = false;
};
