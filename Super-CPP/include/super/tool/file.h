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

	struct FileInfo
	{
		FileType file_type = FileType::Unknown;
		std::wstring path;
		std::wstring name; // 不带文件名后缀
		std::wstring file_name;
		std::wstring dir;
		size_t line_size = 0;
		uint64_t data_size_byte = 0;
	};

	void MakeFileInfo(FileInfo& fi);

	std::wstring ReadFileAllContent(const std::wstring& filePath);

	CREATE_DIR_CODE CreateDir(const std::wstring& folderPath);

	FileType GetFileType(const std::wstring& file);
}