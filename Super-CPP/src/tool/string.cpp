#include <super/tool/string.h>
#include <super/type/token_name.h>
#include <iomanip>
#include <algorithm>
#include <unicode/ucnv.h> 
#include <unicode/unistr.h>
#include <unicode/uchar.h>

namespace Super::Tool::String
{
	std::string ConvertEncoding(const std::string& input, const char* fromCode, const char* toCode)
	{
		UErrorCode status = U_ZERO_ERROR;
		UConverter* fromConv = ucnv_open(fromCode, &status);
		if (U_FAILURE(status)) throw std::runtime_error("打开源编码失败");

		UConverter* toConv = ucnv_open(toCode, &status);
		if (U_FAILURE(status))
		{
			ucnv_close(fromConv);
			throw std::runtime_error("打开目标编码失败");
		}

		// 先转成UChar（UTF-16）
		int32_t ulen = ucnv_toUChars(fromConv, nullptr, 0, input.c_str(), input.length(), &status);
		status = U_ZERO_ERROR;
		std::vector<UChar> ustr(ulen + 1);
		ucnv_toUChars(fromConv, ustr.data(), ulen + 1, input.c_str(), input.length(), &status);

		// 再从UChar转到目标编码
		status = U_ZERO_ERROR;
		int32_t outlen = ucnv_fromUChars(toConv, nullptr, 0, ustr.data(), ulen, &status);
		status = U_ZERO_ERROR;
		std::vector<char> outstr(outlen + 1);
		ucnv_fromUChars(toConv, outstr.data(), outlen + 1, ustr.data(), ulen, &status);

		ucnv_close(fromConv);
		ucnv_close(toConv);

		return std::string(outstr.data(), outlen);
	}

	std::wstring ListToStringWithLineNumbers(const std::vector<std::vector<wchar_t>>& lines)
	{
		size_t maxLineLength = std::to_wstring(lines.size()).length();
		std::wstring result;
		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (i > 0)
			{
				result += L"\n";
			}
			result +=
				std::wstring(maxLineLength - std::to_wstring(i + 1).length(), L' ')
				+ std::to_wstring(i + 1) + L"│"
				+ std::wstring(lines[i].begin(), lines[i].end());
		}
		return result;
	}

	std::wstring DictionaryStringToString(const std::unordered_map<std::wstring, std::wstring>& map)
	{
		int maxKeyLength = 0;
		for (const auto& item : map)
		{
			maxKeyLength = std::max(maxKeyLength, static_cast<int>(item.first.length()));
		}
		std::wstring result;
		for (const auto& item : map)
		{
			result += item.first + std::wstring(maxKeyLength - item.first.length(), L' ') + L" = " + item.second + L"\n";
		}
		return result;
	}

	std::wstring TokenToString(const std::vector<Super::Type::Token>& tokens)
	{
		int maxNameLength = 0;
		for (const auto& item : tokens)
		{
			maxNameLength = std::max(maxNameLength, static_cast<int>(Super::Type::TokenNameToString(item.name).size()));
		}
		std::wstring result;
		for (const auto& item : tokens)
		{
			result +=
				std::to_wstring(item.lc.line) + L":" + std::to_wstring(item.lc.column) + L"\t"
				+ Super::Type::TokenNameToString(item.name)
				+ std::wstring(maxNameLength - Super::Type::TokenNameToString(item.name).size(), L' ')
				+ L"\t" + item.value + L"\n";
		}
		return result;
	}

	bool IsWhitespace(wchar_t c)
	{
		UChar32 uchar = static_cast<UChar32>(c);

		return u_isWhitespace(uchar);
	}

	bool IsWhitespace(std::wstring str)
	{
		for (auto& c : str)
		{
			UChar32 uchar = static_cast<UChar32>(c);
			if (u_isWhitespace(uchar))
			{
				return false;
			}
		}
		return true;
	}

	bool IsFirstNonWhitespaceCharSemicolon(const std::wstring& str, wchar_t c)
	{
		for (wchar_t ch : str)
		{
			if (IsWhitespace(ch))
			{
				continue;
			}
			return ch == c;
		}
		return false;
	}

	std::string WStringToEncoded(const std::wstring& input, const char* toCode)
	{
		icu::UnicodeString ustr(input.c_str());
		// 先获取需要的字节数
		int32_t requiredSize = ustr.extract(0, ustr.length(), nullptr, toCode);
		std::string result(requiredSize, '\0');
		ustr.extract(0, ustr.length(), &result[0], toCode);
		return result;
	}

	size_t GetDisplayLength(const std::wstring& str)
	{
		size_t length = 0;
		for (auto& c : str)
		{
			UChar32 u = static_cast<UChar32>(c);
			int width = u_getIntPropertyValue(u, UCHAR_EAST_ASIAN_WIDTH);
			length += (width == U_EA_WIDE || width == U_EA_FULLWIDTH) ? 2 : 1;
		}
		return length;
	}

	std::wstring RemoveFirstAndLastChar(const std::wstring& str)
	{
		if (str.length() > 1)
		{
			return str.substr(1, str.length() - 2);
		}
		return L"";
	}

	void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		if (from.empty()) return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // 防止无限循环
		}
	}

	std::wstring UnifyLineEndings(const std::wstring& fileData)
	{
		std::wstring unifiedFileData = fileData;

		// 替换 \r\n 为 \n
		size_t pos = 0;
		while ((pos = unifiedFileData.find(L"\r\n", pos)) != std::wstring::npos)
		{
			unifiedFileData.replace(pos, 2, L"\n");
			pos += 1; // 跳过新插入的 \n
		}

		// 替换单独的 \r 为 \n
		std::replace(unifiedFileData.begin(), unifiedFileData.end(), L'\r', L'\n');

		return unifiedFileData;
	}

	bool IsTailEqual(const std::wstring& str1, const std::wstring& str2, bool caseSensitive)
	{
		if (str2.size() > str1.size())
		{
			return false;
		}

		if (caseSensitive)
		{
			// 区分大小写
			return str1.compare(str1.size() - str2.size(), str2.size(), str2) == 0;
		}
		else
		{
			std::wstring str1Lower = str1;
			std::wstring str2Lower = str2;

			std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(), towlower);
			std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(), towlower);

			return str1Lower.compare(str1Lower.size() - str2Lower.size(), str2Lower.size(), str2Lower) == 0;
		}
	}

	bool BreakString(size_t i, std::wstring_view content, size_t& scope)
	{
		bool escape = false;
		i++;
		size_t content_size = content.size();
		for (; i < content_size; i++)
		{
			wchar_t c = content[i];
			scope++;

			if (escape)
			{
				escape = !escape;
			}
			else
			{
				if (c == L'"' || c == L'\'')
				{
					return false;
				}
				if (c == L'\n')
				{
					return true;
				}
				if (c == L'\\')
				{
					escape = !escape;
				}
			}
		}

		return false;
	}

}