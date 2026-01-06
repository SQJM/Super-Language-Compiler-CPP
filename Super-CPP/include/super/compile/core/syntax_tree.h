#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <super/compile/core/token.h>
#include <super/tool/file.h>

namespace Super::Compile::Core::SyntaxTree
{
	enum class NodeType
	{
		Unknown,
		CodeLine,
		Function,
		AssemblyBlock,
		NamespaceType
	};

	enum class CodeLineType
	{
		Unknown,
		Expression,
		Statement
	};

	struct Node
	{
		void* ptr = nullptr;
		NodeType node_type = NodeType::Unknown;
	};

	struct SyntaxTree
	{
		Super::Tool::File::FileInfo file_info;

		std::vector<Node> nodes;
	};

	// 代码行
	struct CodeLine
	{
		std::vector<Super::Compile::Core::Token*> tokens;
		CodeLineType code_line_type = CodeLineType::Unknown;
	};

	// 预处理指令 控制流体
	struct PI_ControlFlow
	{
		std::vector<size_t> head;
		size_t end = 0;
	};

	// 预处理指令 #if
	struct PI_If
	{
		PI_ControlFlow if_block;
		std::vector<PI_ControlFlow> elif_blocks;
		PI_ControlFlow else_block;
	};

	// 功能
	struct Function
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 命名空间类型
	struct NamespaceType
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 类作用域
	struct ClassScope
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 汇编代码块
	struct AssemblyBlock
	{
		std::vector<CodeLine*> blocks;
	};

	// 控制流 if
	struct ControlFlowIf
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 for
	struct ControlFlowFor
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 for in
	struct ControlFlowForIn
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 while
	struct ControlFlowWhile
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 do while
	struct ControlFlowDoWhile
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 switch
	struct ControlFlowSwitch
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	// 控制流 loop
	struct ControlFlowLoop
	{
		std::vector<Node> blocks;
	};

	// 异常处理
	struct ExceptionHandling
	{
		std::vector<Node> blocks;
		std::vector<Super::Compile::Core::Token*> head;
	};

	void Select(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st);

	void SP(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st);

	void SPH(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st);

	void SPM(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st);

	void SPD(const std::wstring& inputFilePath, std::vector<Super::Compile::Core::Token>* tokens, SyntaxTree* st);
}
