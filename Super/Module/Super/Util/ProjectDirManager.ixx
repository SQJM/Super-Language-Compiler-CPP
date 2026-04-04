module;
#include <chrono>
#include <string>
export module Super.Util.ProjectDirManager;

/// @file ProjectDirManager.ixx
/// @brief 项目目录管理工具模块
///
/// 提供项目目录生成和管理功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::ProjectDirManager
{
	/// @brief 生成项目目录
	/// @param name 项目名称
	void GeneratedProjectDir(const std::string& name);
}