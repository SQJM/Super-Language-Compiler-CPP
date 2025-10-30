#include <super/compile/expression_parser/expression_parser.h>

namespace Super::Compile::ExpressionParser
{
	// 括号解析
	static void parser_bracket(std::vector<Super::Type::Token>& tokens)
	{}

	// 过程宏功能调用解析
	static void parser_advanced_macro_function_call(std::vector<Super::Type::Token>& tokens)
	{}

	// 高级变量解析
	static void parser_advanced_variable(std::vector<Super::Type::Token>& tokens)
	{}

	// 普通变量解析
	static void parser_variable(std::vector<Super::Type::Token>& tokens)
	{}

	// 宏变量解析
	static void parser_macro_variable(std::vector<Super::Type::Token>& tokens)
	{}

	// 比较运算符解析
	static void parser_comparison_operator(std::vector<Super::Type::Token>& tokens)
	{}

	// 功能调用解析
	static void parser_function_call(std::vector<Super::Type::Token>& tokens)
	{}

	// 成员访问解析
	static void parser_member_access(std::vector<Super::Type::Token>& tokens)
	{}

	// 运算符解析
	static void parser_operator(std::vector<Super::Type::Token>& tokens)
	{
		std::unordered_map<std::wstring, std::wstring> char_opr = {
			MAP_DEF("+","add"),
			MAP_DEF("-","subtract"),
			MAP_DEF("*","multiply"),
			MAP_DEF("/","division"),
			MAP_DEF("&","bitwise_and"),
			MAP_DEF("|","bitwise_or"),
			MAP_DEF("^","bitwise_xor"),
			MAP_DEF(">","greater"),
			MAP_DEF("<","less"),
			MAP_DEF("=","assignment")
		};

		std::unordered_map<std::wstring, std::wstring> char_2_opr = {
			MAP_DEF("==","equal"),
			MAP_DEF("!=","not_equal"),
			MAP_DEF(">=","greater_equal"),
			MAP_DEF("<=","less_equal"),
			MAP_DEF("&&","logical_and"),
			MAP_DEF("+=","add_assignment"),
			MAP_DEF("<<","left_shift"),
			MAP_DEF(">>","right_shift"),
			MAP_DEF("||","logical_or")
		};
	}

	Parser::Parser(const std::wstring& inputFilePath, std::vector<Super::Type::Token>& tokens)
	{

	}

	void Parser::parserNextTarget(std::vector<Super::Type::Token>& tokens)
	{}
}