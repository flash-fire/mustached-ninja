#include "Scope.h"

Scope::Scope(Scope* par, std::string name) : parent(par), nextSib(this), child(NULL), name(name), vars(SymbolTable()) {}

Scope::~Scope() {}

bool Scope::isVarInScope(std::string name)
{
	return vars.hasEntry(name) || (parent && parent->isVarInScope(name));
}

Type::TYPE Scope::getTypeOfVar(std::string name, std::string* err)
{
	if (isVarInScope(name))
	{
		SymbolTableEntry e = vars.get(name);
		return e.type;
	}
	else
	{
		*err = "Attemptng to use " + name + " before it was given a type.";
		return Type::ERROR;
	}
}

bool Scope::addVar(std::string name, Type::TYPE type, int addr, std::string* err)
{
	if (!vars.hasEntry(name))
	{
		bool isErr = false;
		vars.addEntry(name, &isErr, err);
		if (isErr)
		{
			*err += "\nThis should never happen ever. If this happens, your compiler had issues in addVar and it's not your fault. :( :( :(";
			return false;
		} // these things should never return an error. If they return an error in this scope, this program is so broken.. You have no idea!
		return vars.addType(name, type, err) && vars.addAddr(name, addr, err);
	}
	else
	{
		*err = "SEMERR: Attempting to declare variable " + name + " twice in same scope!";
		return false;
	}
}

bool Scope::isProcCallable(std::string name)
{
	return hasSibling(name) || (parent && parent->isProcCallable(name));
}

bool Scope::addChild(Scope* newChild, std::string* err)
{
	if (child == NULL)
	{
		child = newChild;
		return true;
	}
	return this->child->addSibling(newChild, err);
}

bool Scope::addSibling(Scope* sib, std::string* err)
{
	if (!hasSibling(sib->name))
	{
		if (sib->nextSib != sib)
		{
			*err = "SEMERR: This is really weird, and most definitely shouldn't be happening. Compiler bug! Adding " + sib->name + " to " + name;
			return false;
		}
		Scope* temp = nextSib; // Yes this reverses the order of the scopes for printing later. Annoying but oh well. Any other way would be more complex.
		nextSib = sib;
		sib->nextSib = temp;
		return true;
	}
	*err = "SEMERR: Attempting to add scope -- " + sib->name + " -- when name already exists as a sibling in same level!";
	return false;
}

bool Scope::hasSibling(std::string name)
{
	Scope* curr = this;
	do
	{
		if (curr->name == name)
		{
			return true;
		}
		curr = curr->nextSib;
	} while (this != curr);

	return false;
}
