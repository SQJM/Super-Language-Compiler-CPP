#include <super/compile/core/syntax_tree.h>
#include <super/error.h>
#include <super/compile/global_data.h>
#include <super/keyword.h>
#include <super/tool/console.h>
#include <super/tool/string.h>
#include <super/tool/pointer.hpp>
#include <super/tool/console_style.hpp>
#include <super/compile/expression_parser/expression_parser.h>

static bool is_function()
{
	return false;
}

namespace Super
{
	namespace Context
	{
		struct SP
		{
			Super::Compile::CDB::CompileDataBase::SP* cdb;
			Super::Tool::File::FileInfo file_info;
			std::vector<Super::Compile::Core::Token>* tokens;
			std::vector<Super::Compile::Core::SyntaxTree::Node>* nodes;
			std::vector<std::pair<size_t, size_t>> break_block;
		};
	}

	namespace Handle
	{
		static void ExtractCodeLine(size_t& it_count, Context::SP* context, Super::Compile::Core::SyntaxTree::CodeLine* cl)
		{
			for (; it_count < context->tokens->size(); it_count++)
			{
				Super::Compile::Core::Token token = context->tokens->at(it_count);
				if (token.name == Compile::Core::TokenName::End)
				{
					break;
				}
				cl->tokens.emplace_back(&context->tokens->at(it_count));
			}
		}

		class PI_IF_Parser
		{
		public:
			PI_IF_Parser(size_t& it_count, Context::SP* cont)
			{
				this->context = cont;
				parser(it_count);
			}

		private:
			Compile::Core::SyntaxTree::PI_If pii;
			Context::SP* context;

			void parser_head(size_t& it_count, Compile::Core::SyntaxTree::PI_ControlFlow& pcf)
			{
				it_count++;
				for (; it_count < context->tokens->size(); it_count++)
				{
					auto& token = context->tokens->at(it_count);
					if (token.name == Compile::Core::TokenName::End)
					{
						it_count++;
						break;
					}
					pcf.head.emplace_back(it_count);
				}
			}

			void parser_if(size_t& it_count)
			{
				parser_head(it_count, pii.if_block);
				size_t c = 0;
				for (; it_count < context->tokens->size(); it_count++)
				{
					auto& token = context->tokens->at(it_count);
					if (token.value == L"#if")
					{
						c++;
					}

					if (token.value == L"#endif" && c > 0)
					{
						c--;
					}

					if (c == 0)
					{
						if (token.value == L"#elif")
						{
							parser_elif(it_count);
							break;
						}
						else if (token.value == L"#else")
						{
							parser_else(it_count);
							break;
						}
						else if (token.value == L"#endif")
						{
							pii.if_block.end = it_count;
							break;
						}
					}
				}
			}

			void parser_elif(size_t& it_count)
			{
				Compile::Core::SyntaxTree::PI_ControlFlow pcf;
				parser_head(it_count, pcf);
				pii.elif_blocks.emplace_back(pcf);
				size_t c = 0;
				for (; it_count < context->tokens->size(); it_count++)
				{
					auto& token = context->tokens->at(it_count);
					if (token.value == L"#if")
					{
						c++;
					}

					if (token.value == L"#endif" && c > 0)
					{
						c--;
					}

					if (c == 0)
					{
						if (token.value == L"#elif")
						{
							parser_elif(it_count);
							break;
						}
						else if (token.value == L"#else")
						{
							parser_else(it_count);
							break;
						}
						else if (token.value == L"#endif")
						{
							auto& last = pii.elif_blocks.back();
							last.end = it_count;
							break;
						}
					}
				}
			}

			void parser_else(size_t& it_count)
			{
				size_t c = 0;
				for (; it_count < context->tokens->size(); it_count++)
				{
					auto& token = context->tokens->at(it_count);
					if (token.value == L"#if")
					{
						c++;
					}

					if (token.value == L"#endif" && c > 0)
					{
						c--;
					}

					if (c == 0 && token.value == L"#endif")
					{
						pii.else_block.end = it_count;
						break;
					}
				}
			}

			void parser(size_t& it_count)
			{
				parser_if(it_count);
				it_count++;
				Super::Compile::ExpressionParser::Parser par(context->file_info.path, context->tokens);

				par.PreprocessingInstructionsLogicalCondition(pii.if_block.head);
			}
		};

		class PreprocessingInstructions
		{
		private:
			Context::SP* context;

			std::wstring MsgOut(const std::wstring h, const Super::Compile::Core::Token& v) const
			{
				return
					Super::Tool::ConsoleStyle::fg256(188)
					+ L"["
					+ Super::Tool::ConsoleStyle::reset()
					+ context->file_info.file_name
					+ Super::Tool::ConsoleStyle::fg256(188)
					+ L"]["
					+ Super::Tool::ConsoleStyle::reset()
					+ h
					+ Super::Tool::ConsoleStyle::fg256(188)
					+ L"]:"
					+ Super::Tool::ConsoleStyle::fg::blue()
					+ std::to_wstring(v.ranks.line)
					+ Super::Tool::ConsoleStyle::fg256(188)
					+ L">"
					+ Super::Tool::ConsoleStyle::reset()
					+ Super::Tool::String::RemoveFirstAndLastChar(v.value);
			};

