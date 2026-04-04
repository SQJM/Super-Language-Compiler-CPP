module;
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
module Super.CompileDataBase;
import Super.Error;
import Super.Util.File;
import Super.Compile.GlobalData;
import Super.Util.Pointer;

namespace Super::CompileDataBase
{
	void SP::open(const std::string& file)
	{
	}

	void SP::saved()
	{
	}

	void SPM::open(const std::string& file)
	{
	}

	void SPM::saved()
	{
	}

	namespace CDB_LIST
	{
		std::ofstream FILE;
		std::vector<LIST_ITEM> LIST;

		void Init(const std::string& name)
		{
			Super::Util::File::CREATE_DIR_CODE cdc = Super::Util::File::CreateDir(name + "CDB");
			if (cdc == Super::Util::File::CREATE_DIR_CODE::FAIL_CREATE)
			{
				SUPER_ERROR_THROW_MESSAGE("Failed to create CDB database");
			}

			FILE.open("CDB/CDB_LIST", std::ios::trunc);
		}

		void Colse()
		{
			if (FILE.is_open())
				FILE.close();
			for (auto item : Super::Compile::GlobalData::SP_List)
			{
				Super::Util::Pointer::Destroy(item);
			}
			for (auto item : Super::Compile::GlobalData::SPM_List)
			{
				Super::Util::Pointer::Destroy(item);
			}
		}
	}  // namespace CDB_LIST

	void CreateCBDFile(Super::Util::File::FileType ft, const std::string& file)
	{
		CDB_LIST::LIST_ITEM li;
		li.file_type = ft;
		li.file = file;
		CDB_LIST::LIST.emplace_back(li);

		if (ft == Super::Util::File::FileType::SP)
		{
			SP* sp = new SP();
			sp->source_file = file;
			Super::Compile::GlobalData::SP_List.emplace_back(sp);
		}
		else if (ft == Super::Util::File::FileType::SPM)
		{
			SPM* spm = new SPM();
			spm->source_file = file;
			Super::Compile::GlobalData::SPM_List.emplace_back(spm);
		}
	}

	std::vector<Super::CompileDataBase::Macro>* GetMacroList(const std::string& file)
	{
		Super::Util::File::FileType ft = Super::Util::File::GetFileType(file);
		if (ft == Super::Util::File::FileType::SP)
		{
			return &Super::Compile::GlobalData::GetSP(file)->macro_list;
		}
		else
		{
			return &Super::Compile::GlobalData::GetSPM(file)->macro_list;
		}
	}
}  // namespace Super::CompileDataBase