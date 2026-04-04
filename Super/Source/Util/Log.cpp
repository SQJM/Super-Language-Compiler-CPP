module;
#include <iostream>
#include <mutex>
#include <sstream>
module Super.Util.Log;
import Super.Util.Time;

namespace Super::Util::Log
{
	Logger& Logger::Instance()
	{
		static Logger instance;
		return instance;
	}

	Logger::Logger()
		: minLevel(LogLevel::Trace)
	{
	}

	Logger::~Logger()
	{
		if (file.is_open())
			file.close();
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		minLevel = level;
	}

	void Logger::SetLogFile(const std::string& filename)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (file.is_open())
			file.close();
		file.open(
			filename + "-" + Super::Util::Time::GetDetailTimeStr(Super::Util::Time::now_nano(), "yyyy-MM-dd") +
			".log",
			std::ios::app
		);
	}

	void Logger::Log(LogLevel level, const std::string& msg)
	{
		if (level < minLevel)
			return;
		std::lock_guard<std::mutex> lock(mtx);
		std::string prefix;
		switch (level)
		{
		case LogLevel::Trace:
			prefix = "Trace";
			break;
		case LogLevel::Debug:
			prefix = "Debug";
			break;
		case LogLevel::Info:
			prefix = "Info";
			break;
		case LogLevel::Warning:
			prefix = "Warning";
			break;
		case LogLevel::Error:
			prefix = "Error";
			break;
		case LogLevel::Fatal:
			prefix = "Fatal";
			break;
		case LogLevel::Norm:
			prefix = "";
			break;
		}
		std::string out = prefix + " [" +
			Super::Util::Time::GetDetailTimeStr(Super::Util::Time::now_nano(), "HH:mm:ss.ms") + "]" +
			":\n" + msg + "\n";
		if (file.is_open())
			file << out;
	}

	Logger::LogStream Logger::operator()(LogLevel level)
	{
		return LogStream(*this, level);
	}

	Logger::LogStream::LogStream(Logger& logger, LogLevel level)
		: logger(logger),
		level(level)
	{
	}

	Logger::LogStream::~LogStream()
	{
		logger.Log(level, ss.str());
	}

	Logger::LogStream& Logger::LogStream::operator<<(const std::string& val)
	{
		ss << val;
		return *this;
	}
}  // namespace Super::Util::Log