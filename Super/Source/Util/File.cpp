module;
#include <filesystem>
#include <fstream>
#include <vector>
module Super.Util.File;
import Super.Util.String;
import Super.Error;

namespace Super::Util::File
{
	std::string ReadFileAllContent(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			SUPER_ERROR_THROW_MESSAGE("Failed to open file: " + filePath);
		}

		const auto size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buf(static_cast<size_t>(size));
		if (!file.read(buf.data(), buf.size()) || file.gcount() != size)
		{
			SUPER_ERROR_THROW_MESSAGE("Failed to read file: " + filePath);
		}
		file.close();

		std::string result(buf.begin(), buf.end());

		size_t pos = 0;
		while ((pos = result.find("\r\n", pos)) != std::string::npos)
		{
			result.erase(pos, 1);
		}
		pos = 0;
		while ((pos = result.find('\r', pos)) != std::string::npos)
		{
			result[pos] = '\n';
		}

		return result;
	}

	CREATE_DIR_CODE CreateDir(const std::string& folderPath)
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

	FileType GetFileType(const std::string& file)
	{
		if (Super::Util::String::IsTailEqual(file, ".sp", false))
		{
			return FileType::SP;
		}
		else if (Super::Util::String::IsTailEqual(file, ".spm", false))
		{
			return FileType::SPM;
		}
		else
		{
			return FileType::Unknown;
		}
	}

	void MakeFileInfo(FileInfo& fi)
	{
		std::filesystem::path p(fi.path);
		fi.name = p.stem().string();
		fi.file_name = p.filename().string();
		fi.dir = p.parent_path().string();

		std::error_code ec;
		const auto byte = std::filesystem::file_size(p, ec);
		if (!ec && byte != static_cast<std::uintmax_t>(-1))
			fi.data_size_byte = byte;
		else
			fi.data_size_byte = 0;
	}

	bool CreateFile(const std::string& filePath, const std::string& content, bool isBinary)
	{
		if (!Super::Util::String::IsValidPath(filePath))
		{
			return false;
		}

		if (std::filesystem::exists(filePath))
		{
			return false;
		}

		if (!isBinary)
		{
			std::ofstream file(filePath, std::ios::out);
			if (!file.is_open())
			{
				return false;
			}
			file << content;
			file.close();
		}
		else
		{
			std::ofstream file(filePath, std::ios::binary);
			if (!file.is_open())
			{
				return false;
			}
			file.write(content.data(), content.size());
			file.close();
		}

		return true;
	}

	std::optional<std::string> ReadFileLine(const std::string& filePath, std::size_t lineNumber)
	{
		if (lineNumber == 0)
		{
			return std::nullopt;
		}

		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			return std::nullopt;
		}

		try
		{
			constexpr std::size_t BUFFER_SIZE = 65536;
			char buffer[BUFFER_SIZE];
			file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

			std::string line;
			line.reserve(256);
			std::size_t currentLine = 0;
			char ch;

			while (file.get(ch))
			{
				if (ch == '\n')
				{
					++currentLine;

					if (currentLine == lineNumber)
					{
						file.close();
						return line;
					}

					line.clear();
				}
				else if (ch == '\r')
				{
					char nextCh;
					if (file.get(nextCh))
					{
						if (nextCh == '\n')
						{
							++currentLine;

							if (currentLine == lineNumber)
							{
								file.close();
								return line;
							}

							line.clear();
						}
						else
						{
							++currentLine;

							if (currentLine == lineNumber)
							{
								file.close();
								return line;
							}

							line.clear();

							line.push_back(nextCh);
						}
					}
					else
					{
						++currentLine;

						if (currentLine == lineNumber)
						{
							file.close();
							return line;
						}

						line.clear();
						break; // 文件结束
					}
				}
				else
				{
					line.push_back(ch);
				}
			}

			// 处理文件最后一行没有换行符的情况
			if (!line.empty())
			{
				++currentLine;
				if (currentLine == lineNumber)
				{
					file.close();
					return line;
				}
			}

			// 如果循环结束仍未找到目标行，说明行号超出文件总行数
			file.close();
			return std::nullopt;
		}
		catch (const std::exception&)
		{
			if (file.is_open())
			{
				file.close();
			}
			return std::nullopt;
		}
		catch (...)
		{
			if (file.is_open())
			{
				file.close();
			}
			return std::nullopt;
		}
	}

	std::size_t GetFileLineCount(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			return 0;
		}

		try
		{
			constexpr std::size_t BUFFER_SIZE = 65536;
			char buffer[BUFFER_SIZE];
			file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

			std::size_t lineCount = 0;
			char ch;
			bool hasContentInCurrentLine = false;

			while (file.get(ch))
			{
				if (ch == '\n')
				{
					++lineCount;
					hasContentInCurrentLine = false;
				}
				else if (ch == '\r')
				{
					char nextCh;
					if (file.get(nextCh))
					{
						if (nextCh == '\n')
						{
							++lineCount;
							hasContentInCurrentLine = false;
						}
						else
						{
							++lineCount;
							hasContentInCurrentLine = false;

							ch = nextCh;

							if (nextCh != '\n' && nextCh != '\r')
							{
								hasContentInCurrentLine = true;
							}
							continue; // 继续处理这个字符
						}
					}
					else
					{
						++lineCount;
						hasContentInCurrentLine = false;
						break; // 文件结束
					}
				}
				else
				{
					hasContentInCurrentLine = true;
				}
			}

			if (hasContentInCurrentLine)
			{
				++lineCount;
			}

			file.close();
			return lineCount;
		}
		catch (const std::exception&)
		{
			if (file.is_open())
			{
				file.close();
			}
			return 0;
		}
		catch (...)
		{
			if (file.is_open())
			{
				file.close();
			}
			return 0;
		}
	}

	std::vector<std::string> SearchFilesBySuffix(const std::string& dirPath, const std::string& suffix, bool recursive)
	{
		std::vector<std::string> result;

		if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
		{
			return result;
		}

		if (recursive)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
			{
				if (entry.is_regular_file())
				{
					std::string filePath = entry.path().string();
					std::string fileName = entry.path().filename().string();

					if (suffix.length() <= fileName.length())
					{
						std::string fileSuffix = fileName.substr(fileName.length() - suffix.length());
						if (fileSuffix == suffix)
						{
							result.push_back(filePath);
						}
					}
				}
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(dirPath))
			{
				if (entry.is_regular_file())
				{
					std::string filePath = entry.path().string();
					std::string fileName = entry.path().filename().string();

					if (suffix.length() <= fileName.length())
					{
						std::string fileSuffix = fileName.substr(fileName.length() - suffix.length());
						if (fileSuffix == suffix)
						{
							result.push_back(filePath);
						}
					}
				}
			}
		}

		return result;
	}
} // namespace Super::Util::File