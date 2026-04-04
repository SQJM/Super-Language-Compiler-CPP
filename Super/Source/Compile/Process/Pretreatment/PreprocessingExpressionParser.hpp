#ifndef SUPER_PREPROCESSING_EXPRESSION_PARSER_HPP
#define SUPER_PREPROCESSING_EXPRESSION_PARSER_HPP

#include <string>
#include <vector>
#include <stack>
#include <functional>
#include <cctype>
#include <cmath>

namespace Super::Compile::Process::Pretreatment
{
	class PreprocessingExpressionParser
	{
	public:
		using MacroLookup = std::function<std::string(const std::string&)>;

	private:
		std::string expr;
		size_t pos = 0;
		MacroLookup lookup;

		char Peek() const
		{
			return pos < expr.size() ? expr[pos] : '\0';
		}

		char Advance()
		{
			return pos < expr.size() ? expr[pos++] : '\0';
		}

		void SkipWhitespace()
		{
			while (pos < expr.size() && (expr[pos] == ' ' || expr[pos] == '\t' || expr[pos] == '\r'))
			{
				++pos;
			}
		}

		bool Match(const char* str)
		{
			size_t saved = pos;
			while (*str)
			{
				if (Peek() != *str)
				{
					pos = saved;
					return false;
				}
				Advance();
				++str;
			}
			return true;
		}

		bool MatchChar(char c)
		{
			if (Peek() == c)
			{
				Advance();
				return true;
			}
			return false;
		}

		char PeekNext() const
		{
			return pos + 1 < expr.size() ? expr[pos + 1] : '\0';
		}

		int64_t ParseNumber()
		{
			SkipWhitespace();
			bool negative = false;
			if (Peek() == '-')
			{
				negative = true;
				Advance();
			}
			else if (Peek() == '+')
			{
				Advance();
			}

			int64_t value = 0;
			if (Peek() == '0' && (PeekNext() == 'x' || PeekNext() == 'X'))
			{
				Advance();
				Advance();
				while (std::isxdigit(Peek()))
				{
					char c = Advance();
					value = value * 16 + (std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10);
				}
				return negative ? -value : value;
			}
			else if (Peek() == '0' && (PeekNext() == 'b' || PeekNext() == 'B'))
			{
				Advance();
				Advance();
				while (Peek() == '0' || Peek() == '1')
				{
					value = value * 2 + (Advance() - '0');
				}
				return negative ? -value : value;
			}

			while (std::isdigit(Peek()))
			{
				value = value * 10 + (Advance() - '0');
			}
			return negative ? -value : value;
		}

		int64_t ParseIdentifier()
		{
			SkipWhitespace();
			std::string ident;
			while (std::isalnum(Peek()) || Peek() == '_')
			{
				ident += Advance();
			}

			if (ident == "true" || ident == "TRUE")
			{
				return 1;
			}
			if (ident == "false" || ident == "FALSE")
			{
				return 0;
			}

			if (lookup)
			{
				std::string val = lookup(ident);
				if (!val.empty())
				{
					try
					{
						if (val[0] == '"')
						{
							val = val.substr(1);
							if (!val.empty() && val.back() == '"')
							{
								val.pop_back();
							}
						}
						while (true)
						{
							size_t p = val.find("\\n");
							if (p == std::string::npos)
							{
								break;
							}
							val.replace(p, 2, "\n");
						}
						if (val == "true" || val == "TRUE")
						{
							return 1;
						}
						if (val == "false" || val == "FALSE")
						{
							return 0;
						}
						return std::stoll(val, nullptr, 10);
					}
					catch (...)
					{
						return 0;
					}
				}
			}
			return 0;
		}

		int64_t ParseFactor()
		{
			SkipWhitespace();
			if (MatchChar('!'))
			{
				return ParseFactor() == 0 ? 1 : 0;
			}
			if (MatchChar('-'))
			{
				return -ParseFactor();
			}
			if (MatchChar('+'))
			{
				return ParseFactor();
			}
			if (MatchChar('('))
			{
				int64_t value = ParseOr();
				SkipWhitespace();
				MatchChar(')');
				return value;
			}
			return ParseIdentifier();
		}

		int64_t ParseTerm()
		{
			int64_t left = ParseFactor();
			while (true)
			{
				SkipWhitespace();
				if (MatchChar('*'))
				{
					left *= ParseFactor();
				}
				else if (MatchChar('/'))
				{
					int64_t rhs = ParseFactor();
					left = (rhs != 0) ? left / rhs : 0;
				}
				else if (MatchChar('%'))
				{
					int64_t rhs = ParseFactor();
					left = (rhs != 0) ? left % rhs : 0;
				}
				else
				{
					break;
				}
			}
			return left;
		}

		int64_t ParseExpr3()
		{
			int64_t left = ParseTerm();
			while (true)
			{
				SkipWhitespace();
				if (MatchChar('+'))
				{
					left += ParseTerm();
				}
				else if (MatchChar('-'))
				{
					left -= ParseTerm();
				}
				else
				{
					break;
				}
			}
			return left;
		}

		int64_t ParseExpr2()
		{
			int64_t left = ParseExpr3();
			while (true)
			{
				SkipWhitespace();
				if (Match("<="))
				{
					left = (left <= ParseExpr3()) ? 1 : 0;
				}
				else if (Match(">="))
				{
					left = (left >= ParseExpr3()) ? 1 : 0;
				}
				else if (MatchChar('<'))
				{
					left = (left < ParseExpr3()) ? 1 : 0;
				}
				else if (MatchChar('>'))
				{
					left = (left > ParseExpr3()) ? 1 : 0;
				}
				else
				{
					break;
				}
			}
			return left;
		}

		int64_t ParseExpr1()
		{
			int64_t left = ParseExpr2();
			while (true)
			{
				SkipWhitespace();
				if (Match("=="))
				{
					left = (left == ParseExpr2()) ? 1 : 0;
				}
				else if (Match("!="))
				{
					left = (left != ParseExpr2()) ? 1 : 0;
				}
				else
				{
					break;
				}
			}
			return left;
		}

		int64_t ParseExpr0()
		{
			int64_t left = ParseExpr1();
			while (true)
			{
				SkipWhitespace();
				if (Match("&&"))
				{
					left = (left && ParseExpr1()) ? 1 : 0;
				}
				else
				{
					break;
				}
			}
			return left;
		}

		int64_t ParseOr()
		{
			int64_t left = ParseExpr0();
			while (true)
			{
				SkipWhitespace();
				if (Match("||"))
				{
					left = (left || ParseExpr0()) ? 1 : 0;
				}
				else
				{
					break;
				}
			}
			return left;
		}

	public:
		explicit PreprocessingExpressionParser(MacroLookup lookupFn = nullptr) : lookup(std::move(lookupFn)) {
		}

		int64_t Evaluate(const std::string& expression)
		{
			expr = expression;
			pos = 0;
			try
			{
				return ParseOr();
			}
			catch (...)
			{
				return 0;
			}
		}
	};

	class PreprocessingExpressionEvaluator
	{
	public:
		static int64_t Evaluate(const std::string& expr, std::function<std::string(const std::string&)> lookup = nullptr)
		{
			PreprocessingExpressionParser parser(lookup);
			return parser.Evaluate(expr);
		}

		static bool EvaluateCondition(const std::string& expr, std::function<std::string(const std::string&)> lookup = nullptr)
		{
			return Evaluate(expr, lookup) != 0;
		}
	};
}

#endif