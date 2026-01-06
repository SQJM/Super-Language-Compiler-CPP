#include <super/error.h>
#include <super/compile/global_data.h>
#include <super/tool/string.h>
#include <super/info.h>
#include <super/tool/console.h>
#include <super/tool/log.h>
#include <super/tool/console_style.hpp>
#include <filesystem>

static std::wstring CreateErrorMessage(const std::wstring& file, const std::wstring& msg, const Super::Compile::Core::Token& token)
{
	std::wstring head = std::to_wstring(token.ranks.line) + L":" + std::to_wstring(token.ranks.column) + L'│';
	auto& lines = Super::Compile::GlobalData::FileDataList[file];
	std::wstring body = Super::Tool::String::TabReplaceSpace(std::wstring(lines[token.ranks.line - 1].begin(), lines[token.ranks.line - 1].end()));
	if (body[body.size() - 1] == L';')
	{
		body.erase(body.size() - 1, 1);
	}
	size_t width =
		head.size() + Super::Tool::String::GetDisplayLength(body.substr(0, token.ranks.column));
	std::wstring indicate(width, L'~');
	indicate += L"^";

	return head + body + L"\n"
		+ Super::Tool::ConsoleStyle::fg::gray()
		+ indicate
		+ Super::Tool::ConsoleStyle::reset()
		+ L" " + Super::Tool::ConsoleStyle::fg::red() + msg + Super::Tool::ConsoleStyle::reset();
}

namespace Super::Error
{

	std::unordered_map<std::wstring, std::wstring> CODE =
	{
		MAP_DEF("100010", "此文件不能使用该关键字"),
		MAP_DEF("100030", "文件不存在"),
		MAP_DEF("100040", "没有提取到 Token"),

		MAP_DEF("200000", "左括号缺少匹配的右括号"),
		MAP_DEF("200010", "右括号缺少匹配的左括号"),
		MAP_DEF("200020", "左花括号缺少匹配的右花括号"),
		MAP_DEF("200030", "右花括号缺少匹配的左花括号"),
		MAP_DEF("200040", "左中括号缺少匹配的右中括号"),
		MAP_DEF("200050", "右中括号缺少匹配的左中括号"),

		MAP_DEF("300010", "`#endif` 前面没有匹配的逻辑处理语句 `#if` | `#ifdef` | `#ifndef`"),
		MAP_DEF("300020", "`#if` | `#ifdef` | `#ifndef` 后面没有匹配的 `#endif`"),
		MAP_DEF("300030", "`#end` 前面没有匹配的 `#asm`"),
		MAP_DEF("300040", "`#asm` 后面没有匹配的 `#end`"),
		MAP_DEF("300050", "宏定义名冲突"),
		MAP_DEF("300060", "宏定义值冲突"),
		MAP_DEF("300070", "非法预处理指令"),

		MAP_DEF("", "[null]")
	};

	void NewError(const std::wstring &file, const std::wstring &msg, const Compile::Core::Token &token)
	{
		Tool::Console::PrintConsole(Tool::Log::LogLevel::Error, CreateErrorMessage(file, msg, token), false);
	}

	void NewError(const std::wstring& file, const std::wstring& msg)
	{
		Tool::Console::PrintConsole(Tool::Log::LogLevel::Error, file + L" > " + msg, false);
	}

	std::wstring _cacheError;

	void CacheError(const std::wstring &file, const std::wstring &msg, const Compile::Core::Token &token)
	{
		_cacheError += CreateErrorMessage(file, msg, token) + L"\n\n";
	}

	void PrintMessage(const std::wstring& msg)
	{
		Tool::Console::PrintConsole(msg);
	}

	void CacheOut()
	{
		_cacheError.pop_back();
		Tool::Console::PrintConsole(_cacheError, false);
		_cacheError = L"";
	}
}