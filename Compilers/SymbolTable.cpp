#include "SymbolTable.h"


SymbolTable::SymbolTable()
{
}


SymbolTable::~SymbolTable()
{
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

