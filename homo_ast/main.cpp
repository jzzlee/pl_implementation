#include "ast.h"
#include <iostream>

int main()
{
	auto plus = new AstToken(AstToken::PLUS, "+");
	auto one = new AstToken(AstToken::INT, "1");
	auto two = new AstToken(AstToken::INT, "2");

	auto root = new Ast(plus);
	root->add_child(new Ast(one));
	root->add_child(new Ast(two));
	std::cout << root->to_string_tree() << std::endl;

	auto list = new Ast();
	list->add_child(new Ast(new AstToken(AstToken::INT, "1")));
	list->add_child(new Ast(new AstToken(AstToken::INT, "2")));
	std::cout << list->to_string_tree() << std::endl;
	return 0;

}