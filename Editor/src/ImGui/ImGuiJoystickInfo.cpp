#include "ImGuiJoystickInfo.h"

#include "imgui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "include.h"

ImGuiJoystickInfo::ImGuiJoystickInfo(bool* show)
	:m_Show(show), Layer("JoystickInfo")
{
}

void ImGuiJoystickInfo::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Joystick Info", m_Show))
	{
		if (Joysticks::GetJoystickCount() == 0)
		{
			ImGui::Text("No Joysticks Connected");
			ImGui::End();
			return;
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		for (int i = 0; i < Joysticks::GetJoystickCount(); i++)
		{
			Joysticks::Joystick joystick = Joysticks::GetJoystick(i);

			ImGui::Text(joystick.Name);

			if (joystick.IsMapped)
			{
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

				for (int j = 1; j <= joystick.Hats; j++)
				{

					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					const ImVec2 p = ImGui::GetCursorScreenPos();
					const ImU32 col = Colour(Colours::WHITE).HexValue();
					float radius = 24.0f;
					float x = p.x + (radius * j), y = p.y + (radius * j);

					draw_list->AddCircle(ImVec2(x, y), radius + 0.5f, col, 24, 2.0f);

					int hat = 0;
					if (Input::IsJoystickButtonPressed(i, GAMEPAD_BUTTON_DPAD_UP))
						hat |= 1;
					if (Input::IsJoystickButtonPressed(i, GAMEPAD_BUTTON_DPAD_RIGHT))
						hat |= 2;
					if (Input::IsJoystickButtonPressed(i, GAMEPAD_BUTTON_DPAD_DOWN))
						hat |= 4;
					if (Input::IsJoystickButtonPressed(i, GAMEPAD_BUTTON_DPAD_LEFT))
						hat |= 8;

					if (hat != 0)
					{
						const float angles[] =
						{
							0.f,        0.f,
							(float)PI * 1.5f,  (float)PI * 1.75f,
							(float)PI,         0.f,
							(float)PI * 1.25f, 0.f,
							(float)PI * 0.5f,  (float)PI * 0.25f,
							0.f,        0.f,
							(float)PI * 0.75f, 0.f,
						};

						const float cosa = cos(angles[hat]);
						const float sina = sin(angles[hat]);
						const struct ImVec2 p0 = ImVec2(0.f, -radius);
						const struct ImVec2 p1 = ImVec2(radius / 2.f, -radius / 3.f);
						const struct ImVec2 p2 = ImVec2(-radius / 2.f, -radius / 3.f);


						if (j != 1)
							ImGui::SameLine();
						draw_list->AddTriangleFilled(
							ImVec2(x + cosa * p0.x + sina * p0.y, y + cosa * p0.y - sina * p0.x),
							ImVec2(x + cosa * p1.x + sina * p1.y, y + cosa * p1.y - sina * p1.x),
							ImVec2(x + cosa * p2.x + sina * p2.y, y + cosa * p2.y - sina * p2.x),
							col);
					}

					if (j == 1)
						ImGui::Dummy(ImVec2(radius * 2, radius * 2));
				}
			}
			else
			{
				for (int j = 0; j < joystick.Axes; j++)
				{
					float axisValue = (float)Input::GetJoyStickAxis(i, j);
					ImGui::SliderFloat(("Axis " + std::to_string(j)).c_str(), &axisValue, -1.0f, 1.0f);
				}


				for (int j = 0; j < joystick.Buttons; j++)
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
