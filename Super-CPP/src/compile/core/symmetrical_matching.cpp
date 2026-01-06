#include <super/compile/core/symmetrical_matching.h>
#include <super/error.h>
#include <stdexcept>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <sstream>

std::unordered_map<std::wstring, std::wstring> errorMessages =
{
	{L"(", L"200000"},
	{L")", L"200010"},
	{L"[", L"200040"},
	{L"]", L"200050"},
	{L"{", L"200020"},
	{L"}", L"200030"}
};

namespace Super::Compile::Core
{
	template <typename Stack>
	static void ReportUnclosed(const std::wstring& filePath,
		Stack& sk,
		const std::wstring& errCode)
	{
		if (sk.empty()) { return; }

        std::vector<typename Stack::value_type> tmp;
        tmp.reserve(sk.size());
        while (!sk.empty())
        {
            tmp.emplace_back(sk.top());
            sk.pop();
        }

        for (auto it = tmp.rbegin(); it != tmp.rend(); ++it)
        {
            SUPER_ERROR_CODE_CACHE(filePath, errCode, *it);
        }
		SUPER_ERROR_CACHE_OUT
	}

	void SymmetricalMatching(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens)
	{
        std::stack<Super::Compile::Core::Token> stacks[5];
        /* 0:{}, 1:(), 2:[], 3:#if*, 4:#asm */

        static const struct
        {
            const wchar_t* open;
            const wchar_t* close;
            const wchar_t* errOpen;   /* ReportUnclosed 用 */
        } pair[5] = {
            {L"{" , L"}" , errorMessages.at(L"{").c_str()},
            {L"(" , L")" , errorMessages.at(L"(").c_str()},
            {L"[" , L"]" , errorMessages.at(L"[").c_str()},
            {L""  , L"#endif", L"300020"},   /* #ifdef/#ifndef/#if 共用 */
            {L""  , L"#end",   L"300040"}    /* #asm */
        };

        for (const auto& token : *tokens)
        {
            const std::wstring& str = token.value;
            bool handled = false;

            for (std::size_t i = 0; i < 5; ++i)
            {
                if (str == pair[i].close)
                {
                    if (stacks[i].empty())
                    {
                        SUPER_ERROR_THROW_CODE(inputFilePath, pair[i].errOpen, token);
                    }
                    else
                    {
                        stacks[i].pop();
                    }
                    handled = true;
                    break;
                }
            }
            if (handled) { continue; }

            if (str == L"{") { stacks[0].push(token); continue; }
            if (str == L"(") { stacks[1].push(token); continue; }
            if (str == L"[") { stacks[2].push(token); continue; }
            if (str == L"#ifdef" || str == L"#ifndef" || str == L"#if") { stacks[3].push(token); continue; }
            if (str == L"#asm") { stacks[4].push(token); continue; }
        }

        for (std::size_t i = 0; i < 5; ++i)
        {
            ReportUnclosed(inputFilePath, stacks[i], pair[i].errOpen);
        }
	}
}