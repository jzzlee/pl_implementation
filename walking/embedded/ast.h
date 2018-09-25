#ifndef _AST_H
#define _AST_H

#include <string>
#include <vector>

class AstToken
{
public:
	static const int INVALID_TOKEN_TYPE;
	static const int PLUS;
	static const int INT;
	static const int MULT;
	static const int DOT;
	static const int VEC;
	static const int ID;
	static const int ASSIGN;
	static const int PRINT;
	static const int STAT_LIST;

public:
	AstToken(int type, const std::string &text);
	AstToken(int type);
	std::string to_string() const;
	int get_type() const;

private:
	int type;
	std::string text;

};


class Ast
{
public:
	Ast() = default;
	Ast(AstToken *token);
	Ast(int type);
	virtual ~Ast();

	int get_node_type() const;
	virtual std::string to_string() const;

	virtual void print() const;

protected:
	AstToken *token = nullptr;
};


class AddNode :public Ast
{
public:
	AddNode(Ast *left, AstToken *add, Ast *right);
	void print() const override;

private:
	Ast *left;
	Ast *right;
};

class AssignNode : public Ast
{
public:
	AssignNode(Ast *left, AstToken *assign, Ast *right);
	void print() const override;

private:
	Ast *left;
	Ast *right;
};

class DotProductNode : public Ast
{
public:
	DotProductNode(Ast *left, AstToken *dot, Ast *right);
	void print() const override;

private:
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
	void print() const override;

private:
	Ast *left;
	Ast *right;
};

class PrintNode : public Ast
{
public:
	PrintNode(AstToken *pr, Ast *element);
	void print() const override;

private:
	Ast *element;
};

class StatListNode : public Ast
{
public:
	StatListNode(const std::vector<Ast*> &elements);
	StatListNode(const std::initializer_list<Ast*> &elements);
	void print() const override;

private:
	Ast *elements;
};

class VarNode : public Ast
{
	VarNode(Ast *var);
};


class VecNode : public Ast
{
public:
	VecNode(AstToken *token, std::vector<Ast*> &&elements);
	VecNode(AstToken *token, const std::initializer_list<Ast*> &elements);

private:
	std::vector<Ast*> elements;

};

#endif // !_AST_H
