#ifndef __SCOPE_H__
#define __SCOPE_H__

#include "SymbolTable.h"
#include <vector>
#include <list>

class Scope
{
public:
	typedef struct VAR_WRAP {
		std::string name;
		Type::TYPE type;
	};
	Scope(Scope* parent, std::string name);
	~Scope();

	Scope* getParent() { return parent; };

	bool addParam(std::string name, Type::TYPE type);
	bool addChild(Scope* child, std::string* err);
	bool addSibling(Scope* sib, std::string* err);
	bool addVar(std::string name, Type::TYPE type, int addr, std::string* err);
	
	bool isVarInScope(std::string name);
	Type::TYPE getTypeOfVar(std::string name, std::string* err);

	bool hasSibling(std::string name);

	bool isProcCallable(std::string name);

	std::string name; // For procedures (since they each have their own scope)
private:


	SymbolTable vars; // For variables. Will also allow to get entries and stuff.
	std::vector<VAR_WRAP> params;
	Scope* parent; // For backtracking in case someone needs to reference parent's variables for whatever reason
	Scope* nextSib;
	Scope* child;

};

#endif

