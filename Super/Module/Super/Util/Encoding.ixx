module;
#include <string>
#include <string_view>
#include <vector>
export module Super.Util.Encoding;

/// @file Encoding.ixx
/// @brief 编码转换工具模块
///
/// 提供进制转换、字符串编码/解码、UTF编码转换等功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::Encoding
{
	/// @brief 进制枚举
	enum class NumberBase : uint8_t
	{
		Binary = 2,        ///< 二进制 (2)
		Octal = 8,         ///< 八进制 (8)
		Decimal = 10,      ///< 十进制 (10)
		Hexadecimal = 16   ///< 十六进制 (16)
	};

	/// @brief 进制转字符串
	/// @param base 进制类型
	/// @return 进制对应的字符串表示 (如 "2", "8", "10", "16")
	[[nodiscard]] constexpr std::string_view NumberBaseToString(NumberBase base) noexcept;

	/// @brief 数字转字符串
	/// @param value 要转换的数字
	/// @param base 目标进制，默认为十进制
	/// @return 转换后的字符串
	/// @example ToString(255, NumberBase::Hexadecimal) -> "ff"
	[[nodiscard]] std::string ToString(uint64_t value, NumberBase base = NumberBase::Decimal) noexcept;

	/// @brief 字节数据转十六进制字符串
	/// @param data 数据的起始指针
	/// @param size 数据大小（字节数）
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	/// @example ToHexString(data, 4) -> "e4bda0e5a5bd"
	[[nodiscard]] std::string ToHexString(const void* data, size_t size, bool uppercase = true) noexcept;

	/// @brief 字符串转十六进制字符串
	/// @param str 要转换的字符串
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	/// @example ToHexString("你好") -> "e4bda0e5a5bd"
	[[nodiscard]] std::string ToHexString(const std::string& str, bool uppercase = true) noexcept;

	/// @brief 字符串视图转十六进制字符串
	/// @param str 要转换的字符串视图
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	[[nodiscard]] std::string ToHexString(std::string_view str, bool uppercase = true) noexcept;

	/// @brief 十六进制字符串转字符串
	/// @param hex 十六进制字符串
	/// @return 转换后的字符串，失败返回空字符串
	/// @example FromHexString("e4bda0e5a5bd") -> "你好"
	[[nodiscard]] std::string FromHexString(std::string_view hex) noexcept;

	/// @brief UTF-8 字符串转十六进制（直接转换，不解析 Unicode）
	/// @param utf8Str UTF-8 字符串
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	[[nodiscard]] std::string ToHexStringUtf8(std::string_view utf8Str, bool uppercase = true) noexcept;

	/// @brief UTF-16 字符串转十六进制
	/// @param utf16Str UTF-16 字符串
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	[[nodiscard]] std::string ToHexStringUtf16(std::u16string_view utf16Str, bool uppercase = true) noexcept;

	/// @brief UTF-32 字符串转十六进制
	/// @param utf32Str UTF-32 字符串
	/// @param uppercase 是否大写，默认为 true
	/// @return 十六进制字符串
	[[nodiscard]] std::string ToHexStringUtf32(std::u32string_view utf32Str, bool uppercase = true) noexcept;

	/// @brief UTF-8 转 UTF-16 十六进制
	/// @param utf8 UTF-8 编码的字符串
	/// @return UTF-16 的十六进制表示
	/// @example Utf8ToUtf16Hex("你好") -> "4f60597d"
	[[nodiscard]] std::string Utf8ToUtf16Hex(std::string_view utf8) noexcept;

	/// @brief UTF-8 转 UTF-32 十六进制
	/// @param utf8 UTF-8 编码的字符串
	/// @return UTF-32 的十六进制表示
	/// @example Utf8ToUtf32Hex("你好") -> "00004f60597d"
	[[nodiscard]] std::string Utf8ToUtf32Hex(std::string_view utf8) noexcept;

	/// @brief UTF-16 十六进制转 UTF-8
	/// @param hex UTF-16 的十六进制表示
	/// @return UTF-8 编码的字符串
	/// @example Utf16HexToUtf8("4f60597d") -> "你好"
	[[nodiscard]] std::string Utf16HexToUtf8(std::string_view hex) noexcept;

	/// @brief 字符串编码（支持任意进制）
	/// @param str 要编码的字符串
	/// @param base 目标进制，默认为十六进制
	/// @return 编码后的字符串
	/// @example Encode("ABC") -> "414243" (Hex)
	[[nodiscard]] std::string Encode(const std::string& str, NumberBase base = NumberBase::Hexadecimal) noexcept;

	/// @brief 字符串解码
	/// @param encoded 已编码的字符串
	/// @param base 原始进制，默认为十六进制
	/// @return 解码后的字符串
	/// @example Decode("414243") -> "ABC"
	[[nodiscard]] std::string Decode(std::string_view encoded, NumberBase base = NumberBase::Hexadecimal) noexcept;

	/// @brief 字符串数组编码
	/// @param arr 要编码的字符串数组
	/// @param base 目标进制，默认为十六进制
	/// @return 编码后的字符串数组
	/// @example EncodeArray({"A", "B"}) -> {"41", "42"}
	[[nodiscard]] std::vector<std::string> EncodeArray(const std::vector<std::string>& arr, NumberBase base = NumberBase::Hexadecimal) noexcept;

	/// @brief 字符串数组解码
	/// @param arr 已编码的字符串数组
	/// @param base 原始进制，默认为十六进制
	/// @return 解码后的字符串数组
	/// @example DecodeArray({"41", "42"}) -> {"A", "B"}
	[[nodiscard]] std::vector<std::string> DecodeArray(const std::vector<std::string>& arr, NumberBase base = NumberBase::Hexadecimal) noexcept;

	/// @brief 解析 JSON 数组字符串
	/// @param str JSON 数组字符串，如 "[\"abc\",\"def\"]"
	/// @return 解析后的字符串向量
	/// @note 支持转义字符：\\, \", \n, \t, \r, \b, \f, \uXXXX
	/// @example ParseStringArray("[\"a\\\"b\",\"c\"]") -> {"a\"b", "c"}
	[[nodiscard]] std::vector<std::string> ParseStringArray(std::string_view str) noexcept;

	/// @brief 字符串数组转 JSON 数组字符串
	/// @param arr 字符串向量
	/// @return JSON 数组字符串
	/// @note 自动转义特殊字符
	/// @example ToStringArray({"a\"b", "c"}) -> "[\"a\\\"b\",\"c\"]"
	[[nodiscard]] std::string ToStringArray(const std::vector<std::string>& arr) noexcept;

	/// @brief 转义字符串（用于 JSON）
	/// @param str 要转义的字符串
	/// @return 转义后的字符串
	/// @note 转义字符：\\, \", \n, \t, \r, \b, \f 及控制字符
	/// @example EscapeString("a\"b") -> "a\\\"b"
	[[nodiscard]] std::string EscapeString(std::string_view str) noexcept;
}  // namespace Super::Util::Encoding