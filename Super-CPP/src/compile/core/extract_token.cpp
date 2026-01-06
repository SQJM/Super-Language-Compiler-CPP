#include <super/compile/core/extract_token.h>
#include <fstream>
#include <algorithm>
#include <super/keyword.h>
#include <iostream>
#include <super/tool/string.h>
#include <super/compile/global_data.h>
#include <super/debug.h>
#include <super/compile/core/token.h>
#include <super/error.h>
#include <super/tool/file.h>

const std::vector<std::wstring> Symbols =
{
	L"$",
	L"!",
	L"-",
	L"+",
	L"/",
	L"*",
	L"=",
	L"\\",
	L"[",
	L"]",
	L"{",
	L"}",
	L".",
	L":",
	L"?",
	L"(",
	L")",
	L"|",
	L"^",
	L"&",
	L"%",
	L"<",
	L">",
	L",",
	L";",
	L"~",
	L"@"
};

const std::vector<std::wstring> Rule_SymbolChar =
{
	L"<",
	L">",
	L"+",
	L"=",
	L"!",
	L"&",
	L"|",
	L"-",
	L":"
};

const std::vector<std::wstring> Rule_SymbolCharBack =
{
	L"<",
	L">",
	L"+",
	L"=",
	L"&",
	L"|",
	L"-",
	L":"
};

namespace Super::Compile::Core
{
	inline static void IdentificationTokenName(Super::Compile::Core::Token& token)
	{
		std::wstring value = token.value;
		
		if (Super::Keyword::Contains(value, Super::Keyword::PreprocessingInstructions))
		{
			token.name = Super::Compile::Core::TokenName::PreprocessingInstructions;
		}
		else if (Super::Keyword::Contains(value, Super::Keyword::Modifier))
		{
			token.name = Super::Compile::Core::TokenName::ModifierKeyword;
		}
		else if (Super::Keyword::Contains(value, Super::Keyword::Other))
		{
			token.name = Super::Compile::Core::TokenName::OtherKeywords;
		}
		else if (Super::Keyword::Contains(value, Super::Keyword::ControlFlow))
		{
			token.name = Super::Compile::Core::TokenName::ControlFlowKeywords;
		}
		else if (value == L"true" || value == L"false")
		{
			token.name = Super::Compile::Core::TokenName::BoolValue;
		}
		else if (value[0] == L'\'')
		{
			token.name = Super::Compile::Core::TokenName::Char;
		}
		else if (value[0] == L'"')
		{
			token.name = Super::Compile::Core::TokenName::String;
		}
		else if (value == L"null")
		{
			token.name = Super::Compile::Core::TokenName::Null;
			token.value = L"";
		}
		else if (value == L"nullptr")
		{
			token.name = Super::Compile::Core::TokenName::Nullptr;
			token.value = L"";
		}
		else if (value == L";")
		{
			token.name = Super::Compile::Core::TokenName::End;
			token.value = L"";
		}
		else
		{
			token.name = Super::Compile::Core::TokenName::None;
		}
	}

	void ExtractToken(const std::wstring &inputFile, std::vector<Super::Compile::Core::Token>* tokens)
	{
		auto& data = Super::Compile::GlobalData::FileDataList[inputFile];
		std::wstring lineStr;
		Super::Compile::Core::Token token;
		token.value = L"";
		bool string_continue_state = false;
		size_t scope = 0;
		bool is_symbol = false;
		bool is_combination_symbol = false;
		bool is_exceed = false;

		for (size_t line = 0; line < data.size(); line++)
		{
			std::vector<wchar_t> lineData = data[line];
			if (lineData.empty()) continue;
			size_t lineLength = lineData.size();
			lineStr = std::wstring(lineData.begin(), lineData.end());
			token.ranks.line = line + 1;
			for (size_t column = 0; column < lineLength; column++)
			{
				wchar_t c = lineData[column];
				is_exceed = column + 1 < lineLength - 1;

				if (c == L'@')
				{
					token.value += L'@';
					if (is_exceed && lineData[column + 1] != L'@')
					{
						token.name = Super::Compile::Core::TokenName::Pointer;
						goto next_1;
					}
					continue;
				}

				if (string_continue_state)
				{
					if (lineData.empty())
					{
						continue;
					}
					else if (!lineData.empty())
					{
						string_continue_state = Super::Tool::String::BreakString(column, lineStr, scope);
					}
				}

				if (scope > 0)
				{
					scope--;
				}
				else if(c == L'\"' || c == L'\'')
				{
					string_continue_state = Super::Tool::String::BreakString(column + 1, lineStr, scope);
				}
				else if (Super::Tool::String::IsWhitespace(c))
				{
					if (!token.value.empty())
					{
						goto next;
					}
					continue;
				}
				else if (Super::Keyword::Contains(c, Rule_SymbolChar) && is_exceed && Super::Keyword::Contains(lineData[column + 1], Rule_SymbolCharBack))
				{
					if (!token.value.empty())
					{
						is_combination_symbol = true;
						goto next;
					}
					continue_combination_symbol:
					wchar_t dc = lineData[column + 1];
					if (c == L'=')
					{
						if (dc == L'=') { token.value = L"=="; }
					}
					else if (c == L'!') 
					{
						if (dc == L'=') { token.value = L"!="; }
					}
					else if (c == L'<')
					{
						if (dc == L'<') { token.value = L"<<"; }
						else if (dc == L'=') { token.value = L"<="; }
					}
					else if (c == L'>') 
					{
						if (dc == L'>') { token.value = L">>"; }
						else if (dc == L'=') { token.value = L">="; }
					}
					else if (c == L'&') 
					{
						if (dc == L'&') { token.value = L"&&"; }
					}
					else if (c == L'|')
					{
						if (dc == L'|') { token.value = L"||"; }
					}
					else if (c == L'+') 
					{
						if (dc == L'+') { token.value = L"++"; }
						else if (dc == L'=') { token.value = L"+="; }
					}
					else if (c == L'-') 
					{
						if (dc == L'-') { token.value = L"--"; }
						else if (dc == L'=') { token.value = L"-="; }
					}
					else if (c == L':')
					{
						if (dc == L':') { token.value = L"::"; }
					}
					token.name = Super::Compile::Core::TokenName::CombinationSymbol;
					column++;
					goto next_1;
				}
				else if (Super::Keyword::Contains(c, Symbols))
				{
					is_symbol = true;
					if (token.value.empty())
					{
						goto next_2;
					}
					else
					{
						goto next;
					}
				}

				token.value += c;
				if (is_exceed && lineData[column + 1] == L'@')
				{
					goto next;
				}

				if (column == lineLength - 1 && scope == 0)
				{
				next:
					IdentificationTokenName(token);
				next_1:
					token.ranks.column = column + 1 - token.value.size();
					tokens->emplace_back(Super::Compile::Core::Token(token));
					token.value = L"";
				next_2:
					if (is_symbol)
					{
						is_symbol = false;
						token.value += c;
						goto next;
					}
					if (is_combination_symbol)
					{
						is_combination_symbol = false;
						goto continue_combination_symbol;
					}
				}
			}
		}
	}
}