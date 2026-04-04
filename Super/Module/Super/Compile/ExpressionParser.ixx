module;
#include <cstdint>
#include <string>
#include <vector>
export module Super.Compile.ExpressionParser;

export namespace Super::Compile::ExpressionParser
{
	enum class OperationSymbol
	{
		add,
		subtract,
		multiply,
		division,
		logical_not,
		bitwise_and,
		bitwise_or,
		bitwise_xor,
		bitwise_not,
		left_shift,
		right_shift,
		assignment,
		add_assignment,
		equal,
		not_equal,
		greater,
		less,
		greater_equal,
		less_equal,
	};

	enum class ComparisonOperatorSymbol
	{
		logical_and,
		logical_or,
	};
} // namespace Super::Compile::ExpressionParser