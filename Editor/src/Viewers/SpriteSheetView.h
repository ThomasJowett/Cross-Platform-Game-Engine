#pragma once

#include "ViewerManager.h"
#include "Renderer/SpriteSheet.h"
#include "Interfaces/ISaveable.h"

class SpriteSheetView :
	public View, public ISaveable
{
public:
	SpriteSheetView(bool* show, std::filesystem::path filepath);
	~SpriteSheetView() = default;

	virtual void OnAttach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnImGuiRender() override;
	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override { return m_Dirty; }
private:
	static bool CompareName(std::pair<std::string,Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareStartFrame(std::pair<std::string,Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareFrameCount(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareHoldTime(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareNameReverse(std::pair<std::string,Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareStartFrameReverse(std::pair<std::string,Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareFrameCountReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second);
	static bool CompareHoldTimeReverse(std::pair<std::string, Animation*>& first, std::pair<std::string, Animation*>& second);

	void GetListOfAnimations();
	bool* m_Show;

	std::filesystem::path m_Filepath;

	Ref<SpriteSheet> m_SpriteSheet;
	Ref<SpriteSheet> m_LocalSpriteSheet;

	float m_Zoom = 1.0f;
	bool m_Dirty = false;

	std::vector<std::vector<bool>> m_SelectedFrames;
	int m_SelectedAnimation = 0;
	std::vector<std::pair<std::string, Animation*>> m_AnimationsSorted;

	char m_InputBuffer[1024] = "";
};
