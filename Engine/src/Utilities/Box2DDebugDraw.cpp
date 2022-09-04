#include "stdafx.h"
#include "Box2DDebugDraw.h"
#include "Renderer/Renderer2D.h"

void Box2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<Vector3f> verticesVec;
	for (size_t i = 0; i < vertexCount; i++)
	{
		verticesVec.push_back(Vector3f(vertices[i].x, vertices[i].y, 1.0f));
	}
	Renderer2D::DrawHairLinePolygon(verticesVec, Colour(color.r, color.g, color.b, 1.0f));
}

void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<Vector3f> verticesVec;
	for (size_t i = 0; i < vertexCount; i++)
	{
		verticesVec.push_back(Vector3f(vertices[i].x, vertices[i].y, 1.0f));
	}
	Renderer2D::DrawHairLinePolygon(verticesVec, Colour(color.r, color.g, color.b, 1.0f));
}

void Box2DDebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	Matrix4x4 transform = Matrix4x4::Translate(Vector3f(center.x, center.y, 0.0f));
	Renderer2D::DrawHairLineCircle(transform, 60, Colour(color.r, color.g, color.b, 1.0f));
}

void Box2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	Matrix4x4 transform = Matrix4x4::Translate(Vector3f(center.x, center.y, 1.0f)) * Matrix4x4::Scale(Vector3f(radius, radius, 1.0f));
	Renderer2D::DrawHairLineCircle(transform, 60, Colour(color.r, color.g, color.b, 1.0f));
}

void Box2DDebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	Matrix4x4 transform = Matrix4x4::Translate(Vector3f(p.x, p.y, 1.0f)) * Matrix4x4::Scale(Vector3f(size, size, 1.0f));
	Renderer2D::DrawQuad(transform, Colour(color.r, color.g, color.b, 1.0f));
}
