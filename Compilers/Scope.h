#ifndef __SCOPE_H__
#define __SCOPE_H__

#include "SymbolTable.h"
#include <vector>
#include <list>

class Scope
{
public:
	Scope(Scope* parent, std::string name);
	~Scope();

	bool addChild(Scope* child, std::string* err);
	bool addSibling(Scope* sib, std::string* err);
	bool hasSibling(std::string name);
	std::string name();
private:
	std::string name; // For procedures (since they each have their own scope)
	SymbolTable vars; // For variables. Will also allow to get entries and stuff.

	Scope* parent; // For backtracking in case someone needs to reference parent's variables for whatever reason
	Scope* nextSib;
	Scope* child;
};

#endif

