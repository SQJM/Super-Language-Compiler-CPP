module;
#include <algorithm>
#include <string>
#include <vector>
export module Super.CompileDataBase;
import Super.Compile.Core;
import Super.Util.File;

struct DB_BaseOperation
{
	template <typename T>
	inline bool IsEmpty(std::vector<T>* ptr)
	{
		return ptr->empty();
	}

	template <typename T>
	inline bool IsExist(std::vector<T>* ptr, const std::string& name)
	{
		return std::any_of(
			ptr->begin(),
			ptr->end(),
			[&name](const T& t)
			{
				return std::string(t.name) == name;
			}
		);
	}

	template <typename T>
	inline bool IsExist(std::vector<T>* ptr, const T& t)
	{
		return std::find(ptr->begin(), ptr->end(), t) != ptr->end();
	}

	template <typename T>
	bool Set(std::vector<T>* ptr, const T& t)
	{
		if (IsExist(ptr, t))
		{
			return false;
		}
		ptr->emplace_back(t);
		return true;
	}

	template <typename T>
	bool Remove(std::vector<T>* ptr, const std::string& name)
	{
		auto it = std::remove_if(
			ptr->begin(),
			ptr->end(),
			[&name](const T& t)
			{
				return std::string(t.name) == name;
			}
		);
		bool changed = (it != ptr->end());
		ptr->erase(it, ptr->end());
		return changed;
	}

	template <typename T>
	bool Remove(std::vector<T>* ptr, const T& t)
	{
		auto it = std::remove(ptr->begin(), ptr->end(), t);
		bool changed = (it != ptr->end());
		ptr->erase(it, ptr->end());
		return changed;
	}
};

struct BaseLC
{
	size_t line_column[2];

	inline void MakeLineColumn(const Super::Compile::Core::Ranks& ranks)
	{
		line_column[0] = ranks.line;
		line_column[1] = ranks.column;
	}
};

export namespace Super::CompileDataBase
{
	enum class Type
	{
		Char,
		Char16,
		Char32,
		Short,
		Half,
		Float,
		Int,
		Double,
		Byte,
		Class,
		Union,
		Struct,
		Enum,
		UserDefine
	};

	struct Var : BaseLC
	{
		size_t up_idx;
		size_t idx[2];
		Type type;
		bool is_unsigned;
		short long_size;
		std::string name;
		void* value;
		std::int16_t array_size = 0;
		std::int8_t advanced_var = 0;
	};

	struct FunctionArgv : Var
	{
		bool is_mutable = false;
		bool is_template = false;
		size_t tag[2];
		Type type;
		bool is_unsigned;
		short long_size;
		std::string name;
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
		FunctionReturnValue return_value;
		std::string name;
		std::vector<FunctionArgv> args;
		std::vector<std::string> front_modifier;
		std::vector<std::string> back_modifier;
	};

	enum class ImportFileType
	{
		None,
		SPH,
		SPM,
		SPD,
		SP
	};

	struct ImportFile
	{
		std::string path;
		ImportFileType ift;
		bool import_type;
	};

	struct Macro : BaseLC
	{
		std::string name;
		std::string value;

		bool operator==(const Macro& other) const
		{
			return name == other.name && value == other.value;
		}
	};

	struct SPBase
	{
		virtual ~SPBase() = default;
		std::string source_file;
		std::vector<Macro> macro_list;
		virtual void open(const std::string& file) = 0;
		virtual void saved() = 0;
	};

	struct SP : public SPBase
	{
		std::vector<ImportFile> import_file;
		std::vector<Function> function;
		std::vector<FunctionArgv> function_argv;
		std::vector<FunctionReturnValue> function_return_value;
		std::vector<Var> var;

		void open(const std::string& file) override;
		void saved() override;
	};

	struct SPM : public SPBase
	{
		std::vector<ImportFile> import_file;
		std::vector<Function> function;
		std::vector<FunctionArgv> function_argv;
		std::vector<FunctionReturnValue> function_return_value;
		std::vector<Var> var;

		void open(const std::string& file) override;
		void saved() override;
	};

	namespace CDB_LIST
	{
		struct LIST_ITEM
		{
			std::string file;
			Super::Util::File::FileType file_type;
		};

		extern std::ofstream FILE;
		extern std::vector<LIST_ITEM> LIST;

		void Init(const std::string& name = "");
		void Colse();
	}  // namespace CDB_LIST

	void CreateCBDFile(Super::Util::File::FileType ft, const std::string& file);

	std::vector<Macro>* GetMacroList(const std::string& file);
}  // namespace Super::CompileDataBase