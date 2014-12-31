#ifndef __SYMBOLTABLEENTRY_H_
#define __SYMBOLTABLEENTRY_H_

#include "Token.h"
#include <string>

// Rudimentary entry in the symbol table.
class SymbolTableEntry
{
public:
	SymbolTableEntry();
	SymbolTableEntry(std::string name, int token, int attr);
	
	SymbolTableEntry(const SymbolTableEntry& entry);
	~SymbolTableEntry();

	friend std::ostream &operator<<(std::ostream& out, const SymbolTableEntry& entry)
	{
		return out << entry.name << " : " << entry.token;
	}

	std::string name;
	Token token; // attr/id
private:
};

#endif