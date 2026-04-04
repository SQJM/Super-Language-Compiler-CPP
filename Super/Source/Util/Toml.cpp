module;
#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <iostream>
#include <stack>
#include <regex>
module Super.Util.Toml;
import Super.Error;

namespace Super::Util::Toml
{
	namespace
	{
		std::string Trim(const std::string& s)
		{
			size_t start = 0;
			while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
				++start;
			size_t end = s.size();
			while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
				--end;
			return s.substr(start, end - start);
		}

		std::string TrimComment(const std::string& s)
		{
			size_t start = 0;
			while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
				++start;
			return s.substr(start);
		}

		bool IsDigit(char c)
		{
			return c >= '0' && c <= '9';
		}

		std::optional<int> ParseInteger(const std::string& s)
		{
			if (s.empty())
				return std::nullopt;

			size_t start = 0;
			bool negative = false;
			if (s[0] == '-')
			{
				negative = true;
				start = 1;
			}
			else if (s[0] == '+')
			{
				start = 1;
			}

			if (start >= s.size() || !IsDigit(s[start]))
				return std::nullopt;

			int result = 0;
			for (size_t i = start; i < s.size(); ++i)
			{
				if (!IsDigit(s[i]))
					return std::nullopt;
				result = result * 10 + (s[i] - '0');
			}

			return negative ? -result : result;
		}

		std::optional<float> ParseFloat(const std::string& s)
		{
			if (s.empty())
				return std::nullopt;

			try
			{
				float result = std::stof(s);
				return result;
			}
			catch (...)
			{
				return std::nullopt;
			}
		}

		std::optional<bool> ParseBool(const std::string& s)
		{
			std::string lower = s;
			std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
			if (lower == "true")
				return true;
			if (lower == "false")
				return false;
			return std::nullopt;
		}

