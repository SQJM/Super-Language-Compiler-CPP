module;
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
export module Super.Util.CommandParsing;

/// @file CommandParsing.ixx
/// @brief 命令行解析工具模块
///
/// 提供命令行参数解析、命令注册、回调执行等功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

namespace Super::Util
{
	/// @brief 回调函数信息结构体
	struct CallbackInfo
	{
		/// @brief 回调函数
		std::function<int(const std::vector<std::string>&, const std::map<std::string, std::string>&)> fn;
		/// @brief 帮助文本
		std::string helpText;
		/// @brief 参数格式
		std::string paramFormat;

		/// @brief 默认构造函数
		CallbackInfo() = default;

		/// @brief 构造函数
		/// @param f 回调函数
		/// @param h 帮助文本
		/// @param p 参数格式
		CallbackInfo(
			std::function<int(const std::vector<std::string>&, const std::map<std::string, std::string>&)> f,
			std::string h,
			std::string p
		)
			: fn(std::move(f)),
			helpText(std::move(h)),
			paramFormat(std::move(p))
		{
		}
	};

	/// @brief 命令行解析类
	export class CommandParsing
	{
	public:
		/// @brief 构造函数
		/// @param args 命令行参数列表
		/// @param argvType 参数类型映射
		/// @param appName 程序名
		CommandParsing(
			const std::vector<std::string>& args, const std::vector<std::pair<std::string, std::type_index>>& argvType, const std::string& appName
		);

		/// @brief 设置回调函数
		/// @param name 命令名称
		/// @param fn 回调函数
		/// @param helpText 帮助文本
		/// @param paramFormat 参数格式
		void SetCallback(
			const std::string& name,
			std::function<int(const std::vector<std::string>&, const std::map<std::string, std::string>&)> fn,
			const std::string& helpText,
			const std::string& paramFormat = ""
		);

		/// @brief 获取退出码
		/// @return 退出码
		int ExitCode();

	private:
		/// @brief 参数类型映射
		std::vector<std::pair<std::string, std::type_index>> _argvType;
		/// @brief 回调函数列表（保持插入顺序）
		std::vector<std::pair<std::string, CallbackInfo>> _callbacks;
		/// @brief 命令映射
		std::map<std::string, std::vector<std::string>> _commands;
		/// @brief 选项映射
		std::map<std::string, std::string> _options;
		/// @brief 程序名
		std::string _appName;

		/// @brief 解析命令行参数
		/// @param args 命令行参数列表
		/// @return 命令和选项的映射
		static std::pair<std::map<std::string, std::vector<std::string>>, std::map<std::string, std::string>>
			ParseArguments(const std::vector<std::string>& args);

		/// @brief 调用回调函数
		/// @param name 命令名称
		/// @param value 命令值
		/// @param options 选项映射
		/// @return 回调函数返回值
		int InvokeCallback(
			const std::string& name,
			const std::vector<std::string>& value,
			const std::map<std::string, std::string>& options
		);

		/// @brief 打印帮助信息
		void PrintHelp();
	};
}  // namespace Super::Util