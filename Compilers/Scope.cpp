#include "Scope.h"

Scope::Scope(Scope* par, std::string name, int lineNum) : parent(par), nextSib(this), child(NULL), name(name), vars(SymbolTable()), lineNum(lineNum), params(std::vector<VAR_WRAP>()) {}

Scope::~Scope() {}

std::vector<Scope::VAR_WRAP> Scope::getParams()
{
	return params;
}

void Scope::printScope(Scope* targ, std::ostream* os, int level, bool printSibs)
{
	std::string tab = "";
	if (level > 0)
	{
		tab = std::string(level * 2, ' '); // switching level with ' ' is hilariously beepy
		//out = std::string(' ', level);
	}

	if (targ->name == "~~DUMMY~~"){
		goto printShit;
	}
	
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
		*os << tab << "    NAME " << id << " TYPE " << Type::typeToString(entry.type)<< " ADDR " << entry.address << "\n";
	}
	
	if (targ->child)
	{
		printScope(targ->child, os, level + 1, true);
	}
	printShit:
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
	//std::cout << out;
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

// User must check if variable is in scope virst.
// Although technically not required, it made the logic easier.
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
			*err = "This should never happen unless if you called this method without first checking if variable was in scope first.";
			// TODO: Refactor so that everyoen just uses this return value for error checking rather than juts having the outside world perform more work than necessary.
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
		*err = "SEMERR: Attempting to declare variable " + name + " twice in same scope called" + this->name + "!";
		return false;
	}
}

// Treat a procedure scope like a variable in a scope one above the current one.
// In that regard, you can call all of your siblings, and any parent or parent above you.
// Returns the scope of the name that is trying to be called as is resolved by compiler.

// CAN RETURN NULL!

Scope* Scope::isProcCallable(std::string name)
{
	Scope* sib = hasSibling(name);
	if (sib != NULL)
	{
		return sib;
	}
	Scope* parent = this->parent;
	while (parent != NULL)
	{
		if (parent->name == name)
		{
			return parent;
		}
		parent = parent->parent;
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
		if (sib->nextSib != sib)
		{
			*err = "SEMERR: This is really weird, and most definitely shouldn't be happening. Compiler bug! Adding " + sib->name + " to " + name + "\n";
			return false;
		}
		Scope* temp = nextSib; // Yes this reverses the order of the scopes for printing later. Annoying but oh well. Any other way would be more complex.
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