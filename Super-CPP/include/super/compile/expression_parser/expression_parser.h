#pragma once
#include <string>
#include <vector>
#include <super/type/token.h>
#include <super/type/expression_parser_type.h>
#include <super/config.h>
#include <unordered_map>

namespace Super::Compile::ExpressionParser
{
	// Rule
	std::unordered_map<std::wstring,std::wstring> Rule_OperationSymbol = {
		MAP_DEF("+","add"),
		MAP_DEF("-","subtract"),
		MAP_DEF("*","multiply"),
		MAP_DEF("/","division"),
		MAP_DEF("&","bitwise_and"),
		MAP_DEF("|","bitwise_or"),
		MAP_DEF("^","bitwise_xor"),
		MAP_DEF("<<","left_shift"),
		MAP_DEF(">>","right_shift"),
		MAP_DEF("=","assignment"),
		MAP_DEF("+=","add_assignment")
	};

	std::unordered_map<std::wstring, std::wstring> Rule_ComparisonOperatorSymbol = {
		MAP_DEF("==","equal"),
		MAP_DEF("!=","not_equal"),
		MAP_DEF(">","greater"),
		MAP_DEF("<","less"),
		MAP_DEF(">=","greater_equal"),
		MAP_DEF("<=","less_equal"),
		MAP_DEF("&&","logical_and"),
		MAP_DEF("||","logical_or")
	};

	class Parser
	{
	public:
		Parser(const std::wstring& inputFilePath, std::vector<Super::Type::Token>& tokens);
	private:
		std::wstring _inputFilePath;

		Super::Type::ExpressionParser::Expression _expression;

		void parserNextTarget(std::vector<Super::Type::Token>& tokens);
	};
}