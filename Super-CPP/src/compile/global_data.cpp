#include <super/compile/global_data.h>
#include <super/error.h>
#include <super/tool/string.h>
#include <super/tool/file.h>

namespace Super::Compile::GlobalData
{
	std::unordered_map<std::wstring, std::vector<std::vector<wchar_t>>> FileDataList;

	std::vector<Super::Compile::ADB::AppleDataBase::SP*> SP_List;
	std::vector<Super::Compile::ADB::AppleDataBase::SPH*> SPH_List;
	std::vector<Super::Compile::ADB::AppleDataBase::SPM*> SPM_List;
	std::vector<Super::Compile::ADB::AppleDataBase::SPD*> SPD_List;

	Super::Compile::ADB::AppleDataBase::SP* GetSP(const std::wstring& file)
	{
		for (auto* it : SP_List)
		{
			if (it->source_file == file)
			{
				return it;
			}
		}
		return nullptr;
	}

	Super::Compile::ADB::AppleDataBase::SPH* GetSPH(const std::wstring& file)
	{
		for (auto* it : SPH_List)
		{
			if (it->source_file == file)
			{
				return it;
			}
		}
		return nullptr;
	}

	Super::Compile::ADB::AppleDataBase::SPM* GetSPM(const std::wstring& file)
	{
		for (auto* it : SPM_List)
		{
			if (it->source_file == file)
			{
				return it;
			}
		}
		return nullptr;
	}

	Super::Compile::ADB::AppleDataBase::SPD* GetSPD(const std::wstring& file)
	{
		for (auto* it : SPD_List)
		{
			if (it->source_file == file)
			{
				return it;
			}
		}
		return nullptr;
	}
}