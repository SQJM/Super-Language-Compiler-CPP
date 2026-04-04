module;
#include <string>
#include <vector>
module Super.Compile.GlobalData;
import Super.CompileDataBase;

namespace Super::Compile::GlobalData
{
	Super::CompileDataBase::SP* GetSP(const std::string& file)
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

	Super::CompileDataBase::SPM* GetSPM(const std::string& file)
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
}  // namespace Super::Compile::GlobalData