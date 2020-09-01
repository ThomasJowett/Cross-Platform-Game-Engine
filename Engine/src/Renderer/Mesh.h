#pragma once

#include "VertexArray.h"

class Mesh
{
public:
	Mesh() = default;
	Mesh(std::filesystem::path filepath);
	Mesh(Ref<VertexArray> vertexArray, std::string name);
	~Mesh() = default;
	void LoadModel(Ref<VertexArray> vertexArray, std::string name);
	void LoadModel(std::filesystem::path filepath);

	const Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
private:
	Ref<VertexArray> m_VertexArray;
	std::string m_Name;
};