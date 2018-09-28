#ifndef _AST_H
#define _AST_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class AstToken
{
public:
	static const int INVALID_TOKEN_TYPE = 0;
	static const int PLUS = 1;
	static const int INT = 2;
	static const int MULT = 3;
	static const int DOT = 4;
	static const int VEC = 5;
	static const int ID = 6;
	static const int ASSIGN = 7;
	static const int PRINT = 8;
	static const int STAT_LIST = 9;

public:
	AstToken(int type, const std::string &text);
	AstToken(int type);
	std::string to_string() const;
	int get_type() const;

private:
	int type;
	std::string text;

};


class AstVisitor;

class Ast
{
public:
	Ast() = default;
	Ast(AstToken *token);
	Ast(int type);
	virtual ~Ast();

	int get_node_type() const;
	virtual std::string to_string() const;

	virtual void visit(AstVisitor *visitor) const;

protected:
	AstToken *token = nullptr;
};


class AddNode :public Ast
{
public:
	AddNode(Ast *left, AstToken *add, Ast *right);
	~AddNode();

	Ast *left;
	Ast *right;
};

class AssignNode : public Ast
{
public:
	AssignNode(Ast *left, AstToken *assign, Ast *right);
	~AssignNode();

	Ast *left;
	Ast *right;
};

class DotProductNode : public Ast
{
public:
	DotProductNode(Ast *left, AstToken *dot, Ast *right);
	~DotProductNode();

	Ast *left;
	Ast *right;
};

class IntNode :public Ast
{
public:
	IntNode(AstToken *token);
};

class MultNode : public Ast
{
public:
	MultNode(Ast *left, AstToken *mult, Ast *right);
	~MultNode();

	Ast *left;
	Ast *right;
};

class PrintNode : public Ast
{
public:
	PrintNode(AstToken *pr, Ast *element);
	~PrintNode();

	Ast *element;
};

class StatListNode : public Ast
{
public:
	StatListNode(const std::vector<Ast*> &elements);
	StatListNode(const std::initializer_list<Ast*> &elements);
	~StatListNode();

	std::vector<Ast*> elements;
};

class VarNode : public Ast
{
public:
	VarNode(AstToken *var);
};


class VecNode : public Ast
{
public:
	VecNode(AstToken *token, std::vector<Ast*> &&elements);
	VecNode(AstToken *token, const std::initializer_list<Ast*> &elements);
	~VecNode();

	std::vector<Ast*> elements;

};

class AstVisitor
{
public:
	AstVisitor() = default;
	void visit(const Ast *node) const;
	void visit(const AssignNode *node) const;
	void visit(const PrintNode *node) const;
	void visit(const StatListNode *node) const;
	void visit(const VarNode *node) const;
	void visit(const AddNode *node) const;
	void visit(const DotProductNode *node) const;
	void visit(const IntNode *node) const;
	void visit(const MultNode *node) const;
	void visit(const VecNode *node) const;
};

#endif // !_AST_H
