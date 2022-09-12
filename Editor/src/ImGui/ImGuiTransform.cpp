#include "ImGuiTransform.h"

#include  "IconsFontAwesome5.h"
#include "Scene/SceneManager.h"

#include "math/Matrix.h"

bool ImGui::Transform(Vector3f& position, Vector3f& rotation, Vector3f& scale)
{
	bool edited = false;

	ImGui::TextUnformatted("Position");
	float width = ImGui::GetContentRegionAvail().x;

	float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posX", &position.x, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		position.x = 0.0f;
		edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posY", &position.y, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		position.y = 0.0f;
		edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##posZ", &position.z, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		position.z = 0.0f;
		edited = true;
	}

	//--------------------------------------

	Vector3f rotationDegrees((float)RadToDeg(rotation.x), (float)RadToDeg(rotation.y), (float)RadToDeg(rotation.z));
	ImGui::TextUnformatted("Rotation");
	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##rotX", &rotationDegrees.x, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		rotationDegrees.x = 0.0f;
		edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##RotY", &rotationDegrees.y, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		rotationDegrees.y = 0.0f;
		edited = true;
	}

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	if (ImGui::DragFloat("##RotZ", &rotationDegrees.z, 0.1f))
		edited = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		rotationDegrees.z = 0.0f;
		edited = true;
	}

	//--------------------------------------
	ImGui::TextUnformatted("Scale");
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
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.x = scale.y = scale.z = 1.0f;
			edited = true;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleX", &scale.x, 0.1f))
			edited = true;
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.x = 1.0f;
			edited = true;
		}
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
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.x = scale.y = scale.z = 1.0f;
			edited = true;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleY", &scale.y, 0.1f))
			edited = true;
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.y = 1.0f;
			edited = true;
		}
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
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.x = scale.y = scale.z = 1.0f;
			edited = true;
		}
	}
	else
	{
		if (ImGui::DragFloat("##scaleZ", &scale.z, 0.1f))
			edited = true;
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			scale.z = 1.0f;
			edited = true;
		}
	}

	if (edited)
	{
		rotation = { (float)DegToRad(rotationDegrees.x), (float)DegToRad(rotationDegrees.y), (float)DegToRad(rotationDegrees.z) };
		SceneManager::CurrentScene()->MakeDirty();
	}

	return edited;
}