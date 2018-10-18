#include <sstream>
#include "symbol.h"

using std::stringstream;

Symbol::Symbol(const string &name)
	: name(name) {}

Symbol::Symbol(string &&name)
	: name(name) {}

Symbol::Symbol(const string &name, shared_ptr<Type> type)
	: name(name), type(type) { }

Symbol::Symbol(string &&name, shared_ptr<Type> type)
	: name(name), type(type) {}

Symbol::~Symbol()
{ }

const string& Symbol::symbol_name() const
{
	return name;
}

const string& Symbol::to_string() const
{
	if (!type.expired())
	{
		auto p = type.lock();
		return "<" + symbol_name() + ": " + p->get_name() + ">";
	}
	else
		return symbol_name();
}

void Symbol::set_scope(shared_ptr<Scope> sp)
{
	scope = sp;
}

VarSymbol::VarSymbol(const string &name, shared_ptr<Type> type)
	: Symbol(name, type) { }

VarSymbol::VarSymbol(string &&name, shared_ptr<Type> type)
	: Symbol(name, type) { }

BuiltinSymbol::BuiltinSymbol(const string &name)
	:Symbol(name) {}

BuiltinSymbol::BuiltinSymbol(string &&name)
	:Symbol(name) {}

const string& BuiltinSymbol::get_name() const
{
	return name;
}

Scope::~Scope() { }

MethodSymbol::MethodSymbol(const string &name, shared_ptr<Type> type, shared_ptr<Scope> scope)
	: Symbol(name, type), enclosingScope(scope)
{ }

MethodSymbol::MethodSymbol(string &&name, shared_ptr<Type> type, shared_ptr<Scope> scope)
	: Symbol(name, type), enclosingScope(scope)
{ }

MethodSymbol::~MethodSymbol()
{ }

const string& MethodSymbol::scope_name() const
{
	return name;
}

shared_ptr<Scope> MethodSymbol::get_enclosing_scope() const
{
	return enclosingScope;
}

void MethodSymbol::define(shared_ptr<Symbol> symbol)
{
	auto item = std::make_pair(symbol->symbol_name(), symbol);
	orderedArgs.push_back(item);
	symbol->set_scope(shared_from_this());
}

shared_ptr<Symbol> MethodSymbol::resolve(const string &name) const
{
	for (const auto &ele : orderedArgs)
		if (ele.first == name)
			return ele.second;
	if (enclosingScope)
		return enclosingScope->resolve(name);
	return nullptr;
}

SymbolTable::SymbolTable()
	: symbols(std::make_shared<unordered_map<string, shared_ptr<Symbol>>>())
{ }

SymbolTable::~SymbolTable()
{ }

const string& SymbolTable::scope_name() const
{
	return "global";
}

shared_ptr<Scope> SymbolTable::get_enclosing_scope() const
{
	return nullptr;
}

void SymbolTable::define(shared_ptr<Symbol> symbol)
{
	symbols->insert(std::make_pair(symbol->symbol_name(), symbol));
}

shared_ptr<Symbol> SymbolTable::resolve(const string &name) const
{
	auto iter = symbols->find(name);
	if (iter != symbols->end())
		return iter->second;
	else
		return nullptr;
}

const string SymbolTable::to_string() const
{
	stringstream ss;
	ss << scope_name() << ": ";
	for (auto &ele : *symbols)
		ss << ele.first << ", ";
	ss << std::endl;
	return ss.str();
}


std::unique_ptr<BuiltinSymbolTable> BuiltinSymbolTable::_instance = std::make_unique<BuiltinSymbolTable>();

BuiltinSymbolTable::BuiltinSymbolTable()
	: SymbolTable() 
{
	init_type_system();
}

const string& BuiltinSymbolTable::scope_name() const
{
	return "builtin";
}

std::unique_ptr<BuiltinSymbolTable>& BuiltinSymbolTable::instance()
{
	return _instance;
}

void BuiltinSymbolTable::init_type_system()
{
	define(std::make_shared<BuiltinSymbol>("int"));
	define(std::make_shared<BuiltinSymbol>("float"));
}
