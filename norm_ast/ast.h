#ifndef _AST_H
#define _AST_H

#include <string>
#include <vector>

class AstToken
{
public:
	AstToken(int type, const std::string &text);
	AstToken(int type);
	std::string to_string() const;
	int get_type() const;

	static const int INVALID_TOKEN_TYPE = 0;
	static const int PLUS = 1;
	static const int INT = 2;

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
	void add_child(Ast *t);
	bool is_nil() const;
	virtual std::string to_string() const;
	std::string to_string_tree() const;

protected:
	AstToken *token = nullptr;
	std::vector<Ast*> children;

};


class ExprNode : public Ast
{
public:
	ExprNode(AstToken *token);

	virtual const int get_eval_type() const;
	std::string to_string() const override;

	static const int tINVALID = 0;
	static const int tINT = 1;
	static const int tVEC = 2;

protected:
	int eval_type;
};

class AddNode :public ExprNode
{
public:
	AddNode(ExprNode *left, AstToken *add, ExprNode *right);

	const int get_eval_type() const override;
};

class IntNode :public ExprNode
{
public:
	IntNode(AstToken *token);
};

class VecNode : public ExprNode
{
public:
	VecNode(AstToken *token, std::vector<ExprNode*> elements);
	VecNode(AstToken *token, std::initializer_list<ExprNode*> elements);
};

#endif // !_AST_H
