#pragma once
#include <string>
#include <vector>
#include <super/compile/core/token.h>

namespace Super::Compile::Core
{
	void SymmetricalMatching(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens);
}