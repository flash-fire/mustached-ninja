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

int* SymbolTable::addEntry(std::string id)
{
	SymbolTableEntry* sym = new SymbolTableEntry(id, 42, 0); // Lol this is the most hilariously evil thing ever.
	sym->token.attr = (int)sym;
	table[id] = *sym;
	return (int*) sym;
}

Token SymbolTable::hasEntry(std::string id)
{
	if (table.count(id) > 0)
	{
		return table[id].token;
	}
	else
	{
		return Token();
	}
}

Token SymbolTable::get(std::string id)
{
	return hasEntry(id);
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
