module;
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
export module Super.Util.String;
import Super.Compile.Core;

/// @file String.ixx
/// @brief 字符串处理工具模块
///
/// 提供字符串转换、编码转换、格式化等功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::String
{
	/// @brief 转换字符串编码
	/// @param input 输入字符串
	/// @param fromCode 源编码
	/// @param toCode 目标编码
	/// @return 转换后的字符串
	std::string ConvertEncoding(const std::string& input, const char* fromCode, const char* toCode);

	/// @brief 打印行
	/// @param lines 行数据
	void PrintLines(const std::vector<std::vector<char>>& lines);

	/// @brief 将字典转换为字符串
	/// @param map 字典
	/// @return 转换后的字符串
	std::string DictionaryStringToString(const std::unordered_map<std::string, std::string>& map);

	/// @brief 将token数组转换为字符串
	/// @param tokens token数组
	/// @return 转换后的字符串
	std::string TokenToString(std::vector<Super::Compile::Core::Token>* tokens);

	/// @brief 将单个token转换为字符串
	/// @param token token
	/// @return 转换后的字符串
	std::string TokenToString(const Super::Compile::Core::Token& token);

	/// @brief 检查字符是否为空白字符
	/// @param c 字符
	/// @return 如果是空白字符则返回true，否则返回false
	inline bool IsWhitespace(char c);

	/// @brief 检查字符串是否为空白字符串
	/// @param str 字符串
	/// @return 如果是空白字符串则返回true，否则返回false
	inline bool IsWhitespace(std::string str);

	/// @brief 检查字符串中第一个非空白字符是否为分号
	/// @param str 字符串
	/// @param c 要检查的字符
	/// @return 如果是则返回true，否则返回false
	bool IsFirstNonWhitespaceCharSemicolon(const std::string& str, char c);

	/// @brief 获取字符串的显示长度
	/// @param str 字符串
	/// @return 显示长度
	std::string GetDisplayLength(const std::string& str);

	/// @brief 移除字符串的第一个和最后一个字符
	/// @param str 字符串
	/// @return 移除后的字符串
	std::string RemoveFirstAndLastChar(const std::string& str);

	/// @brief 替换字符串中所有匹配的子字符串
	/// @param str 要修改的字符串
	/// @param from 要替换的子字符串
	/// @param to 替换后的子字符串
	void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

	/// @brief 统一换行符
	/// @param src 源字符串
	/// @return 统一换行符后的字符串视图
	std::string_view UnifyLineEndingsView(std::string& src);

	/// @brief 检查字符串尾部是否与另一个字符串相等
	/// @param str 源字符串
	/// @param str2 要比较的字符串
	/// @param caseSensitive 是否区分大小写，默认为true
	/// @return 如果尾部相等则返回true，否则返回false
	bool IsTailEqual(const std::string& str, const std::string& str2, bool caseSensitive = true);

	/// @brief 中断字符串
	/// @param i 起始位置
	/// @param content 内容
	/// @param scope 范围
	/// @return 是否中断成功
	bool BreakString(size_t i, std::string_view content, size_t& scope);

	/// @brief 移除字符串中的所有空白字符
	/// @param str 字符串
	/// @return 移除空白字符后的字符串
	inline std::string RemoveWhitespace(const std::string& str);

	/// @brief 移除字符串开头的空白字符
	/// @param str 字符串
	/// @return 移除开头空白字符后的字符串
	inline std::string RemoveWhitespaceFront(const std::string& str);

	/// @brief 将制表符替换为空格
	/// @param str 字符串
	/// @return 替换后的字符串
	std::string TabReplaceSpace(const std::string& str);

	/// @brief 向左填充字符串
	/// @param src 源字符串
	/// @param width 宽度
	/// @return 填充后的字符串
	inline std::string PaddingLeft(const std::string& src, std::size_t width);

	/// @brief 查找无效字符
	/// @param str 字符串
	/// @param invalidChars 无效字符集
	/// @return 第一个找到的无效字符，如果没有则返回0
	char FindInvalidChar(std::string_view str, std::string_view invalidChars);

	/// @brief 检查字符串中所有字符是否都有效
	/// @param str 字符串
	/// @param invalidChars 无效字符集
	/// @return 如果所有字符都有效则返回true，否则返回false
	inline bool IsAllValidChars(std::string_view str, std::string_view invalidChars);

	/// @brief 检查文件名是否有效
	/// @param fileName 文件名
	/// @return 如果有效则返回true，否则返回false
	inline bool IsValidFileName(std::string_view fileName);

	/// @brief 检查路径是否有效
	/// @param path 路径
	/// @return 如果有效则返回true，否则返回false
	inline bool IsValidPath(std::string_view path);
}  // namespace Super::Util::String