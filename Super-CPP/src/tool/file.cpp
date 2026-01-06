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
		std::ifstream file(Super::Tool::String::WStringToEncoded(filePath, "UTF-8"), std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			SUPER_ERROR_THROW_MESSAGE(L"无法打开文件");
		}

		const auto size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buf(static_cast<size_t>(size));
		if (!file.read(buf.data(), buf.size()) || file.gcount() != size)
		{
			SUPER_ERROR_THROW_MESSAGE(L"读取文件失败");
		}
		file.close();

		icu::UnicodeString ustr(buf.data(), static_cast<int32_t>(buf.size()), "UTF-8");
		if (ustr.isBogus())
		{
			SUPER_ERROR_THROW_MESSAGE(L"UTF-8 转码失败");
		}

		static const UChar CRLF[] = { u'\r', u'\n', 0 };
		static const UChar LF[] = { u'\n', 0 };
		static const UChar CR[] = { u'\r', 0 };

		ustr.findAndReplace(CRLF, LF);   // 把 \r\n -> \n
		ustr.findAndReplace(CR, LF);   // 把 \r   -> \n

		std::wstring result;
		result.reserve(ustr.length());
		for (int32_t i = 0; i < ustr.length(); ++i)
		{
			result.push_back(static_cast<wchar_t>(ustr[i]));
		}
		return result;
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

	void MakeFileInfo(FileInfo& fi)
	{
		std::filesystem::path p(fi.path);
		fi.name = p.stem().wstring();
		fi.file_name = p.filename().wstring();
		fi.dir = p.parent_path().wstring();

		std::error_code ec;
		const auto byte = std::filesystem::file_size(p, ec);
		if (!ec && byte != static_cast<std::uintmax_t>(-1))
			fi.data_size_byte = byte;
		else
			fi.data_size_byte = 0;
	}
}