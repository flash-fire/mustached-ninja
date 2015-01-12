#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <map>
#include "SymbolTableEntry.h"
#include "Type.h"
class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();
	int* addEntry(std::string name);
	Token hasEntry(std::string name); // returns invalid token if table does not contain string.
	Token get(std::string name);	  // Same as hasEntry. Lol
	std::string getID(Token t);	// returns name of ID if exists in table.
	bool addType(std::string name, Type::TYPE t, std::string* errorMsg);
	std::map<std::string, SymbolTableEntry> table;
};

#endif