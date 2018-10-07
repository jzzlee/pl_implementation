#include <sstream>
#include "symbol.h"

using std::stringstream;


Symbol::Symbol(const string &name, Type *type=nullptr)
	: name(name), type(type) { }

Symbol::Symbol(string &&name, Type *type=nullptr)
	: name(name), type(type) {}

Symbol::~Symbol()
{
	delete type;
	type = nullptr;
}

const string Symbol::symbol_name() const
{
	return name;
}

const string Symbol::to_string() const
{
	if (type)
		return "<" + symbol_name() + ": " + type->get_name() + ">";
	else
		return symbol_name();
}

VarSymbol::VarSymbol(const string &name, Type *type=nullptr)
	: Symbol(name, type) { }

VarSymbol::VarSymbol(string &&name, Type *type=nullptr)
	: Symbol(name, type) { }

BuiltinSymbol::BuiltinSymbol(const string &name)
	:Symbol(name) {}

BuiltinSymbol::BuiltinSymbol(string &&name)
	:Symbol(name) {}

const string BuiltinSymbol::get_name() const
{
	return name;
}

SymbolTable::SymbolTable()
	: symbols(new unordered_map<string, Symbol*>)
{
	init_type_system();
}

SymbolTable::~SymbolTable()
{
	for (auto &ele : *symbols)
	{
		delete ele.second;
	}
	delete symbols;
}

void SymbolTable::init_type_system()
{

}

const string SymbolTable::scope_name() const
{
	return "global";
}

Scope* SymbolTable::get_enclosing_scope() const
{
	return nullptr;
}

void SymbolTable::define(Symbol *sym)
{
	symbols->insert(std::make_pair(sym->symbol_name(), sym));
}

Symbol* SymbolTable::resolve(const string &name) const
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


BuiltinSymbolTable* BuiltinSymbolTable::_instance = new BuiltinSymbolTable();

BuiltinSymbolTable::BuiltinSymbolTable()
	: SymbolTable() {}

const string BuiltinSymbolTable::scope_name() const
{
	return "builtin";
}

BuiltinSymbolTable* BuiltinSymbolTable::instance()
{
	if (_instance == nullptr)
		_instance = new BuiltinSymbolTable();
	return _instance;
}

void BuiltinSymbolTable::init_type_system()
{
	define(new BuiltinSymbol("int"));
	define(new BuiltinSymbol("float"));
}
