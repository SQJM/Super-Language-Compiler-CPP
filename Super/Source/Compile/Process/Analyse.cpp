module;
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
module Super.Compile.Process;
import Super.Error;
import Super.Util.File;
import Super.CompileDataBase;
import Super.Compile.Core;
import Super.Compile.GlobalData;
import Super.Util.String;
import Super.Util.Pointer;

namespace Super::Compile::Process
{
	void Analyse(const std::string& inputFile, std::vector<Super::Compile::Core::Token>* tokens)
	{
		std::cout << inputFile;

		if (!std::filesystem::exists(inputFile))
		{
			SUPER_ERROR_THROW_CODE(inputFile, "100030");
		}

		Super::Util::File::FileType ft = Super::Util::File::GetFileType(inputFile);
		if (ft == Super::Util::File::FileType::Unknown)
		{
			SUPER_ERROR_THROW_MESSAGE("Unknown file: " + inputFile);
		}
		Super::CompileDataBase::CreateCBDFile(ft, inputFile);

		auto fileContent = Super::Util::File::ReadFileAllContent(inputFile);

		Super::Compile::Process::Pretreatment::Preprocessing(fileContent, inputFile);

		auto lines = new std::vector<std::vector<char>>;
		std::vector<char> currentLine;
		currentLine.reserve(4096);
		for (char c : fileContent)
		{
			if (c == '\n')
			{
				lines->emplace_back(currentLine);
				currentLine.clear();
			}
			else
			{
				currentLine.emplace_back(c);
			}
		}
		if (!currentLine.empty())
		{
			lines->emplace_back(currentLine);
		}

		Super::Compile::Process::Pretreatment::ExtractToken(lines, tokens);

		Super::Util::Pointer::Destroy(lines);

		// 移除注释
		tokens->erase(
			std::remove_if(
				tokens->begin(),
				tokens->end(),
				[](const Super::Compile::Core::Token& token)
				{
					return token.name == Super::Compile::Core::TokenName::SingleLineComment ||
						token.name == Super::Compile::Core::TokenName::MultiLineComment;
				}),
			tokens->end());

		if (tokens->empty())
		{
			SUPER_ERROR_THROW_CODE(inputFile, "100040");
		}

		Super::Compile::Process::LanguageServer::SymmetricalMatching(inputFile, tokens);

		//Super::Util::Console::PrintConsole(Super::Util::String::TokenToString(tokens));
	}
} // namespace Super::Compile::Process