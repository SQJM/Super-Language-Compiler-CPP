module;
#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
module Super.Compile.ProjectConfig;
import Super.Util.String;
import Super.Util.Toml;
import Super.Util.Algorithm;
import Super.Util.File;
import Super.Error;

std::unordered_map<std::string, std::string> MacroList
{
	{"$(ProjectDir)", std::filesystem::current_path().string() + "/"}
};

std::unordered_map<std::string, Super::Util::Toml::TOMLDocument> GetConfigToml(const std::string& name)
{
	std::unordered_map<std::string, Super::Util::Toml::TOMLDocument> result;
	std::string guid = Super::Util::Algorithm::GenerateGUID();
	using namespace std::literals;
	Super::Util::Toml::TOMLBuilder sconfig;
	Super::Util::Toml::TOMLBuilder sproject;
	Super::Util::Toml::TOMLBuilder sdata;

	result["sconfig"] =
		sconfig
		.group("sconfig", "项目配置")
		.key("name", name, "项目名称")
		.key("GUID", guid, "项目唯一标识符")
		.key("configuration", "Debug"s, "编译配置类型 (Debug|Release)")
		.key("platform", "x64"s, "目标平台 (x86|x64)")
		.groupEnd()

		.group("sconfig.compile", "编译配置")
		.key("super-version", "2026.3.0"s, "编译器版本")
		.key("project-type", "EP"s, "项目类型: 可执行文件(EP)|静态库(SL)|动态库(DL)")
		.key("work-dir", "build"s, "构建工作目录")
		.key("intermediate-file-dir", "out"s, "中间文件目录")
		.key("app-log", "build"s, "应用程序日志输出目录")
		.key("copy-runtime-out", false, "复制运行时到输出目录")
		.key("module-head-out", false, "模块头文件输出")
		.key("module-head-out-dir", "build/module/"s, "模块头文件输出目录")
		.key("command-line-parameters", std::vector<std::string>{}, "编译器命令行参数")
		.groupEnd()

		.group("sconfig.link", "链接配置")
		.key("output-name", name, "输出可执行文件名")
		.key("command-line-parameters", std::vector<std::string>{}, "链接器命令行参数")
		.key("library-path", std::vector<std::string>{}, "库搜索路径")
		.key("libraries", std::vector<std::string>{}, "外部链接库")
		.key("generate-debug-symbols", true, "生成调试符号")
		.key("enable-lto", false, "启用链接时优化 (LTO)")
		.key("stack-reserve-size", 0, "栈保留大小 (字节)")
		.key("stack-commit-size", 0, "栈提交大小 (字节)")
		.groupEnd()
		.Build();

	result["sproject"] =
		sproject
		.group("sproject", "项目配置")
		.key("GUID", guid, "项目唯一标识符")
		.key("external-inclusion-module", std::vector<std::string>{}, "外部包含模块")
		.key("inclusion-module", std::vector<std::string>{}, "包含模块")
		.groupEnd()

		.group("sproject.item-group", "项目项组")
		.key("macro", std::vector<std::string>{}, "过程宏文件")
		.key("source", std::vector<std::string>{"main.sp"}, "源文件")
		.groupEnd()
		.Build();

	result["sdata"] =
		sdata
		.group("sdata", "项目数据")
		.key("GUID", guid, "项目唯一标识符")
		.key("program-breakpoint ", std::vector<std::string>{}, "程序断点") // [0:0]file.sp
		.groupEnd()
		.Build();
	return result;
}

std::unordered_map<std::string, std::string> ParseAssignments(const std::string& text)
{
	std::unordered_map<std::string, std::string> result;

	std::regex pattern(R"((\$\([^)]+\))=([^\r\n]*)[\r\n]*)");

	std::sregex_iterator it(text.begin(), text.end(), pattern);
	std::sregex_iterator end;

	for (; it != end; ++it) {
		std::string varName = (*it)[1].str();
		std::string varValue = (*it)[2].str();
		result[varName] = varValue;
	}

	return result;
}

namespace Super::Compile::ProjectConfig
{
	Super::Util::Toml::TOMLDocument ProjectConfig;

	void ReplaceMacroString(std::string& str)
	{
		for (auto& it : MacroList)
		{
			Super::Util::String::ReplaceAll(str, it.first, it.second);
		}
	}

	void LoadMacroList()
	{
		auto content = Super::Util::File::ReadFileAllContent(std::filesystem::current_path().string() + "/.super-macro");
		if (content.size() == 0)
		{
			return;
		}
		for (const auto& [key, value] : ParseAssignments(content)) {
			if (MacroList.find(key) == MacroList.end()) {
				MacroList[key] = value;
			}
		}
	}

	std::unordered_map<std::string, std::string> GetMacroList()
	{
		return MacroList;
	}

	std::unordered_map<std::string, std::string> GeneratedProjectFile(const std::string& name)
	{
		std::unordered_map<std::string, std::string> result;
		auto list = GetConfigToml(name);
		result["sconfig"] = list["sconfig"].ToString();
		result["sproject"] = list["sproject"].ToString();
		result["sdata"] = list["sdata"].ToString();
		return result;
	}

	Super::Util::Toml::TOMLDocument GetToml(const std::string& path)
	{
		auto content = Super::Util::File::ReadFileAllContent(path);
		ReplaceMacroString(content);
		return Super::Util::Toml::Parse(content);
	}

	std::unordered_map<std::string, Super::Util::Toml::TOMLDocument> ReadProjectConfigFile(const std::string& dir)
	{
		std::unordered_map<std::string, Super::Util::Toml::TOMLDocument> result;
		auto map = GetConfigToml("super");
		auto list = Super::Util::File::SearchFilesBySuffix(dir, ".sconfig");
		if (list.size() == 0)
		{
			SUPER_ERROR_THROW_MESSAGE("项目 .sconfig 文件不存在");
		}
		auto sconfig = GetToml(list.at(0));
		sconfig.MatchingToml(map["sconfig"]);
		auto name = sconfig.FindValue("sconfig", "name")->As<std::string>();
		auto sproject = GetToml(dir + name + ".sproject");
		auto sdata = GetToml(dir + ".super/" + name + ".sdata");
		result["sconfig"] = sconfig;
		result["sproject"] = sproject;
		result["sdata"] = sdata;
		return result;
	}
} // namespace Super::Compile