#include <super/compile/process/multi_file_compile.h>
#include <super/tool/file.h>
#include <super/error.h>
#include <super/compile/CDB/compile_data_base.h>
#include <super/compile/global_data.h>
#include <super/compile/core/extract_token.h>
#include <super/compile/remove_comments.h>
#include <iostream>
#include <super/compile/core/symmetrical_matching.h>
#include <super/compile/core/syntax_tree.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <super/compile/core/token.h>
#include <super/tool/string.h>

namespace Super::Compile::Process
{
	std::wstring MultiFileCompile(const std::wstring& inputFile)
	{
		std::wcout << inputFile << std::endl;
		if (!std::filesystem::exists(inputFile))
		{
			SUPER_ERROR_THROW_CODE_NONE(inputFile, L"100030");
		}

		Tool::File::FileType ft = Tool::File::GetFileType(inputFile);
		if (ft == Tool::File::FileType::Unknown)
		{
			SUPER_ERROR_THROW_MESSAGE(L"未知文件: " + inputFile)
		}
		Compile::CDB::CompileDataBase::CreateCBDFile(ft, inputFile);
		auto _inputFilePath = std::filesystem::absolute(inputFile).wstring();
		auto fileContent = Tool::File::ReadFileAllContent(_inputFilePath);
		

		// 去除注释
		Compile::GlobalData::FileDataList[inputFile] = Compile::RemoveComments(fileContent);
		
		//Tool::String::PrintLines(Compile::GlobalData::FileDataList[inputFile]);

		// 提取词组
		auto tokens = new std::vector<Super::Compile::Core::Token>;
		Compile::Core::ExtractToken(_inputFilePath, tokens);

		if (tokens->empty())
		{
			SUPER_ERROR_THROW_CODE_NONE(inputFile, L"100040");
		}
		
		// 预处理指令首尾关键词和对称符号匹配
		Compile::Core::SymmetricalMatching(_inputFilePath, tokens);
		
		std::wcout << Tool::String::TokenToString(tokens) << std::endl;

		// syntax tree 
		auto st = new Compile::Core::SyntaxTree::SyntaxTree;
		Compile::Core::SyntaxTree::Select(_inputFilePath, tokens, st);
		return L"";
	}
}