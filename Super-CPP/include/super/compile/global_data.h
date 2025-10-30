#pragma once
#include <unordered_map>
#include <string>
#include <super/compile/ADB/apple_data_base.h>
#include <super/type/token.h>

namespace Super::Compile::GlobalData
{
	extern std::unordered_map<std::wstring, std::vector<std::vector<wchar_t>>> FileDataList;

	extern std::vector<Super::Compile::ADB::AppleDataBase::SP*> SP_List;
	extern std::vector<Super::Compile::ADB::AppleDataBase::SPH*> SPH_List;
	extern std::vector<Super::Compile::ADB::AppleDataBase::SPM*> SPM_List;
	extern std::vector<Super::Compile::ADB::AppleDataBase::SPD*> SPD_List;

	Super::Compile::ADB::AppleDataBase::SP* GetSP(const std::wstring& file);
	Super::Compile::ADB::AppleDataBase::SPH* GetSPH(const std::wstring& file);
	Super::Compile::ADB::AppleDataBase::SPM* GetSPM(const std::wstring& file);
	Super::Compile::ADB::AppleDataBase::SPD* GetSPD(const std::wstring& file);
}