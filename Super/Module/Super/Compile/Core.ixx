module;
#include <string>
#include <vector>
export module Super.Compile.Core;
import Super.SuperValueType;
import Super.Util.File;

export namespace Super::Compile::Core
{
	typedef unsigned int tokens_size;
	enum class TokenName
	{
		None,
		End,
		CombinationSymbol,
		TypeKeywords,
		ModifierKeyword,
		OtherKeywords,
		ControlFlowKeywords,
		PreprocessingInstructions,
		DefineName,
		DefineValue,
		ProcessMacro,
		Symbols,
		String,
		Char,
		VarName,
		BoolValue,
		FunctionName,
		Pointer,
		Number,
		Null,
		Nullptr,
		SingleLineComment,
		MultiLineComment
	};

	const std::string TokenNameToString(const TokenName& name);

	struct Ranks
	{
		std::uint32_t line = 0;
		std::uint32_t column = 0;
	};

	struct Token
	{
		Ranks ranks;
		TokenName name = TokenName::None;
		std::string value;
	};
}  // namespace Super::Compile::Core

export namespace Super::Compile::Core::SyntaxTree
{
	enum class NodeType
	{
		Unknown,
	};

	struct Node
	{
		void* ptr = nullptr;
		NodeType node_type = NodeType::Unknown;
	};

	struct RootSyntaxTree
	{
		Super::Util::File::FileInfo file_info;

		std::vector<Node> nodes;
	};

	void Select(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st);

	void SP(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st);

	void SPM(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, RootSyntaxTree* st);
}  // namespace Super::Compile::Core::SyntaxTree