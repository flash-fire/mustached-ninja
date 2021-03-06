#include "SymbolTable.h"
#include "Type.h"

SymbolTable::SymbolTable()
{
}


SymbolTable::~SymbolTable()
{
}


// Returns true if no errors
// Possible errors:
//	1) symbol table already has a type for name [undeclared? I may add a method to add to symbol table WITH type]
//	2) symbol table does not contain name
//	3) If type of t is error, we don't report error as error has already been reported

bool SymbolTable::addType(std::string id, Type::TYPE type, std::string* err)
{
	*err = "";
	std::string* typeErr = &std::string();
	if (table.count(id) == 0)
	{
		*err += "SEM_ERROR: atempting to add type " + Type::typeToString(type) + " to entry " + id + " not in symbol table. ";
		return false;
	}
	else if (table[id].addType(type, typeErr) == false)
	{
		*err += *typeErr;
		return false;
	}
	return true;
}

int* SymbolTable::addEntry(std::string id, bool* isErr, std::string* err)
{
	if (table.count(id) > 0)
	{
		*isErr = true;
		*err = "SEMERROR: trying to initialize an entry more than once. ID in question : " + id;
	}
	SymbolTableEntry* sym = new SymbolTableEntry(id, 42, 0); // Lol this is the most hilariously evil thing ever.
	sym->token.attr = (int)sym;
	table[id] = *sym;
	return (int*) sym;
}

bool SymbolTable::addAddr(std::string id, int addr, std::string* err)
{
	*err = "";
	std::string* typeErr = &std::string();
	if (table.count(id) == 0)
	{
		*err += "SEM_ERROR: atempting to add address " + std::to_string(addr) + " to entry " + id + " not in symbol table. (IMPOSSIBLE ERROR BTW)";
		return false;
	}
	else if (table[id].addAddr(addr, typeErr) == false)
	{
		*err += *typeErr;
		return false;
	}
	return true;
}

bool SymbolTable::hasEntry(std::string id)
{
	return table.count(id) > 0;
}

SymbolTableEntry SymbolTable::get(std::string id)
{
	if (hasEntry(id))
	{
		return table[id];
	}
	else
	{
		return SymbolTableEntry();
	}
}

std::string SymbolTable::getID(Token tok)
{
	for (auto it = table.begin(); it != table.end(); ++it)
	{
		if (it->second.token == tok)
		{
			return it->first;
		}
	}
	return "";
}
