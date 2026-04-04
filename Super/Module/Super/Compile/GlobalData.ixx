module;
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
export module Super.Compile.GlobalData;
import Super.CompileDataBase;
import Super.Util.File;
import Super.Compile.Core;

export namespace Super::Compile::GlobalData
{
	inline std::unordered_map<std::string, std::shared_ptr<std::vector<Super::Compile::Core::Token>>> TokenListMap;
	inline void DestroyTokenListMap(const std::string& file)
	{
		if (TokenListMap.find(file) == TokenListMap.end())
		{
			return;
		}

		TokenListMap[file].reset();
		TokenListMap.erase(file);
	}

	inline std::vector<Super::CompileDataBase::SP*> SP_List;
	inline std::vector<Super::CompileDataBase::SPM*> SPM_List;

	Super::CompileDataBase::SP* GetSP(const std::string& file);
	Super::CompileDataBase::SPM* GetSPM(const std::string& file);
}  // namespace Super::Compile::GlobalData