#include <super/tool/algorithm.h>
#include <cstdint>
#include <cmath>
#include <type_traits>
#include <string>
#include <optional>

namespace Super::Tool::Algorithm 
{

	namespace detail 
	{
		// 快速判断字符类别
		inline static bool is_digit(wchar_t c) noexcept { return c >= L'0' && c <= L'9'; }
		inline static int  c2val(wchar_t c) noexcept { return c - L'0'; }
	}

	bool IsInteger(std::wstring_view s, long long& out, bool is_neg)
	{
		using namespace detail;
		if (s.empty())
		{
			return false;
		}
		const wchar_t* p = s.data();
		const wchar_t* end = p + s.size();

		int base = 10;
		if (*p == L'0' && p + 1 != end) 
		{
			wchar_t c = p[1];
			if (c == L'x' || c == L'X') { base = 16; p += 2; }
			else if (c == L'b' || c == L'B') { base = 2; p += 2; }
			else if (is_digit(c)) { base = 8; ++p; }
		}
		if (p == end)
		{
			return false; // 前缀后无数字
		}

		unsigned long long acc = 0;
		bool any = false;
		while (p != end)
		{
			int digit = -1;
			wchar_t c = *p++;
			if (is_digit(c))
			{
				digit = c2val(c);
			}
			else if (base == 16)
			{
				if (c >= L'A' && c <= L'F') digit = 10 + (c - L'A');
				else if (c >= L'a' && c <= L'f') digit = 10 + (c - L'a');
			}
			if (digit < 0 || digit >= base)
			{
				return false; // 非法字符
			}
			any = true;
			if (acc > (std::numeric_limits<long long>::max() - digit) / base)
			{
				return false; // 溢出
			}
			acc = acc * base + digit;
		}
		if (!any) 
		{
			return false;
		}

		if (is_neg) 
		{
			if (acc > static_cast<unsigned long long>(std::numeric_limits<long long>::max()) + 1ULL)	
			{
				return false;
			}
			out = -static_cast<long long>(acc);
		}
		else 
		{
			if (acc > static_cast<unsigned long long>(std::numeric_limits<long long>::max()))
			{
				return false;
			}
			out = static_cast<long long>(acc);
		}
		return true;
	}

	bool IsFloating(std::wstring_view s, double& out, bool is_neg)
	{
		using namespace detail;
		if (s.empty())
		{
			return false;
		}
		const wchar_t* p = s.data();
		const wchar_t* end = p + s.size();

		unsigned long long int_part = 0;
		bool has_int = false;
		while (p != end && is_digit(*p)) 
		{
			has_int = true;
			int_part = int_part * 10 + c2val(*p++);
		}

		unsigned long long frac_part = 0;
		int frac_digits = 0;
		if (p != end && *p == L'.') 
		{
			++p;
			while (p != end && is_digit(*p))
			{
				if (frac_digits < 18)
				{ // 只取前 18 位精度已足够
					frac_part = frac_part * 10 + c2val(*p);
					++frac_digits;
				}
				++p;
			}
		}
		if (!has_int && frac_digits == 0) return false;

		int exp = 0;
		if (p != end && (*p == L'e' || *p == L'E'))
		{
			++p;
			int exp_sign = 1;
			if (p != end && (*p == L'+' || *p == L'-')) 
			{
				if (*p == L'-') exp_sign = -1;
				++p;
			}
			if (p == end || !is_digit(*p))
			{
				return false; // e 后无数字
			}
			while (p != end && is_digit(*p))
				exp = exp * 10 + c2val(*p++);
			exp *= exp_sign;
		}
		if (p != end) return false; // 多余字符

		double val = static_cast<double>(int_part) + static_cast<double>(frac_part) / std::pow(10.0, frac_digits);
		val *= std::pow(10.0, exp);
		out = is_neg ? -val : val;
		return true;
	}
}