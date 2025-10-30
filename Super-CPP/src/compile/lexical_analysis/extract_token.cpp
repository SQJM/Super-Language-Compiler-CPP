#include <super/compile/lexical_analysis/extract_token.h>
#include <fstream>
#include <algorithm>
#include <super/keyword.h>
#include <iostream>
#include <super/tool/string.h>
#include <super/compile/global_data.h>
#include <super/debug.h>
#include <super/type/token.h>
#include <super/error.h>
#include <super/tool/file.h>

namespace Super::Compile::LexicalAnalysis
{
	//if (Super::Keyword::Contains(value, Super::Keyword::TypeKeywords))
	//{
	//	tokenName = Super::Type::TokenName::TypeKeywords;
	//}
	//else if (Super::Keyword::Contains(value, Super::Keyword::PreprocessingInstructions))
	//{
	//	tokenName = Super::Type::TokenName::PreprocessingInstructions;
	//	if (value == L"#define")
	//	{
	//		isDefineName = true;
	//	}
	//	else if (value == L"#undef" || value == L"#ifdef" || value == L"#ifndef")
	//	{
	//		isSetDefineName = true;
	//	}
	//	goto end;
	//}
	//else if (Super::Keyword::Contains(value, Super::Keyword::Modifier))
	//{
	//	tokenName = Super::Type::TokenName::ModifierKeyword;
	//}
	//else if (Super::Keyword::Contains(value, Super::Keyword::Other))
	//{
	//	tokenName = Super::Type::TokenName::OtherKeywords;
	//}
	//else if (Super::Keyword::Contains(value, Super::Keyword::ControlFlow))
	//{
	//	tokenName = Super::Type::TokenName::ControlFlowKeywords;
	//}

	static void ProcessSemicolon(std::vector<Super::Type::Token>& tokens)
	{
		const std::vector<std::wstring> Symbols =
		{
			L"-",
			L"+",
			L"/",
			L"*",
			L".",
			L"|",
			L"^",
			L"&",
			L",",
			L"{"
		};
		size_t tokens_size = tokens.size();
		for (size_t i = 0; i < tokens_size; i++)
		{
			if (tokens[i].value == L";" &&
				i + 1 < tokens_size &&
				Symbols.end() != std::ranges::find(Symbols, tokens[i + 1].value))
			{
				tokens[i].name = Super::Type::TokenName::Null;
			}
		}
		Super::Type::ClearNullToken(&tokens);
	}

	std::vector<Super::Type::Token>* ExtractToken(const std::wstring &inputFile)
	{
		std::vector<Super::Type::Token>* tokens = new std::vector<Super::Type::Token>;
		std::vector<std::vector<wchar_t>> data = Super::Compile::GlobalData::FileDataList[inputFile];
		std::wstring line;
		std::wostringstream temp;
		Super::Type::TokenName tokenName = Super::Type::TokenName::None;

		for (size_t i = 0; i < data.size(); i++)
		{
			std::vector<wchar_t> lineData = data[i];
			if (lineData.empty()) continue;

			size_t lineLength = lineData.size();
		}

		ProcessSemicolon(*tokens);
		return tokens;
	}
}