#include <super/compile/core/multi_file_compile.h>
#include <super/tool/file.h>
#include <super/error.h>
#include <super/compile/ADB/apple_data_base.h>
#include <super/compile/global_data.h>
#include <super/compile/lexical_analysis/extract_token.h>
#include <super/compile/remove_note.h>
#include <iostream>
#include <super/compile/lexical_analysis/parenthesis.h>
#include <super/compile/lexical_analysis/to_token_group.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <super/type/token.h>
#include <super/compile/lexical_analysis/processing_preprocessing.h>
#include <super/tool/string.h>

namespace Super::Compile::Core
{
	std::wstring MultiFileCompile(const std::wstring& inputFile)
	{
		if (!std::filesystem::exists(inputFile))
		{
			SUPER_ERROR_THROW_CODE_NONE(inputFile, L"100030");
		}

		Super::Tool::File::FileType ft = Super::Tool::File::GetFileType(inputFile);
		if (ft == Super::Tool::File::FileType::Unknown)
		{
			SUPER_ERROR_THROW_MESSAGE(L"未知文件: " + inputFile)
		}
		Super::Compile::ADB::AppleDataBase::CreateABDFile(ft, inputFile);

		std::wstring _inputFilePath = std::filesystem::absolute(inputFile).wstring();
		std::wstring fileContent = Super::Tool::File::ReadFileAllContent(_inputFilePath);

		// 去除注释
		Super::Compile::GlobalData::FileDataList[inputFile] = Super::Compile::RemoveNote(fileContent);
		std::wcout << Super::Tool::String::ListToStringWithLineNumbers(Super::Compile::GlobalData::FileDataList[inputFile]);
		// 提取词组
		std::vector<Super::Type::Token>* tokens =
			Super::Compile::LexicalAnalysis::ExtractToken(_inputFilePath);

		if (tokens->empty())
		{
			SUPER_ERROR_THROW_CODE_NONE(inputFile, L"100040");
		}

		// 括号匹配
		Super::Compile::LexicalAnalysis::Parenthesis(_inputFilePath, tokens);
		//std::wcout << Super::Tool::String::TokenToString(tokens) << std::endl;

		// 预处理指令处理

		//std::wcout << Super::Tool::String::TokenToString(tokens) << std::endl;

		// 转 Token 组
		//Super::Type::TokenGroup tg = Super::Compile::LexicalAnalysis::ToTokenGroup(_inputFilePath, tokens).ToTokenGroupData();

		//std::wcout << L"Define List\n" << Super::Tool::String::DictionaryStringToString(Super::Compile::GlobalData::FileCompileDataList[_inputFilePath].Define) << std::endl;
	}
}