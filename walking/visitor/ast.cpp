
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

void Ast::visit(AstVisitor *visitor) const
{
	visitor->visit(this);
}

AddNode::AddNode(Ast *left, AstToken *add, Ast *right)
	:Ast(add), left(left), right(right) {}


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


DotProductNode::DotProductNode(Ast *left, AstToken *dot, Ast *right)
	: Ast(dot), left(left), right(right) {}


DotProductNode::~DotProductNode()
{
	delete left;
	delete right;
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


PrintNode::PrintNode(AstToken *pr, Ast *element)
	:Ast(pr), element(element) {}

PrintNode::~PrintNode()
{
	delete element;
}


StatListNode::StatListNode(const std::vector<Ast*> &elements)
	: Ast(AstToken::STAT_LIST), elements(elements) {}

StatListNode::StatListNode(const std::initializer_list<Ast*> &elements)
	: Ast(nullptr), elements(elements) {}

StatListNode::~StatListNode()
{
	for (auto ele : elements)
		delete ele;
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

void AstVisitor::visit(const Ast *node) const
{
	Ast *n = const_cast<Ast*>(node);
	switch (node->get_node_type())
	{
	case AstToken::ID:
		visit(reinterpret_cast<VarNode*>(n));
		break;
	case AstToken::ASSIGN:
		visit(reinterpret_cast<AssignNode*>(n));
		break;
	case AstToken::PRINT:
		visit(reinterpret_cast<PrintNode*>(n));
		break;
	case AstToken::PLUS:
		visit(reinterpret_cast<AddNode*>(n));
		break;
	case AstToken::MULT:
		visit(reinterpret_cast<MultNode*>(n));
		break;
	case AstToken::DOT:
		visit(reinterpret_cast<DotProductNode*>(n));
		break;
	case AstToken::INT:
		visit(reinterpret_cast<IntNode*>(n));
		break;
	case AstToken::VEC:
		visit(reinterpret_cast<VecNode*>(n));
		break;
	case AstToken::STAT_LIST:
		visit(reinterpret_cast<StatListNode*>(n));
		break;
	default:
		std::cout << "visit invalid type" << node->get_node_type() << " " << node->to_string() << std::endl;
	}
}

void AstVisitor::visit(const AssignNode *node) const
{
	visit(node->left);
	std::cout << " " << node->to_string() << " ";
	visit(node->right);
}

void AstVisitor::visit(const PrintNode *node) const
{
	std::cout << node->to_string() << " ";
	visit(node->element);
}

void AstVisitor::visit(const StatListNode *node) const
{
	for (auto ele : node->elements)
	{
		visit(ele);
		std::cout << std::endl;
	}
}

void AstVisitor::visit(const VarNode *node) const
{
	std::cout << node->to_string();
}

void AstVisitor::visit(const AddNode *node) const
{
	visit(node->left);
	std::cout << " " << node->to_string() << " ";
	visit(node->right);
}

void AstVisitor::visit(const DotProductNode *node) const
{
	visit(node->left);
	std::cout << " " << node->to_string() << " ";
	visit(node->right);
}

void AstVisitor::visit(const IntNode *node) const
{
	std::cout << node->to_string();
}

void AstVisitor::visit(const MultNode *node) const
{
	visit(node->left);
	std::cout << " " << node->to_string() << " ";
	visit(node->right);
}

void AstVisitor::visit(const VecNode *node) const
{
	std::cout << " [";
	std::cout << node->to_string();
	for (auto ele : node->elements)
	{
		visit(ele);
		std::cout << ", ";
	}
	std::cout << "] ";
}
