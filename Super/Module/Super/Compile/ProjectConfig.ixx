module;
#include <string>
#include <unordered_map>
export module Super.Compile.ProjectConfig;
import Super.Util.Toml;

export namespace Super::Compile::ProjectConfig
{
	void ReplaceMacroString(std::string& str);

	void LoadMacroList();

	std::unordered_map<std::string, std::string> GetMacroList();

	std::unordered_map<std::string, std::string> GeneratedProjectFile(const std::string& name);

	std::unordered_map<std::string, Super::Util::Toml::TOMLDocument> ReadProjectConfigFile(const std::string& dir);
} // namespace Super::Compile