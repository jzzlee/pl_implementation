
#include "rule.h"

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
	}
	auto root = new VecNode(new AstToken(AstToken::VEC), std::move(elements));
	right->elements.clear();
	return root;
}


bool MultZeroRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->left->get_node_type() == AstToken::INT && n->left->to_string() == "0";
}

Ast* MultZeroRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto left = n->left;

	n->left = nullptr;
	return left;
}

bool ZeroMultRule::match(const Ast *node) const
{
	if (node->get_node_type() != AstToken::MULT)
		return false;
	auto n = reinterpret_cast<MultNode*>(const_cast<Ast*>(node));
	return n->right->get_node_type() == AstToken::INT && n->right->to_string() == "0";
}

Ast* ZeroMultRule::rewrite(Ast *node)
{
	auto n = reinterpret_cast<MultNode*>(node);
	auto right = n->right;

	n->left = nullptr;
	return right;
}

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
	auto root = new LeftShiftNode(new AstToken(AstToken::LEFT_SHIFT, " << "), right, new IntNode(new AstToken(AstToken::INT, "1")));
	return root;
}

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
	auto right = reinterpret_cast<IntNode*>(left->right);
	auto v1 = atoi(right->to_string().c_str());
	auto v2 = atoi(n->right->to_string().c_str());
	auto s = v1 + v2;
	std::stringstream ss;
	ss << s;
	right->set_value(ss.str());
	return left;
}

AstRewriter::~AstRewriter()
{
	for (auto rule : topdown_rules)
		delete rule;
	for (auto rule : bottomup_rules)
		delete rule;
}

void AstRewriter::add_rule(Rule *rule, AstRewriter::VISIT_ORDER order)
{
	switch (order)
	{
	case AstRewriter::TOPDOWN:
		topdown_rules.push_back(rule);
		break;
	case AstRewriter::BOTTOMUP:
		bottomup_rules.push_back(rule);
		break;
	default:
		break;
	}
}

Ast* AstRewriter::rewrite(Ast *node)
{
	Ast *new_node = nullptr;
	for (auto rule : topdown_rules)
	{
		if (rule->match(node))
		{
			new_node = rule->rewrite(node);
			delete node;
			node = new_node;
		}
	}
	return new_node;

}

Ast* AstRewriter::rewrite_ex(Ast *node)
{
	Ast *new_node = nullptr;
	switch (node->get_node_type())
	{
	case AstToken::ID:
		new_node = rewrite(reinterpret_cast<VarNode*>(node));
		break;
	case AstToken::ASSIGN:
		new_node = rewrite(reinterpret_cast<AssignNode*>(node));
		break;
	case AstToken::PRINT:
		new_node = rewrite(reinterpret_cast<PrintNode*>(node));
		break;
	case AstToken::PLUS:
		new_node = rewrite(reinterpret_cast<AddNode*>(node));
		break;
	case AstToken::MULT:
		new_node = rewrite(reinterpret_cast<MultNode*>(node));
		break;
	case AstToken::DOT:
		new_node = rewrite(reinterpret_cast<DotProductNode*>(node));
		break;
	case AstToken::INT:
		new_node = rewrite(reinterpret_cast<IntNode*>(node));
		break;
	case AstToken::VEC:
		new_node = rewrite(reinterpret_cast<VecNode*>(node));
		break;
	case AstToken::STAT_LIST:
		new_node = rewrite(reinterpret_cast<StatListNode*>(node));
		break;
	case AstToken::LEFT_SHIFT:
		new_node = rewrite(reinterpret_cast<LeftShiftNode*>(node));
		break;
	default:
		std::cout << "rewrite invalid type" << node->get_node_type() << " " << node->to_string() << std::endl;
	}
	return new_node;
}
