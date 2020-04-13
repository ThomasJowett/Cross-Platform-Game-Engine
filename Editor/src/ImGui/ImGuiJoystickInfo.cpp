#include "ImGuiJoystickInfo.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

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
		if (ImGui::Begin("Joystick Info", m_Show))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			for (int i = 0; i < Joysticks::GetJoystickCount(); i++)
			{
				Joysticks::Joystick joystick = Joysticks::GetJoystick(i);

				ImGui::Text(joystick.Name);

				if (joystick.IsMapped)
				{
					int hat = 0;
					const char* button_names[] =
					{
						"A", "B", "X", "Y",
						"LB", "RB",
						"Back", "Start", "Guide",
						"Left Thumb", "Right Thumb"
					};

					const char* axis_names[] =
					{
						"Left X", "Left Y",
						"Right X", "Right Y",
						"Left Trigger", "Right Trigger"
					};

					for (int j = 0; j <= 5; j++)
					{
						float axisValue = (float)Input::GetJoyStickAxis(i, j);
						ImGui::SliderFloat(axis_names[j], &axisValue, -1.0f, 1.0f);
					}

					for (int j = 0; j <= 10; j++)
					{
						if (j > 0)
							ImGui::SameLine();

						ImGui::PushStyleColor(ImGuiCol_Button,
							Input::IsJoystickButtonPressed(i, j) ? ImVec4(0.9f, 0.9f, 0.9f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
						ImGui::Button(button_names[j]);
						ImGui::PopStyleColor();
					}

					//TODO:: show D-pad
				}
				else
				{
					for (int j = 0; j < joystick.Axes; j++)
					{
						float axisValue = (float)Input::GetJoyStickAxis(i, j);
						ImGui::SliderFloat(("Axis " + std::to_string(j)).c_str(), &axisValue, -1.0f, 1.0f);
					}


					for (int j = 0; j < joystick.Buttons + 1; j++)
					{
						if (j > 0)
							ImGui::SameLine();

						ImGui::PushStyleColor(ImGuiCol_Button,
							Input::IsJoystickButtonPressed(i, j) ? ImVec4(0.9f, 0.9f, 0.9f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
						ImGui::Button(std::to_string(j + 1).c_str());
						ImGui::PopStyleColor();
					}
				}
				ImGui::Separator();
			}
			ImGui::PopItemFlag();
		}
		ImGui::End();
	}
}
