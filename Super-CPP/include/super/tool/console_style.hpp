#pragma once
#include <sstream>
#include <string>
#include <mutex>
#include <cstdlib>
#include <super/config.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Super::Tool::ConsoleStyle
{

    namespace Detail
    {
        // 平台相关 只在 Windows 上做一次 VT 处理开启
        inline void setup_vt() {
#ifdef _WIN32
            static std::once_flag flag;
            std::call_once(flag, []()
                {
                // 仅 Windows 10 1607+ 支持虚拟终端序列
                const DWORD kEnableVT = 0x0004;
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == INVALID_HANDLE_VALUE) return;
                DWORD mode = 0;
                if (!GetConsoleMode(hOut, &mode)) return;
                SetConsoleMode(hOut, mode | kEnableVT);
                });
#endif
        }

        inline std::wstring csi(const std::wstring& args) 
        {
            if (Super::Config::ConsoleStyle::disable)
            {
                return L"";
            }
            setup_vt(); // 确保 Windows VT 已开启
            return L"\033[" + args + L'm';
        }
    }

    // 通用属性
    inline std::wstring reset() { return Detail::csi(L"0"); }
    inline std::wstring bold() { return Detail::csi(L"1"); }
    inline std::wstring faint() { return Detail::csi(L"2"); }
    inline std::wstring italic() { return Detail::csi(L"3"); }
    inline std::wstring underline() { return Detail::csi(L"4"); }
    inline std::wstring blink() { return Detail::csi(L"5"); }
    inline std::wstring reverse() { return Detail::csi(L"7"); }
    inline std::wstring hidden() { return Detail::csi(L"8"); }
    inline std::wstring strike() { return Detail::csi(L"9"); }

    // 8 色快捷函数
    namespace fg 
    {
        inline std::wstring black() { return Detail::csi(L"30"); }
        inline std::wstring red() { return Detail::csi(L"31"); }
        inline std::wstring green() { return Detail::csi(L"32"); }
        inline std::wstring yellow() { return Detail::csi(L"33"); }
        inline std::wstring blue() { return Detail::csi(L"34"); }
        inline std::wstring magenta() { return Detail::csi(L"35"); } // 品红
        inline std::wstring cyan() { return Detail::csi(L"36"); } // 青
        inline std::wstring white() { return Detail::csi(L"37"); }
        inline std::wstring gray() { return Detail::csi(L"90"); } // 灰
    }

    namespace bg 
    {
        inline std::wstring black() { return Detail::csi(L"40"); }
        inline std::wstring red() { return Detail::csi(L"41"); }
        inline std::wstring green() { return Detail::csi(L"42"); }
        inline std::wstring yellow() { return Detail::csi(L"43"); }
        inline std::wstring blue() { return Detail::csi(L"44"); }
        inline std::wstring magenta() { return Detail::csi(L"45"); }
        inline std::wstring cyan() { return Detail::csi(L"46"); }
        inline std::wstring white() { return Detail::csi(L"47"); }
        inline std::wstring gray() { return Detail::csi(L"100"); }
    }

    // 256 色
    inline std::wstring fg256(uint8_t index) 
    {
        return Detail::csi(L"38;5;" + std::to_wstring(index));
    }
    inline std::wstring bg256(uint8_t index) 
    {
        return Detail::csi(L"48;5;" + std::to_wstring(index));
    }

    // 24 位真彩色
    inline std::wstring rgb(uint8_t r, uint8_t g, uint8_t b, bool background = false)
    {
        std::wostringstream os;
        os << (background ? 48 : 38) << ";2;" << +r << ';' << +g << ';' << +b;
        return Detail::csi(os.str());
    }
    inline std::wstring bg_rgb(uint8_t r, uint8_t g, uint8_t b)
    {
        return rgb(r, g, b, true);
    }
}