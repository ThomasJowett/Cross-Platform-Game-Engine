#pragma once

#include <filesystem>
#include "Core/Layer.h"

struct Error
{
	std::filesystem::path filepath;
	uint32_t lineNumber;
	std::string message;

	Error(const std::filesystem::path& path, uint32_t lineNumber, const std::string& message)
		:filepath(path), lineNumber(lineNumber), message(message) {}
};

class ErrorListPanel :
	public Layer
{
public:
	explicit ErrorListPanel(bool* show);
	~ErrorListPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(Event& event) override;

	void AddError(Error& error);
	void ClearAllErrors();
private:
	bool* m_Show;

	std::vector<std::pair<Error, bool>> m_ErrorList;
	uint32_t m_NumberSelected = 0;
};
