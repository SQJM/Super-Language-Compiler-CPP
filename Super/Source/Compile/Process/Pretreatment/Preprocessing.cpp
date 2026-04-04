module;
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "PreprocessingExpressionParser.hpp"
module Super.Compile.Process;
import Super.Keyword;
import Super.Compile.GlobalData;
import Super.CompileDataBase;
import Super.Util.ConsoleStyle;
import Super.Util.String;
import Super.Util.File;

namespace Super::Compile::Process::Pretreatment
{
	using namespace Super::Util::String;

	struct IfFrame
	{
		bool reached_true_branch;
		bool current_branch_true;
	};

	struct MacroInfo
	{
		std::string name;
		std::string value;
		std::string unquoted_value;
	};

	struct LineAnalysis
	{
		std::string line;
		bool in_comment_or_string;
		std::vector<size_t> comment_or_string_positions;
	};

	static Super::CompileDataBase::SPBase* CurrentTarget = nullptr;
	static size_t skip_depth = 0;
	static std::unordered_map<std::string, size_t> macro_index;
	static std::vector<MacroInfo> macro_infos;
	static std::unordered_map<size_t, LineAnalysis> line_analysis_cache;
	static size_t current_content_hash = 0;

	inline bool IsWordChar(char c)
	{
		return std::isalnum(c) || c == '_';
	}

	inline bool IsWhitespace(char c)
	{
		return c == ' ' || c == '\t' || c == '\r';
	}

	inline size_t HashLine(const std::string& line)
	{
		size_t hash = 14695981039346656037ULL;
		for (char c : line)
		{
			hash ^= static_cast<size_t>(c);
			hash *= 1099511628211ULL;
		}
		return hash;
	}

	inline void AnalyzeLine(const std::string& line, LineAnalysis& analysis)
	{
		analysis.line = line;
		analysis.in_comment_or_string = false;
		analysis.comment_or_string_positions.resize(line.length(), false);

		size_t singleQuoteCount = 0;
		size_t doubleQuoteCount = 0;
		size_t multilineDepth = 0;

		for (size_t i = 0; i < line.length(); ++i)
		{
			if (line[i] == '\'' && (i == 0 || line[i - 1] != '\\'))
			{
				++singleQuoteCount;
				analysis.comment_or_string_positions[i] = true;
			}
			else if (line[i] == '"' && (i == 0 || line[i - 1] != '\\'))
			{
				++doubleQuoteCount;
				analysis.comment_or_string_positions[i] = true;
			}
			else if (i + 1 < line.length())
			{
				if (line[i] == '[' && line[i + 1] == '[')
				{
					++multilineDepth;
				}
				else if (line[i] == ']' && line[i + 1] == ']' && multilineDepth > 0)
				{
					--multilineDepth;
				}
			}

			if (multilineDepth > 0 || singleQuoteCount % 2 == 1 || doubleQuoteCount % 2 == 1)
			{
				analysis.comment_or_string_positions[i] = true;
			}
		}

		analysis.in_comment_or_string = multilineDepth > 0 || singleQuoteCount % 2 == 1 || doubleQuoteCount % 2 == 1;
	}

	inline bool IsInCommentOrStringCached(const LineAnalysis& analysis, size_t pos)
	{
		if (pos >= analysis.comment_or_string_positions.size())
		{
			return false;
		}
		return analysis.comment_or_string_positions[pos];
	}

	inline void PrintMessage(const std::string& fileName, size_t line, const char* type, const auto& color, const std::string& msg)
	{
		size_t pos = fileName.find_last_of("\\/");
		std::string shortName = (pos != std::string::npos) ? fileName.substr(pos + 1) : fileName;
		auto output = Super::Util::ConsoleStyle::fg256(188) + "[" + Super::Util::ConsoleStyle::reset() +
			shortName + Super::Util::ConsoleStyle::fg256(188) + "][" +
			Super::Util::ConsoleStyle::reset() + color + type + Super::Util::ConsoleStyle::fg256(188) + "]:" +
			Super::Util::ConsoleStyle::fg::blue() + std::to_string(line) +
			Super::Util::ConsoleStyle::fg256(188) + ">" + Super::Util::ConsoleStyle::reset() + msg;
		std::cout << output;
	}

	size_t CountQuoteEscapes(const std::string& s, char quote)
	{
		size_t count = 0;
		for (size_t i = 0; i < s.length(); ++i)
		{
			if (s[i] == quote && (i == 0 || s[i - 1] != '\\'))
			{
				++count;
			}
		}
		return count;
	}

