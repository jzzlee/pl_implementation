
#include "ast.h"
#include <sstream>

const int AstToken::INVALID_TOKEN_TYPE;
const int AstToken::PLUS;
const int AstToken::INT;

AstToken::AstToken(int type, const std::string &text)
	: type(type), text(text)
{}

AstToken::AstToken(int type)
	: type(type)
{}

std::string AstToken::to_string() const
{
	return text;
}

int AstToken::get_type() const
{
	return type;
}

Ast::Ast(AstToken *token)
	: token(token)
{}

Ast::Ast(int type)
{ 
	token = new AstToken(type);
}

Ast::~Ast()
{
	for (auto child : children)
		delete child;
	delete token;
}

int Ast::get_node_type() const
{
	return token->get_type();
}

void Ast::add_child(Ast *t)
{
	children.push_back(t);
}

bool Ast::is_nil() const
{
	return token == nullptr;
}

std::string Ast::to_string() const
{
	return token != nullptr ? token->to_string() : "nil";
}

std::string Ast::to_string_tree() const
{
	if (children.size() == 0)
		return to_string();
	std::stringstream ss;
	if (!is_nil())
	{
		ss << "(";
		ss << to_string();
		ss << " ";
	}
	for (auto iter = children.cbegin(); iter != children.cend(); ++iter)
	{
		if (iter != children.cbegin())
			ss << " ";
		ss << (*iter)->to_string_tree();
	}
	if (!is_nil())
		ss << ")";
	return ss.str();
}

