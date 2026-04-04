module;
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
module Super.Util.String;
import Super.Util.Algorithm;

namespace Super::Util::String
{
	std::string ConvertEncoding(const std::string& input, const char* fromCode, const char* toCode)
	{
		UErrorCode status = U_ZERO_ERROR;
		UConverter* fromConv = ucnv_open(fromCode, &status);
		if (U_FAILURE(status))
		{
			throw std::runtime_error("Failed to open source encoding");
		}

		UConverter* toConv = ucnv_open(toCode, &status);
		if (U_FAILURE(status))
		{
			ucnv_close(fromConv);
			throw std::runtime_error("Failed to open target encoding");
		}

		int32_t ulen = ucnv_toUChars(fromConv, nullptr, 0, input.c_str(), input.length(), &status);
		status = U_ZERO_ERROR;
		std::vector<UChar> ustr(ulen + 1);
		ucnv_toUChars(fromConv, ustr.data(), ulen + 1, input.c_str(), input.length(), &status);

		status = U_ZERO_ERROR;
		int32_t outlen = ucnv_fromUChars(toConv, nullptr, 0, ustr.data(), ulen, &status);
		status = U_ZERO_ERROR;
		std::vector<char> outstr(outlen + 1);
		ucnv_fromUChars(toConv, outstr.data(), outlen + 1, ustr.data(), ulen, &status);

		ucnv_close(fromConv);
		ucnv_close(toConv);

		return std::string(outstr.data(), outlen);
	}

