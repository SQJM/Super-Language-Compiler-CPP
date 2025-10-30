#include <super/compile/remove_note.h>
#include <sstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <super/tool/string.h>
#include <regex>

// 符号
const std::vector<wchar_t> Symbols =
{
	L'$',
	L'!',
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

static bool isMore(size_t i, std::wstring_view target, wchar_t c)
{
	return i + 1 < target.size() && target[i + 1] == c;
}

namespace Super::Compile
{
	std::vector<std::vector<wchar_t>> RemoveNote(const std::wstring& fileData)
	{
		std::vector<std::vector<wchar_t>> data;
		std::vector<wchar_t> line;
		size_t scope = 0;
		size_t is_scope_end = 0;
		bool multi_line_comments = false;
		bool string_continue_state = false;
		bool comments_state = false;

		// 统一换行符为 LF
		std::wstring content = Super::Tool::String::UnifyLineEndings(fileData);

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

			if (line.empty() && string_continue_state)
			{
				string_continue_state = Super::Tool::String::BreakString(i, content, scope);
			}

			if (scope > 0 && scope == is_scope_end)
			{
				scope = 0;
				is_scope_end = 0;
			}

			if (scope > 0)
			{
				is_scope_end++;
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
					string_continue_state = Super::Tool::String::BreakString(i, content, scope);
				}
			}
			line.emplace_back(c);
			if (i == content_size - 1)
			{
				next:
				data.emplace_back(line);
				line.clear();
			}
		}

		return data;
	}
}