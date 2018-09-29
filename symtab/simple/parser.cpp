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


const char Lexer::LEOF;
const int Lexer::LEOF_TYPE;

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


const int ListLexer::NAME;
const int ListLexer::COMMA;
const int ListLexer::LBRACK;
const int ListLexer::RBRACK;
const int ListLexer::EQUALS;
const int ListLexer::SIMICOLON;
const std::vector<std::string> ListLexer::tokenNames = { "n/a", "<EOF>", "NAME", "COMMA", "LBRACK", "RBRACK", "EQUIALS", "SIMICOLON" };

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
		case ';':
			consume();
			return Token(SIMICOLON, ";");
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


const int SymbolParser::FAILED;


SymbolParser::SymbolParser(Lexer &input) 
	: ListParser(input) 
{ 
	//init();
}


void SymbolParser::init()
{
	sync(1);
}


bool SymbolParser::stat()
{
	if (!match_stat())
		return false;
	else
		return true;
}


bool SymbolParser::match_stat()
{
	auto success = false;
	mark();
	if (match_list_new() && match(ListLexer::LEOF_TYPE))
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

bool SymbolParser::speculate_stat_alt1()
{
	auto success = true;
	mark();
	if (match_list_new() && match(Lexer::LEOF_TYPE))
	{
		success = true;
	}
	else
		success = false;
	release();
	return success;
}


bool SymbolParser::speculate_stat_alt2()
{
	auto success = false;
	mark();
	if (match_list_new() && match(ListLexer::EQUALS) && match_list_new())
		success = true;
	release();
	return success;
}


Token SymbolParser::LT(int i)
{
	sync(i);
	return buff[p + i - 1];
}

int SymbolParser::LA(int i)
{
	return LT(i).getType();
}

bool SymbolParser::match(int x)
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

void SymbolParser::sync(int i)
{
	if ((p + i - 1) > (int(buff.size()) - 1))
	{
		auto n = (p + i - 1) - (int(buff.size()) - 1);
		fill(n);
	}
}

void SymbolParser::fill(int n)
{
	for (int i = 0; i < n; ++i)
	{
		buff.push_back(input.nextToken());
	}
}

void SymbolParser::consume()
{
	p++;
	//if (p + 1 >= buff.size())
		sync(1);
}

int SymbolParser::mark()
{
	markers.push_back(p);
	return p;
}

void SymbolParser::release()
{
	int marker = *(--markers.end());
	markers.pop_back();
	seek(marker);
}

void SymbolParser::pop()
{
	markers.pop_back();
	clear_buff();
}

void SymbolParser::clear_buff()
{
	if (p + 1 == buff.size() && !isSpeculating())
	{
		p = 0;
		buff.clear();
		clear_memo();
	}
}

void SymbolParser::seek(int index)
{
	p = index;
}

bool SymbolParser::isSpeculating()
{
	return markers.size() > 0;
}

bool SymbolParser::assign()
{
	if (_list() && match(ListLexer::EQUALS) && _list())
		return true;
	return false;
}

bool SymbolParser::match_assign()
{
	mark();
	auto success = match_list_new() && match(ListLexer::EQUALS) && match_list_new();
	if (!success)
		release();
	else
		pop();
	return success;
}

bool SymbolParser::already_parsed_rule(const std::unordered_map<int, int> &memo)
{
	auto res = memo.find(p);
	if (res == memo.end())
	{
		return false;
	}
	auto value = res->second;
	std::cout << "parsed list before index " << p << " skip ahead to " << value << ": " << buff[value].getText() << std::endl;
	if (value == FAILED)
		return false;
	seek(value);
	return true;
}

void SymbolParser::memorize(std::unordered_map<int, int> &memo, int index, bool failed)
{
	auto end = failed ? FAILED : p;
	memo.insert(std::make_pair(index, end));
}

void SymbolParser::clear_memo()
{
	list_memo.clear();
}
