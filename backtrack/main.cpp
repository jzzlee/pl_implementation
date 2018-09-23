#include "parser.h"
#include <iostream>
#include <cstdio>
#include <vector>


int main()
{
	std::string s;
	std::cout << "enter a line of code:" << std::endl;;
	//std::getline(std::cin, s);
	s = "[a, b=c, [d, e]]";
	std::string s1 = "[a, b] = [c, d]";
	std::vector<std::string> vec{ s, s1 };


	for (const auto &s : vec)
	{
		std::cout << s << std::endl;
		auto lexer = ListLexer(s);
		auto parser = BackTrackParser(lexer);
		parser.stat();
	}
	//auto lexer = ListLexer(s);

	//auto parser = BackTrackParser(lexer);
	//parser.list();

	//system("pause");

	return 0;
}
