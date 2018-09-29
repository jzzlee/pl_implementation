#pragma once
#include <string>
using std::string;

struct Type 
{
	virtual const string get_name() const = 0;
};

class Symbol
{
public:
	Symbol() = delete;
	Symbol(const string &name, Type *type=nullptr);
	Symbol(string &&name, Type *type=nullptr);
protected:
	string name;
	Type *type;
};

class VarSymbol : public Symbol
{
public:
	VarSymbol() = delete;
	VarSymbol(const string &name, Type *type = nullptr);
	VarSymbol(string &&name, Type *type = nullptr);
};

class BuiltinSymbol : public Symbol, public Type
{
public:
	BuiltinSymbol() = delete;
	BuiltinSymbol(const string &name);
	BuiltinSymbol(string &&name);

	const string get_name() const override;
};

class Scope
{
	virtual const string scope_name() const = 0;
	virtual Scope* get_enclosing_scope() const = 0;
	virtual void define(Symbol *sym) = 0;
	virtual Symbol* resolve(const string &name) = 0;
};

