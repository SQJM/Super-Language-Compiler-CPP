#pragma once
#include <string>

namespace Super::Tool::File
{
	enum class FileType
	{
		Unknown = -1,
		SP,
		SPH,
		SPM,
		SPD
	};

	enum class CREATE_DIR_CODE
	{
		SUCCESS_CREATE,
		FAIL_CREATE,
		FAIL_DIR_EXIST
	};

	std::wstring ReadFileAllContent(const std::wstring& filePath);

	CREATE_DIR_CODE CreateDir(const std::wstring& folderPath);

	FileType GetFileType(const std::wstring& file);
}