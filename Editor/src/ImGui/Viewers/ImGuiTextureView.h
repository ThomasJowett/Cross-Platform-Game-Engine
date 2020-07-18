#pragma once
#include "include.h"
#include <filesystem>

class ImGuiTextureView :
	public Layer
{
public :
	ImGuiTextureView(bool* show, const std::filesystem::path& filepath);
	~ImGuiTextureView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	Ref<Texture2D> m_Texture;

	std::string m_WindowName;
};

