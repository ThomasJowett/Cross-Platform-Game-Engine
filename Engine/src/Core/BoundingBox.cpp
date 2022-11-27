#include "stdafx.h"
#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
	Invalidate();
}

BoundingBox::BoundingBox(const Vector3f& min, const Vector3f& max)
	:m_Min(min), m_Max(max)
{
}

void BoundingBox::EnclosePoints(const float* vertices, uint32_t vertexCount, int stride)
{
	if (vertexCount == 0)
	{
		m_Min = Vector3f();
		m_Max = Vector3f();
		return;
	}

	Invalidate();

	for (uint32_t i = 0; i < vertexCount; i += stride)
	{
		Merge(Vector3f(vertices[i], vertices[i + 1], vertices[i + 2]));
	}
}

void BoundingBox::Merge(const BoundingBox& other)
{
	if (other.m_Min.x < m_Min.x)
		m_Min.x = other.m_Min.x;
	if (other.m_Min.y < m_Min.y)
		m_Min.y = other.m_Min.y;
	if (other.m_Min.z < m_Min.z)
		m_Min.z = other.m_Min.z;
	if (other.m_Max.x > m_Max.x)
		m_Max.x = other.m_Max.x;
	if (other.m_Max.y > m_Max.y)
		m_Max.y = other.m_Max.y;
	if (other.m_Max.z > m_Max.z)
		m_Max.z = other.m_Max.z;
}

void BoundingBox::Merge(const Vector3f& point)
{
	if (point.x < m_Min.x)
		m_Min.x = point.x;
	if (point.y < m_Min.y)
		m_Min.y = point.y;
	if (point.z < m_Min.z)
		m_Min.z = point.z;
	if (point.x > m_Max.x)
		m_Max.x = point.x;
	if (point.y > m_Max.y)
		m_Max.y = point.y;
	if (point.z > m_Max.z)
		m_Max.z = point.z;
}

Vector3f BoundingBox::Center() const
{
	return (m_Min - m_Max) * 0.5f;
}

void BoundingBox::Invalidate()
{
	m_Min.x = FLT_MAX;
	m_Min.y = FLT_MAX;
	m_Min.z = FLT_MAX;

	m_Max.x = FLT_MIN;
	m_Max.y = FLT_MIN;
	m_Max.z = FLT_MIN;
}

bool BoundingBox::IsValid() const
{
	return m_Min.x <= m_Max.x;
}
