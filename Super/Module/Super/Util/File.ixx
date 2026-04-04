module;
#include <optional>
#include <string>
#include <vector>
export module Super.Util.File;

/// @file File.ixx
/// @brief 文件操作工具模块
///
/// 提供文件读取、创建、目录管理等基本功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::File
{
	/// @brief 创建目录结果枚举
	enum class CREATE_DIR_CODE
	{
		SUCCESS_CREATE, ///< 目录创建成功
		FAIL_CREATE,	///< 目录创建失败
		FAIL_DIR_EXIST	///< 目录已存在
	};

	/// @brief 文件类型枚举
	enum class FileType
	{
		Unknown = -1, ///< 未知文件类型
		SP,			  ///< SP文件类型
		SPM,		  ///< SPM文件类型
	};

	/// @brief 文件信息结构体
	struct FileInfo
	{
		FileType file_type = FileType::Unknown; ///< 文件类型
		std::string path;						///< 文件路径
		std::string name;						///< 文件名（无扩展名）
		std::string file_name;					///< 完整文件名（含扩展名）
		std::string dir;						///< 文件所在目录
		std::uint64_t data_size_byte = 0;		///< 文件大小（字节）
	};

	/// @brief 生成文件信息
	/// @param fi 文件信息结构体引用，用于存储生成的信息
	void MakeFileInfo(FileInfo& fi);

	/// @brief 读取文件所有内容
	/// @param filePath 文件路径
	/// @return 文件内容字符串
	std::string ReadFileAllContent(const std::string& filePath);

	/// @brief 创建目录
	/// @param folderPath 目录路径
	/// @return 创建结果枚举
	CREATE_DIR_CODE CreateDir(const std::string& folderPath);

	/// @brief 获取文件类型
	/// @param file 文件路径
	/// @return 文件类型枚举
	FileType GetFileType(const std::string& file);

	/// @brief 创建文件
	/// @param filePath 文件路径
	/// @param content 文件内容
	/// @param isBinary 是否为二进制文件，默认为false（文本文件）
	/// @return 创建成功返回true，失败返回false
	bool CreateFile(const std::string& filePath, const std::string& content, bool isBinary = false);

	/// @brief 高性能读取文件指定行内容
	/// @param filePath 文件路径
	/// @param lineNumber 要读取的行号（从1开始）
	/// @return 如果成功读取到指定行，返回包含该行内容的optional；如果失败（文件不存在、权限不足，行号超出范围等），返回空的optional
	/// @note
	/// 该函数采用流式读取方式，仅读取目标行内容而无需加载整个文件到内存中。
	/// 支持大文件（1GB以上）处理，内存占用不随文件大小显著增加。
	/// 行号从1开始计数，与文本编辑器保持一致。
	std::optional<std::string> ReadFileLine(const std::string& filePath, std::size_t lineNumber);

	/// @brief 获取文件总行数
	/// @param filePath 文件路径
	/// @return 如果成功，返回文件总行数；如果失败（文件不存在、权限不足等），返回0
	/// @note
	/// 该函数采用流式读取方式，逐行计数，无需加载整个文件到内存中。
	/// 支持大文件（1GB以上）处理，内存占用不随文件大小显著增加。
	std::size_t GetFileLineCount(const std::string& filePath);

	/// @brief 在指定目录中搜索指定后缀名的文件
	/// @param dirPath 要搜索的目录路径
	/// @param suffix 要搜索的文件后缀名（例如 ".txt"、"g.text"）
	/// @param recursive 是否递归搜索子目录，默认为 true
	/// @return 匹配的文件路径列表
	/// @note
	/// suffix 参数应该包含点号，例如 ".txt" 或 "g.text"。
	/// 返回的路径是绝对路径。
	std::vector<std::string> SearchFilesBySuffix(const std::string& dirPath, const std::string& suffix, bool recursive = true);
} // namespace Super::Util::File