#include "ast.h"
#include "rule.h"


IntNode* get_int_node(int i)
{
	std::stringstream ss;
	ss << i;

	auto node = new IntNode(new AstToken(AstToken::INT, ss.str()));
	return node;
}

int main()
{
	// x = 1 + 4
	auto stats = std::vector<Ast*>();
	auto *add = new AddNode(get_int_node(1), new AstToken(AstToken::PLUS, "+"), get_int_node(4));
	auto *var_x = new VarNode(new AstToken(AstToken::ID, "x"));
	auto *assign = new AssignNode(var_x, new AstToken(AstToken::ASSIGN, "="), add);
	stats.push_back(assign);

	// print x * [2, 3, 4]
	auto mult = new AstToken(AstToken::MULT, "*");
	auto elements = std::vector<Ast*>();
	elements.push_back(get_int_node(2));
	elements.push_back(get_int_node(3));
	elements.push_back(get_int_node(4));
	auto v = new VecNode(new AstToken(AstToken::VEC), std::move(elements));
	auto xref = new VarNode(new AstToken(AstToken::ID, "x"));
	auto pv = new MultNode(xref, mult, v);
	auto p = new PrintNode(new AstToken(AstToken::PRINT, "print"), pv);

	stats.push_back(p);
	auto stats_list = new StatListNode(stats);
	
	auto visitor = new AstVisitor();
	stats_list->visit(visitor);

	// --------------------------------------------
	// optimize based on rules.

	auto vec_mul_rule = new ScalarVecMultRule(Rule::TOPDOWN);
	auto mul_zero_rule = new MultZeroRule(Rule::BOTTOMUP);
	auto zero_mul_rule = new ZeroMultRule(Rule::BOTTOMUP);
	auto xplusx_rule = new XPlusXRule(Rule::BOTTOMUP);
	auto mult_two_rule = new MultByTwoRule(Rule::BOTTOMUP);
	auto combine_shift_rule = new CombineLeftShiftRule(Rule::BOTTOMUP);
	auto rewriter = AstRewriter();
	rewriter.add_rule(vec_mul_rule);
	rewriter.add_rule(mul_zero_rule);
	rewriter.add_rule(zero_mul_rule);
	rewriter.add_rule(xplusx_rule);
	rewriter.add_rule(mult_two_rule);
	rewriter.add_rule(combine_shift_rule);

	std::cout << "--------------------------------" << std::endl;
	std::cout << "ast rewrite:" << std::endl;
	// 4 * [1, 5 * 0, 0 * 3] --> 4 * 1, 0, 0
	auto four = get_int_node(4);
	auto one = get_int_node(1);
	auto five = get_int_node(5);
	auto three = get_int_node(3);
	auto zero1 = get_int_node(0);
	auto zero2 = get_int_node(0);
	auto mult1 = new MultNode(five, new AstToken(AstToken::MULT, "*"), zero1);
	auto mult2 = new MultNode(zero2, new AstToken(AstToken::MULT, "*"), three);
	auto vec = new VecNode(new AstToken(AstToken::VEC), { one, mult1, mult2 });
	Ast *stat1 = new MultNode(four, new AstToken(AstToken::MULT, "*"), vec);

	stat1->visit(visitor);
	rewriter.rewrite(stat1);
	std::cout << " --> ";
	stat1->visit(visitor);
	std::cout << std::endl;

	// 2 * (x + x) --> x << 2
	auto two2 = get_int_node(2);
	auto x1 = new VarNode(new AstToken(AstToken::ID, "x"));
	auto x2 = new VarNode(new AstToken(AstToken::ID, "x"));
	auto add1 = new AddNode(x1, new AstToken(AstToken::PLUS, "+"), x2);
	Ast *stat2 = new MultNode(two2, new AstToken(AstToken::MULT, "*"), add1);

	stat2->visit(visitor);
	rewriter.rewrite(stat2);
	std::cout << " --> ";
	stat2->visit(visitor);
	std::cout << std::endl;

	return 0;

}