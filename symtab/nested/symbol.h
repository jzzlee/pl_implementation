#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;


struct Type 
{
	virtual const string& get_name() const = 0;
};

class Scope;
class Symbol
{
public:
	Symbol() = delete;
	Symbol(const string &name);
	Symbol(string &&name);
	Symbol(const string &name, shared_ptr<Type> type);
	Symbol(string &&name, shared_ptr<Type> type);
	virtual ~Symbol();

	const string& symbol_name() const;
	const string& to_string() const;
	void set_scope(shared_ptr<Scope> sp);

protected:
	string name;
	weak_ptr<Type> type;
	weak_ptr<Scope> scope;
};

class VarSymbol : public Symbol
{
public:
	VarSymbol() = delete;
	VarSymbol(const string &name, shared_ptr<Type> type);
	VarSymbol(string &&name, shared_ptr<Type> type);
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
	virtual shared_ptr<Scope> get_enclosing_scope() const = 0;
	virtual void define(shared_ptr<Symbol> sym) = 0;
	virtual shared_ptr<Symbol> resolve(const string &name) const = 0;
	virtual ~Scope();
};


class MethodSymbol : public Symbol, public Scope, public std::enable_shared_from_this<MethodSymbol>
{
public:
	MethodSymbol() = delete;
	MethodSymbol(const string &name, shared_ptr<Type> type, shared_ptr<Scope> scope);
	MethodSymbol(string &&name, shared_ptr<Type> type, shared_ptr<Scope> scope);
	MethodSymbol(const MethodSymbol&) = delete;
	MethodSymbol& operator=(const MethodSymbol&) = delete;
	~MethodSymbol();

	const string &scope_name() const override;
	shared_ptr<Scope> get_enclosing_scope() const override;
	void define(shared_ptr<Symbol> symbol) override;
	shared_ptr<Symbol> resolve(const string &name) const override;

private:
	vector<std::pair<string, shared_ptr<Symbol>>> orderedArgs;
	shared_ptr<Scope> enclosingScope;
};


class SymbolTable : public Scope
{
public:
	SymbolTable();
	~SymbolTable();

	SymbolTable(const SymbolTable&) = delete;
	SymbolTable& operator=(const SymbolTable&) = delete;

	const string& scope_name() const override;
	shared_ptr<Scope> get_enclosing_scope() const override;
	void define(shared_ptr<Symbol> symbol) override;
	shared_ptr<Symbol> resolve(const string &name) const override;

	const string to_string() const;


private:

	shared_ptr<unordered_map<string, shared_ptr<Symbol>>> symbols;
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
 