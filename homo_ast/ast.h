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
	~Ast();

	int get_node_type() const;
	void add_child(Ast *t);
	bool is_nil() const;
	std::string to_string() const;
	std::string to_string_tree() const;

private:
	AstToken *token = nullptr;
	std::vector<Ast*> children;

};

#endif // !_AST_H