	bool IsInCommentOrString(const std::string& line, size_t pos)
	{
		size_t singleQuoteCount = 0;
		size_t doubleQuoteCount = 0;
		size_t multilineDepth = 0;

		for (size_t i = 0; i < pos; ++i)
		{
			if (line[i] == '\'' && (i == 0 || line[i - 1] != '\\'))
			{
				++singleQuoteCount;
			}
			else if (line[i] == '"' && (i == 0 || line[i - 1] != '\\'))
			{
				++doubleQuoteCount;
			}
			else if (i + 1 < line.length())
			{
				if (line[i] == '[' && line[i + 1] == '[')
				{
					++multilineDepth;
				}
				else if (line[i] == ']' && line[i + 1] == ']' && multilineDepth > 0)
				{
					--multilineDepth;
				}
			}
		}

		return multilineDepth > 0 || singleQuoteCount % 2 == 1 || doubleQuoteCount % 2 == 1;
	}

	inline void Trim(std::string& s)
	{
		auto start = std::find_if_not(s.begin(), s.end(), IsWhitespace);
		auto end = std::find_if_not(s.rbegin(), s.rend(), IsWhitespace).base();
		s = (start < end) ? std::string(start, end) : std::string();
	}

	inline std::string UnquoteString(const std::string& s)
	{
		if (s.length() >= 2 && s.front() == '"' && s.back() == '"')
		{
			return s.substr(1, s.length() - 2);
		}
		return s;
	}

	inline void RebuildMacroIndex()
	{
		macro_index.clear();
		macro_infos.clear();
		for (size_t i = 0; i < CurrentTarget->macro_list.size(); ++i)
		{
			const auto& macro = CurrentTarget->macro_list[i];
			MacroInfo info;
			info.name = macro.name;
			info.value = macro.value;
			info.unquoted_value = UnquoteString(macro.value);
			macro_infos.push_back(info);
			macro_index[macro.name] = i;
		}
	}

	inline void InvalidateCache()
	{
		line_analysis_cache.clear();
	}

	inline bool IsMacroDefined(const std::string& name)
	{
		return macro_index.find(name) != macro_index.end();
	}

	std::string GetMessageText(const std::string& line, size_t afterDirective)
	{
		while (afterDirective < line.length() && IsWhitespace(line[afterDirective]))
		{
			++afterDirective;
		}
		std::string msg = (afterDirective < line.length()) ? line.substr(afterDirective) : std::string();
		Trim(msg);
		return UnquoteString(msg);
	}

	inline std::string ExpandMacrosInLine(const std::string& line, const LineAnalysis* analysis = nullptr)
	{
		std::string result = line;
		for (const auto& pair : macro_index)
		{
			const std::string& macro_name = pair.first;
			size_t macro_idx = pair.second;
			const std::string& replace = macro_infos[macro_idx].unquoted_value;
			size_t pos = 0;
			while ((pos = result.find(macro_name, pos)) != std::string::npos)
			{
				bool word_boundary = (pos == 0 || !IsWordChar(result[pos - 1])) &&
					(pos + macro_name.length() >= result.length() || !IsWordChar(result[pos + macro_name.length()]));

				if (word_boundary)
				{
					if (analysis)
					{
						if (IsInCommentOrStringCached(*analysis, pos))
						{
							++pos;
							continue;
						}
					}
					else
					{
						if (IsInCommentOrString(result, pos))
						{
							++pos;
							continue;
						}
					}
					result.replace(pos, macro_name.length(), replace);
					pos += replace.length();
				}
				else
				{
					++pos;
				}
			}
		}
		return result;
	}

	inline std::string EvaluateExpression(const std::string& expr)
	{
		std::string e = expr;
		Trim(e);

		for (const auto& pair : macro_index)
		{
			const std::string& macro_name = pair.first;
			size_t macro_idx = pair.second;
			std::string replace = macro_infos[macro_idx].unquoted_value;
			while (true)
			{
				size_t pos = replace.find("\\n");
				if (pos == std::string::npos)
				{
					break;
				}
				replace.replace(pos, 2, "\n");
			}
			size_t pos = 0;
			while ((pos = e.find(macro_name, pos)) != std::string::npos)
			{
				bool word_boundary = (pos == 0 || !IsWordChar(e[pos - 1])) &&
					(pos + macro_name.length() >= e.length() || !IsWordChar(e[pos + macro_name.length()]));

				if (word_boundary && !IsInCommentOrString(e, pos))
				{
					e.replace(pos, macro_name.length(), replace);
					pos += replace.length();
				}
				else
				{
					++pos;
				}
			}
		}

		return e;
	}

	bool EvaluateCondition(const std::string& cond)
	{
		std::string e = EvaluateExpression(cond);

		auto lookup = [](const std::string& name) -> std::string
			{
				auto it = macro_index.find(name);
				if (it != macro_index.end())
				{
					return macro_infos[it->second].unquoted_value;
				}
				return "";
			};

		return PreprocessingExpressionEvaluator::EvaluateCondition(e, lookup);
	}

