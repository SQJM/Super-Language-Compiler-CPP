module;
#include <chrono>
module Super.Util.Time;

namespace Super::Util::Time
{
	YMD_HMSM Decompose(const std::chrono::system_clock::time_point& tp)
	{
		const auto local_tp = std::chrono::current_zone()->to_local(tp);

		const auto dp = std::chrono::floor<std::chrono::days>(local_tp);
		std::chrono::year_month_day ymd{ dp };

		std::chrono::hh_mm_ss hms{ std::chrono::floor<std::chrono::milliseconds>(local_tp - dp) };

		YMD_HMSM out{};

		out.year = static_cast<int>(ymd.year());
		out.mon = static_cast<unsigned>(ymd.month());
		out.day = static_cast<unsigned>(ymd.day());

		out.hour = hms.hours().count();
		out.min = hms.minutes().count();
		out.sec = static_cast<unsigned>(hms.seconds().count());
		out.ms = static_cast<unsigned>(hms.subseconds().count());
		return out;
	}

	std::string GetDetailTimeStr(const std::chrono::system_clock::time_point& tp, const std::string& fmt)
	{
		const YMD_HMSM t = Decompose(tp);

		std::string out;
		out.reserve(fmt.size() + 16);

		for (std::size_t i = 0; i < fmt.size();)
		{
			if (fmt[i] != 'y' && fmt[i] != 'M' && fmt[i] != 'd' && fmt[i] != 'H' && fmt[i] != 'm' &&
				fmt[i] != 's')
			{
				out.push_back(fmt[i++]);
				continue;
			}

			auto consume = [&](std::string_view key, int value, int width) -> bool
				{
					if (fmt.compare(i, key.size(), key) != 0)
						return false;

					std::format_to(std::back_inserter(out), "{:0>{}}", value, width);
					i += key.size();
					return true;
				};

			if (consume("yyyy", t.year, 4))
				continue;
			else if (consume("MM", t.mon, 2))
				continue;
			else if (consume("dd", t.day, 2))
				continue;
			else if (consume("HH", t.hour, 2))
				continue;
			else if (consume("mm", t.min, 2))
				continue;
			else if (consume("ss", t.sec, 2))
				continue;
			else if (consume("ms", t.ms, 3))
				continue;

			out.push_back(fmt[i++]);
		}
		return out;
	}
}  // namespace Super::Util::Time