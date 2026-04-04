module;
#include <string>
#include <vector>
module Super.Compile.Process;
import Super.Compile.Core;
import Super.Util.String;
import Super.Util.Pointer;

namespace Super::Compile::Process
{
	std::string MultiFileCompile(const std::string& inputFile, std::vector<Super::Compile::Core::Token>* tokens)
	{
		auto st = new Compile::Core::SyntaxTree::RootSyntaxTree;
		Compile::Core::SyntaxTree::Select(inputFile, tokens, st);
		Super::Util::Pointer::Destroy(st);
		return "";
	}
}  // namespace Super::Compile::Process