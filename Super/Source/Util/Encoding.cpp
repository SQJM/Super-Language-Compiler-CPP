module;
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
module Super.Util.Encoding;
import Super.Error;

namespace Super::Util::Encoding
{
	constexpr std::string_view NumberBaseToString(NumberBase base) noexcept
	{
		switch (base)
		{
		case NumberBase::Binary: return "2";
		case NumberBase::Octal: return "8";
		case NumberBase::Decimal: return "10";
		case NumberBase::Hexadecimal: return "16";
		default: return "10";
		}
	}

	std::string ToString(uint64_t value, NumberBase base) noexcept
	{
		if (value == 0)
			return "0";

		constexpr char digits[] = "0123456789abcdef";
		std::string result;
		uint8_t baseVal = static_cast<uint8_t>(base);

		while (value > 0)
		{
			result.push_back(digits[value % baseVal]);
			value /= baseVal;
		}

		std::reverse(result.begin(), result.end());
		return result;
	}

	std::string ToHexString(const void* data, size_t size, bool uppercase) noexcept
	{
		if (size == 0)
			return "";

		constexpr char hexLower[] = "0123456789abcdef";
		constexpr char hexUpper[] = "0123456789ABCDEF";
		const char* hexChars = uppercase ? hexUpper : hexLower;

		const auto* bytes = static_cast<const uint8_t*>(data);
		std::string result;
		result.reserve(size * 2);

		for (size_t i = 0; i < size; ++i)
		{
			result.push_back(hexChars[(bytes[i] >> 4) & 0x0F]);
			result.push_back(hexChars[bytes[i] & 0x0F]);
		}
		return result;
	}

	std::string ToHexString(const std::string& str, bool uppercase) noexcept
	{
		return ToHexString(str.data(), str.size(), uppercase);
	}

	std::string ToHexString(std::string_view str, bool uppercase) noexcept
	{
		return ToHexString(str.data(), str.size(), uppercase);
	}

