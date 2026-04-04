module;
#include <string>
module Super.Compile.Core;

namespace Super::Compile::Core
{
	const std::string TokenNameToString(const TokenName& name)
	{
		switch (name)
		{
		case TokenName::None:
			return "None";
		case TokenName::End:
			return "End";
		case TokenName::CombinationSymbol:
			return "CombinationSymbol";
		case TokenName::TypeKeywords:
			return "TypeKeywords";
		case TokenName::ModifierKeyword:
			return "ModifierKeyword";
		case TokenName::OtherKeywords:
			return "OtherKeywords";
		case TokenName::ControlFlowKeywords:
			return "ControlFlowKeywords";
		case TokenName::PreprocessingInstructions:
			return "PreprocessingInstructions";
		case TokenName::DefineName:
			return "DefineName";
		case TokenName::DefineValue:
			return "DefineValue";
		case TokenName::ProcessMacro:
			return "ProcessMacro";
		case TokenName::Symbols:
			return "Symbols";
		case TokenName::String:
			return "String";
		case TokenName::Char:
			return "Char";
		case TokenName::VarName:
			return "VarName";
		case TokenName::BoolValue:
			return "BoolValue";
		case TokenName::FunctionName:
			return "FunctionName";
		case TokenName::Pointer:
			return "Pointer";
		case TokenName::Number:
			return "Number";
		case TokenName::Null:
			return "Null";
		case TokenName::Nullptr:
			return "Nullptr";
		case TokenName::SingleLineComment:
			return "SingleLineComment";
		case TokenName::MultiLineComment:
			return "MultiLineComment";
		default:
			return "Unknown";
		}
	}
}  // namespace Super::Compile::Core