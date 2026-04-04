module;
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>
export module Super.Error;
import Super.Compile.Core;

namespace Super::Error
{
	export std::unordered_map<std::string, std::string> CODE
	{
		{ "100010", "This file cannot use this keyword" },
		{ "100030", "File does not exist" },
		{ "100040", "No tokens extracted" },

		{ "200000", "Left parenthesis missing matching right parenthesis" },
		{ "200010", "Right parenthesis missing matching left parenthesis" },
		{ "200020", "Left brace missing matching right brace" },
		{ "200030", "Right brace missing matching left brace" },
		{ "200040", "Left bracket missing matching right bracket" },
		{ "200050", "Right bracket missing matching left bracket" },

		{ "300010", "#endif without matching #if | #ifdef | #ifndef" },
		{ "300020", "#if | #ifdef | #ifndef without matching #endif" },
		{ "300030", "#end without matching #asm" },
		{ "300040", "#asm without matching #end" },
		{ "300050", "Macro definition name conflict" },
		{ "300060", "Macro definition value conflict" },
		{ "300070", "Invalid preprocessing directive" },

		{ "", "[null]" }
	};

	std::string _cacheError = "";

	void NewError(const std::string& file, const std::string& msg, const Super::Compile::Core::Token& token);

	void NewError(const std::string& file, const std::string& msg);

	void CacheError(const std::string& file, const std::string& msg, const Super::Compile::Core::Token& token);

	void CacheOut();
}  // namespace Super::Error

export inline void SUPER_ERROR_CACHE_TOKEN(const std::string& file, const std::string& msg, const Super::Compile::Core::Token& token)
{
	Super::Error::CacheError(file, msg, token);
}

export inline void SUPER_ERROR_CODE_CACHE_TOKEN(const std::string& file, const std::string& code, const Super::Compile::Core::Token& token)
{
	auto it = Super::Error::CODE.find(code);
	if (it != Super::Error::CODE.end())
	{
		Super::Error::CacheError(file, it->second, token);
	}
	else
	{
		Super::Error::CacheError(file, "Unknown error code: " + code, token);
	}
}

export inline void SUPER_ERROR_CACHE_OUT()
{
	Super::Error::CacheOut();
	std::exit(0);
}

export inline void SUPER_ERROR_THROW_TOKEN(const std::string& file, const std::string& msg, const Super::Compile::Core::Token& token)
{
	Super::Error::NewError(file, msg, token);
	std::exit(0);
}

export inline void SUPER_ERROR_THROW_CODE_TOKEN(const std::string& file, const std::string& code, const Super::Compile::Core::Token& token)
{
	auto it = Super::Error::CODE.find(code);
	if (it != Super::Error::CODE.end())
	{
		Super::Error::NewError(file, it->second, token);
	}
	else
	{
		Super::Error::NewError(file, "Unknown error code: " + code, token);
	}
	std::exit(0);
}

export inline void SUPER_ERROR_THROW_CODE(const std::string& file, const std::string& code)
{
	auto it = Super::Error::CODE.find(code);
	if (it != Super::Error::CODE.end())
	{
		Super::Error::NewError(file, it->second);
	}
	else
	{
		Super::Error::NewError(file, "Unknown error code: " + code);
	}
	std::exit(0);
}

export inline void SUPER_ERROR_THROW_MESSAGE(const std::string& msg)
{
	std::cout << msg;
	std::exit(0);
}