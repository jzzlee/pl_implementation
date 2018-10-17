#include "parser.h"
#include <iostream>
#include <cstdio>
#include <vector>


int main()
{
	std::string s2 = "float b;";
	std::string s1 = "int a = b;";

	std::vector<std::string> vec{ s1, s2 };

	for (const auto &s : vec)
	{
		std::cout << s << std::endl;
		auto lexer = CymbolLexer(s);
		auto parser = SymbolParser(lexer);
		auto table = new SymbolTable();
		parser.compile(table);
		delete table;
	}
	return 0;
}
