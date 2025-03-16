#pragma once

#include "ViewerManager.h"

#include <filesystem>

class AudioClip;

class AudioView :
	public View
{
public:
	AudioView(bool* show, std::filesystem::path filepath);
	~AudioView() = default;
	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

private:
	bool* m_Show;
	std::filesystem::path m_Filepath;
	Ref<AudioClip> m_Audio;
};