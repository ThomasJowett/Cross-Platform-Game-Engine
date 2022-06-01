#include "ImGuiVectorEdit.h"

#include "math/Vector2f.h"

#include "Engine.h"

bool ImGui::Vector(const char* label, Vector2f& vector, float resetValue)
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
		vector.x = resetValue;
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
		vector.y = resetValue;
		edited = true;
	}
	
	SameLine();
	Text(label);
	EndGroup();
    return edited;
}

bool ImGui::Vector(const char* label, Vector3f& vector, float resetValue)
{
	bool edited = false;
	float width = CalcItemWidth();
	TextColored({ 245,0,0,255 }, "X");
	SameLine();
	SetNextItemWidth(width / 3 - 20);
	std::string idX = "##" + std::string(label) + "X";
	if (DragFloat(idX.c_str(), &vector.x, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.x = resetValue;
		edited = true;
	}

	SameLine();
	TextColored({ 0,245,0,255 }, "Y");
	SameLine();
	SetNextItemWidth(width / 3 - 20);
	std::string idY = "##" + std::string(label) + "Y";
	if (DragFloat(idY.c_str(), &vector.x, 0.1f))
		edited = true;
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Right))
	{
		vector.y = resetValue;
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
		vector.z = resetValue;
		edited = true;
	}

	SameLine();
	Text(label);
	return edited;
}
