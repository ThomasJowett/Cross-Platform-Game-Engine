#include <Events/Event.h>

class LuaErrorEvent : public Event
{
public:
	LuaErrorEvent(int line, const std::string& file, const std::string& errorMessage)
		: m_Line(line), m_File(file), m_Message(errorMessage) {
	}

	LuaErrorEvent(const std::string& file, const char* luaError)
		: m_File(file)
	{
		PROFILE_FUNCTION();
		std::string errorStr = luaError;
		int line = 1;
		size_t chunkStart = errorStr.find("[string ");
		if (chunkStart != std::string::npos) {
			// Example: [string "chunk"]:3:
			size_t closeQuote = errorStr.find("\"]:", chunkStart);
			if (closeQuote != std::string::npos) {
				size_t lineNumStart = closeQuote + 3;
				size_t lineNumEnd = errorStr.find(':', lineNumStart);
				if (lineNumEnd != std::string::npos) {
					std::string lineStr = errorStr.substr(lineNumStart, lineNumEnd - lineNumStart);
					try {
						line = std::stoi(lineStr);
					}
					catch (...) {
						line = 1;
					}
					errorStr = errorStr.substr(lineNumEnd + 1);
				}
			}
		}

		m_Line = line;
		m_Message = errorStr;
	}

	inline int GetLine() const { return m_Line; }
	inline const std::string& GetFile() const { return m_File; }
	inline const std::string& GetErrorMessage() const { return m_Message; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "LuaErrorEvent: " << m_File << ":(" << m_Line << ") " << m_Message;
		return ss.str();
	}

	EVENT_CLASS_TYPE(LUA_ERROR);
	EVENT_CLASS_CATEGORY(EventCategory::APPLICATION);
private:
	int m_Line;
	std::string m_File;
	std::string m_Message;
};