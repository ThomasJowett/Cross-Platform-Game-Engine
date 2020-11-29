#pragma once

#include "VertexArray.h"

#include "cereal/access.hpp"

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::filesystem::path& filepath);
	Mesh(Ref<VertexArray> vertexArray, std::string name);
	~Mesh() = default;
	void LoadModel(Ref<VertexArray> vertexArray, std::string name);
	void LoadModel(const std::filesystem::path& filepath);
	void LoadModel();

	std::filesystem::path& GetFilepath() { return m_Filepath; }

	const Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
private:
	Ref<VertexArray> m_VertexArray;
	std::string m_Name;
	std::filesystem::path m_Filepath;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		LoadModel();
	}
};