	void PrintLines(const std::vector<std::vector<char>>& lines)
	{
		size_t maxLineLength = std::to_string(lines.size()).length();
		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (i > 0)
			{
				std::cout << '\n';
			}

			std::cout
				<< std::string(maxLineLength - std::to_string(i + 1).length(), ' ')
				<< std::to_string(i + 1)
				<< "│" << std::string(lines[i].begin(), lines[i].end());
		}
		std::cout << std::endl;
	}

	std::string DictionaryStringToString(const std::unordered_map<std::string, std::string>& map)
	{
		int maxKeyLength = 0;
		for (const auto& item : map)
		{
			maxKeyLength = std::max(maxKeyLength, static_cast<int>(item.first.length()));
		}
		std::string result;
		for (const auto& item : map)
		{
			result +=
				item.first + std::string(maxKeyLength - item.first.length(), ' ') + " = " + item.second + "\n";
		}
		return result;
	}

	std::string TokenToString(std::vector<Super::Compile::Core::Token>* tokens)
	{
		std::vector<std::string> nameCache;
		nameCache.reserve(tokens->size());
		int maxNameLength = 0;
		for (const auto& item : *tokens)
		{
			std::string name = Super::Compile::Core::TokenNameToString(item.name);
			maxNameLength = std::max(maxNameLength, static_cast<int>(name.size()));
			nameCache.emplace_back(std::move(name));
		}

		const int idxWidth = Algorithm::DigitLen(tokens->size());
		std::string result;
		result.reserve(tokens->size() * 32);

		for (size_t i = 0; i < tokens->size(); ++i)
		{
			const auto& item = (*tokens)[i];
			const auto& name = nameCache[i];
			result += PaddingLeft(std::to_string(i), idxWidth)
				+ "│ " + std::to_string(item.ranks.line) + ':'
				+ std::to_string(item.ranks.column) + '\t' + name
				+ std::string(maxNameLength - static_cast<int>(name.size()), ' ')
				+ '\t' + item.value + '\n';
		}
		return result;
	}

	std::string TokenToString(const Super::Compile::Core::Token& token)
	{
		return std::to_string(token.ranks.line) + ':' + std::to_string(token.ranks.column) + '\t' +
			Super::Compile::Core::TokenNameToString(token.name) + '\t' + token.value + '\n';
	}

	bool IsWhitespace(char c)
	{
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	bool IsWhitespace(std::string str)
	{
		for (auto& c : str)
		{
			if (IsWhitespace(c))
			{
				return true;
			}
		}
		return false;
	}

	bool IsFirstNonWhitespaceCharSemicolon(const std::string& str, char c)
	{
		for (char ch : str)
		{
			if (IsWhitespace(ch))
			{
				continue;
			}
			return ch == c;
		}
		return false;
	}

	std::string GetDisplayLength(const std::string& str)
	{
		size_t length = 0;
		for (size_t i = 0; i < str.size();)
		{
			unsigned char byte = static_cast<unsigned char>(str[i]);
			UChar32 uchar;
			int32_t offset = 0;
			if (byte < 0x80)
			{
				uchar = byte;
				offset = 1;
			}
			else if ((byte & 0xE0) == 0xC0 && i + 1 < str.size())
			{
				uchar = ((byte & 0x1F) << 6) | (static_cast<unsigned char>(str[i + 1]) & 0x3F);
				offset = 2;
			}
			else if ((byte & 0xF0) == 0xE0 && i + 2 < str.size())
			{
				uchar = ((byte & 0x0F) << 12) | ((static_cast<unsigned char>(str[i + 1]) & 0x3F) << 6) |
					(static_cast<unsigned char>(str[i + 2]) & 0x3F);
				offset = 3;
			}
			else if ((byte & 0xF8) == 0xF0 && i + 3 < str.size())
			{
				uchar = ((byte & 0x07) << 18) | ((static_cast<unsigned char>(str[i + 1]) & 0x3F) << 12) |
					((static_cast<unsigned char>(str[i + 2]) & 0x3F) << 6) |
					(static_cast<unsigned char>(str[i + 3]) & 0x3F);
				offset = 4;
			}
			else
			{
				uchar = byte;
				offset = 1;
			}
			int width = u_getIntPropertyValue(uchar, UCHAR_EAST_ASIAN_WIDTH);
			length += (width == U_EA_WIDE || width == U_EA_FULLWIDTH) ? 2 : 1;
			i += offset;
		}
		return std::to_string(length);
	}

	std::string RemoveFirstAndLastChar(const std::string& str)
	{
		if (str.length() > 1)
		{
			return str.substr(1, str.length() - 2);
		}
		return "";
	}

	void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
		{
			return;
		}
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}

	std::string_view UnifyLineEndingsView(std::string& src)
	{
		size_t w = 0;
		for (size_t r = 0; r < src.size(); ++r)
		{
			char c = src[r];
			if (c == '\r')
			{
				src[w++] = '\n';
				if (r + 1 < src.size() && src[r + 1] == '\n')
					++r;
			}
			else
			{
				src[w++] = c;
			}
		}
		src.resize(w);
		return std::string_view(src);
	}

	bool IsTailEqual(const std::string& str, const std::string& str2, bool caseSensitive)
	{
		if (str2.size() > str.size())
		{
			return false;
		}

		if (caseSensitive)
		{
			return str.compare(str.size() - str2.size(), str2.size(), str2) == 0;
		}
		else
		{
			std::string str1Lower = str;
			std::string str2Lower = str2;

			std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(),
						   [](unsigned char c) { return std::tolower(c); });
			std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(),
						   [](unsigned char c) { return std::tolower(c); });

			return str1Lower.compare(str1Lower.size() - str2Lower.size(), str2Lower.size(), str2Lower) == 0;
		}
	}

	bool BreakString(size_t i, std::string_view content, size_t& scope)
	{
		bool escape = false;
		size_t content_size = content.size();
		for (; i < content_size; i++)
		{
			char c = content[i];
			scope++;

			if (escape)
			{
				escape = !escape;
			}
			else
			{
				if (c == '"' || c == '\'' || (i >= content_size && c == '\\'))
				{
					return false;
				}
				if (c == '\n')
				{
					return true;
				}
				if (c == '\\')
				{
					escape = !escape;
				}
			}
		}

		return false;
	}

	std::string RemoveWhitespace(const std::string& str)
	{
		std::string result;
		for (auto& c : str)
		{
			if (!IsWhitespace(c))
			{
				result += c;
			}
		}
		return result;
	}

	std::string RemoveWhitespaceFront(const std::string& str)
	{
		std::string result = "";
		bool is_add = false;
		for (auto& c : str)
		{
			if (!IsWhitespace(c) && !is_add)
			{
				is_add = true;
			}
			if (is_add)
			{
				result += c;
			}
		}
		return result;
	}

	std::string TabReplaceSpace(const std::string& str)
	{
		std::string result = "";
		for (auto& c : str)
		{
			if (c == '\t')
			{
				result += ' ';
			}
			else
			{
				result += c;
			}
		}
		return result;
	}

	std::string PaddingLeft(const std::string& src, std::size_t width)
	{
		std::ostringstream oss;
		oss << std::setw(static_cast<int>(width)) << std::setfill(' ') << src;
		return oss.str();
	}

	char FindInvalidChar(std::string_view str, std::string_view invalidChars)
	{
		for (char c : str)
		{
			if (invalidChars.find(c) != std::string_view::npos)
			{
				return c;
			}
		}
		return '\0';
	}

	inline bool IsAllValidChars(std::string_view str, std::string_view invalidChars)
	{
		return FindInvalidChar(str, invalidChars) == '\0';
	}

	inline bool IsValidFileName(std::string_view fileName)
	{
		static constexpr char invalidChars[] = "<>:\"/\\|?*";
		if (!IsAllValidChars(fileName, invalidChars))
		{
			return false;
		}
		if (fileName.length() > 255)
		{
			return false;
		}
		if (fileName.find("CON") != std::string_view::npos ||
			fileName.find("PRN") != std::string_view::npos ||
			fileName.find("AUX") != std::string_view::npos ||
			fileName.find("NUL") != std::string_view::npos ||
			fileName.find("COM1") != std::string_view::npos ||
			fileName.find("COM2") != std::string_view::npos ||
			fileName.find("COM3") != std::string_view::npos ||
			fileName.find("COM4") != std::string_view::npos ||
			fileName.find("COM5") != std::string_view::npos ||
			fileName.find("COM6") != std::string_view::npos ||
			fileName.find("COM7") != std::string_view::npos ||
			fileName.find("COM8") != std::string_view::npos ||
			fileName.find("COM9") != std::string_view::npos ||
			fileName.find("LPT1") != std::string_view::npos ||
			fileName.find("LPT2") != std::string_view::npos ||
			fileName.find("LPT3") != std::string_view::npos ||
			fileName.find("LPT4") != std::string_view::npos ||
			fileName.find("LPT5") != std::string_view::npos ||
			fileName.find("LPT6") != std::string_view::npos ||
			fileName.find("LPT7") != std::string_view::npos ||
			fileName.find("LPT8") != std::string_view::npos ||
			fileName.find("LPT9") != std::string_view::npos)
		{
			return false;
		}
		if (!fileName.empty() && (fileName.back() == ' ' || fileName.back() == '.'))
		{
			return false;
		}
		return true;
	}

	inline bool IsValidPath(std::string_view path)
	{
		static constexpr char invalidChars[] = "\"<>|?*";
		return IsAllValidChars(path, invalidChars);
	}
}  // namespace Super::Util::String