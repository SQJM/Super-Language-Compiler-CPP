#include <super/error.h>
#include <super/compile/global_data.h>
#include <super/tool/string.h>
#include <super/info.h>

std::wstring GetInfo()
{
	return L"\nSuper " + Super::Info::Version;
}

namespace Super::Error
{

	std::unordered_map<std::wstring, std::wstring> CODE =
		{
			MAP_DEF("000000", "不能使用该名作为宏"),
			MAP_DEF("000010", "不能使用该名解除宏定义或条件"),
			MAP_DEF("000020", "不能使用内置关键字作为宏条件"),
			MAP_DEF("000030", "文件路径必须是字符串"),
			MAP_DEF("000040", "使用了这个预处理指令但是他需要后续的代码"),
			MAP_DEF("000050", "不能使用变量名作为宏"),
			MAP_DEF("000060", "不能使用自定义类型名字作为宏"),
			MAP_DEF("000070", "无效的文件路径"),
			MAP_DEF("000080", "一个 SPD 文件只能有一个前缀定义"),
			MAP_DEF("000090", "SPD 文件里面只能包含预处理指令关键字"),
			MAP_DEF("000100", "无法取消一个不存在的导入"),

			MAP_DEF("100000", "字符串双引号没有对齐"),
			MAP_DEF("100010", "字符串双引号后面不是连接符号"),
			MAP_DEF("100020", "这个关键字不允许这样使用"),
			MAP_DEF("100030", "文件不存在"),
			MAP_DEF("100040", "没有提取到 Token"),

			MAP_DEF("200000", "左括号缺少匹配的右括号"),
			MAP_DEF("200010", "右括号缺少匹配的左括号"),
			MAP_DEF("200020", "左花括号缺少匹配的右花括号"),
			MAP_DEF("200030", "右花括号缺少匹配的左花括号"),
			MAP_DEF("200040", "左中括号缺少匹配的右中括号"),
			MAP_DEF("200050", "右中括号缺少匹配的左中括号"),

			MAP_DEF("300000", "禁止 2 个文件之间互相导入"),
			MAP_DEF("300010", "`#endif` 前面没有匹配的逻辑处理语句 `#if` | `#ifdef` | `#ifndef`"),

			MAP_DEF("400000", "类型后面缺少名字"),
			MAP_DEF("400010", "类型后面重复类型"),
			MAP_DEF("400020", "符号不连续"),
			MAP_DEF("400030", "修饰关键字不能放在类型关键字后面"),
			MAP_DEF("400040", "多个变量名"),
			MAP_DEF("400050", "变量的声明不允许此符号"),
			MAP_DEF("400060", "未知符号"),
			MAP_DEF("400060", "未知 Token"),

			MAP_DEF("", "[null]")};

	static std::wstring CreateErrorMessage(const std::wstring &file, const std::wstring &msg, const Super::Type::Token &token)
	{
		std::wstring head = std::to_wstring(token.lc.line) + L":" + std::to_wstring(token.lc.column) + L'│';
		auto &lines = Super::Compile::GlobalData::FileDataList[file];
		std::wstring body = std::wstring(lines[token.lc.line - 1].begin(), lines[token.lc.line - 1].end());
		if (body[body.size() - 1] == L';')
		{
			body.erase(body.size() - 1, 1);
		}
		size_t width = head.size() + Super::Tool::String::GetDisplayLength(body.substr(0, token.lc.column)) - 1;
		std::wstring indicate(width, L'~');
		indicate += L"^";

		return head + body + L"\n" + indicate + L"\n" + msg;
	}

	void NewError(const std::wstring &file, const std::wstring &msg, const Super::Type::Token &token)
	{
		std::wcerr << CreateErrorMessage(file, msg, token) << GetInfo() << std::endl;
	}

	void NewError(const std::wstring& file, const std::wstring& msg)
	{
		std::wcerr << L"[ERROR]: " << file << " > " << msg << GetInfo() << std::endl;
	}

	std::wstring _cacheError;

	void CacheError(const std::wstring &file, const std::wstring &msg, const Super::Type::Token &token)
	{
		_cacheError += CreateErrorMessage(file, msg, token) + L"\n";
	}

	void PrintMessage(const std::wstring& msg)
	{
		std::wcerr << L"[MESSAGE]: " << msg << GetInfo() << std::endl;
	}

	void CacheOut()
	{
		std::wcerr << _cacheError << GetInfo() << std::endl;
		_cacheError = L"";
	}
}