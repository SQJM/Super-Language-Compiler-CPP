#pragma once
#include <string>
#include <chrono>

namespace Super::Tool::Algorithm
{
    // 对正整数返回十进制位数，0 算 1 位
    template <class T>
    inline int DigitLen(T n)
    {
        static_assert(std::is_integral<T>::value, "integer required");
        return n == 0 ? 1 : static_cast<int>(std::log10(n)) + 1;
    }

    inline bool IsInteger(std::wstring_view s, long long& out, bool is_neg = false);

    inline bool IsFloating(std::wstring_view s, double& out, bool is_neg = false);
}
