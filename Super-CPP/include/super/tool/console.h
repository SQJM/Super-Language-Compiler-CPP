#pragma once
#include <string>
#include <super/tool/log.h>

namespace Super::Tool::Console
{
	void PrintConsole(Log::LogLevel l, const std::wstring& data, bool is_convert_encoding = true);

	void PrintConsole(const std::wstring& data, bool is_convert_encoding = true);

	void PrintConsoleLine(const std::wstring& data, bool is_convert_encoding = true);
}