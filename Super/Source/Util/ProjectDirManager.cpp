module;
#include <string>
#include <iostream>
module Super.Util.ProjectDirManager;
import Super.CompileDataBase;
import Super.Compile.ProjectConfig;
import Super.Util.File;
import Super.Error;
import Super.Util.String;

namespace Super::Util::ProjectDirManager
{
	void GeneratedProjectDir(const std::string& name)
	{
		if (!Super::Util::String::IsValidFileName(name))
		{
			SUPER_ERROR_THROW_MESSAGE("Invalid project name");
		}
		Super::Util::File::CREATE_DIR_CODE cdc = Super::Util::File::CreateDir(name);
		if (cdc == Super::Util::File::CREATE_DIR_CODE::FAIL_CREATE)
		{
			SUPER_ERROR_THROW_MESSAGE("Failed to create project");
		}
		else if (cdc == Super::Util::File::CREATE_DIR_CODE::FAIL_DIR_EXIST)
		{
			SUPER_ERROR_THROW_MESSAGE("A directory with the same name already exists");
		}
		std::string RootDir = name + "/";

		std::string _SuperDir = RootDir + ".super/";
		std::string SData = _SuperDir + name + ".sdata";
		std::string SuperMacro = RootDir + ".super-macro";
		std::string BuildDir = RootDir + "build/";
		std::string OutDir = RootDir + "out/";
		std::string CodeDir = RootDir + name + "/";
		std::string SConfigFile = RootDir + name + ".sconfig";
		std::string SProjectFile = RootDir + name + ".sproject";
		std::string SampleFile = CodeDir + "main.sp";

		Super::Util::File::CreateDir(_SuperDir);
		Super::Util::File::CreateDir(BuildDir);
		Super::Util::File::CreateDir(OutDir);
		Super::Util::File::CreateDir(CodeDir);

		auto map = Super::Compile::ProjectConfig::GeneratedProjectFile(name);
		Super::Util::File::CreateFile(SConfigFile, map["sconfig"]);
		Super::Util::File::CreateFile(SProjectFile, map["sproject"]);
		Super::Util::File::CreateFile(SData, map["sdata"]);

		Super::Util::File::CreateFile(SuperMacro, "$(SuperName)=Super");

		Super::Util::File::CreateFile(SampleFile, "; hello super\n\nfunc int main()\n{\n\treturn 0\n}");

		Super::CompileDataBase::CDB_LIST::Init(_SuperDir);
		Super::CompileDataBase::CDB_LIST::Colse();

		std::cout << "Project created successfully";
	}
}