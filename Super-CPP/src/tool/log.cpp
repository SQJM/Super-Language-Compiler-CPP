#include <super/tool/log.h>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <super/tool/time.h>

namespace Super::Tool::Log
{
	Logger& Logger::Instance()
	{
		static Logger instance;
		return instance;
	}

	Logger::Logger() : minLevel(LogLevel::Trace)
	{}

	Logger::~Logger()
	{
		if (file.is_open()) file.close();
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		minLevel = level;
	}

	void Logger::SetLogFile(const std::wstring& filename)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (file.is_open()) file.close();
		file.open(filename + L"-" + Tool::Time::GetDetailTimeStr(Tool::Time::now_nano(), L"yyyy-MM-dd") + L".log", std::ios::app);
	}

	void Logger::Log(LogLevel level, const std::wstring& msg)
	{
		if (level < minLevel) return;
		std::lock_guard<std::mutex> lock(mtx);
		std::wstring prefix;
		switch (level)
		{
		case LogLevel::Trace: prefix = L"Trace"; break;
		case LogLevel::Debug: prefix = L"Debug"; break;
		case LogLevel::Info: prefix = L"Info"; break;
		case LogLevel::Warning: prefix = L"Warning"; break;
		case LogLevel::Error: prefix = L"Error"; break;
		case LogLevel::Fatal: prefix = L"Fatal"; break;
		case LogLevel::Norm: prefix = L""; break;
		}
		std::wstring out =
			prefix + 
			L" [" + Tool::Time::GetDetailTimeStr(Tool::Time::now_nano(), L"HH:mm:ss.ms") + L"]" +
			L":\n" + msg + L"\n";
		if (file.is_open()) file << out;
	}

	// 流式接口
	Logger::LogStream Logger::operator()(LogLevel level)
	{
		return LogStream(*this, level);
	}

	Logger::LogStream::LogStream(Logger& logger, LogLevel level)
		: logger(logger), level(level)
	{}

	Logger::LogStream::~LogStream()
	{
		logger.Log(level, ss.str());
	}

	Logger::LogStream& Logger::LogStream::operator<<(const std::wstring& val)
	{
		ss << val;
		return *this;
	}

}