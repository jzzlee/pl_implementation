#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
using std::string;
using std::unordered_map;
using std::vector;


struct Type 
{
	virtual const string& get_name() const = 0;
};

class Scope;
class Symbol
{
public:
	Symbol() = delete;
	Symbol(const string &name, Type *type=nullptr);
	Symbol(string &&name, Type *type=nullptr);
	virtual ~Symbol();

	const string& symbol_name() const;
	const string& to_string() const;
protected:
	string name;
	Type *type;
	std::weak_ptr<Scope> scope;
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

	const string& get_name() const override;
};

class Scope
{
public:
	virtual const string &scope_name() const = 0;
	virtual Scope* get_enclosing_scope() const = 0;
	virtual void define(Symbol *sym) = 0;
	virtual Symbol* resolve(const string &name) const = 0;
	virtual ~Scope();
};


class MethodSymbol : public Symbol, public Scope
{
public:
	MethodSymbol() = delete;
	MethodSymbol(const string &name, Type *type, Scope *scope);
	MethodSymbol(string &&name, Type *type, Scope *scope);
	MethodSymbol(const MethodSymbol&) = delete;
	MethodSymbol& operator=(const MethodSymbol&) = delete;
	~MethodSymbol();

	const string &scope_name() const override;
	Scope* get_enclosing_scope() const override;
	void define(Symbol* symbol) override;
	Symbol* resolve(const string &name) const override;

private:
	vector<std::pair<string, Symbol*>> orderedArgs;
	Scope *enclosingScope;
};


class SymbolTable : public Scope
{
public:
	SymbolTable();
	~SymbolTable();

	SymbolTable(const SymbolTable&) = delete;
	SymbolTable& operator=(const SymbolTable&) = delete;

	const string& scope_name() const override;
	Scope* get_enclosing_scope() const override;
	void define(Symbol *sym) override;
	Symbol* resolve(const string &name) const override;

	const string to_string() const;


private:

	unordered_map<string, Symbol*> *symbols;
};

class BuiltinSymbolTable : public SymbolTable
{
public:
	BuiltinSymbolTable();
	BuiltinSymbolTable(const BuiltinSymbolTable&) = delete;
	BuiltinSymbolTable& operator=(const BuiltinSymbolTable&) = delete;

	const string& scope_name() const override;

	static std::unique_ptr<BuiltinSymbolTable>& instance();

protected:
	void init_type_system();

private:
	static std::unique_ptr<BuiltinSymbolTable> _instance;
};
 