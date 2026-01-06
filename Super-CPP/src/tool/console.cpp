#include <super/tool/console.h>
#include <super/tool/log.h>
#include <super/config.h>
#include <iostream>
#include <super/info.h>
#include <super/tool/console_style.hpp>
#include <super/tool/string.h>

static void OutBase(const std::wstring& data, bool r)
{
	std::wcout << data << std::endl;
}

static void Out(Super::Tool::Log::LogLevel l, const std::wstring& data, bool r)
{
	std::wstring prefix;
	switch (l)
	{
	case Super::Tool::Log::LogLevel::Trace: prefix = L"Trace"; break;
	case Super::Tool::Log::LogLevel::Debug: prefix = L"Debug"; break;
	case Super::Tool::Log::LogLevel::Info: prefix = L"Info"; break;
	case Super::Tool::Log::LogLevel::Warning: prefix = L"Warning"; break;
	case Super::Tool::Log::LogLevel::Error: prefix = Super::Tool::ConsoleStyle::fg::red() + L"Error"; break;
	case Super::Tool::Log::LogLevel::Fatal: prefix = L"Fatal"; break;
	default: break;
	}

	OutBase(
		prefix
		+ Super::Tool::ConsoleStyle::reset()
		+ Super::Tool::ConsoleStyle::fg::magenta()
		+ ((l == Super::Tool::Log::LogLevel::Norm) ? L"" : L":\n")
		+ Super::Tool::ConsoleStyle::reset()
		+ Super::Tool::ConsoleStyle::reset()
		+ data
		+ Super::Tool::ConsoleStyle::reset()
		, r );

#ifdef SUPER_OUT_MESSAGE_APPEND_FILE
	Super::Tool::Log::Logger::Instance().Log(l, data);
#endif
}

namespace Super::Tool::Console
{
	void PrintConsole(Super::Tool::Log::LogLevel l, const std::wstring& data, bool is_convert_encoding)
	{
		Out(l, data, is_convert_encoding);
	}

	void PrintConsole(const std::wstring& data, bool is_convert_encoding)
	{
		Out(Super::Tool::Log::LogLevel::Norm, data, is_convert_encoding);
	}

	void PrintConsoleLine(const std::wstring& data, bool is_convert_encoding)
	{
		OutBase(Super::Tool::ConsoleStyle::reset() + data, is_convert_encoding);
	}
}