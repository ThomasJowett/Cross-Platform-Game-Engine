#pragma once

#include "ViewerManager.h"

#include <filesystem>

class AudioClip;
struct ma_sound;
struct ma_engine;

class AudioView :
	public View
{
public:
	AudioView(bool* show, std::filesystem::path filepath);
	~AudioView();
	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

private:
	bool* m_Show;
	std::filesystem::path m_Filepath;
	Ref<AudioClip> m_Audio;

	std::vector<float> m_VolumeHistory;
	bool m_Playing = false;
	float m_PlaybackTime = 0.0f;
	float m_Length = 0.0f;
	Ref<ma_sound> m_Sound;

	Ref<ma_engine> m_AudioEngine;
};