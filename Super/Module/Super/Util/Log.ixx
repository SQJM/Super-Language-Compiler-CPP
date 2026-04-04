module;
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
export module Super.Util.Log;

/// @file Log.ixx
/// @brief 日志工具模块
///
/// 提供日志记录功能，支持多级别日志输出
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

namespace Super::Util::Log
{
	/// @brief 日志级别枚举
	export enum class LogLevel {
		Norm,	 ///< 正常级别
		Trace,	 ///< 跟踪级别
		Debug,	 ///< 调试级别
		Info,	 ///< 信息级别
		Warning, ///< 警告级别
		Error,	 ///< 错误级别
		Fatal	 ///< 致命级别
	};

	/// @brief 日志记录器类
	export class Logger
	{
	public:
		/// @brief 获取Logger单例实例
		/// @return Logger引用
		static Logger& Instance();

		/// @brief 设置日志级别
		/// @param level 日志级别
		void SetLogLevel(LogLevel level);

		/// @brief 设置日志文件
		/// @param filename 日志文件名
		void SetLogFile(const std::string& filename);

		/// @brief 记录日志
		/// @param level 日志级别
		/// @param msg 日志消息
		void Log(LogLevel level, const std::string& msg);

		/// @brief 日志流类
		class LogStream;

		/// @brief 获取日志流
		/// @param level 日志级别
		/// @return LogStream对象
		LogStream operator()(LogLevel level);

	private:
		/// @brief 构造函数
		Logger();
		/// @brief 析构函数
		~Logger();
		LogLevel minLevel;	///< 最小日志级别
		std::ofstream file; ///< 日志文件流
		std::mutex mtx;		///< 互斥锁
	};

	/// @brief 日志流类
	class Logger::LogStream
	{
	public:
		/// @brief 构造函数
		/// @param logger Logger引用
		/// @param level 日志级别
		LogStream(Logger& logger, LogLevel level);
		/// @brief 析构函数
		~LogStream();

		/// @brief 插入操作符模板
		/// @tparam T 插入数据类型
		/// @param val 要插入的数据
		/// @return LogStream引用
		template <typename T>
		LogStream& operator<<(const T& val)
		{
			ss << val;
			return *this;
		}

		/// @brief 字符串插入操作符
		/// @param val 要插入的字符串
		/// @return LogStream引用
		LogStream& operator<<(const std::string& val);

	private:
		Logger& logger;		   ///< Logger引用
		LogLevel level;		   ///< 日志级别
		std::ostringstream ss; ///< 输出字符串流
	};
} // namespace Super::Util::Log