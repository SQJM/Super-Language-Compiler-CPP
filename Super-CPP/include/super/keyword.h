#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

namespace Super::Keyword
{
	bool Contains(const std::wstring& str, const std::vector<std::wstring>& keywords);
	bool Contains(const std::wstring_view& str, const std::vector<std::wstring>& keywords);
	bool Contains(const wchar_t& c, const std::vector<std::wstring>& keywords);

	// 修饰符关键字
	const std::vector<std::wstring> Modifier =
	{
		L"constexpr",
		L"static",
		L"global",
		L"private",
		L"public",
		L"protected",
		L"virtual",
		L"friend",
		L"operator",
		L"await",
		L"async",
		L"inline",
		L"const",
		L"unsigned",
		L"signed"
	};

	// 其他关键字
	const std::vector<std::wstring> Other =
	{
		L"using",
		L"new",
		L"auto_new",
		L"delete",
		L"throw",
		L"sizeof",
		L"type_name",
		L"namespace",
		L"this",
		L"module",
		L"exports",
		L"export",
		L"template",
		L"import",
		L"null",
		L"nullptr"
	};

	// 类型关键字
	const std::vector<std::wstring> TypeKeywords =
	{
		L"void",
		L"bool",
		L"char",
		L"wchar",
		L"short",
		L"int8",
		L"int16",
		L"int32",
		L"int64",
		L"int128",
		L"half",
		L"float",
		L"double",
		L"typedef",
		L"class",
		L"union",
		L"struct",
		L"function",
		L"enum"
	};

	// 控制流关键字
	const std::vector<std::wstring> ControlFlow =
	{
		L"if",
		L"else",
		L"elif",
		L"while",
		L"for",
		L"do",
		L"loop",
		L"switch_over",
		L"switch",
		L"case",
		L"other",
		L"break",
		L"result",
		L"ok",
		L"err",
		L"finally",
		L"continue",
		L"return"
	};

	// 预处理指令
	const std::vector<std::wstring> PreprocessingInstructions =
	{
		L"#define",
		L"#undef",
		L"#ifdef",
		L"#ifndef",
		L"#endif",
		L"#error",
		L"#message",
		L"#asm",
		L"#end",
		L"#if",
		L"#elif",
		L"#else",
		L"#prefix",
		L"#over"
	};

	// 过程宏
	const std::vector<std::wstring> ProcessMacro =
	{
		L"#def",
		L"#edit",
		L"#get",
		L"#prefix",
		L"#type",
		L"#error",
		L"#message",
		L"#export"
	};

	// 符号
	const std::vector<std::wstring> Symbols =
	{
		L"$",
		L"\"",
		L"'",
		L"!",
		L"-",
		L"+",
		L"/",
		L"*",
		L"=",
		L"\\",
		L"[",
		L"]",
		L"{",
		L"}",
		L".",
		L":",
		L"?",
		L"(",
		L")",
		L"|",
		L"^",
		L"&",
		L"%",
		L"<",
		L">",
		L",",
		L";",
		L"~",
		L"@"
	};

	// 特殊符号
	const std::vector<std::wstring> SpecialSymbols =
	{
		L"#",
		L"_"
	};

	// 检查字符是否为特定符号
	inline bool IsSymbol(const std::wstring& c, std::vector<std::wstring> cs)
	{
		return std::find(cs.begin(), cs.end(), c) != cs.end();
	}
}