#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <super/type/token.h>

namespace Super::Tool::String
{
	// 转换字符串的编码格式
	std::string ConvertEncoding(const std::string& input, const char* fromCode, const char* toCode);

	std::wstring ListToStringWithLineNumbers(const std::vector<std::vector<wchar_t>>& lines);

	std::wstring DictionaryStringToString(const std::unordered_map<std::wstring, std::wstring>& map);

	std::wstring TokenToString(const std::vector<Super::Type::Token>& tokens);

	// 判断字符或字符串是否为空白字符
	bool IsWhitespace(wchar_t c);
	bool IsWhitespace(std::wstring str);
	bool IsFirstNonWhitespaceCharSemicolon(const std::wstring& str, wchar_t c);

	// 将宽字符串转换为指定编码的字符串
	std::string WStringToEncoded(const std::wstring& input, const char* toCode);

	// 获取字符串的显示长度
	size_t GetDisplayLength(const std::wstring& str);

	// 去掉字符串的第一个和最后一个字符
	std::wstring RemoveFirstAndLastChar(const std::wstring& str);

	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);

	// 统一线结尾格式
	std::wstring UnifyLineEndings(const std::wstring& fileData);

	// 判断字符串的尾部是否与另一个字符串相等
	bool IsTailEqual(const std::wstring& str1, const std::wstring& str2, bool caseSensitive = true);

	bool BreakString(size_t i, std::wstring_view content, size_t& scope);
}