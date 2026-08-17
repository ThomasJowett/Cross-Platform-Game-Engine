#include "TextureView.h"

#include "ImGui/ImGuiUtilities.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "Utilities/AssetReferenceUtils.h"

TextureView::TextureView(bool* show, const std::filesystem::path& filepath)
	:View("TextureView"), m_Show(show), m_FilePath(filepath)
{

}
void TextureView::OnAttach()
{
	m_Texture = AssetManager::GetTexture(m_FilePath);

	m_WindowName = ICON_FA_IMAGE + std::string(" ") + m_FilePath.filename().string() + "##" + m_Texture->GetUUID().to_string();

	if (m_Texture->GetWidth() <= 32 || m_Texture->GetHeight() <= 32)
		m_Zoom = 2.0f;
	if (m_Texture->GetWidth() <= 16 || m_Texture->GetHeight() <= 16)
		m_Zoom = 4.0f;
}
void TextureView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	ImVec2 displaySize = ImVec2((float)m_Texture->GetWidth() * m_Zoom, (float)m_Texture->GetHeight() * m_Zoom);

	ImGui::SetNextWindowSize(ImVec2(std::max(displaySize.x, 400.0f), std::max(displaySize.y + 200, 400.0f)), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		std::string size = "Size: " + std::to_string(m_Texture->GetWidth()) + "x" + std::to_string(m_Texture->GetHeight());
		ImGui::TextUnformatted(size.c_str());
		auto absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_Texture->GetFilepath());
		ImGui::TextUnformatted(absolutePath.string().c_str());

		if (m_Texture->GetChannels() == 3)
		{
			if(ImGui::Button("Convert to 4 channel image"))
			{
				int width, height, original_channels;
				stbi_set_flip_vertically_on_load(1);
				unsigned char* data = stbi_load(absolutePath.string().c_str(), &width, &height, &original_channels, STBI_rgb_alpha);

				if (data == nullptr) {
					CLIENT_ERROR("Failed to load image: {0}", stbi_failure_reason());
				}
				else
				{
					std::filesystem::path oldRelativePath = m_Texture->GetFilepath();
					std::filesystem::path newRelativePath = oldRelativePath;
					newRelativePath.replace_extension(".png");
					std::filesystem::path newAbsolutePath = absolutePath;
					newAbsolutePath.replace_extension(".png");

					stbi_flip_vertically_on_write(1);
					int success = stbi_write_png(newAbsolutePath.string().c_str(), width, height, 4, data, width * 4);

					stbi_image_free(data);

					if (success && newRelativePath != oldRelativePath)
					{
						std::filesystem::remove(absolutePath);

						std::vector<std::filesystem::path> updatedFiles = AssetReferenceUtils::UpdateReferences(oldRelativePath, newRelativePath);
						AssetManager::RemoveAsset(oldRelativePath);
						AssetReferenceUtils::UpdateCurrentSceneTextureReferences(oldRelativePath, newRelativePath);
						AssetReferenceUtils::ReloadAffectedNonSceneAssets(updatedFiles);

						m_FilePath = newRelativePath;
						m_Texture = AssetManager::GetTexture(m_FilePath);
						m_WindowName = ICON_FA_IMAGE + std::string(" ") + m_FilePath.filename().string() + "##" + m_Texture->GetUUID().to_string();
					}
					else
					{
						m_Texture->Reload();
					}
				}
			}
			ImGui::Tooltip("3 channels images are not supported by WebGPU");
		}

		const bool is_selected = false;
		bool edited = false;
		if (ImGui::BeginCombo("Wrap Method", GetWrapMethodName(m_Texture->GetWrapMethod())))
		{
			if (ImGui::Selectable(GetWrapMethodName(Texture::WrapMethod::Clamp), is_selected))
			{
				m_Texture->SetWrapMethod(Texture::WrapMethod::Clamp);
				edited = true;
			}
			if (ImGui::Selectable(GetWrapMethodName(Texture::WrapMethod::Mirror), is_selected))
			{
				m_Texture->SetWrapMethod(Texture::WrapMethod::Mirror);
				edited = true;
			}
			if (ImGui::Selectable(GetWrapMethodName(Texture::WrapMethod::Repeat), is_selected))
			{
				m_Texture->SetWrapMethod(Texture::WrapMethod::Repeat);
				edited = true;
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Filter Method", GetFilterMethodName(m_Texture->GetFilterMethod())))
		{
			if (ImGui::Selectable(GetFilterMethodName(Texture::FilterMethod::Linear), is_selected))
			{
				m_Texture->SetFilterMethod(Texture::FilterMethod::Linear);
				edited = true;
			}
			if (ImGui::Selectable(GetFilterMethodName(Texture::FilterMethod::Nearest), is_selected))
			{
				m_Texture->SetFilterMethod(Texture::FilterMethod::Nearest);
				edited = true;
			}
			ImGui::EndCombo();
		}
		if (edited)
		{
			m_Texture->Reload();
		}

		ImGui::SliderFloat("Zoom", &m_Zoom, 0.25f, 8.0f, "%.2f");

		ImGuiWindowFlags window_flags_image = ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
		ImGui::BeginChild("Image", ImGui::GetContentRegionAvail(), false, window_flags_image);
		ImGui::Image(m_Texture, displaySize);
		ImGui::EndChild();
	}
	ImGui::End();
}

const char* TextureView::GetWrapMethodName(Texture::WrapMethod wrappingMethod)
{
	switch (wrappingMethod)
	{
	case Texture::WrapMethod::Clamp: return "Clamp";
	case Texture::WrapMethod::Mirror: return "Mirror";
	case Texture::WrapMethod::Repeat: return "Repeat";
	default: return "None";
	}
}

const char* TextureView::GetFilterMethodName(Texture::FilterMethod filterMethod)
{
	switch (filterMethod)
	{
	case Texture::FilterMethod::Linear: return "Linear";
	case Texture::FilterMethod::Nearest: return "Nearest";
	default: return "None";
	}
}
