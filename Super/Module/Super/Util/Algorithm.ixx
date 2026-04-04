module;
#include <chrono>
#include <string>
export module Super.Util.Algorithm;

/// @file Algorithm.ixx
/// @brief 算法工具模块
///
/// 提供常用算法功能，如数字处理、GUID生成等
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::Algorithm
{
	/// @brief 计算整数的位数
	/// @tparam T 整数类型
	/// @param n 要计算位数的整数
	/// @return 整数的位数
	template <class T>
	inline int DigitLen(T n)
	{
		static_assert(std::is_integral<T>::value, "integer required");
		return n == 0 ? 1 : static_cast<int>(std::log10(n)) + 1;
	}

	/// @brief 检查字符串是否为整数
	/// @param s 要检查的字符串
	/// @param out 输出的整数值
	/// @param is_neg 是否允许负数
	/// @return 如果是整数则返回true，否则返回false
	inline bool IsInteger(std::string_view s, long long& out, bool is_neg = false);

	/// @brief 检查字符串是否为浮点数
	/// @param s 要检查的字符串
	/// @param out 输出的浮点数值
	/// @param is_neg 是否允许负数
	/// @return 如果是浮点数则返回true，否则返回false
	inline bool IsFloating(std::string_view s, double& out, bool is_neg = false);

	/// @brief 生成GUID
	/// @return 生成的GUID字符串
	std::string GenerateGUID();
} // namespace Super::Util::Algorithm