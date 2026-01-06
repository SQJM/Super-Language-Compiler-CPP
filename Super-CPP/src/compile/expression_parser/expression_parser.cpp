#include <super/compile/expression_parser/expression_parser.h>

// 括号解析
static void parser_bracket(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 过程宏功能调用解析
static void parser_advanced_macro_function_call(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 高级变量解析
static void parser_advanced_variable(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 普通变量解析
static void parser_variable(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 宏变量解析
static void parser_macro_variable(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 比较运算符解析
static void parser_comparison_operator(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 功能调用解析
static void parser_function_call(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 成员访问解析
static void parser_member_access(std::vector<Super::Compile::Core::Token>& tokens)
{
}

// 运算符解析
static void parser_operator(std::vector<Super::Compile::Core::Token>& tokens)
{
}

namespace Super::Compile::ExpressionParser
{
	void Parser::parserNextTarget()
	{}

	bool Parser::PreprocessingInstructionsLogicalCondition(const std::vector<size_t>& token_indexs)
	{
		for (auto& index : token_indexs)
		{
			auto& token = this->_tokens->at(index);

		}
		return false;
	}
	
}