	void ProcessLineContinuation(std::string& content)
	{
		std::string result;
		size_t pos = 0;
		size_t length = content.length();

		while (pos < length)
		{
			size_t line_start = pos;
			while (pos < length && content[pos] != '\n')
			{
				++pos;
			}

			if (pos == line_start)
			{
				result += '\n';
				++pos;
				continue;
			}

			std::string line = content.substr(line_start, pos - line_start);

			bool isDefine = line.length() >= 7 &&
				(line.substr(0, 7) == "#define" || line.substr(0, 8) == "#define ");

			size_t defineLineCount = 1;

			while (!line.empty() && line.back() == '\\')
			{
				line.pop_back();
				result += line;
				if (isDefine)
				{
					result += "\\n";
				}
				++defineLineCount;

				if (pos < length && content[pos] == '\n')
				{
					++pos;
				}

				if (pos < length)
				{
					size_t next_line_start = pos;
					while (pos < length && content[pos] != '\n')
					{
						++pos;
					}
					line = content.substr(next_line_start, pos - next_line_start);
				}
				else
				{
					line.clear();
					break;
				}
			}

			if (!line.empty())
			{
				result += line;
			}

			if (isDefine && defineLineCount > 1)
			{
				for (size_t i = 1; i < defineLineCount; ++i)
				{
					result += '\n';
				}
			}

			if (pos < length && content[pos] == '\n')
			{
				result += '\n';
				++pos;
			}
		}

		content = result;
	}

	std::string GetLineContent(const std::string& line, size_t start_pos)
	{
		size_t pos = start_pos;
		while (pos < line.length() && IsWhitespace(line[pos]))
		{
			++pos;
		}
		return line.substr(pos);
	}

	bool IsPreprocessorDirective(const std::string& line)
	{
		size_t i = 0;
		while (i < line.length() && IsWhitespace(line[i]))
		{
			++i;
		}
		return i < line.length() && line[i] == '#';
	}

	std::string GetDirective(const std::string& line)
	{
		size_t i = 0;
		while (i < line.length() && IsWhitespace(line[i]))
		{
			++i;
		}
		if (i >= line.length() || line[i] != '#')
		{
			return "";
		}

		++i;
		while (i < line.length() && IsWhitespace(line[i]))
		{
			++i;
		}

		size_t start = i;
		while (i < line.length() && !IsWhitespace(line[i]))
		{
			++i;
		}
		return line.substr(start, i - start);
	}

