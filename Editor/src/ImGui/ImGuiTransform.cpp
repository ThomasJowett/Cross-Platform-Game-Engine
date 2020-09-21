#include "ImGuiTransform.h"

#include  "IconsFontAwesome5.h"

#include "Engine.h"

bool ImGui::Transform(Matrix4x4& transform)
{
	bool edited = false;
	Vector3f position = transform.ExtractTranslation();
	Vector3f rotation = transform.ExtractRotation().EulerAngles();
	Vector3f scale = transform.ExtractScale();//TODO: fix extract scale
	//scale = Vector3f(1,1,1);

	ImGui::Text("Position");
	float width = ImGui::GetContentRegionAvailWidth();
	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##posX", &position.x, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##posY", &position.y, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##posZ", &position.z, 0.1f);

	//--------------------------------------
	ImGui::Text("Rotation");
	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##rotX", &rotation.x, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##RotY", &rotation.y, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##RotZ", &rotation.z, 0.1f);

	//--------------------------------------
	ImGui::Text("Scale");
	ImGui::SameLine();
	static bool locked = false;
	ImGui::Checkbox(ICON_FA_UNLOCK_ALT, &locked);
	ImGui::TextColored({ 245,0,0,255 }, "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##scaleX", &scale.x, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,245,0,255 }, "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##scaleY", &scale.y, 0.1f);

	ImGui::SameLine();
	ImGui::TextColored({ 0,0,245,255 }, "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width / 3 - 20);
	ImGui::DragFloat("##scaleZ", &scale.z, 0.1f);

	transform = Matrix4x4::Translate(position) * Matrix4x4::Rotate(rotation) * Matrix4x4::Scale(scale);

	return bool();
}
