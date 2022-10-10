#include "stdafx.h"
#include "AABB.h"

AABB::AABB(const Vector3f& min, const Vector3f& max)
	:m_Min(min), m_Max(max)
{
}

void AABB::Merge(const AABB& other)
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

void AABB::Merge(const Vector3f& point)
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

Vector3f AABB::Center() const
{
	return Vector3f();
}