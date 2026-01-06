#pragma once
#include <concepts>

namespace Super::Tool::Pointer
{
    template <std::destructible T>
    void Destroy(T*& p) noexcept
    {
        delete p;
        p = nullptr;
    }
}