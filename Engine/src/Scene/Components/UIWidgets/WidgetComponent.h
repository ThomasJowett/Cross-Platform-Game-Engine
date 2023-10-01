#pragma once

#include "cereal/access.hpp"

struct WidgetComponent
{
	WidgetComponent() = default;
	WidgetComponent(const WidgetComponent& other) = default;

	bool fixedWidth = true;
	bool fixedHeight = true;

	Vector2f position = Vector2f(0.0f,0.0f);
	Vector2f size = Vector2f(100.0f, 100.0f);
	float rotation = 0.0f;

	float anchorLeft = 0.0f;
	float anchorTop = 0.0f;
	float anchorRight = 0.0f;
	float anchorBottom = 0.0f;

	float marginLeft = 0.0f;
	float marginTop = 0.0f;
	float marginRight = 100.0f;
	float marginBottom = 100.0f;

	//bool OnPressed();
	//bool OnReleased();
	//bool OnHovered();
	//bool OnUnHovered();

	void SetAnchorLeft(float left);
	void SetAnchorRight(float right);
	void SetAnchorTop(float top);
	void SetAnchorBottom(float bottom);

	void SetMarginLeft(float left);
	void SetMarginTop(float top);
	void SetMarginRight(float right);
	void SetMarginBottom(float bottom);

	void SetPositionX(float x);
	void SetPositionY(float y);

	void SetSizeX(float sizeX);
	void SetSizeY(float sizeY);

	Matrix4x4 GetTransformMatrix() const
	{
		return m_TransformMatrix;
	}

	void SetTransformMatrix(Matrix4x4 transformMatrix)
	{
		m_TransformMatrix = transformMatrix;
	}

	enum class WidgetState
	{
		normal,
		hovered,
		clicked,
		disabled
	}state;

	static const uint32_t s_referenceWidth = 1920;
	static const uint32_t s_referenceHeight = 1080;

private:
	Matrix4x4 m_TransformMatrix;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(fixedWidth, fixedHeight);
		archive(position, size, rotation);
		archive(anchorLeft, anchorTop, anchorRight, anchorBottom);
		archive(marginLeft, marginTop, marginRight, marginBottom);
	}
};
