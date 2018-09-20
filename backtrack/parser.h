#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <vector>


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
	static const char LEOF;
	static const int LEOF_TYPE;
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

	static const int NAME;
	static const int COMMA;
	static const int LBRACK;
	static const int RBRACK;
	static const int EQUALS;
	static const std::vector<std::string> tokenNames;
};


class Parser {
public:
	Parser(Lexer &input);
	virtual ~Parser();

	virtual void init() = 0;
	virtual void element() = 0;
	virtual void elements() = 0;
	virtual void list() = 0;
	virtual void match(int x) = 0;
	virtual void consume() = 0;

protected:
	Lexer & input;
	Token lookahead;
};


class ListParser : public Parser {
public:
	ListParser(Lexer &input);

	void init() override;
	void element() override;
	void elements() override;
	void list() override;
	void match(int x) override;
	void consume() override;
};

class ListLLKParser : public ListParser {
public:
	ListLLKParser(Lexer &lexer, const size_t n);

	Token LT(int i) const;
	int LA(int i) const;

	void init() override;
	void element() override;
	void elements() override;
	void match(int x) override;
	void consume() override;

private:
	const size_t length;
	std::vector<Token> buff;
	int p = 0;
};


class BackTrackParser : public Parser {
public:
	BackTrackParser(Lexer &input);

	bool speculate_stat_alt1();
	bool specilate_stat_alt2();
	Token LT(int i);
	int LA(int i);
	void match(int x) override;
	void sync(int i);
	void fill(int n);
	void consume() override;
	int mark();
	void release();
	void seek(int index);
	bool isSpeculating();
	void stat();

private:
	std::vector<int> markers;
	std::vector<Token> buff;
	int p = 0;
};
