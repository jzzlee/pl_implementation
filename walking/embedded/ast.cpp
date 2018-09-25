
#include "ast.h"
#include <sstream>
#include <iostream>

const int AstToken::INVALID_TOKEN_TYPE = 0;
const int AstToken::PLUS = 1;
const int AstToken::INT = 2;
const int AstToken::MULT = 3;
const int AstToken::DOT = 4;
const int AstToken::VEC = 5;
const int AstToken::ID = 6;
const int AstToken::ASSIGN = 7;
const int AstToken::PRINT = 8;
const int AstToken::STAT_LIST = 9;


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
	delete token;
}

std::string Ast::to_string() const
{
	return token != nullptr ? token->to_string() : "nil";
}

int Ast::get_node_type() const
{
	return token->get_type();
}

void Ast::print() const
{
	std::cout << to_string() << std::endl;
}

