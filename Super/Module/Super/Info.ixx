module;
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
export module Super.Info;

export namespace Super::Info
{
	inline const std::string Version = "2026.3.1";
	inline const std::string BuildDate = []()
		{
			std::time_t now = std::time(nullptr);
			std::tm now_tm;
			localtime_s(&now_tm, &now);
			std::ostringstream oss;
			oss << std::put_time(&now_tm, "%Y/%m/%d %H:%M:%S");
			return oss.str();
		}();

	inline std::string GetVersionInfo()
	{
		return "Version: " + Version + "\n" + "Build Date: " + BuildDate;
	}
}  // namespace Super::Info