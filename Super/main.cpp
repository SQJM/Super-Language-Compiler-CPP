#include <unicode/unistr.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <iostream>
#include <map>
#include <string>
#include <typeindex>

import Super.Util.Pointer;
import Super.Compile.ProjectConfig;
import Super.Compile.Process;
import Super.CompileDataBase;
import Super.Util.CommandParsing;
import Super.Util.Log;
import Super.Util.ConsoleStyle;
import Super.Util.Time;
import Super.Compile.Core;
import Super.Error;
import Super.Info;
import Super.Compile.GlobalData;
import Super.Util.ProjectDirManager;
import Super.Util.File;
import Super.Util.String;

static std::string AppDir;
static std::string AppName;

static std::vector<std::string> ArgsToString(int argc, char* argv[])
{
	std::vector<std::string> args;
	for (int i = 1; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}
	return args;
}

static int CP_Info(const std::vector<std::string>& args, const std::map<std::string, std::string>& options)
{
	std::cout << Super::Info::GetVersionInfo();
	return 0;
}

static int CP_Create(const std::vector<std::string>& args, const std::map<std::string, std::string>& options)
{
	Super::Util::ProjectDirManager::GeneratedProjectDir(args[0]);
	return 0;
}

static int CP_Build(const std::vector<std::string>& args, const std::map<std::string, std::string>& options)
{
	auto map = Super::Compile::ProjectConfig::ReadProjectConfigFile(std::filesystem::current_path().string() + "/");

	map["sconfig"].FindValue("sconfig.compile", "super-version");
	//std::string outFile = args[0];
	//std::ofstream outFileStream(outFile, std::ios::trunc);

	//for (size_t i = 1; i < args.size(); ++i)
	//{
	//	std::string inputFile = std::filesystem::absolute(args[i]).string();
	//	if (!std::filesystem::exists(inputFile))
	//	{
	//		SUPER_ERROR_THROW_CODE(inputFile, "100030");
	//	}
	//	Super::Compile::GlobalData::TokenListMap[inputFile] = std::make_shared<std::vector<Super::Compile::Core::Token>>();
	//	auto tokens = Super::Compile::GlobalData::TokenListMap[inputFile].get();

	//	Super::Compile::Process::Analyse(inputFile, tokens);

	//	auto t0 = std::chrono::steady_clock::now();

	//	outFileStream << Super::Compile::Process::MultiFileCompile(inputFile, tokens);

	//	Super::Util::Time::PrintElapsedTime(t0);

	//	Super::Compile::GlobalData::DestroyTokenListMap(inputFile);
	//}
	//outFileStream.close();

	return 0;
}

static int CP_Project(const std::vector<std::string>& args, const std::map<std::string, std::string>& options)
{
	std::string result;
	if (options.find("config") != options.end())
	{
		result = options.at("config");
		if (result == "reset")
		{
			auto sdata = Super::Util::File::SearchFilesBySuffix(AppDir + ".super/", ".sdata", false);
			auto sconfig = Super::Util::File::SearchFilesBySuffix(AppDir, ".sconfig", false);
			auto sproject = Super::Util::File::SearchFilesBySuffix(AppDir, ".sproject", false);
			if (sdata.size() > 0)
			{

			}
			else
			{
				Super::Util::File::CreateDir(AppDir + ".super/");
			}
		}
	}
	return 0;
}

int RETURN_RUN_CODE = -2;

static void on_exit() noexcept
{
	std::cout << "\n"
		<< Super::Util::ConsoleStyle::fg::blue() << "Super " << Super::Info::Version
		<< Super::Util::ConsoleStyle::reset() << std::endl;
}

int main(int argc, char* argv[])
{
	if (std::atexit(on_exit) != 0)
	{
		return 99;
	}

	std::locale::global(std::locale("chs"));
	std::cout.imbue(std::locale());

	AppDir = std::filesystem::absolute(argv[0]).parent_path().string() + "/";
	AppName = std::filesystem::path(argv[0]).filename().string();

	Super::Util::Log::Logger::Instance().SetLogLevel(Super::Util::Log::LogLevel::Norm);
	Super::Util::Log::Logger::Instance().SetLogFile(AppDir + "super-build");

	Super::Compile::ProjectConfig::LoadMacroList();

	std::vector<std::string> args = ArgsToString(argc, argv);
	std::vector<std::pair<std::string, std::type_index>> argvType =
	{
		{"operation", typeid(std::string)},
		{"project_name", typeid(std::string)},
		{"project_type", typeid(std::string)},
		{"dir_path", typeid(std::string)},
		{"file_path", typeid(std::string)},
		{"file_path...", typeid(std::vector<std::string>)}
	};

	Super::Util::CommandParsing cp(args, argvType, AppName);

	cp.SetCallback("info", CP_Info, "Super Info", "-info");
	cp.SetCallback("create", CP_Create, "Create Super Project", "-create <name:project_name>");
	cp.SetCallback("build", CP_Build, "Build Super Project", "-build");
	//cp.SetCallback("project", CP_Project, "Project manage", "-project <-config:reset>");

	try
	{
		RETURN_RUN_CODE = cp.ExitCode();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return RETURN_RUN_CODE;
}