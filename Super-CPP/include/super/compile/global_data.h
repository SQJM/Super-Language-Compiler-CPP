#pragma once
#include <unordered_map>
#include <string>
#include <super/compile/CDB/compile_data_base.h>

namespace Super::Compile::GlobalData
{
	extern std::unordered_map<std::wstring, std::vector<std::vector<wchar_t>>> FileDataList;

	extern std::vector<Super::Compile::CDB::CompileDataBase::SP*> SP_List;
	extern std::vector<Super::Compile::CDB::CompileDataBase::SPH*> SPH_List;
	extern std::vector<Super::Compile::CDB::CompileDataBase::SPM*> SPM_List;
	extern std::vector<Super::Compile::CDB::CompileDataBase::SPD*> SPD_List;

	Super::Compile::CDB::CompileDataBase::SP* GetSP(const std::wstring& file);
	Super::Compile::CDB::CompileDataBase::SPH* GetSPH(const std::wstring& file);
	Super::Compile::CDB::CompileDataBase::SPM* GetSPM(const std::wstring& file);
	Super::Compile::CDB::CompileDataBase::SPD* GetSPD(const std::wstring& file);
}