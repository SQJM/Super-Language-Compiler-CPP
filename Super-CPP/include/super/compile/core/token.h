#pragma once
#include <string>
#include <vector>
#include <super/compile/super_value_type.h>

namespace Super::Compile::Core
{
	enum class TokenName
	{
		None,
		Rev,
		End,
		// 组合符号
		CombinationSymbol,
		// 类型关键字
		TypeKeywords,
		// 修饰符关键字
		ModifierKeyword,
		// 其他关键字
		OtherKeywords,
		// 控制流关键字
		ControlFlowKeywords,
		// 预处理指令
		PreprocessingInstructions,
		// 宏名字
		DefineName,
		// 宏定义的值
		DefineValue,
		// 过程宏
		ProcessMacro,
		// 符号
		Symbols,
		// 字符串
		String,
		// 字符
		Char,
		// 变量名
		VarName,
		// 布尔值
		BoolValue,
		// 功能名
		FunctionName,
		// 指针
		Pointer,
		// 空值
		Null,
		// 空指针
		Nullptr
	};

	enum class RValueTokenType
	{
		None,
		Bool,
		Char,
		WChar,
		Int16,
		Int32,
		Int64,
		Int128,
		Float,
		Double,
		String,
		WString
	};

	const std::wstring TokenNameToString(const TokenName& name);

	struct Ranks
	{
		uint32_t line = 0;
		uint32_t column = 0;
	};

	struct Token
	{
		Ranks ranks;
		TokenName name = TokenName::None;
		std::wstring value;
	};

	struct RValueToken
	{
		size_t token_index = 0;
		RValueTokenType type = RValueTokenType::None;
		Super::SVT::RValue rvalue;
	};

	inline void ClearNullToken(std::vector<Super::Compile::Core::Token>* tokens)
	{
		tokens->erase(std::remove_if(tokens->begin(), tokens->end(), [](const Super::Compile::Core::Token& token)
			{
				return token.name == Super::Compile::Core::TokenName::Rev;
			}),
			tokens->end());
	}
}