#pragma once
#include <filesystem>

#include "Core/Layer.h"
#include "Renderer/Texture.h"
#include "ViewerManager.h"

class TextureView :
	public View
{
public :
	TextureView(bool* show, const std::filesystem::path& filepath);
	~TextureView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

	const std::string& GetWindowName() { return m_WindowName; }

private:
	static const char* GetWrapMethodName(Texture::WrapMethod wrappingMethod);
	static const char* GetFilterMethodName(Texture::FilterMethod filterMethod);

private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	Ref<Texture2D> m_Texture;

	float m_Zoom = 1.0f;
};