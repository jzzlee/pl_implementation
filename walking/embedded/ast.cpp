
#include "ast.h"

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
	std::cout << to_string();
}

AddNode::AddNode(Ast *left, AstToken *add, Ast *right)
	:Ast(add), left(left), right(right) {}

void AddNode::print() const
{
	left->print();
	std::cout << " + ";
	right->print();
}

AddNode::~AddNode()
{
	delete left;
	delete right;
}

AssignNode::AssignNode(Ast *left, AstToken *assign, Ast *right)
	: Ast(assign), left(left), right(right) {}

AssignNode::~AssignNode()
{
	delete left;
	delete right;
}

void AssignNode::print() const
{
	left->print();
	std::cout << " = ";
	right->print();
}


DotProductNode::DotProductNode(Ast *left, AstToken *dot, Ast *right)
	: Ast(dot), left(left), right(right) {}


DotProductNode::~DotProductNode()
{
	delete left;
	delete right;
}

void DotProductNode::print() const
{
	left->print();
	std::cout << " . ";
	right->print();
}

IntNode::IntNode(AstToken *token)
	: Ast(token) {}


MultNode::MultNode(Ast *left, AstToken *mult, Ast *right)
	: Ast(mult), left(left), right(right) {}

MultNode::~MultNode()
{
	delete left;
	delete right;
}

void MultNode::print() const
{
	std::stringstream ss;

	left->print();
	std::cout << " * ";
	right->print();
}

PrintNode::PrintNode(AstToken *pr, Ast *element)
	:Ast(pr), element(element) {}

PrintNode::~PrintNode()
{
	delete element;
}

void PrintNode::print() const
{
	std::cout << "print ";
	element->print();
}

StatListNode::StatListNode(const std::vector<Ast*> &elements)
	: Ast(nullptr), elements(elements) {}

StatListNode::StatListNode(const std::initializer_list<Ast*> &elements)
	: Ast(nullptr), elements(elements) {}

StatListNode::~StatListNode()
{
	for (auto ele : elements)
		delete ele;
}

void StatListNode::print() const
{
	for (auto ele : elements)
	{
		ele->print();
		std::cout << std::endl;
	}
}

VarNode::VarNode(AstToken *var)
	: Ast(var) {}

VecNode::VecNode(AstToken *token, std::vector<Ast*> &&elements)
	: Ast(token), elements(std::forward<std::vector<Ast*>>(elements)) 
{ }

VecNode::VecNode(AstToken *token, const std::initializer_list<Ast*> &elements)
	: Ast(token), elements(elements)
{ }

VecNode::~VecNode()
{
	for (auto ele : elements)
		delete ele;
}

void VecNode::print() const
{
	std::cout << " [";
	std::cout << to_string();
	for (auto ele : elements)
	{
		ele->print();
		std::cout << ", ";
	}
	std::cout << "] ";
}