			std::wstring MacroValueMerge(size_t& it_count)
			{
				std::wstring value;
				Super::Compile::Core::Token token;
				for (; it_count < context->tokens->size(); it_count++)
				{
					token = context->tokens->at(it_count);
					if (token.value == L"\\")
					{
						continue;
					}
					if (token.name == Compile::Core::TokenName::End)
					{
						return value;
					}
					value += token.value;
				}
				return value;
			}

			void MacroCheck(const Compile::CDB::CompileDataBase::Macro& macro, const Super::Compile::Core::Token& v)
			{
				auto fn = [&](const std::wstring& v) -> bool
					{
						return
							Compile::CDB::CompileDataBase::Operation::Base::IsExist<Compile::CDB::CompileDataBase::Var>(&context->cdb->var, v)
							&& Compile::CDB::CompileDataBase::Operation::Base::IsExist<Compile::CDB::CompileDataBase::Function>(&context->cdb->function, v)
							&& Compile::CDB::CompileDataBase::Operation::Base::IsExist<Compile::CDB::CompileDataBase::FunctionArgv>(&context->cdb->function_argv, v);
					};
				if (fn(macro.name))
				{
					SUPER_ERROR_THROW_CODE(context->file_info.path, L"300050", v)
				}
				else if (fn(macro.value))
				{
					SUPER_ERROR_THROW_CODE(context->file_info.path, L"300060", v)
				}
			}

			void ParserASM(size_t& it_count, Compile::Core::SyntaxTree::Node& node)
			{
				auto ab = new Compile::Core::SyntaxTree::AssemblyBlock;
				node.node_type = Super::Compile::Core::SyntaxTree::NodeType::AssemblyBlock;
				node.ptr = ab;
				while (context->tokens->at(it_count).value != L"#end")
				{
					auto cl = new Super::Compile::Core::SyntaxTree::CodeLine;
					ExtractCodeLine(it_count, context, cl);
					it_count++;
					ab->blocks.emplace_back(cl);
				}
				it_count++;
			}

			void ParserIfDef(size_t& it_count)
			{
				auto& first = context->tokens->at(it_count);
				auto& macro = context->tokens->at(it_count + 1).value;
				bool r = Compile::CDB::CompileDataBase::Operation::Base::IsExist<Compile::CDB::CompileDataBase::Macro>(&context->cdb->macro_list, macro);
				it_count += 3;
				if ((first.value == L"#ifndef") ? r = !r : r)
				{
					BreakCodeBlock(it_count);
				}
			}

			void BreakCodeBlock(size_t& it_count)
			{
				for (size_t i = 0; it_count < context->tokens->size(); it_count++)
				{
					auto& str = context->tokens->at(it_count).value;
					if (str == L"#endif")
					{
						if (i <= 0)
						{
							it_count++;
							return;
						}
						else
						{
							i--;
						}
					}
					else if (str == L"#ifdef" || str == L"#ifndef" || str == L"#if")
					{
						i++;
					}
				}
			}

		public:
			PreprocessingInstructions(size_t& it_count, Context::SP* context, Compile::Core::SyntaxTree::Node& node)
			{
				this->context = context;
				auto& first = context->tokens->at(it_count);

				if (first.value == L"#message")
				{
					Super::Tool::Console::PrintConsoleLine(MsgOut(Super::Tool::ConsoleStyle::fg::cyan() + L"MESSAGE", context->tokens->at(it_count + 1)));
					it_count += 2;
				}
				else if (first.value == L"#error")
				{
					Super::Tool::Console::PrintConsoleLine(MsgOut(Super::Tool::ConsoleStyle::fg::red() + L"ERROR", context->tokens->at(it_count + 1)));
					it_count += 2;
				}
				else if (first.value == L"#over")
				{
					Super::Tool::Console::PrintConsoleLine(MsgOut(Super::Tool::ConsoleStyle::fg::yellow() + L"OVER", context->tokens->at(it_count + 1)));
					std::exit(0);
				}
				else if (first.value == L"#define")
				{
					if (context->tokens->at(it_count + 1).name == Compile::Core::TokenName::End)
					{
						it_count += 2;
						return;
					}
					auto& token = context->tokens->at(it_count);
					Compile::CDB::CompileDataBase::Macro macro;
					macro.name = context->tokens->at(it_count + 1).value;
					macro.MakeLineColumn(context->tokens->at(it_count).ranks);
					it_count += 2;
					macro.value = MacroValueMerge(it_count);
					MacroCheck(macro, token);

					Compile::CDB::CompileDataBase::Operation::Base::Set
						<Compile::CDB::CompileDataBase::Macro>( &context->cdb->macro_list, macro);
				}
				else if (first.value == L"#undef")
				{
					Compile::CDB::CompileDataBase::Operation::Base::Remove<Compile::CDB::CompileDataBase::Macro>(
						&context->cdb->macro_list,
						context->tokens->at(it_count + 1).value);
					it_count += 2;
				}
				else if (first.value == L"#asm")
				{
					it_count += 2;
					ParserASM(it_count, node);
				}
				else if (first.value == L"#ifdef" || first.value == L"#ifndef")
				{
					ParserIfDef(it_count);
				}
				else if (first.value == L"#if")
				{
					PI_IF_Parser pip(it_count, context);
				}
				else
				{
					SUPER_ERROR_THROW_CODE(context->file_info.path, L"300070", first)
				}
			}
		};
	}

