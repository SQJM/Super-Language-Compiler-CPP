#include <super/tool/file.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <super/error.h>
#include <super/tool/string.h>
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <filesystem>

namespace Super::Tool::File
{
    std::wstring ReadFileAllContent(const std::wstring& filePath)
    {
        std::wifstream file(filePath, std::ios::binary);
        if (!file.is_open())
        {
            SUPER_ERROR_THROW_MESSAGE(L"无法打开文件")
        }

        std::wstringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

	CREATE_DIR_CODE CreateDir(const std::wstring& folderPath)
	{
		if (std::filesystem::exists(folderPath))
		{
			return CREATE_DIR_CODE::FAIL_DIR_EXIST;
		}
		else
		{
			if (std::filesystem::create_directory(folderPath))
			{
				return CREATE_DIR_CODE::SUCCESS_CREATE;
			}
			else
			{
				return CREATE_DIR_CODE::FAIL_CREATE;
			}
		}
	}

	FileType GetFileType(const std::wstring& file)
	{
		if (Super::Tool::String::IsTailEqual(file, L".sp", false))
		{
			return FileType::SP;
		}
		else if (Super::Tool::String::IsTailEqual(file, L".sph", false))
		{
			return FileType::SPH;
		}
		else if (Super::Tool::String::IsTailEqual(file, L".spm", false))
		{
			return FileType::SPM;
		}
		else if (Super::Tool::String::IsTailEqual(file, L".spd", false))
		{
			return FileType::SPD;
		}
		else
		{
			return FileType::Unknown;
		}
	}
}