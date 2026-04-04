module;
#include <string>
#include <vector>
export module Super.Compile.Process;
import Super.Compile.Core;
import Super.Compile.GlobalData;

export namespace Super::Compile::Process
{
	namespace Pretreatment
	{
		void ExtractToken(std::vector<std::vector<char>>* lines, std::vector<Super::Compile::Core::Token>* tokens);
		void Preprocessing(std::string& fileContent, const std::string& fileName);
	} // namespace Pretreatment

	namespace LanguageServer
	{
		void SymmetricalMatching(const std::string& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens);
	} // namespace LanguageServer

	void Analyse(const std::string& inputFile, std::vector<Super::Compile::Core::Token>* tokens);

	std::string MultiFileCompile(const std::string& inputFile, std::vector<Super::Compile::Core::Token>* tokens);
} // namespace Super::Compile::Process