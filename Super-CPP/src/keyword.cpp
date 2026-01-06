#include <super/keyword.h>
#include <iostream>
#include <unordered_set>


inline static bool contains(std::wstring_view s, std::vector<std::wstring> kw)
{
	std::unordered_set<std::wstring_view> set(std::move(kw).begin(), std::move(kw).end());
	return set.find(s) != set.end();
}


namespace Super::Keyword
{
	bool Contains(const std::wstring& str, const std::vector<std::wstring>& keywords)
	{
		return contains(str, keywords);
	}

	bool Contains(const std::wstring_view& str, const std::vector<std::wstring>& keywords)
	{
		return contains(str, keywords);
	}

	bool Contains(const wchar_t& c, const std::vector<std::wstring>& keywords)
	{
		return contains(std::wstring(1, c), keywords);
	}
}