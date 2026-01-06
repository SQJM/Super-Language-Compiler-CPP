#pragma once
#include <string>
#include <vector>

namespace Super::Compile
{
	std::vector<std::vector<wchar_t>> RemoveComments(const std::wstring& fileData);
}