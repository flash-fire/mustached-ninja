#include "Scope.h"
#include <cassert>

Scope::Scope(Scope* par, std::string name, int lineNum) : parent(par), nextSib(this), child(NULL), name(name), vars(SymbolTable()), lineNum(lineNum), params(std::vector<VAR_WRAP>()) {}

Scope::~Scope() {}

void Scope::printScope(Scope* targ, std::ostream* os, int level, bool printSibs)
{
	if (targ == NULL)
	{ // Happens when the file does not exist.
		return;
	}
	std::string tab = "";
	if (level > 0)
	{
		tab = std::string(level * 2, ' '); // switching level with ' ' is hilariously beepy
	}

	if (targ->name != "~~DUMMY~~")
	{
		*os << tab << targ->name << "\n";
		*os << tab << "  LINE NUM :: " << targ->lineNum << "\n";
		*os << tab << "  PARAMS:\n";
		if (targ->params.size() == 0)
		{
			*os << tab << "    VOID\n";
		}
		for (VAR_WRAP wrap : targ->params)
		{
			*os << tab << "    NAME " << wrap.name << " TYPE " << Type::typeToString(wrap.type) << " ADDR " << wrap.addr << "\n";
		}

		*os << tab << "  LOCAL_VARS:\n";
		for (auto x : targ->vars.table)
		{
			std::string id = x.first;
			SymbolTableEntry entry = x.second;
			*os << tab << "    NAME " << id << " TYPE " << Type::typeToString(entry.type) << " ADDR " << entry.address << "\n";
		}

		if (targ->child)
		{
			printScope(targ->child, os, level + 1, true);
		}
	}
	if (printSibs)
	{
		Scope* curr = targ->nextSib;
		std::string targName = targ->name;
		std::string currName = curr->name;
		while (targName != currName)
		{
			printScope(curr, os, level, false);
			curr = curr->nextSib;
			currName = curr->name;
			targName = targ->name;
		}
	}
	os->flush();
}

bool Scope::hasParam(std::string name)
{
	for (VAR_WRAP wrap : params)
	{
		if (wrap.name == name)
			return true;
	}
	return false;
}

std::vector<Scope::VAR_WRAP> Scope::getParams()
{
	return params;
}

// Returns type of variable or error if not in scope with err message.
Type::TYPE Scope::getTypeOfVar(std::string name, std::string* err)
{
	if (hasParam(name))
	{
		for (VAR_WRAP wrap : params)
		{
			if (wrap.name == name)
			{
				return wrap.type;
			}
		}
		return Type::ERROR;
	}
	else if (vars.hasEntry(name))
	{
		SymbolTableEntry e = vars.get(name);
		return e.type;
	}
	else
	{
		if (parent == NULL)
		{
			*err = "SEMERR: Variable " + name + " desired but is not currently initialized";
			return Type::ERROR;
		}
		return parent->getTypeOfVar(name, err);
	}
}

bool Scope::addParam(std::string name, Type::TYPE type, int addr, std::string* err)
{
	// Don't need to check symbol table since the params occur BEFORE variable initialization
	if (hasParam(name))
	{
		*err = "SEMERR: Attempting to add parameter " + name + " when it already exists in parameters!";
		return false;
	}
	VAR_WRAP wrap = { name, type, addr };
	params.push_back(wrap);
	return true;
}

bool Scope::addVar(std::string name, Type::TYPE type, int addr, std::string* err)
{
	if (hasParam(name))
	{
		*err = "SEMERR: Attempting to add variable " + name + " when it already exists in parameters in scope " + this->name;
		return false;
	}
	else if (!vars.hasEntry(name))
	{
		bool isErr = false;
		vars.addEntry(name, &isErr, err);
		assert(isErr == false);
		return vars.addType(name, type, err) && vars.addAddr(name, addr, err);
	}
	else
	{
		*err = "SEMERR: Attempting to declare variable " + name + " twice in same scope called" + this->name + "!";
		return false;
	}
}

// Treat a procedure scope like a variable in a scope one above the current one.
// In that regard, you can call yourself and all of your children that have been declared.
// IF having yourself one scope higher includes all other procedures of same level, simply call hasChildOrSibsRec directly.
Scope* Scope::isProcCallable(std::string name)
{
	if (this->name == name)
	{
		return this;
	}

	if (child)
	{
		return child->hasChildOrSibsRec(name);
	}
	return NULL;
}

// Used to instantiate a dummy node into a scope.
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
	*err = "";
	Scope* curr = this;
	while (curr != NULL)
	{
		if (curr->name == sib->name)
		{
			*err = "WARNING: Attempting to add scope -- " + sib->name + " -- when name already exists in parenting scope!\nNew scope is on line " 
				+ std::to_string(sib->lineNum) + " conflicting scope is line " + std::to_string(this->lineNum) + "\n";
			break; // Technically this isn't an error, but who would want to do this?!!
		}
		curr = curr->parent;
	}
	
	if (hasSibling(sib->name) == NULL)
	{
		Scope* temp = nextSib;
		nextSib = sib;
		sib->nextSib = temp;
		return true;
	}
	else
	{
		*err = "SEMERR: Attempting to add scope -- " + sib->name + " -- when name already exists as a sibling in same level!\n"
			+ "New sibling is on line " + std::to_string(sib->lineNum);
		return false;
	}
}

Scope* Scope::hasSibling(std::string name)
{
	Scope* curr = this;
	do
	{
		if (curr->name == name)
		{
			return curr;
		}
		curr = curr->nextSib;
	} while (this != curr);

	return NULL;
}

Scope* Scope::hasChildOrSibsRec(std::string name)
{
	if (this == NULL)
	{
		return NULL;
	}
	Scope* curr = this;
	do
	{
		if (curr->name == name)
		{
			return curr;
		}
		else if (curr->child)
		{
			Scope* nested = curr->child->hasChildOrSibsRec(name);
			if (nested) return nested;
		}
		curr = curr->nextSib;
	} while (curr != this);
	return NULL;
}