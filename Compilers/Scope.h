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
		int addr;
	};
	Scope(Scope* parent, std::string name, int lineNum);
	~Scope();

	Scope* getParent() { return parent; };

	static void printScope(Scope* targ, std::ostream* out, int level = 0, bool printSibs = true);

	bool addParam(std::string name, Type::TYPE type, int addr, std::string* err);
	bool addChild(Scope* child, std::string* err);
	bool addSibling(Scope* sib, std::string* err);
	bool addVar(std::string name, Type::TYPE type, int addr, std::string* err);

	std::vector<VAR_WRAP> getParams();

	Type::TYPE getTypeOfVar(std::string name, std::string* err); // Returns the type of a variable.  If not found, err is created, and error is returned.

	Scope* isProcCallable(std::string name); // Returns the called scope if it is callable, null otherwise.

	std::string name; // For procedures (since they each have their own scope)
private:
	Scope* hasSibling(std::string name);
	Scope* hasChildOrSibsRec(std::string name);
	bool hasParam(std::string name);

	SymbolTable vars; // For variables. Will also allow to get entries and stuff.
	int lineNum;
	std::vector<VAR_WRAP> params;
	Scope* parent; // For backtracking in case someone needs to reference parent's variables for whatever reason
	Scope* nextSib;
	Scope* child;

};

#endif

