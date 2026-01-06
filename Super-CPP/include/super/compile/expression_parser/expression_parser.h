#pragma once
#include <string>
#include <vector>
#include <super/compile/core/token.h>
#include <super/config.h>
#include <unordered_map>
#include <stack>
#include <variant>

namespace Super::Compile::ExpressionParser
{
	enum class OperationSymbol
	{
		add,            // +
		subtract,       // -
		multiply,       // *
		division,       // /
		logical_not,    // !
		bitwise_and,    // &
		bitwise_or,     // |
		bitwise_xor,    // ^
		bitwise_not,    // ~
		left_shift,     // <<
		right_shift,    // >>
		assignment,     // =
		add_assignment, // +=
		equal,          // ==
		not_equal,      // !=
		greater,        // >
		less,           // <
		greater_equal,  // >=
		less_equal,     // <=
	};

	enum class ComparisonOperatorSymbol
	{
		logical_and,    // &&
		logical_or      // ||
	};

	class Parser
	{
	public:
		Parser(const std::wstring& path, std::vector<Super::Compile::Core::Token>* tokens) :
			_inputFilePath(path),
			_tokens(tokens)
		{
		}

		bool PreprocessingInstructionsLogicalCondition(const std::vector<size_t>& token_indexs);

	private:
		std::wstring _inputFilePath;
		std::vector<Super::Compile::Core::Token>* _tokens;

		void parserNextTarget();
	};
}