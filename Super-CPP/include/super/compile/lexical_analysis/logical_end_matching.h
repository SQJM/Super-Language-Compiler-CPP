#pragma once
#include <string>
#include <vector>
#include <super/type/token.h>

namespace Super::Compile::LexicalAnalysis
{
	void LogicalEndMatching(const std::wstring& inputFilePath, std::vector<Super::Type::Token>* tokens);
}