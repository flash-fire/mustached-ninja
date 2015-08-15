#include "Scope.h"

Scope::Scope(Scope* par, std::string name) : parent(par), nextSib(this), child(NULL), name(name), vars(SymbolTable()), params(std::vector<VAR_WRAP>()) {}

Scope::~Scope() {}


bool Scope::procInScope(std::string targ)
{
	if (name == targ)
	{
		return true;
	}
	return hasSibling(targ) || (parent && parent->hasSibling(targ));
}

std::vector<Scope::VAR_WRAP> Scope::getParamIfSis(std::string targ)
{
	if (name == targ)
	{
		return params;
	}
	Scope* curr = this;
	do
	{
		if (curr->name == targ)
		{
			return curr->params;
		}
		curr = curr->nextSib;
	} while (curr != this);
	return std::vector<Scope::VAR_WRAP>();
}

/*std::vector<Scope::VAR_WRAP> Scope::getParams(std::string targ)
{
	if (false == procInScope(targ))
	{
		return std::vector<Scope::VAR_WRAP>();
	}
	if (name == targ)
	{
		return params;
	}
	if (hasSibling(targ))
	{
		return getParamIfSis(targ);
	}
	return parent->getParams(targ);
} */

std::vector<Scope::VAR_WRAP> Scope::getParams(std::string targ)
{
	if (isProcCallable(targ) == false)
	{
		return std::vector<Scope::VAR_WRAP>();
	}
	Scope* curr = this;
	do
	{
		if (curr->name == targ) // dirty hack! If its void, who cares if the array returned is size 0!
		{
			return curr->params;
		}
		if (curr->child)
		{
			std::vector<Scope::VAR_WRAP> ret = curr->child->getParams(targ);
			if (ret.size() > 0)
			{
				return ret;
			}
		}
		curr = curr->nextSib;
	} while (curr != this);
	return std::vector<Scope::VAR_WRAP>();
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
bool Scope::isVarInScope(std::string name)
{
	return vars.hasEntry(name) || hasParam(name) || (parent && 
		parent->isVarInScope(name));
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

Type::TYPE Scope::getTypeOfVar(std::string name, std::string* err)
{
	if (isVarInScope(name))
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
		{ // null pointer is impossible since I already checked the scope. That's a nice advantage of inneficiency. 
			return parent->getTypeOfVar(name, err);
		}
	}
	else
	{
		*err = "Attemptng to use " + name + " before it was given a type.";
		return Type::ERROR;
	}
}

bool Scope::addParam(std::string name, Type::TYPE type, int addr, std::string* err)
{
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
		*err = "SEMERR: Attempting to add variable " + name + " when it already exists in parameters!";
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
		*err = "SEMERR: Attempting to declare variable " + name + " twice in same scope!";
		return false;
	}
}
bool Scope::deepHasSibling(std::string name)
{
	if (!this) // null check bitches
	{
		return false;
	}
	Scope* curr = this;
	do
	{ 
		if (curr->name == name || (curr->child && curr->child->deepHasSibling(name)))
		{
			return true;
		}
		curr = curr->nextSib;
	} while (curr != this);
	return false;

}

bool Scope::isProcCallable(std::string name)
{
	if (this->deepHasSibling(name))

		// You can call your parents, and any subsequent grandparent of that.  You can call your siblings, and all of your siblings children, and subsequent children
	{
		return true;
	}
	
	// If Shenoi wants it so that you can't call nested children of siblings, uncomment this code, and comment the above if statement
	/*if
	(this->hasSibling(name) && (this->child && this->child->deepHasSibling(name)))
	{
		return true;
	}
	*/
	Scope* curr = this;
	do
	{
		if (curr->name == name)
		{
			return true;
		}
		curr = curr->parent;
	} while (curr != NULL); // Checks if scope is in parents
	if (child == NULL)
	{ 
		return false;
	}
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
	Scope* curr = this;
	while (curr != NULL)
	{
		if (curr->name == sib->name)
		{
			*err = "SEMERR: Attempting to add scope -- " + sib->name + " -- when name already exists in parenting scope!";
			return false;
		}
		curr = curr->parent;
	}
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
