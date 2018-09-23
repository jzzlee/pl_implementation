#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <vector>
#include <unordered_map>


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
	virtual bool element() = 0;
	virtual bool elements() = 0;
	virtual bool _list() = 0;
	virtual bool match(int x) = 0;
	virtual void consume() = 0;

protected:
	Lexer & input;
	Token lookahead;
};


class ListParser : public Parser {
public:
	ListParser(Lexer &input);

	void init() override;
	bool element() override;
	bool elements() override;
	bool _list() override;
	bool match(int x) override;
	void consume() override;
};


class BackTrackParser : public ListParser {
public:
	BackTrackParser(Lexer &input);

	bool speculate_stat_alt1();
	bool speculate_stat_alt2();
	Token LT(int i);
	int LA(int i);
	bool match(int x) override;
	void sync(int i);
	void fill(int n);
	void consume() override;
	int mark();
	void release();
	void pop();
	void seek(int index);
	bool isSpeculating();
	void clear_buff();

	void init() override;

	bool stat();
	bool match_stat();

	bool assign();
	bool match_assign();

	bool elements() override;
	bool match_elements();
	
	bool element() override;
	bool match_element();

	bool _list() override;
	bool match_list();
	bool match_list_new();

	bool already_parsed_rule(const std::unordered_map<int, int> &memo);
	void memorize(std::unordered_map<int, int> &memo, int index, bool failed);
	void clear_memo();

private:
	std::vector<int> markers;
	std::vector<Token> buff;
	int p = 0;
	std::unordered_map<int, int> list_memo;
	static const int FAILED;
};
