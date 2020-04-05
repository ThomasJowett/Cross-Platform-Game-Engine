#include "ImGuiJoystickInfo.h"

#include "ImGui/imgui.h"

#include "include.h"

ImGuiJoystickInfo::ImGuiJoystickInfo(bool* show)
	:m_Show(show), Layer("JoystickInfo")
{
}

void ImGuiJoystickInfo::OnImGuiRender()
{
	if (*m_Show)
	{
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		ImGui::Begin("Joystick Info", m_Show);
		{
			for (int i = 0; i < Joysticks::GetJoystickCount(); i++)
			{
				Joysticks::Joystick joystick = Joysticks::GetJoystick(i);

				for (int j = 0; j < joystick.axes; j++)
				{
					float axisValue = (float)Input::GetJoyStickAxis(i, j);
					ImGui::SliderFloat(std::to_string(j).c_str(), &axisValue, -1.0f, 1.0f);
				}

				for (int j = 0; j < joystick.buttons; j++)
				{

				}
			}
		}
		ImGui::End();
	}
}
