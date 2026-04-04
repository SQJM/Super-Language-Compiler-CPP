module;
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
module Super.Compile.Process;
import Super.Keyword;
import Super.Util.String;

inline bool IsDigit(char c)
{
	return c >= '0' && c <= '9';
}

inline bool IsHexDigit(char c)
{
	return (c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F');
}

inline bool IsBinDigit(char c)
{
	return c == '0' || c == '1';
}

inline std::string ConvertRadixToDecimal(const std::string& token_value)
{
	if (token_value.length() >= 3)
	{
		if (token_value[0] == '0' && (token_value[1] == 'x' || token_value[1] == 'X'))
		{
			std::string hex_part = token_value.substr(2);
			size_t decimal_value = 0;
			for (char c : hex_part)
			{
				decimal_value *= 16;
				if (c >= '0' && c <= '9')
				{
					decimal_value += c - '0';
				}
				else if (c >= 'a' && c <= 'f')
				{
					decimal_value += c - 'a' + 10;
				}
				else if (c >= 'A' && c <= 'F')
				{
					decimal_value += c - 'A' + 10;
				}
			}
			return std::to_string(decimal_value);
		}
		if (token_value[0] == '0' && (token_value[1] == 'b' || token_value[1] == 'B'))
		{
			std::string bin_part = token_value.substr(2);
			size_t decimal_value = 0;
			for (char c : bin_part)
			{
				decimal_value *= 2;
				if (c == '1')
				{
					decimal_value += 1;
				}
			}
			return std::to_string(decimal_value);
		}
	}
	return token_value;
}

const std::unordered_set<char> Symbols =
{
	'$', '!', '-', '+', '/', '*', '=', '\\', '[',
	']', '{', '}', '.', ':', '?', '(', ')', '|',
	'^', '&', '%', '<', '>', ',', ';', '~', '@' };

const std::unordered_set<char> Rule_SymbolChar =
{
	'<', '>', '+', '=', '!', '&', '|', '-', ':' };

const std::unordered_set<char> Rule_SymbolCharBack =
{
	'<', '>', '+', '=', '&', '|', '-', ':' };

const std::unordered_set<char> Symbols2 =
{
	'-', '+', '/', '*', '|', '^', '&', '%', '~', '{' };

struct CombinationSymbolInfo
{
	char second;
	const char* value;
};

const std::unordered_map<char, std::vector<CombinationSymbolInfo>> CombinationSymbols =
{
	{'=', {{'=', "=="}}},
	{'!', {{'=', "!="}}},
	{'<', {{'<', "<<"}, {'=', "<="}}},
	{'>', {{'>', ">>"}, {'=', ">="}}},
	{'&', {{'&', "&&"}}},
	{'|', {{'|', "||"}}},
	{'+', {{'+', "++"}, {'=', "+="}}},
	{'-', {{'-', "--"}, {'=', "-="}}},
	{':', {{':', "::"}}} };

inline static void IdentificationTokenName(Super::Compile::Core::Token& token)
{
	std::string value = token.value;

	if (Super::Keyword::Contains(value, Super::Keyword::PreprocessingInstructions))
	{
		token.name = Super::Compile::Core::TokenName::PreprocessingInstructions;
	}
	else if (Super::Keyword::Contains(value, Super::Keyword::Modifier))
	{
		token.name = Super::Compile::Core::TokenName::ModifierKeyword;
	}
	else if (Super::Keyword::Contains(value, Super::Keyword::TypeKeywords))
	{
		token.name = Super::Compile::Core::TokenName::TypeKeywords;
	}
	else if (Super::Keyword::Contains(value, Super::Keyword::Other))
	{
		token.name = Super::Compile::Core::TokenName::OtherKeywords;
	}
	else if (Super::Keyword::Contains(value, Super::Keyword::ControlFlow))
	{
		token.name = Super::Compile::Core::TokenName::ControlFlowKeywords;
	}
	else if (value == "true" || value == "false")
	{
		token.name = Super::Compile::Core::TokenName::BoolValue;
	}
	else if (value[0] == '\'')
	{
		token.name = Super::Compile::Core::TokenName::Char;
	}
	else if (value[0] == '"')
	{
		token.name = Super::Compile::Core::TokenName::String;
	}
	else if (value == "null")
	{
		token.name = Super::Compile::Core::TokenName::Null;
		token.value = "";
	}
	else if (value == "nullptr")
	{
		token.name = Super::Compile::Core::TokenName::Nullptr;
		token.value = "";
	}
	else if (value == ";")
	{
		token.name = Super::Compile::Core::TokenName::End;
		token.value = "";
	}
	else if (!value.empty() && IsDigit(value[0]))
	{
		token.name = Super::Compile::Core::TokenName::Number;
		token.value = ConvertRadixToDecimal(value);
	}
	else
	{
		token.name = Super::Compile::Core::TokenName::None;
	}
}

namespace Super::Compile::Process::Pretreatment
{
	void ExtractToken(std::vector<std::vector<char>>* lines, std::vector<Super::Compile::Core::Token>* tokens)
	{
		std::string currentToken;
		Super::Compile::Core::Token token;
		bool string_continue_state = false;
		size_t scope = 0;
		bool is_symbol = false;
		bool is_combination_symbol = false;
		bool is_exceed = false;
		bool single_line_comment = false;
		bool multi_line_comment = false;
		bool line_continue_state = false;
		std::uint32_t continue_column = 1;
		std::uint32_t currentLine = 1;
		std::uint32_t currentColumn = 1;
		std::uint32_t commentStartLine = 1;
		std::uint32_t commentStartColumn = 1;

		auto flushToken = [&](void)
			{
				if (!currentToken.empty())
				{
					token.value = currentToken;
					IdentificationTokenName(token);
					token.ranks.line = currentLine;
					token.ranks.column = (continue_column > 1) ? continue_column : currentColumn;
					tokens->emplace_back(token);
					currentToken.clear();
					continue_column = 1;
				}
			};

		auto addSymbolToken = [&](char c)
			{
				token.value = std::string(1, c);
				token.name = Super::Compile::Core::TokenName::Symbols;
				token.ranks.line = currentLine;
				token.ranks.column = currentColumn;
				tokens->emplace_back(token);
			};

		auto addEndToken = [&](void)
			{
				token.value = "";
				token.name = Super::Compile::Core::TokenName::End;
				tokens->emplace_back(token);
			};

		auto addCommentToken = [&](bool isMultiLine)
			{
				if (isMultiLine)
				{
					if (currentToken.length() >= 4)
					{
						token.value = currentToken.substr(2, currentToken.length() - 4);
					}
					else
					{
						token.value = "";
					}
					token.name = Super::Compile::Core::TokenName::MultiLineComment;
				}
				else
				{
					if (currentToken.length() >= 2)
					{
						token.value = currentToken.substr(1);
					}
					else
					{
						token.value = "";
					}
					token.name = Super::Compile::Core::TokenName::SingleLineComment;
				}
				token.ranks.line = commentStartLine;
				token.ranks.column = commentStartColumn;
				tokens->emplace_back(token);
				currentToken.clear();
			};

		size_t lineCount = lines->size();
		for (size_t lineIndex = 0; lineIndex < lineCount; lineIndex++)
		{
			auto& line = (*lines)[lineIndex];
			size_t lineSize = line.size();
			for (size_t j = 0; j < lineSize; j++)
			{
				char c = line[j];
				is_exceed = j + 1 < lineSize;

				if (single_line_comment)
				{
					currentToken += c;
					currentColumn++;
					continue;
				}

				if (multi_line_comment)
				{
					currentToken += c;
					currentColumn++;
					if (c == '-' && is_exceed && line[j + 1] == ';')
					{
						addCommentToken(true);
						multi_line_comment = false;
						j++;
						currentColumn++;
					}
					continue;
				}

				if (string_continue_state)
				{
					currentToken += c;
					currentColumn++;
					if (j + 1 < lineSize)
					{
						string_continue_state =
							Super::Util::String::BreakString(j + 1, std::string_view(line.data(), line.size()), scope);
					}
					else
					{
						string_continue_state = false;
					}
					continue;
				}

				if (scope > 0)
				{
					scope--;
					currentToken += c;
					currentColumn++;
				}
				else if (c == '"' || c == '\'')
				{
					flushToken();
					currentToken += c;
					if (j + 1 < lineSize)
					{
						string_continue_state =
							Super::Util::String::BreakString(j + 1, std::string_view(line.data(), line.size()), scope);
					}
					else
					{
						string_continue_state = false;
					}
					currentColumn++;
				}
				else if (Super::Util::String::IsWhitespace(c) && !line_continue_state)
				{
					flushToken();
					currentColumn++;
				}
				else if (Super::Util::String::IsWhitespace(c) && line_continue_state)
				{
					currentColumn++;
				}
				else if (c == ';')
				{
					flushToken();
					if (is_exceed && line[j + 1] == '-')
					{
						multi_line_comment = true;
						commentStartLine = currentLine;
						commentStartColumn = currentColumn;
						currentToken += ";-";
						j++;
						currentColumn += 2;
					}
					else
					{
						single_line_comment = true;
						commentStartLine = currentLine;
						commentStartColumn = currentColumn;
						currentToken += c;
						currentColumn++;
					}
					continue;
				}
				else if (Rule_SymbolChar.contains(c) && is_exceed &&
						 Rule_SymbolCharBack.contains(line[j + 1]))
				{
					flushToken();
					char nextChar = line[j + 1];
					auto it = CombinationSymbols.find(c);
					if (it != CombinationSymbols.end())
					{
						for (const auto& info : it->second)
						{
							if (info.second == nextChar)
							{
								token.value = info.value;
								break;
							}
						}
					}
					if (!token.value.empty())
					{
						token.name = Super::Compile::Core::TokenName::CombinationSymbol;
						token.ranks.column = currentColumn;
						tokens->emplace_back(token);
						token.value.clear();
						j++;
						currentColumn += 2;
					}
					else
					{
						currentToken += c;
						currentColumn++;
					}
				}
				else if (c == '\\')
				{
					if (j + 1 >= lineSize)
					{
						line_continue_state = true;
						continue_column = currentColumn;
					}
					else
					{
						currentToken += c;
						currentColumn++;
					}
				}
				else if (Symbols.contains(c))
				{
					flushToken();
					if (c == '@')
					{
						currentToken += c;
						token.name = Super::Compile::Core::TokenName::Pointer;
						currentColumn++;
						if (is_exceed && line[j + 1] != '@')
						{
							token.value = currentToken;
							token.ranks.column = currentColumn - currentToken.size();
							tokens->emplace_back(token);
							currentToken.clear();
							token.name = Super::Compile::Core::TokenName::None;
						}
					}
					else
					{
						addSymbolToken(c);
						currentColumn++;
					}
				}
				else
				{
					currentToken += c;
					currentColumn++;
				}
			}

			if (single_line_comment || multi_line_comment)
			{
				if (!multi_line_comment)
				{
					addCommentToken(false);
					single_line_comment = false;
				}
			}
			else
			{
				if (!line_continue_state)
				{
					flushToken();
				}
			}

			if (!tokens->empty() && !line_continue_state)
			{
				auto& lastToken = tokens->back();
				if (lastToken.name != Super::Compile::Core::TokenName::End &&
					lastToken.name != Super::Compile::Core::TokenName::SingleLineComment &&
					lastToken.name != Super::Compile::Core::TokenName::MultiLineComment)
				{
					bool next_line_has_operator = false;
					if (lineIndex + 1 < lineCount)
					{
						const auto& next_line = (*lines)[lineIndex + 1];
						for (size_t k = 0; k < next_line.size(); k++)
						{
							char next_c = next_line[k];
							if (Super::Util::String::IsWhitespace(next_c))
								continue;

							if (Symbols.contains(next_c))
							{
								next_line_has_operator = true;
							}
							break;
						}
					}

					if (!next_line_has_operator)
					{
						addEndToken();
					}
				}
			}

			if (!line_continue_state)
			{
				currentLine++;
				currentColumn = 1;
			}
			else
			{
				currentColumn = 1;
				line_continue_state = false;
			}
			token.ranks.line = currentLine;
		}

		if (single_line_comment || multi_line_comment)
		{
			addCommentToken(multi_line_comment);
		}
		else
		{
			flushToken();
		}

		if (!tokens->empty())
		{
			auto& lastToken = tokens->back();
			if (lastToken.name != Super::Compile::Core::TokenName::End &&
				lastToken.name != Super::Compile::Core::TokenName::SingleLineComment &&
				lastToken.name != Super::Compile::Core::TokenName::MultiLineComment)
			{
				std::string lastValue = lastToken.value;
				if (!lastValue.empty())
				{
					char lastChar = lastValue.back();
					if (!Symbols.contains(lastChar))
					{
						addEndToken();
					}
					else if (!Symbols2.contains(lastChar))
					{
						addEndToken();
					}
				}
			}
		}
	}
}  // namespace Super::Compile::Process::Pretreatment