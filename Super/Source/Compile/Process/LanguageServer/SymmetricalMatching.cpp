module;
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <unordered_map>
module Super.Compile.Process;
import Super.Util.String;
import Super.Compile.GlobalData;
import Super.Error;

std::unordered_map<std::string, std::string> errorMessages =
{
	{ "(", "200000" }, { ")", "200010" },
	{ "[", "200040" }, { "]", "200050" },
	{ "{", "200020" }, { "}", "200030" }
};

template <typename Stack>
static void ReportUnclosed(const std::string& filePath, Stack& sk, const std::string& errCode)
{
	if (sk.empty())
	{
		return;
	}

	std::vector<typename Stack::value_type> tmp;
	tmp.reserve(sk.size());
	while (!sk.empty())
	{
		tmp.emplace_back(sk.top());
		sk.pop();
	}

	for (auto it = tmp.rbegin(); it != tmp.rend(); ++it)
	{
		SUPER_ERROR_CODE_CACHE_TOKEN(filePath, errCode, *it);
	}
	SUPER_ERROR_CACHE_OUT();
}

namespace Super::Compile::Process::LanguageServer
{
	void SymmetricalMatching(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens)
	{
		std::stack<Super::Compile::Core::Token> stacks[5];

		static const struct
		{
			const char* open;
			const char* close;
			const char* errOpen;
		} pair[5] =
		{
			{ "{", "}", errorMessages.at("{").c_str() },
			{ "(", ")", errorMessages.at("(").c_str() },
			{ "[", "]", errorMessages.at("[").c_str() },
			{ "", "#endif", "300020" },
			{ "", "#end", "300040" }
		};

		for (const auto& token : *tokens)
		{
			const std::string& str = token.value;
			bool handled = false;

			for (std::size_t i = 0; i < 5; ++i)
			{
				if (str == pair[i].close)
				{
					if (stacks[i].empty())
					{
						SUPER_ERROR_THROW_CODE_TOKEN(inputFilePath, pair[i].errOpen, token);
					}
					else
					{
						stacks[i].pop();
					}
					handled = true;
					break;
				}
			}
			if (handled)
			{
				continue;
			}

			if (str == "{")
			{
				stacks[0].push(token);
				continue;
			}
			if (str == "(")
			{
				stacks[1].push(token);
				continue;
			}
			if (str == "[")
			{
				stacks[2].push(token);
				continue;
			}
			if (str == "#ifdef" || str == "#ifndef" || str == "#if")
			{
				stacks[3].push(token);
				continue;
			}
			if (str == "#asm")
			{
				stacks[4].push(token);
				continue;
			}
		}

		for (std::size_t i = 0; i < 5; ++i)
		{
			ReportUnclosed(inputFilePath, stacks[i], pair[i].errOpen);
		}
	}
}  // namespace Super::Compile::Process::LanguageServer