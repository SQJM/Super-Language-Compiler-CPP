#pragma once
#include <string>
#include <vector>
#include <super/type/token_name.h>

namespace Super::Type
{
	struct LineColumn
	{
		size_t line;
		size_t column;
	};

	struct Token
	{
		LineColumn lc;
		TokenName name;
		std::wstring value;
	};

	inline void ClearNullToken(std::vector<Super::Type::Token>* tokens)
	{
		tokens->erase(std::remove_if(tokens->begin(), tokens->end(), [](const Super::Type::Token& token)
									{
										return token.name == Super::Type::TokenName::Null;
									}),
					 tokens->end());
	}
}