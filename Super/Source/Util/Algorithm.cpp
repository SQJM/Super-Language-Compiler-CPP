module;
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <chrono>
#include <random>
module Super.Util.Algorithm;

namespace detail
{
	inline static bool is_digit(char c) noexcept
	{
		return c >= '0' && c <= '9';
	}

	inline static int c2val(char c) noexcept
	{
		return c - '0';
	}
}  // namespace detail

namespace Super::Util::Algorithm
{
	bool IsInteger(std::string_view s, long long& out, bool is_neg)
	{
		using namespace detail;
		if (s.empty())
		{
			return false;
		}
		const char* p = s.data();
		const char* end = p + s.size();

		int base = 10;
		if (*p == '0' && p + 1 != end)
		{
			char c = p[1];
			if (c == 'x' || c == 'X')
			{
				base = 16;
				p += 2;
			}
			else if (c == 'b' || c == 'B')
			{
				base = 2;
				p += 2;
			}
			else if (is_digit(c))
			{
				base = 8;
				++p;
			}
		}
		if (p == end)
		{
			return false;
		}

		unsigned long long acc = 0;
		bool any = false;
		while (p != end)
		{
			int digit = -1;
			char c = *p++;
			if (is_digit(c))
			{
				digit = c2val(c);
			}
			else if (base == 16)
			{
				if (c >= 'A' && c <= 'F')
					digit = 10 + (c - 'A');
				else if (c >= 'a' && c <= 'f')
					digit = 10 + (c - 'a');
			}
			if (digit < 0 || digit >= base)
			{
				return false;
			}
			any = true;
			if (acc > (std::numeric_limits<unsigned long long>::max() - digit) / base)
			{
				return false;
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

	bool IsFloating(std::string_view s, double& out, bool is_neg)
	{
		using namespace detail;
		if (s.empty())
		{
			return false;
		}
		const char* p = s.data();
		const char* end = p + s.size();

		unsigned long long int_part = 0;
		bool has_int = false;
		while (p != end && is_digit(*p))
		{
			has_int = true;
			int_part = int_part * 10 + c2val(*p++);
		}

		unsigned long long frac_part = 0;
		int frac_digits = 0;
		if (p != end && *p == '.')
		{
			++p;
			while (p != end && is_digit(*p))
			{
				if (frac_digits < 18)
				{
					frac_part = frac_part * 10 + c2val(*p);
					++frac_digits;
				}
				++p;
			}
		}
		if (!has_int && frac_digits == 0)
			return false;

		int exp = 0;
		if (p != end && (*p == 'e' || *p == 'E'))
		{
			++p;
			int exp_sign = 1;
			if (p != end && (*p == '+' || *p == '-'))
			{
				if (*p == '-')
					exp_sign = -1;
				++p;
			}
			if (p == end || !is_digit(*p))
			{
				return false;
			}
			while (p != end && is_digit(*p))
				exp = exp * 10 + c2val(*p++);
			exp *= exp_sign;
		}
		if (p != end)
			return false;

		double val = static_cast<double>(int_part) + static_cast<double>(frac_part) / std::pow(10.0, frac_digits);
		val *= std::pow(10.0, exp);
		out = is_neg ? -val : val;
		return true;
	}

	std::string GenerateGUID()
	{
		auto now = std::chrono::system_clock::now();
		auto epoch = now.time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();

		std::mt19937_64 rng(static_cast<uint64_t>(millis));
		auto seq = rng();

		std::string guid;
		guid.reserve(36);

		auto append_hex = [&](uint32_t value, int width)
			{
				char buf[16];
				for (int i = width - 1; i >= 0; --i)
				{
					buf[i] = "0123456789ABCDEF"[value & 0xF];
					value >>= 4;
				}
				for (int i = 0; i < width; ++i)
					guid += buf[i];
			};

		append_hex(static_cast<uint32_t>(seq >> 32), 8);
		guid += '-';
		append_hex(static_cast<uint32_t>(seq & 0xFFFFFFFF), 8);
		guid += '-';
		append_hex(static_cast<uint32_t>(millis >> 16), 4);
		guid += '-';
		append_hex(static_cast<uint16_t>(millis & 0xFFFF), 4);
		guid += '-';
		append_hex(static_cast<uint32_t>((seq >> 16)), 8);

		return guid;
	}
}  // namespace Super::Util::Algorithm