module;
#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>
export module Super.Util.Toml;

/// @file Toml.ixx
/// @brief TOML 解析和构建工具模块
///
/// 提供 TOML 格式的解析、构建和操作功能
///
/// @note 所有函数均为 noexcept，可安全用于异常处理代码

export namespace Super::Util::Toml
{
	/// @brief 值类型枚举
	enum class ValueType
	{
		Null,    ///< 空值
		Boolean, ///< 布尔值
		Integer, ///< 整数值
		Float,   ///< 浮点值
		String,  ///< 字符串值
		Array,   ///< 数组值
	};

	/// @brief 注释样式枚举
	enum class CommentStyle
	{
		None,  ///< 无注释
		Line,  ///< 行注释
		Block  ///< 块注释
	};

	/// @brief 注释结构体
	struct Comment
	{
		CommentStyle style = CommentStyle::None; ///< 注释样式
		std::string content;                     ///< 注释内容
		std::string prefix;                      ///< 注释前缀
	};

	struct Array
	{
		std::vector<std::variant<bool, std::string, float, int>> array;
		ValueType arrayValueType;
	};

	std::string ReplaceEscapeSequence(const std::string& str);

	struct Value
	{
		std::variant<bool, std::string, float, int, Array> value;
		ValueType valueType;

		template <typename T>
		T As() const
		{
			if constexpr (std::is_same_v<T, bool>)
				return std::get<bool>(value);
			else if constexpr (std::is_same_v<T, std::string>)
				return std::get<std::string>(value);
			else if constexpr (std::is_same_v<T, float>)
				return std::get<float>(value);
			else if constexpr (std::is_same_v<T, int>)
				return std::get<int>(value);
			else if constexpr (std::is_same_v<T, Array>)
				return std::get<Array>(value);
			throw std::runtime_error("Invalid type conversion");
		}

		/// @brief 获取字符串值
		/// @param enableReplace 是否启用变量替换，默认为 true
		/// @param resolver 变量解析函数，输入变量名返回对应的值，仅在 enableReplace 为 true 时使用
		/// @return 字符串值
		/// @note
		/// - 如果 enableReplace 为 false，直接返回原始字符串
		/// - 如果 enableReplace 为 true，会替换 ${variable} 格式的变量
		/// - $$ 是转义序列，$$ 会替换为 $，$${} 不会被替换
		std::string AsString(bool enableReplace = true, const std::function<std::optional<std::string>(const std::string&)>& resolver = nullptr) const;
	};

	struct Key
	{
		std::string keyName;
		Value value;
		Comment comment;
	};

	struct Group
	{
		std::string groupName;
		Comment comment;
		std::vector<Key> keys;
	};

	/// @brief TOML文档类
	class TOMLDocument
	{
	public:
		/// @brief 调试用：打印文档结构
		/// @return 文档结构的字符串表示
		std::string Dump() const;

		/// @brief 通过键获取根表中的值
		/// @param key 键
		/// @return 值引用
		Value& operator[](const std::string& key);

		/// @brief 设置指定路径的注释
		/// @param path 路径
		/// @param comment 注释内容
		/// @param style 注释样式，默认为行注释
		void SetComment(const std::string& path, const std::string& comment, CommentStyle style = CommentStyle::Line);

		/// @brief 添加指定路径的注释
		/// @param path 路径
		/// @param comment 注释内容
		/// @param style 注释样式，默认为行注释
		void AddComment(const std::string& path, const std::string& comment, CommentStyle style = CommentStyle::Line);

		/// @brief 查找指定键的值
		/// @param groupName 组名
		/// @param keyName 键名
		/// @return 值指针，如果未找到则返回nullptr
		Value* FindValue(const std::string& groupName, const std::string& keyName);

		/// @brief 将另一个文档的值合并到当前文档
		/// @param other 另一个文档
		/// @note
		/// - t2 中与 t1 相同的 key 会被 t2 的值覆盖
		/// - t1 有但 t2 没有的 key 保留
		/// - t2 有但 t1 没有的 key 添加到结果中
		/// @example t1 = {a={x=1,h=1}}, t2 = {a={x=2,y=3}} -> 结果: {a={x=2,y=3,h=1}}
		void Merge(const TOMLDocument& other);

		/// @brief 检查当前文档是否满足模板文档的要求（键和类型匹配）
		/// @param templateDoc 模板文档
		/// @param msg 错误信息，如果为nullptr则使用默认信息
		/// @note 检查当前文档是否包含模板文档的所有键，且类型一致
		void MatchingToml(const TOMLDocument& templateDoc, const char* msg = nullptr) const;

		/// @brief 将文档转换为字符串
		/// @return 文档的字符串表示
		std::string ToString() const;

		/// @brief 替换文档中的变量占位符
		/// @param resolver 变量解析函数，输入变量名返回对应的值，如果返回空则不替换
		/// @note 支持 ${variable} 格式的变量替换，支持字符串和字符串数组
		void ReplaceVariables(const std::function<std::optional<std::string>(const std::string&)>& resolver);

		std::vector<Group> Groups;
	};

	/// @brief 解析TOML内容的便捷函数
	/// @param tomlContent TOML内容
	/// @param errors 解析错误列表指针，默认为nullptr
	/// @return 解析后的TOML文档
	TOMLDocument Parse(const std::string& tomlContent);

	/// @brief TOML构建器类
	class TOMLBuilder
	{
	public:
		/// @brief 默认构造函数
		TOMLBuilder();

		/// @brief 移动构造函数
		TOMLBuilder(TOMLBuilder&&) noexcept;

		/// @brief 移动赋值运算符
		/// @return 当前对象引用
		TOMLBuilder& operator=(TOMLBuilder&&) noexcept;

		/// @brief 开始一个新的分组
		/// @param name 分组名称
		/// @return 构建器引用
		TOMLBuilder& group(const std::string& name);

		/// @brief 开始一个新的分组（带注释）
		/// @param name 分组名称
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& group(const std::string& name, const std::string& comment);

		/// @brief 结束当前分组
		/// @return 构建器引用
		TOMLBuilder& groupEnd();

		/// @brief 添加布尔值键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, bool v);

		/// @brief 添加布尔值键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, bool v, const std::string& comment);

		/// @brief 添加整数值键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, int v);

		/// @brief 添加整数值键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, int v, const std::string& comment);

		/// @brief 添加浮点值键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, float v);

		/// @brief 添加浮点值键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, float v, const std::string& comment);

		/// @brief 添加字符串值键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::string& v);

		/// @brief 添加字符串值键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::string& v, const std::string& comment);

		/// @brief 添加整数数组键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<int>& v);

		/// @brief 添加整数数组键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<int>& v, const std::string& comment);

		/// @brief 添加浮点数组键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<float>& v);

		/// @brief 添加浮点数组键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<float>& v, const std::string& comment);

		/// @brief 添加字符串数组键值对
		/// @param k 键
		/// @param v 值
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<std::string>& v);

		/// @brief 添加字符串数组键值对（带注释）
		/// @param k 键
		/// @param v 值
		/// @param comment 注释
		/// @return 构建器引用
		TOMLBuilder& key(const std::string& k, const std::vector<std::string>& v, const std::string& comment);

		/// @brief 构建TOML文档
		/// @return 构建后的TOML文档
		TOMLDocument Build();

		TOMLDocument doc;
	};
} // namespace Super::Util::Toml
