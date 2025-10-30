#pragma once
#include <string>
#include <vector>
#include <super/tool/file.h>

namespace Super::Compile::ADB::AppleDataBase
{
	enum class Type
	{
		Char,		// 字符 8位 支持的修饰符:signed,unsigned
		WChar,		// 宽字符 16位 支持的修饰符:signed,unsigned
		Short,		// 短整型 16位 支持的修饰符:signed,unsigned
		Half,       // 半精度浮点数 16位 支持的修饰符:signed,unsigned
		Float,      // 单精度浮点数 32位 支持的修饰符:signed,unsigned
		Int,		// 整型 32位 支持的修饰符:signed,unsigned,long
		Double,     // 双精度浮点数 64位 支持的修饰符:signed,unsigned,long
		Byte,		// 比特 1位
		Class,      // 类
		Union,      // 联合体
		Struct,     // 结构体
		Enum,       // 枚举
		UserDefine  // 用户重定义类型
	};

	struct Var
	{
		size_t up_idx;			// 上级序号
		size_t idx[2];			// 序号
		Type type;
		bool is_unsigned;		// 是否为无符号类型
		short long_size;		// long 修饰数量
		std::wstring name;	    // 名字
		void* value;
		int16_t array_size = 0; // 大于 0 表示数组长度
		size_t line_column[2];  // 行列
		int8_t advanced_var = 0;
		// 0 普通变量
		// 小于 0 为 & 最大为 2; 1 取地址 2 右值引用
		// 大于 0 为 @
	};

	struct FunctionArgv : Var
	{
		bool is_mutable = false;
		bool is_template = false;
		size_t tag[2];
		Type type;
		bool is_unsigned;
		short long_size;
		std::wstring name;
		void* value;
		int16_t array_size = 0;
		int8_t advanced_var = 0;
	};

	struct FunctionReturnValue : Var
	{
		size_t tag;
		Type type;
		bool is_unsigned;
		short long_size;
		int16_t array_size = 0;
		int8_t advanced_var = 0;
	};

	struct Function
	{
		size_t up_idx;
		size_t idx[2];
		FunctionReturnValue return_value;			// 返回值
		std::wstring name;
		std::vector<FunctionArgv> args;				// 参数
		std::vector<std::wstring> front_modifier;	// 前修饰符
		std::vector<std::wstring> back_modifier;	// 后修饰符
	};

	enum class ImportFileType
	{
		SPH,
		SPM,
		SPD,
		SP
	};

	struct ImportFile
	{
		ImportFileType ift;
		bool import_type = false; // true 文件
		std::wstring path;
		size_t line_column[2];
	};

	struct Macro
	{
		std::wstring macro;
		std::wstring value;
		size_t line_column[2];
	};


	struct SP
	{
	public:
		std::wstring source_file;
		std::vector<Macro> macro_list;
		std::vector<ImportFile> import_file;
		std::vector<Function> function;
		std::vector<FunctionArgv> function_argv;
		std::vector<FunctionReturnValue> function_return_value;
		std::vector<Var> var;

		void open(const std::wstring& file);
		void saved();
	};

	struct SPH
	{
	public:
		std::wstring source_file;
		std::vector<Macro> macro_list;
		std::vector<ImportFile> import_file;
		std::vector<Function> function;
		std::vector<FunctionArgv> function_argv;
		std::vector<FunctionReturnValue> function_return_value;
		std::vector<Var> var;

		void open(const std::wstring& file);
		void saved();
	};

	struct SPM
	{
	public:
		std::wstring source_file;
		std::vector<Macro> macro_list;
		std::vector<ImportFile> import_file;
		std::vector<Function> function;
		std::vector<FunctionArgv> function_argv;
		std::vector<FunctionReturnValue> function_return_value;
		std::vector<Var> var;

		void open(const std::wstring& file);
		void saved();
	};

	struct SPD
	{
	public:
		std::wstring source_file;
		std::vector<Macro> macro_list;
		std::vector<ImportFile> import_file;

		void open(const std::wstring& file);
		void saved();
	};

	namespace Operation::MacroList
	{
		bool IsEmpty(std::vector<Macro>* ptr);
		bool IsExist(std::vector<Macro>* ptr, const std::wstring& macro);
	}

	namespace ADB_LIST
	{
		enum class StorageType
		{
			File,
			Project,
			Module
		};

		struct LIST_ITEM
		{
			StorageType storage_type;
			std::wstring file;
			Super::Tool::File::FileType file_type;
		};

		extern std::wofstream FILE;
		extern std::vector<LIST_ITEM> LIST;

		void Init();
		void Colse();
	}

	void CreateABDFile(Super::Tool::File::FileType ft, const std::wstring& file);

	std::vector<Super::Compile::ADB::AppleDataBase::Macro>* GetMacroList(const std::wstring& file);

	ImportFile ConvertToImportFile(SP* sp);
}