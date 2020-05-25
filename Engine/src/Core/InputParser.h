#pragma once
#include "stdafx.h"

class InputParser
{
public:
	InputParser(int& argc, char** argv)
	{
		for (int i = 1; i < argc; ++i)
		{
			this->m_Tokens.push_back(std::string(argv[i]));
		}
	}

	const std::string& GetCmdOption(const std::string& option) const
	{
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->m_Tokens.begin(), this->m_Tokens.end(), option);
		if (itr != this->m_Tokens.end() && ++itr != this->m_Tokens.end())
		{
			m_TokensFound++;
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}

	bool CmdOptionExists(const std::string& option) const
	{
		if (m_Tokens.size() == (0 || m_TokensFound))
			return false;

		auto itr = std::find(this->m_Tokens.begin(), this->m_Tokens.end(), option);

		if (itr != this->m_Tokens.end())
		{
			m_TokensFound++;
			return true;
		}

		return false;
	}

	std::string OpenFile()const
	{
		std::ifstream file;
		file.open(m_Tokens[0]);

		if (file.good())
			return m_Tokens[0];
		else
			return std::string();
	}

	bool HasFoundAllArguments()
	{
		return m_TokensFound == m_Tokens.size();
	}

private:
	std::vector<std::string> m_Tokens;

	mutable int m_TokensFound = 0;
};