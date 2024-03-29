#pragma once

#include "math/Vector3f.h"
#include "math/Matrix.h"

#include "cereal/cereal.hpp"

struct TransformComponent
{
	Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f rotation = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f);

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const Vector3f & position)
		:position(position) {}
	TransformComponent(const Vector3f& position, const Vector3f& rotation, const Vector3f& scale)
		:position(position), rotation(rotation), scale(scale) {}

	const Matrix4x4& GetWorldMatrix() const
	{
		return m_WorldMatrix;
	}

	const Vector3f GetWorldPosition() const
	{
		return m_ParentMatrix * position;
	}

	const Vector3f GetWorldRotation() const
	{
		return m_ParentMatrix.ExtractRotation().EulerAngles() + rotation;
	}

	Matrix4x4 GetLocalMatrix() const
	{
		return Matrix4x4::Translate(position) * Matrix4x4::Rotate(Quaternion(rotation)) * Matrix4x4::Scale(scale);
	}

	void SetWorldMatrix(Matrix4x4 parentMatrix)
	{ 
		m_ParentMatrix = parentMatrix;
		m_WorldMatrix = parentMatrix * GetLocalMatrix();
	}

	const Matrix4x4& GetParentMatrix() const { return m_ParentMatrix; }

private:
	Matrix4x4 m_WorldMatrix;
	Matrix4x4 m_ParentMatrix;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(position, rotation, scale);
	}
};