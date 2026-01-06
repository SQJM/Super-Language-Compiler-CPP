#pragma once
#include <super/compile/core/token.h>
#include <vector>

namespace Super::Compile::Core
{
	void ExtractToken(const std::wstring& inputFile, std::vector<Super::Compile::Core::Token>* tokens);
}

