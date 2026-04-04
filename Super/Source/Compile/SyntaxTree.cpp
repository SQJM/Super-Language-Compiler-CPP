module;
#include <string>
#include <vector>
module Super.Compile.Core;
import Super.Util.File;
import Super.Compile.GlobalData;
import Super.Keyword;
import Super.CompileDataBase;
import Super.Error;
import Super.Util.Pointer;

namespace Super::Compile::Core::SyntaxTree::Context
{
	struct SP
	{
		Super::CompileDataBase::SP* cdb;
		Super::Util::File::FileInfo file_info;
		std::vector<Super::Compile::Core::Token>* tokens;
		std::vector<Super::Compile::Core::SyntaxTree::Node>* nodes;
	};
} // namespace Context

namespace ExcludeKeyword
{
	const std::vector<std::string> SP = { "#prefix" };
} // namespace ExcludeKeyword

namespace Super::Compile::Core::SyntaxTree
{
	void Select(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st)
	{
		Util::File::FileType ft = Util::File::GetFileType(inputFilePath);
		st->file_info.path = inputFilePath;
		st->file_info.file_type = ft;
		Util::File::MakeFileInfo(st->file_info);
		if (ft == Util::File::FileType::SP)
		{
			SP(inputFilePath, tokens, st);
		}
		else if (ft == Util::File::FileType::SPM)
		{
			SPM(inputFilePath, tokens, st);
		}
	}

	void SP(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st)
	{
		auto sp = GlobalData::GetSP(inputFilePath);
		tokens_size it_count = 0;
		auto context = new Context::SP{ sp, st->file_info, tokens, &st->nodes };
		for (; it_count < tokens->size(); it_count++)
		{
			Super::Compile::Core::Token token = tokens->at(it_count);
			if (Super::Keyword::Contains(token.value, ExcludeKeyword::SP))
			{
				SUPER_ERROR_THROW_CODE_TOKEN(inputFilePath, "100010", token);
			}
		}
		Super::Util::Pointer::Destroy(context);
	}

	void SPM(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st)
	{
		auto spm = GlobalData::GetSPM(inputFilePath);
		for (size_t i = 0; i < tokens->size(); i++)
		{
			Super::Compile::Core::Token token = tokens->at(i);
			std::string value = token.value;
		}
	}
} // namespace Super::Compile::Core::SyntaxTree