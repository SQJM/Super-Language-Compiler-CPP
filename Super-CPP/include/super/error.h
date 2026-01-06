#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <super/compile/core/token.h>
#include <super/config.h>

#define SUPER_ERROR_CACHE(file, msg, token) \
	Super::Error::CacheError(file, msg , token);

#define SUPER_ERROR_CODE_CACHE(file, code, token) \
	Super::Error::CacheError(file, Super::Error::CODE[code] , token);

#define SUPER_ERROR_CACHE_OUT \
	Super::Error::CacheOut();std::exit(0);

#define SUPER_ERROR_THROW(file, msg, token) \
	Super::Error::NewError(file, msg , token);std::exit(0);

#define SUPER_ERROR_THROW_CODE(file, code, token) \
	Super::Error::NewError(file, Super::Error::CODE[code] , token);std::exit(0);

#define SUPER_ERROR_THROW_CODE_NONE(file, code) \
	Super::Error::NewError(file, Super::Error::CODE[code]);std::exit(0);

#define SUPER_ERROR_THROW_MESSAGE(msg) \
	Super::Error::PrintMessage(msg);std::exit(0);

namespace Super::Error
{
	extern std::unordered_map<std::wstring, std::wstring> CODE;

	extern std::wstring _cacheError;

	void NewError(const std::wstring &file, const std::wstring &msg, const Super::Compile::Core::Token &token);

	void NewError(const std::wstring& file, const std::wstring& msg);

	void CacheError(const std::wstring& file, const std::wstring& msg, const Super::Compile::Core::Token& token);

	void PrintMessage(const std::wstring& msg);

	void CacheOut();
}