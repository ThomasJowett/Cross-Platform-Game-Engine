#include "SceneTestLayer.h"

#include "Core/Application.h"
#include "Scene/SceneManager.h"
#include "Scripting/Lua/LuaManager.h"
#include "Scripting/Lua/LuaBindings.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

SceneTestLayer::SceneTestLayer(const SceneTestOptions& options)
	:Layer("SceneTest"), m_Options(options)
{
}

void SceneTestLayer::OnAttach()
{
	// Loaded here rather than before Run(), so errors raised while the scripts load are counted
	SceneManager::ChangeSceneState(SceneState::Play);
	m_SceneLoaded = SceneManager::ChangeScene(m_Options.scene);
	if (!m_SceneLoaded)
	{
		CLIENT_ERROR("RESULT: FAIL could not load scene {0}", m_Options.scene.string());
		Application::RequestQuit();
	}
}

void SceneTestLayer::OnUpdate(float deltaTime)
{
	if (++m_Frames == m_Options.maxFrames)
	{
		CLIENT_INFO("SceneTest: reached the frame limit of {0}", m_Options.maxFrames);
		Application::RequestQuit();
	}

	if (m_Options.display)
	{
		RenderCommand::Clear();
		Renderer2D::ResetStats();
		Renderer::RenderScene(SceneManager::CurrentScene());
	}
}

void SceneTestLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<LuaErrorEvent>(BIND_EVENT_FN(SceneTestLayer::OnLuaError));
	dispatcher.Dispatch<AppQuitRequestedEvent>(BIND_EVENT_FN(SceneTestLayer::OnQuitRequested));
}

bool SceneTestLayer::OnLuaError(LuaErrorEvent& event)
{
	++m_LuaErrors;
	CLIENT_WARN("SceneTest: Lua error {0}: {1}", m_LuaErrors, event.to_string());
	return false;
}

bool SceneTestLayer::OnQuitRequested(AppQuitRequestedEvent& event)
{
	// Stop the scene while the application is still running, so errors in OnDestroy are counted
	SceneManager::Shutdown();
	Application::Get().Close();
	return true;
}

void SceneTestLayer::Expect(bool condition, const std::string& message)
{
	if (condition)
		CLIENT_INFO("RESULT: OK {0}", message);
	else
	{
		++m_FailedExpectations;
		CLIENT_ERROR("RESULT: FAIL {0}", message);
	}
}

void SceneTestLayer::Pass(const std::string& message)
{
	CLIENT_INFO("RESULT: OK {0}", message);
	m_Passed = true;
	Application::RequestQuit();
}

void SceneTestLayer::Fail(const std::string& message)
{
	CLIENT_ERROR("RESULT: FAIL {0}", message);
	m_Failed = true;
	Application::RequestQuit();
}

SceneTestResult SceneTestLayer::GetResult() const
{
	CLIENT_INFO("SceneTest: {0} frames, {1} failed expectations, {2} of {3} expected Lua errors",
		m_Frames, m_FailedExpectations, m_LuaErrors, m_Options.expectedErrors);

	if (!m_SceneLoaded || m_Failed || m_FailedExpectations > 0)
		return SceneTestResult::Failed;
	if (m_LuaErrors != m_Options.expectedErrors)
	{
		CLIENT_ERROR("SceneTest: expected {0} Lua errors but {1} were raised", m_Options.expectedErrors, m_LuaErrors);
		return SceneTestResult::ErrorCountMismatch;
	}
	if (m_Options.requireVerdict && !m_Passed)
	{
		CLIENT_ERROR("SceneTest: the scene ended without calling Test.Pass or Test.Fail");
		return SceneTestResult::NoVerdict;
	}
	return SceneTestResult::Passed;
}

static std::string ToString(const sol::object& value)
{
	return LuaManager::GetState()["tostring"](value);
}

void SceneTestLayer::BindTestTable(SceneTestLayer* layer)
{
	sol::state& state = LuaManager::GetState();
	sol::table test = state.create_named_table("Test");
	LuaManager::AddIdentifier("Test", "Assertions for automated scene tests, only available under SceneTest");

	Lua::SetFunction(test, "Test", "Expect", "Record a failure if the condition is false, and keep running",
		[layer](sol::object condition, sol::optional<std::string> message)
		{
			// Lua truthiness: only nil and false fail
			bool truthy = condition.get_type() != sol::type::lua_nil
				&& !(condition.get_type() == sol::type::boolean && !condition.as<bool>());
			layer->Expect(truthy, message.value_or("Test.Expect"));
		});
	Lua::SetFunction(test, "Test", "Equal", "Record a failure if the two values are not equal, and keep running",
		[layer](sol::this_state s, sol::object actual, sol::object expected, sol::optional<std::string> message)
		{
			lua_State* L = s;
			actual.push(L);
			expected.push(L);
			bool equal = lua_compare(L, -2, -1, LUA_OPEQ) != 0;
			lua_pop(L, 2);

			std::string text = message.value_or("Test.Equal");
			if (!equal)
				text += " (expected " + ToString(expected) + ", got " + ToString(actual) + ")";
			layer->Expect(equal, text);
		});
	Lua::SetFunction(test, "Test", "Pass", "End the run as passed, unless an expectation has failed",
		[layer](sol::optional<std::string> message) { layer->Pass(message.value_or("Test.Pass")); });
	Lua::SetFunction(test, "Test", "Fail", "End the run as failed",
		[layer](sol::optional<std::string> message) { layer->Fail(message.value_or("Test.Fail")); });
	Lua::SetFunction(test, "Test", "ExpectErrors", "Declare how many Lua errors this scene raises on purpose",
		[layer](int count) { layer->m_Options.expectedErrors = count; });
}
