#pragma once
#include <iostream>

namespace Super::Compile::ADB::Encoding
{
    struct DataChar
    {
        uint16_t high;
        uint16_t low;
    };

    // 将 UTF-16 编码还原为字符
    char32_t Utf16ToChar32(const DataChar& dc);

    // 将字符转换为 UTF-16 编码
    DataChar Char32ToUtf16(char32_t c);
}