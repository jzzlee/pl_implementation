
#include "rule.h"


Rule::Rule(Rule::VISIT_ORDER order)
	: order(order) {}

Rule::~Rule()
{

}

Ast* Rule::rewrite(Ast *node)
{
	return node;
}

bool Rule::match(const Ast *node) const
{
	return false;
}

const Rule::VISIT_ORDER Rule::get_order() const
{
	return order;
}

ScalarVecMultRule::ScalarVecMultRule(Rule::VISIT_ORDER order)
	:Rule(order) {}

bool ScalarVecMultRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->left->get_node_type() == AstToken::INT && n->right->get_node_type() == AstToken::VEC;
}

Ast* ScalarVecMultRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto left = reinterpret_cast<IntNode*>(n->left);
	auto right = reinterpret_cast<VecNode*>(n->right);

	std::vector<Ast*> elements;
	for (auto n_right : right->elements)
	{
		auto n_left = new IntNode(new AstToken(AstToken::INT, left->to_string()));
		auto ele = new MultNode(n_left, new AstToken(AstToken::MULT, "*"), n_right);
		elements.push_back(ele);
	}
	auto root = new VecNode(new AstToken(AstToken::VEC), std::move(elements));
	right->elements.clear();
	return root;
}

MultZeroRule::MultZeroRule(Rule::VISIT_ORDER order)
	:Rule(order) {}

bool MultZeroRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->right->get_node_type() == AstToken::INT && n->right->to_string() == "0";
}

Ast* MultZeroRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto right = n->right;

	n->right = nullptr;
	return right;
}

ZeroMultRule::ZeroMultRule(Rule::VISIT_ORDER order)
	: Rule(order) {}

bool ZeroMultRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->left->get_node_type() == AstToken::INT && n->left->to_string() == "0";
}

Ast* ZeroMultRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto left = n->left;

	n->left = nullptr;
	return left;
}

XPlusXRule::XPlusXRule(Rule::VISIT_ORDER order)
	:Rule(order) {}

bool XPlusXRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::PLUS)
		return false;
	auto n = reinterpret_cast<AddNode*>(const_cast<Ast*>(node));
	return n->left->get_node_type() == AstToken::ID && n->right->get_node_type() == AstToken::ID &&
		n->left->to_string() == n->right->to_string();
}

Ast* XPlusXRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<AddNode*>(node);
	auto v = n->left->to_string();
	
	auto left = n->left;
	n->left = nullptr;

	auto root = new MultNode(new IntNode(new AstToken(AstToken::INT, "2")), new AstToken(AstToken::MULT, "*"), left);
	return root;
}

MultByTwoRule::MultByTwoRule(Rule::VISIT_ORDER order)
	:Rule(order) {}

bool MultByTwoRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->left->get_node_type() == AstToken::INT && n->left->to_string() == "2";
}

Ast* MultByTwoRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto right = n->right;
	n->right = nullptr;
	auto root = new LeftShiftNode(right, new AstToken(AstToken::LEFT_SHIFT, " << "), new IntNode(new AstToken(AstToken::INT, "1")));
	return root;
}

CombineLeftShiftRule::CombineLeftShiftRule(Rule::VISIT_ORDER order)
	:Rule(order) {}

bool CombineLeftShiftRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::LEFT_SHIFT)
		return false;
	auto n = reinterpret_cast<LeftShiftNode*>(const_cast<Ast*>(node));
	if (n->right->get_node_type() != AstToken::INT)
		return false;
	if (n->left->get_node_type() != AstToken::LEFT_SHIFT)
		return false;
	auto left = reinterpret_cast<LeftShiftNode*>(n->left);
	return left->right->get_node_type() == AstToken::INT;
}

Ast* CombineLeftShiftRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<LeftShiftNode*>(node);
	auto left = reinterpret_cast<LeftShiftNode*>(n->left);
	n->left = nullptr;
	IntNode *right = reinterpret_cast<IntNode*>(left->right);
	auto v1 = atoi(right->to_string().c_str());
	auto v2 = atoi(n->right->to_string().c_str());
	auto s = v1 + v2;
	std::stringstream ss;
	ss << s;
	std::string text = ss.str();
	right->set_value(text);
	return left;
}

