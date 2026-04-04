module;
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
export module Super.Util.Time;

/// @file Time.ixx
/// @brief 时间处理工具模块
///
/// 提供时间获取、时间分解、格式化等功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::Time
{
	/// @brief 获取当前时间点（纳秒精度）
	/// @return 当前系统时间点
	inline auto now_nano()
	{
		return std::chrono::system_clock::now();
	}

	/// @brief 年月日时分秒毫秒结构体
	struct YMD_HMSM
	{
		int year; ///< 年
		int mon;  ///< 月
		int day;  ///< 日
		int hour; ///< 时
		int min;  ///< 分
		int sec;  ///< 秒
		int ms;	  ///< 毫秒
	};

	/// @brief 分解时间点为年月日时分秒毫秒
	/// @param tp 时间点
	/// @return 分解后的年月日时分秒毫秒结构体
	YMD_HMSM Decompose(const std::chrono::system_clock::time_point& tp);

	/// @brief 获取详细时间字符串
	/// @param tp 时间点
	/// @param fmt 格式字符串，默认为"yyyy-MM-dd HH:mm:ss.ms"
	/// @return 格式化后的时间字符串
	std::string GetDetailTimeStr(
		const std::chrono::system_clock::time_point& tp, const std::string& fmt = "yyyy-MM-dd HH:mm:ss.ms");

	/// @brief 打印经过的时间
	/// @param start 开始时间点
	inline void PrintElapsedTime(const std::chrono::steady_clock::time_point& start)
	{
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		auto total_ms = duration.count();

		if (total_ms <= 3599999999)
		{
			auto hours = total_ms / 3600000;
			auto minutes = (total_ms % 3600000) / 60000;
			auto seconds = (total_ms % 60000) / 1000;
			auto ms = total_ms % 1000;

			char buffer[32];
			snprintf(buffer, 32, "%03d:%02d:%02d.%03d", (int)hours, (int)minutes, (int)seconds, (int)ms);
			std::cout << "\nElapsed " + std::string(buffer) << std::endl;
		}
		else
		{
			auto now = std::chrono::system_clock::now();
			auto time_t_now = std::chrono::system_clock::to_time_t(now);
			auto ms_part = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

			tm tm_now;
			localtime_s(&tm_now, &time_t_now);
			char time_buffer[32];
			strftime(time_buffer, 32, "%Y-%m-%d %H:%M:%S", &tm_now);

			std::cout << "\nElapsed " + std::string(time_buffer) + '.' + std::to_string(ms_part) << std::endl;
		}
	}
} // namespace Super::Util::Time