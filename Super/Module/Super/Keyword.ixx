module;
#include <string>
#include <unordered_set>
#include <vector>
export module Super.Keyword;

export namespace Super::Keyword
{
	inline const std::vector<std::string> Modifier =
	{
		"constexpr", "static",  "private", "public",   "capture",
		"protected", "virtual", "friend",  "operator", "await",
		"async",	  "inline",  "const",   "unsigned", "signed"
	};

	inline const std::vector<std::string> Other =
	{
		"using",	  "new",		  "auto_new", "delete", "throw",
		"type_name", "namespace",   "this",	   "module", "exports", "export",
		"import",	  "null",	      "nullptr",  "func"
	};

	inline const std::vector<std::string> TypeKeywords =
	{
		"void",  "bool",	 "char",	"char16",	"char32",
		"short", "int8",  "int16",  "int32",	"int64",	"int128",
		"half",  "float",  "double", "typedef", "class",
		"union", "struct", "enum"
	};

	inline const std::vector<std::string> ControlFlow =
	{
		"if",		"else",	 "elif",	 "while",	"for",
		"do",		"loop",	 "switch",  "case",
		"default",	"break",    "ok",		 "err",
		"continue", "return"
	};

	inline const std::vector<std::string> PreprocessingInstructions =
	{
		"#define", "#undef", "#ifdef",   "#ifndef",
		"#endif",  "#error", "#message", "#asm",
		"#end",    "#if",    "#elif",    "#else",
		"#over"
	};

	inline const std::vector<std::string> ProcessMacro =
	{ "#def",  "#edit",  "#get",	  "#prefix",
		"#type", "#error", "#message", "#export"
	};

	inline const std::vector<std::string> Symbols =
	{
		"$", "\"", "'", "!", "-", "+", "/", "*", "=", "\\",
		"[", "]",	 "{", "}", ".", ":", "?", "(", ")", "|",
		"^", "&",	 "%", "<", ">", ",", ";", "~", "@"
	};

	inline const std::vector<std::string> SpecialSymbols = { "#", "_" };

	inline bool IsSymbol(const std::string& c, const std::vector<std::string>& cs)
	{
		return std::find(cs.begin(), cs.end(), c) != cs.end();
	}

	inline bool Contains(const std::string& str, const std::vector<std::string>& keywords)
	{
		return std::find(keywords.begin(), keywords.end(), str) != keywords.end();
	}

	inline bool Contains(const std::string_view& str, const std::vector<std::string>& keywords)
	{
		return std::find(keywords.begin(), keywords.end(), std::string(str)) != keywords.end();
	}

	inline bool Contains(const char& c, const std::vector<std::string>& keywords)
	{
		return std::find(keywords.begin(), keywords.end(), std::string(1, c)) != keywords.end();
	}
}  // namespace Super::Keyword