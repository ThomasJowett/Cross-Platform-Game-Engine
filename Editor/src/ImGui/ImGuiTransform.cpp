#include "ImGuiTransform.h"

#include  "IconsFontAwesome5.h"
#include "Scene/SceneManager.h"

#include "Engine.h"

bool ImGui::Transform(Vector3f& position, Vector3f& rotation, Vector3f& scale)
{
	bool edited = false;

	ImGui::Text("Position");
	float width = ImGui::GetContentRegionAvailWidth();

	float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::TextColored({ 245,0,0,255 }, "X");
	//ImGui::PushStyleColor(ImGuiCol_Button, { 225,0,0,255 });
	//if (ImGui::Button("X", buttonSize))
	//	position.x = 0.0f;
	//ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posX", &position.x, 0.1f))
		edited = true;

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posY", &position.y, 0.1f))
		edited = true;

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posZ", &position.z, 0.1f))
		edited = true;

	//--------------------------------------

	Vector3f rotationDegrees((float)RadToDeg(rotation.x), (float)RadToDeg(rotation.y), (float)RadToDeg(rotation.z));
	ImGui::Text("Rotation");
	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##rotX", &rotationDegrees.x, 0.1f))
		edited = true;

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##RotY", &rotationDegrees.y, 0.1f))
		edited = true;

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##RotZ", &rotationDegrees.z, 0.1f))
		edited = true;

	//--------------------------------------
	ImGui::Text("Scale");
	ImGui::SameLine();
	static bool locked = true;
	Vector3f lockedScale = scale;
	if (locked)
	{
		ImGui::Checkbox(ICON_FA_LOCK, &locked);
	}
	else
		ImGui::Checkbox(ICON_FA_UNLOCK, &locked);

	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (locked)
	{
		if (ImGui::DragFloat("##scaleX", &lockedScale.x, 0.1f))
		{
			edited = true;
			float difference = lockedScale.x - scale.x;
			scale.x += difference;
			scale.y += difference;
			scale.z += difference;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleX", &scale.x, 0.1f))
			edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (locked)
	{
		if (ImGui::DragFloat("##scaleY", &lockedScale.y, 0.1f))
		{
			edited = true;
			float difference = lockedScale.y - scale.y;
			scale.x += difference;
			scale.y += difference;
			scale.z += difference;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleY", &scale.y, 0.1f))
			edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (locked)
	{
		if (ImGui::DragFloat("##scaleZ", &lockedScale.z, 0.1f))
		{
			edited = true;
			float difference = lockedScale.z - scale.z;
			scale.x += difference;
			scale.y += difference;
			scale.z += difference;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleZ", &scale.z, 0.1f))
			edited = true;
	}

	if (edited)
	{
		rotation = { (float)DegToRad(rotationDegrees.x), (float)DegToRad(rotationDegrees.y), (float)DegToRad(rotationDegrees.z) };
		SceneManager::CurrentScene()->MakeDirty();
	}

	return bool();
}