	static void parser_sp(size_t& it_count, Context::SP* context)
	{
		Super::Compile::Core::SyntaxTree::Node node;
		Super::Compile::Core::Token first = context->tokens->at(it_count);
		std::wstring value = first.value;

		if (value == L"result")
		{

		}
		else if (value == L"if")
		{

		}
		else if (value == L"while")
		{

		}
		else if (value == L"loop")
		{

		}
		else if (value == L"for")
		{

		}
		else if (value == L"do")
		{

		}
		else if (value == L"switch")
		{

		}
		else if (value == L"class")
		{

		}
		else if (value == L"union")
		{

		}
		else if (value == L"struct")
		{

		}
		else if (value == L"enum")
		{

		}
		else if (value == L"namespace")
		{

		}
		else if (first.name == Super::Compile::Core::TokenName::PreprocessingInstructions)
		{
			Super::Handle::PreprocessingInstructions(it_count, context, node);
		}
		else if (Super::Keyword::Contains(value, Super::Keyword::TypeKeywords) || value == L"async" && is_function())
		{

		}
		else
		{
			auto cl = new Super::Compile::Core::SyntaxTree::CodeLine;
			node.node_type = Super::Compile::Core::SyntaxTree::NodeType::CodeLine;
			Super::Handle::ExtractCodeLine(it_count, context, cl);
			node.ptr = cl;
		}

		context->nodes->emplace_back(node);
	}
}

namespace ExcludeKeyword
{
	const std::vector<std::wstring> SP =
	{
		L"#prefix"
	};
}

namespace Super::Compile::Core::SyntaxTree
{
	void Select(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st)
	{
		Tool::File::FileType ft = Tool::File::GetFileType(inputFilePath);
		st->file_info.path = inputFilePath;
		st->file_info.file_type = ft;
		Tool::File::MakeFileInfo(st->file_info);
		st->file_info.line_size = Compile::GlobalData::FileDataList[inputFilePath].size() - 1;
		if (ft == Tool::File::FileType::SP)
		{
			SP(inputFilePath, tokens, st);
		}
		else if (ft == Tool::File::FileType::SPH)
		{
			SPH(inputFilePath, tokens, st);
		}
		else if (ft == Tool::File::FileType::SPM)
		{
			SPM(inputFilePath, tokens, st);
		}
		else if (ft == Tool::File::FileType::SPD)
		{
			SPD(inputFilePath, tokens, st);
		}
	}

	void SP(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st)
	{
		auto sp = GlobalData::GetSP(inputFilePath);
		size_t it_count = 0;
		auto context = new Context::SP{ sp, st->file_info, tokens, &st->nodes };
		for (; it_count < tokens->size(); it_count++)
		{
			Super::Compile::Core::Token token = tokens->at(it_count);
			if (token.name == Super::Compile::Core::TokenName::Rev)
			{
				continue;
			}
			if (Keyword::Contains(token.value, ExcludeKeyword::SP))
			{
				SUPER_ERROR_THROW_CODE(inputFilePath, L"100010", token)
			}
			if (token.value == L"#endif")
			{
				continue;
			}
			parser_sp(it_count, context);
		}

		Tool::Pointer::Destroy(context);
	}

	void SPH(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st)
	{
		auto sph = GlobalData::GetSP(inputFilePath);
		for (size_t i = 0; i < tokens->size(); i++)
		{
			Super::Compile::Core::Token token = tokens->at(i);
			std::wstring value = token.value;
		}
	}

	void SPM(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st)
	{
		auto spm = GlobalData::GetSP(inputFilePath);
		for (size_t i = 0; i < tokens->size(); i++)
		{
			Super::Compile::Core::Token token = tokens->at(i);
			std::wstring value = token.value;
		}
	}

	void SPD(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st)
	{
		auto spd = GlobalData::GetSP(inputFilePath);
		for (size_t i = 0; i < tokens->size(); i++)
		{
			Super::Compile::Core::Token token = tokens->at(i);
			std::wstring value = token.value;
		}
	}
}
