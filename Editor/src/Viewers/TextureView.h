#pragma once
#include "Engine.h"
#include <filesystem>

class TextureView :
	public Layer
{
public :
	TextureView(bool* show, const std::filesystem::path& filepath);
	~TextureView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

private:
	static const char* GetWrapMethodName(Texture::WrapMethod wrappingMethod);
	static const char* GetFilterMethodName(Texture::FilterMethod filterMethod);

private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	Ref<Texture2D> m_Texture;

	std::string m_WindowName;

	float m_Zoom = 1.0f;
};