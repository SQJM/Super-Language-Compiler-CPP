module;
#include <string>
#include <iostream>
module Super.Error;
import Super.Compile.GlobalData;
import Super.Util.String;
import Super.Compile.Core;
import Super.Util.ConsoleStyle;
import Super.Util.File;

static std::string CreateErrorMessage(const std::string& file, const std::string& msg, const Super::Compile::Core::Token& token)
{
	std::string head = std::to_string(token.ranks.line) + ":" + std::to_string(token.ranks.column) + "│";
	std::string body;
	auto re = Super::Util::File::ReadFileLine(file, token.ranks.line);
	for (auto& tk : *Super::Compile::GlobalData::TokenListMap[file])
	{
		if (tk.ranks.line == token.ranks.line)
		{
			body += token.value + " ";
		}
	}

	std::string indicate;
	size_t columnPos = token.ranks.column;
	size_t headWidth = head.size();
	size_t prefixSpaces = 0;
	for (size_t i = 0; i < body.size() && i < columnPos - 1; i++)
	{
		if (body[i] == ' ' || body[i] == '\t')
		{
			indicate += body[i];
			prefixSpaces++;
		}
		else
		{
			indicate += ' ';
		}
	}
	if (indicate.size() < columnPos)
	{
		indicate += std::string(columnPos - indicate.size() - 1, ' ');
	}
	size_t minSpaces = headWidth > prefixSpaces ? headWidth : prefixSpaces;
	if (indicate.size() < minSpaces)
	{
		indicate = std::string(minSpaces - indicate.size(), ' ') + indicate;
	}
	indicate += "^";

	return head + body + "\n" + Super::Util::ConsoleStyle::fg::gray() + indicate + Super::Util::ConsoleStyle::reset() +
		" " + Super::Util::ConsoleStyle::fg::red() + msg + Super::Util::ConsoleStyle::reset();
}

namespace Super::Error
{
	void NewError(const std::string& file, const std::string& msg, const Compile::Core::Token& token)
	{
		std::cout << CreateErrorMessage(file, msg, token);
	}

	void NewError(const std::string& file, const std::string& msg)
	{
		std::cout << file + " > " + msg;
	}

	void CacheError(const std::string& file, const std::string& msg, const Compile::Core::Token& token)
	{
		_cacheError += CreateErrorMessage(file, msg, token) + "\n\n";
	}

	void CacheOut()
	{
		_cacheError.pop_back();
		std::cout << _cacheError;
		_cacheError = "";
	}
}  // namespace Super::Error