AstRewriter::~AstRewriter()
{
	for (auto rule : topdown_rules)
		delete rule;
	for (auto rule : bottomup_rules)
		delete rule;
}

void AstRewriter::add_rule(Rule *rule)
{
	auto order = rule->get_order();
	switch (order)
	{
	case Rule::TOPDOWN:
		topdown_rules.push_back(rule);
		break;
	case Rule::BOTTOMUP:
		bottomup_rules.push_back(rule);
		break;
	default:
		break;
	}
}

void AstRewriter::rewrite(Ast *&node)
{
	Ast *new_node = node;
	for (auto rule : topdown_rules)
	{
		if (rule->match(node))
		{
			new_node = rule->rewrite(node);
			delete node;
			node = new_node;
		}
	}
	rewrite_ex(node);
	rewrite_onback(node);
}

void AstRewriter::rewrite_onback(Ast *&node)
{
	Ast *new_node = nullptr;
	for (auto rule : bottomup_rules)
	{
		if (rule->match(node))
		{
			new_node = rule->rewrite(node);
			delete node;
			node = new_node;
		}
	}
}

void AstRewriter::rewrite_ex(Ast *&node)
{
	switch (node->get_node_type())
	{
	case AstToken::ID:
		rewrite_elements(reinterpret_cast<VarNode*&>(node));
		break;
	case AstToken::ASSIGN:
		rewrite_elements(reinterpret_cast<AssignNode*&>(node));
		break;
	case AstToken::PRINT:
		rewrite_elements(reinterpret_cast<PrintNode*&>(node));
		break;
	case AstToken::PLUS:
		rewrite_elements(reinterpret_cast<AddNode*&>(node));
		break;
	case AstToken::MULT:
		rewrite_elements(reinterpret_cast<MultNode*&>(node));
		break;
	case AstToken::DOT:
		rewrite_elements(reinterpret_cast<DotProductNode*&>(node));
		break;
	case AstToken::INT:
		rewrite_elements(reinterpret_cast<IntNode*&>(node));
		break;
	case AstToken::VEC:
		rewrite_elements(reinterpret_cast<VecNode*&>(node));
		break;
	case AstToken::STAT_LIST:
		rewrite_elements(reinterpret_cast<StatListNode*&>(node));
		break;
	case AstToken::LEFT_SHIFT:
		rewrite_elements(reinterpret_cast<LeftShiftNode*&>(node));
		break;
	default:
		rewrite_elements(node);
	}
}

void AstRewriter::rewrite_elements(Ast *&node)
{

}

void AstRewriter::rewrite_elements(AssignNode *&node)
{
	rewrite(node->left);
	rewrite(node->right);
}

void AstRewriter::rewrite_elements(PrintNode *&node)
{
	rewrite(node->element);
}

void AstRewriter::rewrite_elements(StatListNode *&node)
{
	for (auto &ele : node->elements)
		rewrite(ele);
}

void AstRewriter::rewrite_elements(VarNode *&node)
{

}

void AstRewriter::rewrite_elements(AddNode *&node)
{
	rewrite(node->left);
	rewrite(node->right);
}

void AstRewriter::rewrite_elements(DotProductNode *&node)
{
	rewrite(node->left);
	rewrite(node->right);
}

void AstRewriter::rewrite_elements(IntNode *&node)
{

}

void AstRewriter::rewrite_elements(MultNode *&node)
{
	rewrite(node->left);
	rewrite(node->right);
}

void AstRewriter::rewrite_elements(VecNode *&node)
{
	for (auto &ele : node->elements)
		rewrite(ele);
}

void AstRewriter::rewrite_elements(LeftShiftNode *&node)
{
	rewrite(node->left);
	rewrite(node->right);
}
