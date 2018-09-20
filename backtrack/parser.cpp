#include "parser.h"
#include <cassert>
#include <iostream>


Token::Token(int type, const std::string &text) :
	type(type), text(text) { }

std::string Token::toString() const
{
	std::string tname = ListLexer::getTokenName(type);
	auto s = std::string("<'" + text + "', " + tname + ">");
	return s;
}


const char Lexer::LEOF = (char)-1;
const int Lexer::LEOF_TYPE = 1;

Lexer::Lexer(const std::string &input) :
	input(input)
{
	p = 0;
	c = input[p];
}

void Lexer::consume()
{
	++p;
	if (p < input.size())
		c = input[p];
	else
		c = LEOF;
}

void Lexer::reset()
{
	p = 0;
	c = input[p];
}


void Lexer::match(char x)
{
	assert(c == x);
	consume();
}

Lexer::~Lexer() { }


const int ListLexer::NAME = 2;
const int ListLexer::COMMA = 3;
const int ListLexer::LBRACK = 4;
const int ListLexer::RBRACK = 5;
const int ListLexer::EQUALS = 6;
const std::vector<std::string> ListLexer::tokenNames = { "n/a", "<EOF>", "NAME", "COMMA", "LBRACK", "RBRACK", "EQUIALS" };

ListLexer::ListLexer(const std::string &input) :
	Lexer(input) {}

const std::string &ListLexer::getTokenName(int tokenType)
{
	if (tokenType < LEOF || tokenType >= (int)tokenNames.size())
	{
		std::cout << "invalid tokenType: " << tokenType << std::endl;
		return tokenNames[0];
	}
	else
		return tokenNames[tokenType];
}

bool ListLexer::isLetter()
{
	return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
}

void ListLexer::WS()
{
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		consume();
}

Token ListLexer::Name()
{
	std::string s;
	do {
		s += c;
		consume();
	} while (isLetter());

	return Token(NAME, s);
}

Token ListLexer::nextToken()
{
	while (c != LEOF)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			WS();
			continue;
		case ',':
			consume();
			return Token(COMMA, ",");
		case '[':
			consume();
			return Token(LBRACK, "[");
		case ']':
			consume();
			return Token(RBRACK, "]");
		case '=':
			consume();
			return Token(EQUALS, "=");
		default:
			if (isLetter())
				return Name();
			else
				std::cout << "invalid character: " << c << std::endl;
		}
	}
	return Token(LEOF_TYPE, "<EOF>");
}

Parser::Parser(Lexer &input) :
	input(input) {
}

Parser::~Parser() { }

ListParser::ListParser(Lexer &input) :
	Parser(input) {
}

void ListParser::init()
{
	consume();
}

void ListParser::element()
{
	if (lookahead.getType() == ListLexer::NAME)
	{
		match(ListLexer::NAME);
	}
	else if (lookahead.getType() == ListLexer::LBRACK)
	{
		list();
	}
	else
	{
		std::cout << "ListParser get noknown element: " << lookahead.toString() << std::endl;
	}
}

void ListParser::elements()
{
	element();
	while (lookahead.getType() == ListLexer::COMMA) {
		match(ListLexer::COMMA);
		if (lookahead.getType() == ListLexer::NAME)
			element();
		else
			break;
	}
}

void ListParser::list()
{
	match(ListLexer::LBRACK);
	elements();
	match(ListLexer::RBRACK);
}

void ListParser::match(int x)
{
	if (lookahead.getType() == x)
	{
		consume();
	}
	else
	{
		std::cout << "ListParser cannot match " << lookahead.getType() << " with type " << x << std::endl;
	}
}

void ListParser::consume()
{
	lookahead = input.nextToken();
	std::cout << lookahead.toString() << std::endl;
}


ListLLKParser::ListLLKParser(Lexer &lexer, const size_t n) :
	ListParser(lexer), length(n)
{
	buff.resize(n);
}

void ListLLKParser::init()
{
	for (auto &c : buff)
		c = input.nextToken();
}


Token ListLLKParser::LT(int i) const
{
	return buff[(p + i - 1) % length];
}

int ListLLKParser::LA(int i) const
{
	return LT(i).getType();
}

void ListLLKParser::consume()
{
	buff[p] = input.nextToken();
	p = (p + 1) % length;
}

void ListLLKParser::match(int x)
{
	if (LA(1) == x)
	{
		consume();
	}
	else
	{
		std::cout << "can not match " << LA(1) << " with type " << x << std::endl;
	}
}

void ListLLKParser::element()
{
	auto ahead1 = LA(1);
	auto ahead2 = LA(2);
	if (ahead1 == ListLexer::NAME && ahead2 == ListLexer::EQUALS)
	{
		match(ListLexer::NAME);
		match(ListLexer::EQUALS);
		match(ListLexer::NAME);
	}
	else if (ahead1 == ListLexer::NAME)
	{
		match(ListLexer::NAME);
	}
	else if (ahead1 == ListLexer::LBRACK)
	{
		list();
	}
	else
	{
		std::cout << "ListLLKParser get noknown element: " << ahead1 << std::endl;
	}
}

void ListLLKParser::elements()
{
	element();
	while (LA(1) == ListLexer::COMMA) {
		match(ListLexer::COMMA);
		element();

	}
}


BackTrackParser::BackTrackParser(Lexer &input) 
	: Parser(input) { }


void 
