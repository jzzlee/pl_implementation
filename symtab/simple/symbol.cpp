#include "symbol.h"

Symbol::Symbol(const string &name, Type *type=nullptr)
	: name(name), type(type) { }

Symbol::Symbol(string &&name, Type *type=nullptr)
	: name(name), type(type) {}

VarSymbol::VarSymbol(const string &name, Type *type=nullptr)
	: Symbol(name, type) { }

VarSymbol::VarSymbol(string &&name, Type *type=nullptr)
	: Symbol(name, type) { }

BuiltinSymbol::BuiltinSymbol(const string &name)
	:Symbol(name) { }

BuiltinSymbol::BuiltinSymbol(string &&name)
	:Symbol(name) {}

const string BuiltinSymbol::get_name() const
{
	return name;
}
