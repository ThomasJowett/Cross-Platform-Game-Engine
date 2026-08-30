#include "WebGPUShader.h"
#include "Core/core.h"
#include "Logging/Instrumentor.h"
#include "Scene/AssetManager.h"
#include "WebGPUContext.h"

#include <filesystem>
#include <fstream>

WebGPUShader::WebGPUShader(const std::string& name, const std::filesystem::path& fileDirectory, bool postProcess) : m_Name(name)
{
	PROFILE_FUNCTION();
	if (!LoadShaderFromDisk(fileDirectory / name))
	{
		if (!LoadShaderFromBundle(fileDirectory / name))
		{
			ENGINE_ERROR("Could not load shader from file: {0}", (fileDirectory / name).string());
		}
	}
}

WebGPUShader::~WebGPUShader()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
	{
	}
}

void WebGPUShader::Bind() const { PROFILE_FUNCTION(); }

void WebGPUShader::UnBind() const { PROFILE_FUNCTION(); }

bool WebGPUShader::LoadShaderFromDisk(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	if (filepath.empty())
		return false;

	std::filesystem::path shaderPath = filepath;

	shaderPath.replace_extension(".wgsl");

	if (std::filesystem::exists(shaderPath))
	{
		std::string shaderSource = ReadFile(shaderPath);

		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc{};
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
		shaderCodeDesc.code = shaderSource.c_str();
		wgpu::ShaderModuleDescriptor shaderDesc{};
#ifndef __EMSCRIPTEN__
		shaderDesc.hintCount = 0;
		shaderDesc.hints = nullptr;
#endif
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
		auto webGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

		auto device = webGPUContext->GetWebGPUDevice();
		m_Shader = device.createShaderModule(shaderDesc);
	}

	return m_Shader;
}

bool WebGPUShader::LoadShaderFromBundle(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	if (!AssetManager::HasBundle())
	{
		ENGINE_ERROR("Trying to load shader from bundle, but no Asset Bundle loaded");
		return false;
	}

	std::vector<uint8_t> data;
	std::filesystem::path shaderPath = filepath;

	shaderPath.replace_extension(".wgsl");
	if (AssetManager::FileExistsInBundle(shaderPath) && AssetManager::GetFileData(shaderPath, data))
	{
		std::string shaderSource(data.begin(), data.end());

		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc{};
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
		shaderCodeDesc.code = shaderSource.c_str();
		wgpu::ShaderModuleDescriptor shaderDesc{};
#ifndef __EMSCRIPTEN__
		shaderDesc.hintCount = 0;
		shaderDesc.hints = nullptr;
#endif
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
		auto webGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

		auto device = webGPUContext->GetWebGPUDevice();
		m_Shader = device.createShaderModule(shaderDesc);
	}
	return m_Shader;
}

std::string WebGPUShader::ReadFile(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::string result;
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (file)
	{
		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();
	}

	return result;
}