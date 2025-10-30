#include <super/compile/ADB/encoding.h>
#include <iomanip>
#include <string>
#include <super/error.h>

namespace Super::Compile::ADB::Encoding
{
    char32_t Utf16ToChar32(const DataChar& dc)
    {
        if (dc.high >= 0xD800 && dc.high <= 0xDBFF && dc.low >= 0xDC00 && dc.low <= 0xDFFF)
        {
            // 代理对
            return 0x10000 + ((dc.high - 0xD800) << 10) + (dc.low - 0xDC00);
        }
        else if (dc.high <= 0xFFFF)
        {
            // 单个 UTF-16 码点
            return dc.high;
        }
        else
        {
            SUPER_ERROR_THROW_MESSAGE(L"无效的 UTF-16 代理对")
        }
    }

    DataChar Char32ToUtf16(char32_t c)
    {
        DataChar result{};
        if (c <= 0xFFFF)
        {
            // BMP 中的字符，直接使用
            result.high = static_cast<uint16_t>(c);
            result.low = 0; // 低字节部分为 0
        }
        else
        {
            // 超出 BMP 的字符，需要使用代理对
            c -= 0x10000;
            result.high = static_cast<uint16_t>(0xD800 + (c >> 10));
            result.low = static_cast<uint16_t>(0xDC00 + (c & 0x3FF));
        }
        return result;
    }
}