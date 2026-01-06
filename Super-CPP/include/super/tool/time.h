#pragma once
#include <chrono>

namespace Super::Tool::Time
{
    // 获取当前系统时间 纳秒
    inline auto now_nano()
    {
        return std::chrono::system_clock::now();
    }

    struct YMD_HMSM
    {
        int year,
            mon,
            day,
            hour,
            min, 
            sec, 
            ms;
    };

    YMD_HMSM Decompose(const std::chrono::system_clock::time_point& tp);

    std::wstring GetDetailTimeStr(const std::chrono::system_clock::time_point& tp,
        const std::wstring& fmt = L"yyyy-MM-dd HH:mm:ss.ms");
}