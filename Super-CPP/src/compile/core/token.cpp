#include <super/compile/core/token.h>
#include <super/keyword.h>


namespace Super::Compile::Core
{
	std::vector<std::wstring_view> list
	{
	L"None",
	L"Rev",
	L"End",
	L"CombinationSymbol",
	L"TypeKeywords",
	L"ModifierKeyword",
	L"OtherKeywords",
	L"ControlFlowKeywords",
	L"PreprocessingInstructions",
	L"DefineName",
	L"DefineValue",
	L"ProcessMacro",
	L"Symbols",
	L"String",
	L"Char",
	L"VarName",
	L"BoolValue",
	L"FunctionName",
	L"Pointer",
	L"Null",
	L"Nullptr"
	};

	const std::wstring TokenNameToString(const TokenName& name)
	{
		auto idx = static_cast<size_t>(name);
		return idx < list.size() ? std::wstring{ list[idx] } : L"Unknown";
	}
}