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

	static const int INVALID_TOKEN_TYPE;
	static const int PLUS;
	static const int INT;

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
	bool is_nil() const;
	virtual std::string to_string() const;
	virtual std::string to_string_tree() const;

protected:
	AstToken *token = nullptr;

};


class ExprNode : public Ast
{
public:
	ExprNode() = default;
	ExprNode(AstToken *token);

	virtual const int get_eval_type() const;
	std::string to_string() const override;

	static const int tINVALID;
	static const int tINT;
	static const int tVEC;

protected:
	int eval_type;
};

class AddNode :public ExprNode
{
public:
	AddNode(ExprNode *left, AstToken *add, ExprNode *right);

	const int get_eval_type() const override;
	std::string to_string_tree() const override;

private:
	ExprNode *left;
	ExprNode *right;
};

class IntNode :public ExprNode
{
public:
	IntNode(AstToken *token);
	std::string to_string_tree() const override;
};

class VecNode : public ExprNode
{
public:
	VecNode(AstToken *token, std::vector<ExprNode*> elements);
	VecNode(AstToken *token, std::initializer_list<ExprNode*> elements);
	std::string to_string_tree() const override;

private:
	void add_child(ExprNode *node);

private:
	std::vector<ExprNode*> elements;

};

#endif // !_AST_H
