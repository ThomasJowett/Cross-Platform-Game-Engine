#include "ImGuiVectorEdit.h"

#include "math/Vector2f.h"

#include "imgui/imgui_internal.h"

bool ImGui::Vector(const char* label, Vector2f& vector, float resetValue)
{
	return Vector(label, vector, Vector2f(resetValue, resetValue));
}

bool ImGui::Vector(const char* label, Vector2f& vector, const Vector2f& resetValue)
{
	bool edited = false;
	float width = CalcItemWidth();

	BeginGroup();
	TextColored({ 245,0,0,255 }, "X");
	SameLine();
	SetNextItemWidth(width / 2 - 20);
	std::string idX = "##" + std::string(label) + "X";
	if (DragFloat(idX.c_str(), &vector.x, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.x = resetValue.x;
		edited = true;
	}

	SameLine();
	TextColored({ 0,245,0,255 }, "Y");
	SameLine();
	SetNextItemWidth(width / 2 - 20);
	std::string idY = "##" + std::string(label) + "Y";
	if (DragFloat(idY.c_str(), &vector.y, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.y = resetValue.y;
		edited = true;
	}
	
	SameLine();
	TextUnformatted(label, FindRenderedTextEnd(label));
	EndGroup();
	return edited;
}

bool ImGui::Vector(const char* label, Vector3f& vector, float resetValue)
{
	return Vector(label, vector, Vector3f(resetValue, resetValue, resetValue));
}

bool ImGui::Vector(const char* label, Vector3f& vector, const Vector3f& resetValue) {
		bool edited = false;
	float width = CalcItemWidth();

	BeginGroup();
	TextColored({ 245,0,0,255 }, "X");
	SameLine();
	SetNextItemWidth(width / 3 - 20);
	std::string idX = "##" + std::string(label) + "X";
	if (DragFloat(idX.c_str(), &vector.x, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.x = resetValue.x;
		edited = true;
	}

	SameLine();
	TextColored({ 0,245,0,255 }, "Y");
	SameLine();
	SetNextItemWidth(width / 3 - 20);
	std::string idY = "##" + std::string(label) + "Y";
	if (DragFloat(idY.c_str(), &vector.y, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.y = resetValue.y;
		edited = true;
	}

	SameLine();
	TextColored({ 0,0,245,255 }, "Z");
	SameLine();
	SetNextItemWidth(width / 3 - 20);
	std::string idZ = "##" + std::string(label) + "Z";
	if (DragFloat(idZ.c_str(), &vector.z, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.z = resetValue.z;
		edited = true;
	}

	SameLine();
	TextUnformatted(label, FindRenderedTextEnd(label));
	EndGroup();
	return edited;
}
