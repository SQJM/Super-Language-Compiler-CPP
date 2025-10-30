#pragma once
#include <super/type/token.h>
#include <vector>

namespace Super::Compile::LexicalAnalysis
{
	std::vector<Super::Type::Token>* ExtractToken(const std::wstring& inputFile);
}

