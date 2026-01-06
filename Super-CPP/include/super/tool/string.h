#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <super/compile/core/token.h>

namespace Super::Tool::String
{
	// 转换字符串的编码格式
	std::string ConvertEncoding(const std::string& input, const char* fromCode, const char* toCode);

	void PrintLines(const std::vector<std::vector<wchar_t>>& lines);

	std::wstring DictionaryStringToString(const std::unordered_map<std::wstring, std::wstring>& map);

	std::wstring TokenToString(std::vector<Super::Compile::Core::Token>* tokens);

	// 判断字符或字符串是否为空白字符
	inline bool IsWhitespace(wchar_t c);
	inline bool IsWhitespace(std::wstring str);
	bool IsFirstNonWhitespaceCharSemicolon(const std::wstring& str, wchar_t c);

	// 将宽字符串转换为指定编码的字符串
	std::string WStringToEncoded(const std::wstring& input, const char* toCode);

	// 获取字符串的显示长度
	size_t GetDisplayLength(const std::wstring& str);

	// 去掉字符串的第一个和最后一个字符
	std::wstring RemoveFirstAndLastChar(const std::wstring& str);

	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);

	// 统一线结尾格式
	std::wstring_view UnifyLineEndingsView(std::wstring& src);

	// 判断字符串的尾部是否与另一个字符串相等
	bool IsTailEqual(const std::wstring& str, const std::wstring& str2, bool caseSensitive = true);

	// 解析跳过字符串
	bool BreakString(size_t i, std::wstring_view content, size_t& scope);

	// 移除空白符
	inline std::wstring RemoveWhitespace(const std::wstring& str);
	inline std::wstring RemoveWhitespaceFront(const std::wstring& str);

	// 制表符替换为空格
	std::wstring TabReplaceSpace(const std::wstring& str);

	// 字符串前面补齐
	inline std::wstring PaddingLeft(const std::wstring& src, std::size_t width);
}