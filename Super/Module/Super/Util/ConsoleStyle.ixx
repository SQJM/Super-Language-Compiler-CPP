module;

#ifdef _WIN32
#	include <windows.h>
#endif

#include <iostream>
#include <sstream>
#include <string>

export module Super.Util.ConsoleStyle;
import Super.Config;

/// @file ConsoleStyle.ixx
/// @brief 控制台样式工具模块
///
/// 提供控制台文本样式设置功能，包括颜色、粗体、斜体等
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

namespace Detail
{
	inline void setup_vt()
	{
#ifdef _WIN32
		static std::once_flag flag;
		std::call_once(
			flag,
			[]()
			{
				const DWORD kEnableVT = 0x0004;
				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				if (hOut == INVALID_HANDLE_VALUE)
					return;
				DWORD mode = 0;
				if (!GetConsoleMode(hOut, &mode))
					return;
				SetConsoleMode(hOut, mode | kEnableVT);
			}
		);
#endif
	}

	inline std::string csi(const std::string& args)
	{
		if (Super::Config::ConsoleStyle::disable)
		{
			return "";
		}
		setup_vt();
		return "\033[" + args + "m";
	}
}  // namespace Detail

export namespace Super::Util::ConsoleStyle
{
	/// @brief 重置所有控制台样式
	/// @return 重置样式的ANSI转义序列
	inline std::string reset()
	{
		return Detail::csi("0");
	}

	/// @brief 设置粗体文本样式
	/// @return 粗体样式的ANSI转义序列
	inline std::string bold()
	{
		return Detail::csi("1");
	}

	/// @brief 设置淡色文本样式
	/// @return 淡色样式的ANSI转义序列
	inline std::string faint()
	{
		return Detail::csi("2");
	}

	/// @brief 设置斜体文本样式
	/// @return 斜体样式的ANSI转义序列
	inline std::string italic()
	{
		return Detail::csi("3");
	}

	/// @brief 设置下划线文本样式
	/// @return 下划线样式的ANSI转义序列
	inline std::string underline()
	{
		return Detail::csi("4");
	}

	/// @brief 设置闪烁文本样式
	/// @return 闪烁样式的ANSI转义序列
	inline std::string blink()
	{
		return Detail::csi("5");
	}

	/// @brief 设置反转文本样式（前景色和背景色交换）
	/// @return 反转样式的ANSI转义序列
	inline std::string reverse()
	{
		return Detail::csi("7");
	}

	/// @brief 设置隐藏文本样式
	/// @return 隐藏样式的ANSI转义序列
	inline std::string hidden()
	{
		return Detail::csi("8");
	}

	/// @brief 设置删除线文本样式
	/// @return 删除线样式的ANSI转义序列
	inline std::string strike()
	{
		return Detail::csi("9");
	}

	/// @brief 前景色命名空间
	namespace fg
	{
		/// @brief 设置黑色前景色
		/// @return 黑色前景色的ANSI转义序列
		inline std::string black()
		{
			return Detail::csi("30");
		}

		/// @brief 设置红色前景色
		/// @return 红色前景色的ANSI转义序列
		inline std::string red()
		{
			return Detail::csi("31");
		}

		/// @brief 设置绿色前景色
		/// @return 绿色前景色的ANSI转义序列
		inline std::string green()
		{
			return Detail::csi("32");
		}

		/// @brief 设置黄色前景色
		/// @return 黄色前景色的ANSI转义序列
		inline std::string yellow()
		{
			return Detail::csi("33");
		}

		/// @brief 设置蓝色前景色
		/// @return 蓝色前景色的ANSI转义序列
		inline std::string blue()
		{
			return Detail::csi("34");
		}

		/// @brief 设置品红色前景色
		/// @return 品红色前景色的ANSI转义序列
		inline std::string magenta()
		{
			return Detail::csi("35");
		}

		/// @brief 设置青色前景色
		/// @return 青色前景色的ANSI转义序列
		inline std::string cyan()
		{
			return Detail::csi("36");
		}

		/// @brief 设置白色前景色
		/// @return 白色前景色的ANSI转义序列
		inline std::string white()
		{
			return Detail::csi("37");
		}

		/// @brief 设置灰色前景色
		/// @return 灰色前景色的ANSI转义序列
		inline std::string gray()
		{
			return Detail::csi("90");
		}
	}  // namespace fg

	/// @brief 背景色命名空间
	namespace bg
	{
		/// @brief 设置黑色背景色
		/// @return 黑色背景色的ANSI转义序列
		inline std::string black()
		{
			return Detail::csi("40");
		}

		/// @brief 设置红色背景色
		/// @return 红色背景色的ANSI转义序列
		inline std::string red()
		{
			return Detail::csi("41");
		}

		/// @brief 设置绿色背景色
		/// @return 绿色背景色的ANSI转义序列
		inline std::string green()
		{
			return Detail::csi("42");
		}

		/// @brief 设置黄色背景色
		/// @return 黄色背景色的ANSI转义序列
		inline std::string yellow()
		{
			return Detail::csi("43");
		}

		/// @brief 设置蓝色背景色
		/// @return 蓝色背景色的ANSI转义序列
		inline std::string blue()
		{
			return Detail::csi("44");
		}

		/// @brief 设置品红色背景色
		/// @return 品红色背景色的ANSI转义序列
		inline std::string magenta()
		{
			return Detail::csi("45");
		}

		/// @brief 设置青色背景色
		/// @return 青色背景色的ANSI转义序列
		inline std::string cyan()
		{
			return Detail::csi("46");
		}

		/// @brief 设置白色背景色
		/// @return 白色背景色的ANSI转义序列
		inline std::string white()
		{
			return Detail::csi("47");
		}

		/// @brief 设置灰色背景色
		/// @return 灰色背景色的ANSI转义序列
		inline std::string gray()
		{
			return Detail::csi("100");
		}
	}  // namespace bg

	/// @brief 设置256色前景色
	/// @param index 颜色索引（0-255）
	/// @return 256色前景色的ANSI转义序列
	inline std::string fg256(uint8_t index)
	{
		return Detail::csi("38;5;" + std::to_string(index));
	}

	/// @brief 设置256色背景色
	/// @param index 颜色索引（0-255）
	/// @return 256色背景色的ANSI转义序列
	inline std::string bg256(uint8_t index)
	{
		return Detail::csi("48;5;" + std::to_string(index));
	}

	/// @brief 设置RGB颜色
	/// @param r 红色分量（0-255）
	/// @param g 绿色分量（0-255）
	/// @param b 蓝色分量（0-255）
	/// @param background 是否为背景色，默认为false（前景色）
	/// @return RGB颜色的ANSI转义序列
	inline std::string rgb(uint8_t r, uint8_t g, uint8_t b, bool background = false)
	{
		std::ostringstream os;
		os << (background ? 48 : 38) << ";2;" << +r << ';' << +g << ';' << +b;
		return Detail::csi(os.str());
	}

	/// @brief 设置RGB背景色
	/// @param r 红色分量（0-255）
	/// @param g 绿色分量（0-255）
	/// @param b 蓝色分量（0-255）
	/// @return RGB背景色的ANSI转义序列
	inline std::string bg_rgb(uint8_t r, uint8_t g, uint8_t b)
	{
		return rgb(r, g, b, true);
	}
}  // namespace Super::Util::ConsoleStyle