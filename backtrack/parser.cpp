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

bool ListParser::element()
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
	return true;
}

bool ListParser::elements()
{
	element();
	while (lookahead.getType() == ListLexer::COMMA) {
		match(ListLexer::COMMA);
		if (lookahead.getType() == ListLexer::NAME)
			element();
		else
			break;
	}
	return true;
}

bool ListParser::list()
{
	bool success = match(ListLexer::LBRACK);
	if (!success)
		return false;
	success = elements();
	if (!success)
		return false;
	success = match(ListLexer::RBRACK);
	return success;
}

bool ListParser::match(int x)
{
	if (lookahead.getType() == x)
	{
		consume();
		return true;
	}
	else
	{
		return false;
	}
}

void ListParser::consume()
{
	lookahead = input.nextToken();
	std::cout << lookahead.toString() << std::endl;
}


BackTrackParser::BackTrackParser(Lexer &input) 
	: ListParser(input) 
{ 
	//init();
}


void BackTrackParser::init()
{
	sync(1);
}


bool BackTrackParser::stat()
{
	if (!match_stat())
		return false;
	else
		return true;
	if (match_list() &&match(ListLexer::LEOF_TYPE))
	{
		list();
		match(Lexer::LEOF_TYPE);
		return true;
	}
	else if (match_assign() && match(ListLexer::LEOF_TYPE))
	{
		assign();
		match(Lexer::LEOF_TYPE);
		return true;
	}
	else
		std::cout << "error in stat" << std::endl;
	return false;
}


bool BackTrackParser::match_stat()
{
	auto success = false;
	mark();
	if (match_list() && match(ListLexer::LEOF_TYPE))
		success = true;
	else
	{
		release();
		mark();
	}
	if(!success && match_assign() && match(ListLexer::LEOF_TYPE))
		success = true;
	if (!success)
		release();
	else
		pop();
	return success;
}

bool BackTrackParser::speculate_stat_alt1()
{
	auto success = true;
	mark();
	if (match_list() && match(Lexer::LEOF_TYPE))
	{
		success = true;
	}
	else
		success = false;
	release();
	return success;
}


bool BackTrackParser::speculate_stat_alt2()
{
	auto success = false;
	mark();
	if (match_list() && match(ListLexer::EQUALS) && match_list())
		success = true;
	release();
	return success;
}


Token BackTrackParser::LT(int i)
{
	sync(i);
	return buff[p + i - 1];
}

int BackTrackParser::LA(int i)
{
	return LT(i).getType();
}

bool BackTrackParser::match(int x)
{
	mark();
	if (LA(1) == x)
	{
		consume();
		pop();
		return true;
	}
	else
	{
		release();
		return false;
	}
}

void BackTrackParser::sync(int i)
{
	if ((p + i - 1) > (int(buff.size()) - 1))
	{
		auto n = (p + i - 1) - (int(buff.size()) - 1);
		fill(n);
	}
}

void BackTrackParser::fill(int n)
{
	for (int i = 0; i < n; ++i)
	{
		buff.push_back(input.nextToken());
	}
}

void BackTrackParser::consume()
{
	p++;
	//if (p + 1 >= buff.size())
		sync(1);
}

int BackTrackParser::mark()
{
	markers.push_back(p);
	return p;
}

void BackTrackParser::release()
{
	int marker = *(--markers.end());
	markers.pop_back();
	seek(marker);
}

void BackTrackParser::pop()
{
	markers.pop_back();
	clear_buff();
}

void BackTrackParser::clear_buff()
{
	if (p + 1 == buff.size() && !isSpeculating())
	{
		p = 0;
		buff.clear();
	}
}

void BackTrackParser::seek(int index)
{
	p = index;
}

bool BackTrackParser::isSpeculating()
{
	return markers.size() > 0;
}

bool BackTrackParser::assign()
{
	if (list() && match(ListLexer::EQUALS) && list())
		return true;
	return false;
}

bool BackTrackParser::match_assign()
{
	mark();
	auto success = match_list() && match(ListLexer::EQUALS) && match_list();
	if (!success)
		release();
	else
		pop();
	return success;
}

bool BackTrackParser::elements()
{	

	element();
	while (LA(1) == ListLexer::COMMA) {
		match(ListLexer::COMMA);
		element();
	}
	return true;
}

bool BackTrackParser::match_elements()
{
	mark();
	auto success = match_element();
	if (!success)
	{
		release();
		return false;
	}
	while (LA(1) == ListLexer::COMMA) {
		if (!match(ListLexer::COMMA))
		{
			release();
			return false;
		}
		success = match_element();
		if (!success)
		{
			release();
			return false;
		}
	}
	pop();
	return true;
}

bool BackTrackParser::element()
{
	if (LA(1) == ListLexer::NAME && LA(2) == ListLexer::EQUALS)
	{
		match(ListLexer::NAME);
		match(ListLexer::EQUALS);
		match(ListLexer::NAME);
		return true;
	}
	else if (LA(1) == ListLexer::NAME)
	{
		match(ListLexer::NAME);
		return true;
	}
	else if (LA(1) == ListLexer::LBRACK)
	{
		list();
		return true;
	}
	return false;
}

bool BackTrackParser::match_element()
{
	mark();
	auto success = LA(1) == ListLexer::NAME && LA(2) == ListLexer::EQUALS;
	if (success)
	{
		match(ListLexer::NAME);
		match(ListLexer::EQUALS);
		match(ListLexer::NAME);
		pop();
		return true;
	}
	else
	{
		release();
		mark();
	}
	success = LA(1) == ListLexer::NAME;
	if (success)
	{
		if (match(ListLexer::NAME))
		{
			pop();
			return true;
		}
	}
	else
	{
		release();
		mark();
	}
	success = LA(1) == ListLexer::LBRACK;
	if (success)
	{
		auto success = match_list();
		if (success)
		{
			pop();
			return true;
		}
	}
	release();
	return false;
}

bool BackTrackParser::list()
{
	if (match_list())
		return true;
	else
		return false;
}


bool BackTrackParser::match_list()
{
	mark();
	bool success = match(ListLexer::LBRACK);
	if (!success)
	{
		release();
		return false;
	}
	success = match_elements();
	if (!success)
	{
		release();
		return false;
	}
	success = match(ListLexer::RBRACK);
	if (!success)
	{
		release();
		return false;
	}
	pop();
	return true;
}