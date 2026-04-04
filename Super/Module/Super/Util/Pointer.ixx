module;
#include <concepts>
export module Super.Util.Pointer;

/// @file Pointer.ixx
/// @brief 指针操作工具模块
///
/// 提供指针管理等辅助功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::Pointer
{
	/// @brief 销毁指针并将其置为空
	/// @tparam T 指针类型，必须是可销毁的
	/// @param p 要销毁的指针引用
	template <std::destructible T>
	inline void Destroy(T*& p) noexcept
	{
		delete p;
		p = nullptr;
	}
}  // namespace Super::Util::Pointer