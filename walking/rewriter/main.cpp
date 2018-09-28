#include "ast.h"



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
	
	auto vistor = new AstVisitor();
	stats_list->visit(vistor);

	
	return 0;

}