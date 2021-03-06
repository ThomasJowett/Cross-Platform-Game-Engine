#pragma once

#include <unordered_map>

#include "Core/core.h"

class Texture
{
public:
	virtual ~Texture() = default;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual std::string GetName()const = 0;

	virtual uint32_t GetRendererID()const = 0;


	virtual bool operator==(const Texture& other) const = 0;
};

class Texture2D :public Texture
{
public:
	static Ref<Texture2D> Create(uint32_t width, uint32_t height);
	static Ref<Texture2D> Create(const std::filesystem::path& filepath);
};

class TextureLibrary2D
{
public:
	void Add(const Ref<Texture2D>& texture);
	Ref<Texture2D> Load(const std::filesystem::path& path);
	Ref<Texture2D> Get(const std::string& name);

	bool Exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
};