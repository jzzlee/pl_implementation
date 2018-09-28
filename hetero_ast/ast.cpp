
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
	delete token;
}

int Ast::get_node_type() const
{
	return token->get_type();
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
	return to_string();
}

const int ExprNode::tINVALID;
const int ExprNode::tINT;
const int ExprNode::tVEC;

ExprNode::ExprNode(AstToken *token)
	: Ast(token), eval_type(tINVALID) 
{ }

const int ExprNode::get_eval_type() const
{
	return eval_type;
}

std::string ExprNode::to_string() const
{
	if (eval_type != tINVALID)
		return Ast::to_string() + "<type=" + (eval_type == tINT ? "tINT" : "tVEC") + ">";
	else
		return Ast::to_string();
}

AddNode::AddNode(ExprNode *left, AstToken *add, ExprNode *right)
	:ExprNode(add), left(left), right(right)
{ }

std::string AddNode::to_string_tree() const
{
	std::stringstream ss;
	ss << "(" << to_string() << " " << left->to_string() << " " << right->to_string() << ")";
	return ss.str();
}

const int AddNode::get_eval_type() const
{
	return tINT;
}

IntNode::IntNode(AstToken *token)
	: ExprNode(token)
{
	eval_type = tINT;
}

std::string IntNode::to_string_tree() const
{
	std::stringstream ss;
	ss << "(" << to_string() << ")";
	return ss.str();
}

VecNode::VecNode(AstToken *token, std::vector<ExprNode*> elements)
	: ExprNode(token)
{
	eval_type = tVEC;
	for (auto element : elements)
		add_child(element);
}

VecNode::VecNode(AstToken *token, std::initializer_list<ExprNode*> elements)
	: ExprNode(token)
{
	eval_type = tVEC;
	for (auto element : elements)
		add_child(element);
}

void VecNode::add_child(ExprNode *node)
{
	elements.push_back(node);
}

std::string VecNode::to_string_tree() const
{
	if (elements.size() == 0)
		return to_string();
	std::stringstream ss;
	if (!is_nil())
	{
		ss << "(";
		ss << to_string();
		ss << " ";
	}
	for (auto iter = elements.cbegin(); iter != elements.cend(); ++iter)
	{
		if (iter != elements.cbegin())
			ss << " ";
		ss << (*iter)->to_string_tree();
	}
	if (!is_nil())
		ss << ")";
	return ss.str();
}
