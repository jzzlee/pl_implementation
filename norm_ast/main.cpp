#include "ast.h"
#include <iostream>

int main()
{
	auto plus = new AstToken(AstToken::PLUS, "+");
	auto one = new AstToken(AstToken::INT, "1");
	auto two = new AstToken(AstToken::INT, "2");

	auto root = new AddNode(new IntNode(one), plus, new IntNode(two));
	std::cout << root->to_string_tree() << std::endl;


	return 0;

}