	uint8_t HexCharToValue(char c) noexcept
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		return 0;
	}

	std::string FromHexString(std::string_view hex) noexcept
	{
		if (hex.size() % 2 != 0)
			return {};

		std::string result;
		result.reserve(hex.size() / 2);

		for (size_t i = 0; i < hex.size(); i += 2)
		{
			uint8_t byte = (HexCharToValue(hex[i]) << 4) | HexCharToValue(hex[i + 1]);
			result.push_back(static_cast<char>(byte));
		}
		return result;
	}

	std::string ToHexStringUtf8(std::string_view utf8Str, bool uppercase) noexcept
	{
		return ToHexString(utf8Str, uppercase);
	}

	std::string ToHexStringUtf16(std::u16string_view utf16Str, bool uppercase) noexcept
	{
		return ToHexString(utf16Str.data(), utf16Str.size() * sizeof(char16_t), uppercase);
	}

	std::string ToHexStringUtf32(std::u32string_view utf32Str, bool uppercase) noexcept
	{
		return ToHexString(utf32Str.data(), utf32Str.size() * sizeof(char32_t), uppercase);
	}

	std::string Utf8ToUtf16Hex(std::string_view utf8) noexcept
	{
		std::u16string utf16;
		utf16.reserve(utf8.size());

		for (size_t i = 0; i < utf8.size();)
		{
			uint32_t codepoint = 0;
			uint8_t c = static_cast<uint8_t>(utf8[i]);

			if (c < 0x80)
			{
				codepoint = c;
				i += 1;
			}
			else if ((c & 0xE0) == 0xC0)
			{
				if (i + 1 >= utf8.size()) break;
				codepoint = ((c & 0x1F) << 6) | (static_cast<uint8_t>(utf8[i + 1]) & 0x3F);
				i += 2;
			}
			else if ((c & 0xF0) == 0xE0)
			{
				if (i + 2 >= utf8.size()) break;
				codepoint = ((c & 0x0F) << 12) |
					((static_cast<uint8_t>(utf8[i + 1]) & 0x3F) << 6) |
					(static_cast<uint8_t>(utf8[i + 2]) & 0x3F);
				i += 3;
			}
			else if ((c & 0xF8) == 0xF0)
			{
				if (i + 3 >= utf8.size()) break;
				codepoint = ((c & 0x07) << 18) |
					((static_cast<uint8_t>(utf8[i + 1]) & 0x3F) << 12) |
					((static_cast<uint8_t>(utf8[i + 2]) & 0x3F) << 6) |
					(static_cast<uint8_t>(utf8[i + 3]) & 0x3F);
				i += 4;
			}
			else
			{
				break;
			}

			if (codepoint <= 0xFFFF)
			{
				utf16.push_back(static_cast<char16_t>(codepoint));
			}
			else
			{
				codepoint -= 0x10000;
				utf16.push_back(static_cast<char16_t>(0xD800 | (codepoint >> 10)));
				utf16.push_back(static_cast<char16_t>(0xDC00 | (codepoint & 0x3FF)));
			}
		}

		return ToHexStringUtf16(utf16);
	}

	std::string Utf8ToUtf32Hex(std::string_view utf8) noexcept
	{
		std::u32string utf32;
		utf32.reserve(utf8.size());

		for (size_t i = 0; i < utf8.size();)
		{
			uint32_t codepoint = 0;
			uint8_t c = static_cast<uint8_t>(utf8[i]);

			if (c < 0x80)
			{
				codepoint = c;
				i += 1;
			}
			else if ((c & 0xE0) == 0xC0)
			{
				if (i + 1 >= utf8.size()) break;
				codepoint = ((c & 0x1F) << 6) | (static_cast<uint8_t>(utf8[i + 1]) & 0x3F);
				i += 2;
			}
			else if ((c & 0xF0) == 0xE0)
			{
				if (i + 2 >= utf8.size()) break;
				codepoint = ((c & 0x0F) << 12) |
					((static_cast<uint8_t>(utf8[i + 1]) & 0x3F) << 6) |
					(static_cast<uint8_t>(utf8[i + 2]) & 0x3F);
				i += 3;
			}
			else if ((c & 0xF8) == 0xF0)
			{
				if (i + 3 >= utf8.size()) break;
				codepoint = ((c & 0x07) << 18) |
					((static_cast<uint8_t>(utf8[i + 1]) & 0x3F) << 12) |
					((static_cast<uint8_t>(utf8[i + 2]) & 0x3F) << 6) |
					(static_cast<uint8_t>(utf8[i + 3]) & 0x3F);
				i += 4;
			}
			else
			{
				break;
			}

			utf32.push_back(static_cast<char32_t>(codepoint));
		}

		return ToHexStringUtf32(utf32);
	}

	std::string Utf16HexToUtf8(std::string_view hex) noexcept
	{
		auto utf16 = FromHexString(hex);
		if (utf16.size() % 2 != 0)
			return {};

		std::string result;
		const auto* ptr = reinterpret_cast<const char16_t*>(utf16.data());
		size_t len = utf16.size() / 2;

		for (size_t i = 0; i < len; ++i)
		{
			uint32_t codepoint = ptr[i];

			if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
			{
				if (i + 1 < len)
				{
					uint32_t low = ptr[i + 1];
					if (low >= 0xDC00 && low <= 0xDFFF)
					{
						codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
						++i;
					}
				}
			}

			if (codepoint < 0x80)
			{
				result.push_back(static_cast<char>(codepoint));
			}
			else if (codepoint < 0x800)
			{
				result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
				result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
			}
			else if (codepoint < 0x10000)
			{
				result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
				result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
			}
			else
			{
				result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
				result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
				result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
			}
		}

		return result;
	}

	uint8_t ParseDigit(char c, NumberBase base) noexcept
	{
		uint8_t result = 0;
		if (c >= '0' && c <= '9')
			result = c - '0';
		else if (c >= 'a' && c <= 'z')
			result = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			result = c - 'A' + 10;

		uint8_t baseVal = static_cast<uint8_t>(base);
		return (result < baseVal) ? result : 0xFF;
	}

	std::string Encode(const std::string& str, NumberBase base) noexcept
	{
		if (base == NumberBase::Hexadecimal)
			return ToHexString(str);

		std::string result;
		uint8_t baseVal = static_cast<uint8_t>(base);
		for (unsigned char c : str)
		{
			if (!result.empty())
				result.push_back('_');

			uint64_t value = c;
			if (value == 0)
			{
				result.push_back('0');
				continue;
			}

			std::string digits;
			while (value > 0)
			{
				uint8_t digit = value % baseVal;
				digits.push_back(digit < 10 ? ('0' + digit) : ('a' + digit - 10));
				value /= baseVal;
			}
			std::reverse(digits.begin(), digits.end());
			result += digits;
		}
		return result;
	}

	std::string Decode(std::string_view encoded, NumberBase base) noexcept
	{
		if (base == NumberBase::Hexadecimal)
			return FromHexString(encoded);

		std::string result;
		uint8_t baseVal = static_cast<uint8_t>(base);
		size_t pos = 0;

		while (pos < encoded.size())
		{
			size_t underscore = encoded.find('_', pos);
			std::string_view token = (underscore == std::string_view::npos)
				? encoded.substr(pos)
				: encoded.substr(pos, underscore - pos);

			if (!token.empty())
			{
				uint64_t value = 0;
				for (char c : token)
				{
					uint8_t digit = ParseDigit(c, base);
					if (digit >= baseVal)
						return {};
					value = value * baseVal + digit;
				}
				if (value > 255)
					return {};
				result.push_back(static_cast<char>(value));
			}

			if (underscore == std::string_view::npos)
				break;
			pos = underscore + 1;
		}
		return result;
	}

	std::vector<std::string> EncodeArray(const std::vector<std::string>& arr, NumberBase base) noexcept
	{
		std::vector<std::string> result;
		result.reserve(arr.size());
		for (const auto& str : arr)
		{
			result.push_back(Encode(str, base));
		}
		return result;
	}

	std::vector<std::string> DecodeArray(const std::vector<std::string>& arr, NumberBase base) noexcept
	{
		std::vector<std::string> result;
		result.reserve(arr.size());
		for (const auto& encoded : arr)
		{
			result.push_back(Decode(encoded, base));
		}
		return result;
	}

	std::vector<std::string> ParseStringArray(std::string_view str) noexcept
	{
		std::vector<std::string> result;

		size_t start = str.find('[');
		size_t end = str.find(']');
		if (start == std::string_view::npos || end == std::string_view::npos)
			return result;

		std::string_view content = str.substr(start + 1, end - start - 1);
		if (content.empty())
			return result;

		size_t pos = 0;
		while (pos < content.size())
		{
			if (content[pos] == '"')
			{
				pos++;
				std::string item;
				while (pos < content.size())
				{
					if (content[pos] == '\\' && pos + 1 < content.size())
					{
						pos++;
						switch (content[pos])
						{
						case '"': item.push_back('"'); break;
						case '\\': item.push_back('\\'); break;
						case 'n': item.push_back('\n'); break;
						case 't': item.push_back('\t'); break;
						case 'r': item.push_back('\r'); break;
						case 'b': item.push_back('\b'); break;
						case 'f': item.push_back('\f'); break;
						case 'u':
							if (pos + 4 < content.size())
							{
								uint32_t codepoint = 0;
								for (int i = 1; i <= 4; ++i)
								{
									char c = content[pos + i];
									codepoint <<= 4;
									if (c >= '0' && c <= '9')
										codepoint |= c - '0';
									else if (c >= 'a' && c <= 'f')
										codepoint |= c - 'a' + 10;
									else if (c >= 'A' && c <= 'F')
										codepoint |= c - 'A' + 10;
								}
								if (codepoint <= 0x7F)
									item.push_back(static_cast<char>(codepoint));
								else if (codepoint <= 0x7FF)
								{
									item.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
									item.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
								}
								else if (codepoint <= 0xFFFF)
								{
									item.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
									item.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
									item.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
								}
								else
								{
									item.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
									item.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
									item.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
									item.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
								}
								pos += 4;
							}
							break;
						default: item.push_back(content[pos]); break;
						}
						pos++;
					}
					else if (content[pos] == '"')
					{
						pos++;
						break;
					}
					else
					{
						item.push_back(content[pos]);
						pos++;
					}
				}
				result.push_back(item);

				while (pos < content.size() && (content[pos] == ' ' || content[pos] == ','))
					pos++;
			}
			else
			{
				pos++;
			}
		}

		return result;
	}

	std::string EscapeString(std::string_view str) noexcept
	{
		std::string result;
		result.reserve(str.size() * 2);

		for (char c : str)
		{
			switch (c)
			{
			case '"': result += "\\\""; break;
			case '\\': result += "\\\\"; break;
			case '\n': result += "\\n"; break;
			case '\t': result += "\\t"; break;
			case '\r': result += "\\r"; break;
			case '\b': result += "\\b"; break;
			case '\f': result += "\\f"; break;
			default:
				if (static_cast<uint8_t>(c) < 32)
				{
					result += "\\u00";
					result += "0123456789abcdef"[(static_cast<uint8_t>(c) >> 4) & 0xF];
					result += "0123456789abcdef"[static_cast<uint8_t>(c) & 0xF];
				}
				else
				{
					result += c;
				}
				break;
			}
		}
		return result;
	}

	std::string ToStringArray(const std::vector<std::string>& arr) noexcept
	{
		std::string result = "[";
		for (size_t i = 0; i < arr.size(); ++i)
		{
			if (i > 0)
				result += ",";
			result += "\"";
			result += EscapeString(arr[i]);
			result += "\"";
		}
		result += "]";
		return result;
	}
}  // namespace Super::Util::Encoding