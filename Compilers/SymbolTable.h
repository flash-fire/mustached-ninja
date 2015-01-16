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
	int* addEntry(std::string name, bool* isErr, std::string* err);
	bool hasEntry(std::string name); 
	SymbolTableEntry get(std::string name);
	std::string getID(Token t);	// returns name of ID if exists in table.
	bool addType(std::string name, Type::TYPE t, std::string* errorMsg);
	bool addAddr(std::string name, int addr, std::string* errorMsg);
	std::map<std::string, SymbolTableEntry> table;
};

#endif