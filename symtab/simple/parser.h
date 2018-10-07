#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <vector>
#include <unordered_map>

#include "symbol.h"


class Token {
public:
	Token() = default;
	Token(int type, const std::string &text);
	std::string toString() const;
	int getType() const { return type; }
	const std::string &getText() const { return text; }


private:
	int type;
	std::string text;
};


class Lexer {
public:
	Lexer(const std::string &input);
	void consume();
	void match(char x);
	virtual Token nextToken() = 0;
	virtual void reset();
	virtual ~Lexer();

protected:
	std::string input;
	unsigned p;
	char c;

public:
	static const char LEOF = (char)-1;
	static const int LEOF_TYPE = 1;
};


class ListLexer : public Lexer
{
public:
	ListLexer(const std::string &input);
	bool isLetter();
	void WS();
	Token Name();
	Token nextToken();
	static const std::string &getTokenName(int tokenType);

	static const int NAME = 2;
	static const int COMMA = 3;
	static const int LBRACK = 4;
	static const int RBRACK = 5;
	static const int EQUALS = 6;
	static const int SIMICOLON = 7;

	static const int BUILTIN = 1001;

	static const std::vector<std::string> tokenNames;
};


class Parser {
public:
	Parser(Lexer &input);
	virtual ~Parser();

	virtual void init() = 0;
	virtual bool match(int x) = 0;
	virtual void consume() = 0;

protected:
	Lexer & input;
	Token lookahead;
};


class SymbolParser : public Parser {
public:
	SymbolParser(Lexer &input);

	void compile(SymbolTable *table);
	bool mactch_var_declaration(SymbolTable *table);

	Token LT(int i);
	int LA(int i);
	bool match(int x) override;
	void sync(int i);
	void fill(int n);
	void consume() override;
	int mark();
	void release();
	std::vector<Token>* pop_buff();
	void pop();
	void seek(int index);
	bool isSpeculating();

	void init() override;
	Token last_token();

private:
	static const int FAILED = -1; 
	
	std::vector<int> markers;
	std::vector<Token> buff;
	int p = 0;
	
	
};
