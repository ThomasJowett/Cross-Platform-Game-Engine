#pragma once

class PlayPauseToolbar
{
public:
	void Render();
private:
	void PlayButton();
	void SimulateButton();
	void PauseButton();
	void StopButton();
	void RestartButton();
};