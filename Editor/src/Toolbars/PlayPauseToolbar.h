#pragma once

class PlayPauseToolbar
{
public:
	static void Render();
private:
	static void PlayButton();
	static void SimulateButton();
	static void PauseButton();
	static void StopButton();
	static void RestartButton();
};