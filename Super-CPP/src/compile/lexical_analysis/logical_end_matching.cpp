#include <super/compile/lexical_analysis/logical_end_matching.h>
#include <super/error.h>
#include <stack>

namespace Super::Compile::LexicalAnalysis
{
	void LogicalEndMatching(const std::wstring& inputFilePath, std::vector<Super::Type::Token>* tokens)
	{
		std::stack<Super::Type::Token> stack;

		for (const auto& token : *tokens)
		{
			const std::wstring& str = token.value;

			if (str == L"#endif")
			{
				if (stack.empty())
				{
					SUPER_ERROR_THROW_CODE(inputFilePath, L"300010", token);
				}
				else
				{
					stack.pop();
				}
			}
			else if (str == L"#ifdef" || str == L"#ifndef" || str == L"#if")
			{
				stack.push(token);
			}
		}

		if (!stack.empty())
		{
			while (!stack.empty())
			{
				const auto& token = stack.top();
				SUPER_ERROR_CODE_CACHE(inputFilePath, L"", token);
				stack.pop();
			}
			SUPER_ERROR_CACHE_OUT
		}
	}
}
