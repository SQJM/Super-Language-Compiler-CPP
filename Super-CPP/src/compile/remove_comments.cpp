#include <super/compile/remove_comments.h>
#include <sstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <super/tool/string.h>
#include <regex>

// 符号
const std::vector<wchar_t> Symbols =
{
	L'-',
	L'+',
	L'/',
	L'*',
	L'=',
	L'\\',
	L'[',
	L'{',
	L'.',
	L':',
	L'?',
	L'(',
	L'|',
	L'^',
	L'&',
	L'%',
	L'<',
	L',',
	L'~',
	L'@'
};
const std::vector<wchar_t> Symbols2 =
{
	L'-',
	L'+',
	L'/',
	L'*',
	L'|',
	L'^',
	L'&',
	L'%',
	L'~',
	L'{'
};

static inline bool isMore(size_t i, std::wstring_view target, wchar_t c)
{
	return i + 1 < target.size() && target[i + 1] == c;
}

static inline int backIsNullChar(const std::vector<wchar_t>& vec)
{
	for (int i = static_cast<int>(vec.size()) - 1; i >= 0; --i)
	{
		if (!Super::Tool::String::IsWhitespace(vec[i]))
		{
			return i;
		}
	}
	return -1;
}

namespace Super::Compile
{
	std::vector<std::vector<wchar_t>> RemoveComments(const std::wstring& fileData)
	{
		std::vector<std::vector<wchar_t>> data;
		std::vector<wchar_t> line;
		line.reserve(4096);
		size_t scope = 0;
		size_t line_index = 0;
		bool multi_line_comments = false;
		bool string_continue_state = false;
		bool comments_state = false;

		std::wstring_view content = std::wstring_view(fileData);

		size_t content_size = content.size();
		for (size_t i = 0; i < content_size; i++)
		{
			wchar_t c = content[i];

			if (comments_state)
			{
				if (c != L'\n')
				{
					continue;
				}
				else
				{
					comments_state = false;
				}
			}

			if (string_continue_state)
			{
				if (line.empty())
				{
					continue;
				}
				else if (!line.empty())
				{
					string_continue_state = Super::Tool::String::BreakString(i, content, scope);
				}
			}

			if (scope > 0)
			{
				scope--;
				if (c == L'\n')
				{
					goto next;
				}
			}
			else if (c == L'\n')
			{
				goto next;
			}
			else
			{
				if (multi_line_comments)
				{
					if (c == L'-' && isMore(i, content, L';'))
					{
						multi_line_comments = false;
						++i;
						line.emplace_back(L' ');
					}
					line.emplace_back(L' ');
					continue;
				}

				if (c == L';') // 遇到注释符
				{
					comments_state = true;
					if (isMore(i, content, L'-'))
					{
						multi_line_comments = true;
					}
					continue;
				}

				if (c == L'\"' || c == L'\'')
				{
					string_continue_state = Super::Tool::String::BreakString(i + 1, content, scope);
				}
			}
			line.emplace_back(c);
			if (i == content_size - 1)
			{
				if (std::ranges::find(Symbols, line.back()) == Symbols.end())
				{
					line.emplace_back(L';');
				}
			next:
				line_index++;
				// 添加结束符
				int t = backIsNullChar(line);
				if (t != -1)
				{
					if (std::ranges::find(Symbols, line[t]) == Symbols.end())
					{
						for(size_t temp_i = i + 1; temp_i < content_size; temp_i++)
						{
							wchar_t cc = content[temp_i];
							if (!Super::Tool::String::IsWhitespace(cc))
							{
								if (std::ranges::find(Symbols2, cc) == Symbols2.end())
								{
									line.emplace_back(L';');
								}
								break;
							}
						}
					}
				}
				data.emplace_back(line);
				line.clear();
			}
		}
		
		return data;
	}
}