	void ProcessPreprocessing(std::string& content, const std::string& fileName)
	{
		std::string result;
		std::vector<IfFrame> if_stack;
		size_t pos = 0;
		size_t length = content.length();
		size_t line_number = 1;
		size_t last_pos = 0;

		RebuildMacroIndex();
		InvalidateCache();

		while (pos < length)
		{
			size_t line_start = pos;
			while (pos < length && content[pos] != '\n')
			{
				++pos;
			}

			if (pos == line_start)
			{
				result += '\n';
				++pos;
				++line_number;
				continue;
			}

			std::string line = content.substr(line_start, pos - line_start);

			std::string trimmedLine = line;
			Trim(trimmedLine);

			bool isComment = false;
			if (!trimmedLine.empty() && trimmedLine[0] == ';')
			{
				isComment = true;
			}
			else if (trimmedLine.length() >= 2 && trimmedLine.substr(0, 2) == "[[")
			{
				isComment = true;
			}

			if (!IsPreprocessorDirective(trimmedLine) && !isComment)
			{
				if (skip_depth > 0)
				{
					result += '\n';
				}
				else if (if_stack.empty() || if_stack.back().current_branch_true)
				{
					result += ExpandMacrosInLine(line);
					result += '\n';
				}
			}
			else
			{
				std::string directive = GetDirective(line);

				if (directive == "define")
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						size_t def_pos = line.find("define");
						if (def_pos != std::string::npos)
						{
							def_pos += 6;
							while (def_pos < line.length() && IsWhitespace(line[def_pos]))
							{
								++def_pos;
							}
						}
						std::string rest = (def_pos < line.length()) ? line.substr(def_pos) : "";
						size_t space_pos = rest.find(' ');
						std::string macro_name = rest;
						std::string macro_value = "";
						if (space_pos != std::string::npos)
						{
							macro_name = rest.substr(0, space_pos);
							macro_value = rest.substr(space_pos + 1);
						}
						macro_value = "\"" + macro_value + "\"";
						Super::CompileDataBase::Macro macro;
						macro.name = macro_name;
						macro.value = macro_value;
						CurrentTarget->macro_list.push_back(macro);
					}
					result += '\n';
				}
				else if (directive == "undef")
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						size_t undef_pos = line.find("undef");
						if (undef_pos != std::string::npos)
						{
							undef_pos += 5;
							while (undef_pos < line.length() && IsWhitespace(line[undef_pos]))
							{
								++undef_pos;
							}
						}
						std::string macro_name = (undef_pos < line.length()) ? line.substr(undef_pos) : "";
						Trim(macro_name);
						for (auto it = CurrentTarget->macro_list.begin(); it != CurrentTarget->macro_list.end(); ++it)
						{
							if (it->name == macro_name)
							{
								CurrentTarget->macro_list.erase(it);
								break;
							}
						}
					}
					result += '\n';
				}
				else if (directive == "if")
				{
					IfFrame frame;
					frame.reached_true_branch = false;
					frame.current_branch_true = EvaluateCondition(GetLineContent(line, 4));
					if (frame.current_branch_true)
					{
						frame.reached_true_branch = true;
					}
					else if (skip_depth == 0)
					{
						skip_depth = 1;
					}
					if_stack.push_back(frame);
					result += '\n';
				}
				else if (directive == "ifdef")
				{
					IfFrame frame;
					frame.reached_true_branch = false;
					frame.current_branch_true = IsMacroDefined(GetLineContent(line, 7));
					if (frame.current_branch_true)
					{
						frame.reached_true_branch = true;
					}
					else if (skip_depth == 0)
					{
						skip_depth = 1;
					}
					if_stack.push_back(frame);
					result += '\n';
				}
				else if (directive == "ifndef")
				{
					IfFrame frame;
					frame.reached_true_branch = false;
					frame.current_branch_true = !IsMacroDefined(GetLineContent(line, 8));
					if (frame.current_branch_true)
					{
						frame.reached_true_branch = true;
					}
					else if (skip_depth == 0)
					{
						skip_depth = 1;
					}
					if_stack.push_back(frame);
					result += '\n';
				}
				else if (directive == "elif")
				{
					if (!if_stack.empty() && skip_depth == 0)
					{
						if (!if_stack.back().reached_true_branch)
						{
							if_stack.back().current_branch_true = EvaluateCondition(GetLineContent(line, 5));
							if (if_stack.back().current_branch_true)
							{
								if_stack.back().reached_true_branch = true;
							}
						}
						else
						{
							if_stack.back().current_branch_true = false;
						}
					}
					result += '\n';
				}
				else if (directive == "else")
				{
					if (!if_stack.empty() && skip_depth == 0)
					{
						if (!if_stack.back().reached_true_branch)
						{
							if_stack.back().current_branch_true = true;
							if_stack.back().reached_true_branch = true;
						}
						else
						{
							if_stack.back().current_branch_true = false;
						}
					}
					result += '\n';
				}
				else if (directive == "endif")
				{
					if (!if_stack.empty())
					{
						if_stack.pop_back();
					}
					if (if_stack.empty())
					{
						skip_depth = 0;
					}
					result += '\n';
				}
				else if (directive == "message")
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						auto msg = GetMessageText(line, line.find("message") + 7);
						PrintMessage(fileName, line_number, "MESSAGE", Super::Util::ConsoleStyle::fg::cyan(), msg);
					}
					result += '\n';
				}
				else if (directive == "error")
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						auto msg = GetMessageText(line, line.find("error") + 5);
						PrintMessage(fileName, line_number, "ERROR", Super::Util::ConsoleStyle::fg::red(), msg);
					}
					result += '\n';
				}
				else if (directive == "over")
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						auto msg = GetMessageText(line, line.find("over") + 4);
						PrintMessage(fileName, line_number, "OVER", Super::Util::ConsoleStyle::fg::yellow(), msg);
					}
					result += '\n';
					std::exit(0);
				}
				else
				{
					if (if_stack.empty() || if_stack.back().current_branch_true)
					{
						result += line;
						result += '\n';
					}
				}
			}

			if (pos < length && content[pos] == '\n')
			{
				++pos;
				++line_number;
			}
		}

		content = result;
	}

	void Preprocessing(std::string& fileContent, const std::string& fileName)
	{
		auto ft = Super::Util::File::GetFileType(fileName);
		if (ft == Super::Util::File::FileType::SP)
		{
			CurrentTarget = Super::Compile::GlobalData::GetSP(fileName);
		}
		else if (ft == Super::Util::File::FileType::SPM)
		{
			CurrentTarget = Super::Compile::GlobalData::GetSPM(fileName);
		}
		ProcessLineContinuation(fileContent);
		ProcessPreprocessing(fileContent, fileName);
	}
}