		std::string ParseBasicString(const std::string& s)
		{
			if (s.size() < 2)
				return s;

			if ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''))
			{
				char quote = s.front();
				std::string inner = s.substr(1, s.size() - 2);

				if (quote == '"')
				{
					std::string result;
					result.reserve(inner.size());

					size_t i = 0;
					while (i < inner.size())
					{
						if (inner[i] == '\\' && i + 1 < inner.size())
						{
							++i;
							switch (inner[i])
							{
							case 'b': result += '\b'; break;
							case 'f': result += '\f'; break;
							case 'n': result += '\n'; break;
							case 'r': result += '\r'; break;
							case 't': result += '\t'; break;
							case '"': result += '"'; break;
							case '\\': result += '\\'; break;
							default:
								result += inner[i];
								break;
							}
						}
						else
						{
							result += inner[i];
						}
						++i;
					}
					return result;
				}
				return inner;
			}
			return s;
		}

		std::optional<Array> ParseArray(const std::string& s)
		{
			if (s.size() < 2 || s.front() != '[' || s.back() != ']')
				return std::nullopt;

			std::string inner = s.substr(1, s.size() - 2);
			if (Trim(inner).empty())
			{
				return Array{{}, ValueType::Array};
			}

			Array result;
			result.arrayValueType = ValueType::Null;

			std::vector<std::string> items;
			size_t depth = 0;
			std::string current;
			bool inString = false;
			char stringChar = 0;

			for (size_t i = 0; i < inner.size(); ++i)
			{
				char c = inner[i];

				if (!inString && (c == '"' || c == '\''))
				{
					inString = true;
					stringChar = c;
					current += c;
				}
				else if (inString && c == stringChar && (i == 0 || inner[i - 1] != '\\'))
				{
					inString = false;
					current += c;
				}
				else if (!inString)
				{
					if (c == '[' || c == '{')
						++depth;
					else if (c == ']' || c == '}')
						--depth;
					else if (c == ',' && depth == 0)
					{
						items.push_back(Trim(current));
						current.clear();
						continue;
					}
					current += c;
				}
				else
				{
					current += c;
				}
			}

			if (!current.empty())
				items.push_back(Trim(current));

			for (const auto& item : items)
			{
				auto boolVal = ParseBool(item);
				if (boolVal.has_value())
				{
					result.array.push_back(boolVal.value());
					if (result.arrayValueType == ValueType::Null)
						result.arrayValueType = ValueType::Boolean;
					else if (result.arrayValueType != ValueType::Boolean)
						return std::nullopt;
					continue;
				}

				auto intVal = ParseInteger(item);
				if (intVal.has_value())
				{
					result.array.push_back(intVal.value());
					if (result.arrayValueType == ValueType::Null)
						result.arrayValueType = ValueType::Integer;
					else if (result.arrayValueType != ValueType::Integer)
						return std::nullopt;
					continue;
				}

				auto floatVal = ParseFloat(item);
				if (floatVal.has_value())
				{
					result.array.push_back(floatVal.value());
					if (result.arrayValueType == ValueType::Null)
						result.arrayValueType = ValueType::Float;
					else if (result.arrayValueType != ValueType::Float)
						return std::nullopt;
					continue;
				}

				if (!item.empty() && (item.front() == '"' || item.front() == '\''))
				{
					result.array.push_back(ParseBasicString(item));
					if (result.arrayValueType == ValueType::Null)
						result.arrayValueType = ValueType::String;
					else if (result.arrayValueType != ValueType::String)
						return std::nullopt;
					continue;
				}

				return std::nullopt;
			}

			return result;
		}
	}

	class TOMLParserImpl
	{
	public:
		TOMLDocument doc;
		std::string content;
		size_t pos = 0;
		std::string currentGroup;
		std::stack<std::string> groupStack;
		std::string pendingComment;
		CommentStyle pendingCommentStyle = CommentStyle::None;

		void Parse(const std::string& tomlContent)
		{
			content = tomlContent;
			pos = 0;

			SkipWhitespaceAndComments();

			while (pos < content.size())
			{
				ParseLine();
				SkipWhitespaceAndComments();
			}
		}

	private:
		void SkipWhitespaceAndComments()
		{
			while (pos < content.size())
			{
				SkipWhitespace();
				if (MatchComment())
				{
					continue;
				}
				break;
			}
		}

		void SkipWhitespace()
		{
			while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\r'))
				++pos;
		}

		bool MatchComment()
		{
			if (pos < content.size() && (content[pos] == '#' || (pos + 1 < content.size() && content[pos] == '/' && (content[pos + 1] == '/' || content[pos + 1] == '*'))))
			{
				size_t start = pos;
				if (content[pos] == '#' || (content[pos] == '/' && content[pos + 1] == '/'))
				{
					while (pos < content.size() && content[pos] != '\n')
						++pos;

					std::string commentText = content.substr(start + 1, pos - start - 1);
					pendingComment = TrimComment(commentText);
					pendingCommentStyle = CommentStyle::Line;
				}
				else if (pos + 2 < content.size() && content[pos] == '/' && content[pos + 1] == '*')
				{
					pos += 2;
					size_t end = content.find("*/", pos);
					if (end != std::string::npos)
					{
						std::string commentText = content.substr(pos, end - pos);
						pendingComment = TrimComment(commentText);
						pendingCommentStyle = CommentStyle::Block;
						pos = end + 2;
					}
					else
					{
						pendingComment = content.substr(pos);
						pendingCommentStyle = CommentStyle::Block;
						pos = content.size();
					}
				}
				return true;
			}
			return false;
		}

		void ParseLine()
		{
			SkipWhitespace();
			if (pos >= content.size())
				return;

			if (content[pos] == '[')
			{
				ParseGroupHeader();
			}
			else
			{
				ParseKeyValue();
			}
		}

		void ParseGroupHeader()
		{
			++pos;
			SkipWhitespace();

			size_t start = pos;
			while (pos < content.size() && content[pos] != ']' && content[pos] != '\n')
				++pos;

			std::string groupName = Trim(content.substr(start, pos - start));

			if (pos < content.size() && content[pos] == ']')
				++pos;

			Comment comment;
			if (!pendingComment.empty())
			{
				comment.style = pendingCommentStyle;
				comment.content = pendingComment;
				pendingComment.clear();
				pendingCommentStyle = CommentStyle::None;
			}

			Group group;
			group.groupName = groupName;
			group.comment = comment;
			doc.Groups.push_back(group);

			currentGroup = groupName;

			while (!groupStack.empty() && !groupStack.top().empty())
			{
				if (groupStack.top() == groupName.substr(0, groupStack.top().size()))
				{
					break;
				}
				groupStack.pop();
			}
			groupStack.push(groupName);
		}

		void ParseKeyValue()
		{
			size_t start = pos;
			while (pos < content.size() && content[pos] != '=' && content[pos] != '\n')
				++pos;

			if (pos >= content.size() || content[pos] != '=')
			{
				if (pos < content.size() && content[pos] == '\n')
					++pos;
				return;
			}

			std::string key = Trim(content.substr(start, pos - start));
			++pos;

			SkipWhitespace();

			std::string valueStr = ReadValueString();

			Value value;
			Comment comment;

			if (!pendingComment.empty())
			{
				comment.style = pendingCommentStyle;
				comment.content = pendingComment;
				pendingComment.clear();
				pendingCommentStyle = CommentStyle::None;
			}

			auto boolVal = ParseBool(valueStr);
			if (boolVal.has_value())
			{
				value.value = boolVal.value();
				value.valueType = ValueType::Boolean;
			}
			else if (auto intVal = ParseInteger(valueStr))
			{
				value.value = intVal.value();
				value.valueType = ValueType::Integer;
			}
			else if (auto floatVal = ParseFloat(valueStr))
			{
				value.value = floatVal.value();
				value.valueType = ValueType::Float;
			}
			else if (!valueStr.empty() && (valueStr.front() == '"' || valueStr.front() == '\''))
			{
				value.value = ParseBasicString(valueStr);
				value.valueType = ValueType::String;
			}
			else if (!valueStr.empty() && valueStr.front() == '[')
			{
				if (auto arr = ParseArray(valueStr))
				{
					value.value = arr.value();
					value.valueType = ValueType::Array;
				}
			}
			else
			{
				value.value = valueStr;
				value.valueType = ValueType::String;
			}

			Key keyStruct;
			keyStruct.keyName = key;
			keyStruct.value = value;
			keyStruct.comment = comment;

			if (currentGroup.empty())
			{
				Group rootGroup;
				rootGroup.groupName = "";
				doc.Groups.push_back(rootGroup);
			}

			doc.Groups.back().keys.push_back(keyStruct);
		}

		std::string ReadValueString()
		{
			std::string result;
			bool inString = false;
			char stringChar = 0;
			size_t bracketDepth = 0;
			bool inArray = false;
			size_t startPos = pos;

			while (pos < content.size())
			{
				if (pos - startPos > content.size() * 2)
					break;

				char c = content[pos];

				if (!inString && (c == '"' || c == '\''))
				{
					inString = true;
					stringChar = c;
					result += c;
					++pos;
					continue;
				}

				if (inString && c == stringChar && (pos == 0 || content[pos - 1] != '\\'))
				{
					inString = false;
					result += c;
					++pos;
					continue;
				}

				if (inString)
				{
					result += c;
					++pos;
					continue;
				}

				if (c == '[')
				{
					++bracketDepth;
					inArray = true;
				}
				else if (c == ']')
				{
					--bracketDepth;
					if (bracketDepth == 0)
						inArray = false;
				}

				if (c == '\n' && !inArray && bracketDepth == 0)
					break;

				result += c;
				++pos;
			}

			return Trim(result);
		}
	};

	std::string ReplaceEscapeSequence(const std::string& str)
	{
		std::string result;
		result.reserve(str.size());

		for (size_t i = 0; i < str.size(); ++i)
		{
			if (str[i] == '$' && i + 1 < str.size() && str[i + 1] == '$')
			{
				if (i + 2 < str.size() && str[i + 2] == '{')
				{
					result += "$";
					result += "{";
					i += 2;
				}
				else
				{
					result += "$";
					++i;
				}
			}
			else
			{
				result += str[i];
			}
		}

		return result;
	}

	std::string Value::AsString(bool enableReplace, const std::function<std::optional<std::string>(const std::string&)>& resolver) const
	{
		if (valueType != ValueType::String)
			throw std::runtime_error("Value is not a string");

		std::string str = std::get<std::string>(value);

		if (enableReplace && resolver)
		{
			static const std::regex varPattern(R"(\$\{([^}]+)\})");
			std::smatch match;
			size_t searchStart = 0;

			while (searchStart < str.size())
			{
				std::string substr = str.substr(searchStart);
				if (!std::regex_search(substr, match, varPattern))
					break;

				std::string fullMatch = match[0].str();
				std::string varName = match[1].str();

				auto replacement = resolver(varName);
				if (!replacement.has_value())
				{
					searchStart += fullMatch.length();
					continue;
				}

				size_t matchPos = searchStart + match.position();
				str.replace(matchPos, fullMatch.length(), replacement.value());
				searchStart = matchPos + replacement.value().length();

				if (searchStart >= str.size())
					break;
			}
		}

		str = ReplaceEscapeSequence(str);
		return str;
	}

	TOMLDocument Parse(const std::string& tomlContent)
	{
		TOMLParserImpl parser;
		parser.Parse(tomlContent);
		return parser.doc;
	}

	std::string TOMLDocument::Dump() const
	{
		std::string result;
		for (const auto& group : Groups)
		{
			if (!group.groupName.empty())
			{
				result += "[" + group.groupName + "]\n";
			}

			for (const auto& key : group.keys)
			{
				result += key.keyName + " = ";

				switch (key.value.valueType)
				{
				case ValueType::Boolean:
					result += std::get<bool>(key.value.value) ? "true" : "false";
					break;
				case ValueType::Integer:
					result += std::to_string(std::get<int>(key.value.value));
					break;
				case ValueType::Float:
					result += std::to_string(std::get<float>(key.value.value));
					break;
				case ValueType::String:
					result += "\"" + std::get<std::string>(key.value.value) + "\"";
					break;
				case ValueType::Array:
				{
					const auto& arr = std::get<Array>(key.value.value);
					result += "[";
					for (size_t i = 0; i < arr.array.size(); ++i)
					{
						if (i > 0)
							result += ", ";
						const auto& item = arr.array[i];
						if (auto* b = std::get_if<bool>(&item))
							result += *b ? "true" : "false";
						else if (auto* iVal = std::get_if<int>(&item))
							result += std::to_string(*iVal);
						else if (auto* f = std::get_if<float>(&item))
							result += std::to_string(*f);
						else if (auto* s = std::get_if<std::string>(&item))
							result += "\"" + *s + "\"";
					}
					result += "]";
					break;
				}
				case ValueType::Null:
					result += "null";
					break;
				}
				result += "\n";
			}
		}
		return result;
	}

	Value& TOMLDocument::operator[](const std::string& key)
	{
		for (auto& group : Groups)
		{
			if (group.groupName.empty())
			{
				for (auto& k : group.keys)
				{
					if (k.keyName == key)
						return k.value;
				}
			}
		}

		static Value emptyValue;
		emptyValue.valueType = ValueType::Null;
		return emptyValue;
	}

	void TOMLDocument::SetComment(const std::string& path, const std::string& comment, CommentStyle style)
	{
		size_t dotPos = path.find('.');
		std::string groupName = "";
		std::string keyName = path;

		if (dotPos != std::string::npos)
		{
			groupName = path.substr(0, dotPos);
			keyName = path.substr(dotPos + 1);
		}

		Comment c;
		c.style = style;
		c.content = comment;

		for (auto& group : Groups)
		{
			if (group.groupName == groupName)
			{
				for (auto& key : group.keys)
				{
					if (key.keyName == keyName)
					{
						key.comment = c;
						return;
					}
				}
			}
		}
	}

	void TOMLDocument::AddComment(const std::string& path, const std::string& comment, CommentStyle style)
	{
		size_t dotPos = path.find('.');
		std::string groupName = "";
		std::string keyName = path;

		if (dotPos != std::string::npos)
		{
			groupName = path.substr(0, dotPos);
			keyName = path.substr(dotPos + 1);
		}

		Comment c;
		c.style = style;
		c.content = comment;

		for (auto& group : Groups)
		{
			if (group.groupName == groupName)
			{
				for (auto& key : group.keys)
				{
					if (key.keyName == keyName)
					{
						key.comment.content += "\n" + comment;
						return;
					}
				}
			}
		}
	}

	Value* TOMLDocument::FindValue(const std::string& groupName, const std::string& keyName)
	{
		for (auto& group : Groups)
		{
			if (group.groupName == groupName)
			{
				for (auto& key : group.keys)
				{
					if (key.keyName == keyName)
						return &key.value;
				}
			}
		}
		return nullptr;
	}

	void TOMLDocument::Merge(const TOMLDocument& other)
	{
		for (const auto& otherGroup : other.Groups)
		{
			bool found = false;
			for (auto& thisGroup : Groups)
			{
				if (thisGroup.groupName == otherGroup.groupName)
				{
					found = true;
					for (const auto& otherKey : otherGroup.keys)
					{
						bool keyFound = false;
						for (auto& thisKey : thisGroup.keys)
						{
							if (thisKey.keyName == otherKey.keyName)
							{
								thisKey.value = otherKey.value;
								keyFound = true;
								break;
							}
						}
						if (!keyFound)
						{
							thisGroup.keys.push_back(otherKey);
						}
					}
					break;
				}
			}
			if (!found)
			{
				Groups.push_back(otherGroup);
			}
		}
	}

	void TOMLDocument::MatchingToml(const TOMLDocument& templateDoc, const char* msg) const
	{
		for (const auto& templateGroup : templateDoc.Groups)
		{
			bool found = false;
			for (const auto& thisGroup : Groups)
			{
				if (thisGroup.groupName == templateGroup.groupName)
				{
					found = true;
					for (const auto& templateKey : templateGroup.keys)
					{
						bool keyFound = false;
						for (const auto& thisKey : thisGroup.keys)
						{
							if (thisKey.keyName == templateKey.keyName)
							{
								if (thisKey.value.valueType != templateKey.value.valueType)
								{
									std::string errorMsg = msg ? msg : "Type mismatch at ";
									errorMsg += templateGroup.groupName + "." + templateKey.keyName;
									throw std::runtime_error(errorMsg);
								}
								keyFound = true;
								break;
							}
						}
						if (!keyFound)
						{
							std::string errorMsg = msg ? msg : "Missing key: ";
							errorMsg += templateGroup.groupName + "." + templateKey.keyName;
							throw std::runtime_error(errorMsg);
						}
					}
					break;
				}
			}
			if (!found && !templateGroup.keys.empty())
			{
				std::string errorMsg = msg ? msg : "Missing group: ";
				errorMsg += templateGroup.groupName;
				throw std::runtime_error(errorMsg);
			}
		}
	}

	void ReplaceVariableString(std::string& str, const std::function<std::optional<std::string>(const std::string&)>& resolver)
	{
		static const std::regex varPattern(R"(\$\{([^}]+)\})");
		std::smatch match;
		std::string result = str;
		size_t searchStart = 0;

		while (searchStart < result.size())
		{
			std::string substr = result.substr(searchStart);
			if (!std::regex_search(substr, match, varPattern))
				break;

			std::string fullMatch = match[0].str();
			std::string varName = match[1].str();

			auto replacement = resolver(varName);
			if (!replacement.has_value())
			{
				searchStart += fullMatch.length();
				continue;
			}

			size_t matchPos = searchStart + match.position();
			result.replace(matchPos, fullMatch.length(), replacement.value());
			searchStart = matchPos + replacement.value().length();

			if (searchStart >= result.size())
				break;
		}

		str = result;
	}

	void TOMLDocument::ReplaceVariables(const std::function<std::optional<std::string>(const std::string&)>& resolver)
	{
		for (auto& group : Groups)
		{
			for (auto& key : group.keys)
			{
				if (key.value.valueType == ValueType::String)
				{
					std::string& str = std::get<std::string>(key.value.value);
					ReplaceVariableString(str, resolver);
				}
				else if (key.value.valueType == ValueType::Array)
				{
					Array& arr = std::get<Array>(key.value.value);
					if (arr.arrayValueType == ValueType::String)
					{
						for (auto& item : arr.array)
						{
							if (auto* str = std::get_if<std::string>(&item))
							{
								ReplaceVariableString(*str, resolver);
							}
						}
					}
				}
			}
		}
	}

	std::string TOMLDocument::ToString() const
	{
		std::string result;
		for (const auto& group : Groups)
		{
			if (!group.comment.content.empty())
			{
				if (group.comment.style == CommentStyle::Block)
					result += "/* " + group.comment.content + " */\n";
				else
					result += "# " + group.comment.content + "\n";
			}

			if (!group.groupName.empty())
			{
				result += "[" + group.groupName + "]\n";
			}

			for (const auto& key : group.keys)
			{
				if (!key.comment.content.empty())
				{
					if (key.comment.style == CommentStyle::Block)
						result += "/* " + key.comment.content + " */\n";
					else
						result += "# " + key.comment.content + "\n";
				}

				result += key.keyName + " = ";

				switch (key.value.valueType)
				{
				case ValueType::Boolean:
					result += std::get<bool>(key.value.value) ? "true" : "false";
					break;
				case ValueType::Integer:
					result += std::to_string(std::get<int>(key.value.value));
					break;
				case ValueType::Float:
					result += std::to_string(std::get<float>(key.value.value));
					break;
				case ValueType::String:
					result += "\"" + std::get<std::string>(key.value.value) + "\"";
					break;
				case ValueType::Array:
				{
					const auto& arr = std::get<Array>(key.value.value);
					result += "[";
					for (size_t i = 0; i < arr.array.size(); ++i)
					{
						if (i > 0)
							result += ", ";
						const auto& item = arr.array[i];
						if (auto* b = std::get_if<bool>(&item))
							result += *b ? "true" : "false";
						else if (auto* iVal = std::get_if<int>(&item))
							result += std::to_string(*iVal);
						else if (auto* f = std::get_if<float>(&item))
							result += std::to_string(*f);
						else if (auto* s = std::get_if<std::string>(&item))
							result += "\"" + *s + "\"";
					}
					result += "]";
					break;
				}
				case ValueType::Null:
					result += "";
					break;
				}
				result += "\n";
			}

			if (&group != &Groups.back() || !group.keys.empty())
				result += "\n";
		}
		return result;
	}

	TOMLBuilder::TOMLBuilder()
	{
	}

	TOMLBuilder::TOMLBuilder(TOMLBuilder&&) noexcept = default;

	TOMLBuilder& TOMLBuilder::operator=(TOMLBuilder&&) noexcept = default;

	TOMLBuilder& TOMLBuilder::group(const std::string& name)
	{
		Group g;
		g.groupName = name;
		doc.Groups.push_back(g);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::group(const std::string& name, const std::string& comment)
	{
		Group g;
		g.groupName = name;
		g.comment.style = CommentStyle::Line;
		g.comment.content = comment;
		doc.Groups.push_back(g);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::groupEnd()
	{
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, bool v)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Boolean;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, bool v, const std::string& comment)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Boolean;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, int v)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Integer;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, int v, const std::string& comment)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Integer;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, float v)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Float;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, float v, const std::string& comment)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::Float;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::string& v)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::String;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::string& v, const std::string& comment)
	{
		Key key;
		key.keyName = k;
		key.value.value = v;
		key.value.valueType = ValueType::String;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<int>& v)
	{
		Array arr;
		arr.arrayValueType = ValueType::Integer;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<int>& v, const std::string& comment)
	{
		Array arr;
		arr.arrayValueType = ValueType::Integer;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<float>& v)
	{
		Array arr;
		arr.arrayValueType = ValueType::Float;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<float>& v, const std::string& comment)
	{
		Array arr;
		arr.arrayValueType = ValueType::Float;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<std::string>& v)
	{
		Array arr;
		arr.arrayValueType = ValueType::String;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLBuilder& TOMLBuilder::key(const std::string& k, const std::vector<std::string>& v, const std::string& comment)
	{
		Array arr;
		arr.arrayValueType = ValueType::String;
		for (const auto& item : v)
		{
			arr.array.push_back(item);
		}

		Key key;
		key.keyName = k;
		key.value.value = arr;
		key.value.valueType = ValueType::Array;
		key.comment.style = CommentStyle::Line;
		key.comment.content = comment;

		if (doc.Groups.empty())
		{
			Group root;
			doc.Groups.push_back(root);
		}
		doc.Groups.back().keys.push_back(key);
		return *this;
	}

	TOMLDocument TOMLBuilder::Build()
	{
		return doc;
	}
}
