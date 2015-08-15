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
	bool hasParam(std::string name);
	
	std::vector<VAR_WRAP> getParamIfSis(std::string name);

	bool procInScope(std::string name);
	std::vector<VAR_WRAP> getParams(std::string name);
	/*int numParams() { return params.size(); };
	Type::TYPE paramType(int i, std::string* err) {
		if (i > numParams())
		{
			*err = "Too many parameters inputted into procedure " + name;
			return Type::ERROR;
		}
		return params.at(i).type;
	};*/

	bool isVarInScope(std::string name);
	Type::TYPE getTypeOfVar(std::string name, std::string* err);

	bool hasSibling(std::string name);
	bool deepHasSibling(std::string name); // like has sibling but deeper

	bool isProcCallable(std::string name);

	std::string name; // For procedures (since they each have their own scope)
private:


	SymbolTable vars; // For variables. Will also allow to get entries and stuff.
	int lineNum;
	std::vector<VAR_WRAP> params;
	Scope* parent; // For backtracking in case someone needs to reference parent's variables for whatever reason
	Scope* nextSib;
	Scope* child;

};

#endif

