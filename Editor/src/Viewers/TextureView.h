#pragma once
#include "include.h"
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
	bool* m_Show;

	std::filesystem::path m_FilePath;

	Ref<Texture2D> m_Texture;

	std::string m_WindowName;
};