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
const int ListLexer::BUILTIN;
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

	if (BuiltinSymbolTable::instance() != nullptr && BuiltinSymbolTable::instance()->resolve(s)!=nullptr)
		return Token(BUILTIN, s);
	else
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
	: Parser(input) 
{ 
	//init();
}


void SymbolParser::init()
{
	sync(1);
}

Token SymbolParser::last_token()
{
	auto token = *(--buff.end());
	buff.pop_back();
	return token;
}

void SymbolParser::compile(SymbolTable *table)
{
	mactch_var_declaration(table);
}

bool SymbolParser::mactch_var_declaration(SymbolTable *table)
{
	mark();
	auto success = match(ListLexer::BUILTIN) && match(ListLexer::NAME);
	if (!success)
	{
		release();
		return success;
	}
	if (match(ListLexer::EQUALS))
	{
		if (!match(ListLexer::NAME))
		{
			release();
			return false;
		}
	}
	if (!match(ListLexer::SIMICOLON))
	{
		release();
		return false;
	}
	auto ptr = pop_buff();
	auto line = *ptr;

	auto text = line[1].getText();
	auto type = reinterpret_cast<BuiltinSymbol*>(BuiltinSymbolTable::instance()->resolve(line[0].getText()));
	auto vs = new VarSymbol(text, type);
	std::cout << "get new varsymbol, text: " << text << ", type: " << type->get_name() << std::endl;
	table->define(vs);
	return true;
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

std::vector<Token>* SymbolParser::pop_buff()
{
	markers.pop_back();
	if (p + 1 == buff.size() && !isSpeculating())
	{
		p = 0;
		auto ptr = new std::vector<Token>(buff);
		buff.clear();
		return ptr;
	}
	return nullptr;
}

void SymbolParser::pop()
{
	markers.pop_back();
	if (p + 1 == buff.size() && !isSpeculating())
	{
		p = 0;
		buff.clear();
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

