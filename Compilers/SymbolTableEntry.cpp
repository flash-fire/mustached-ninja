#include "SymbolTableEntry.h"
#include "MiscFunc.h"
#include"Type.h"

SymbolTableEntry::SymbolTableEntry() : name(std::string()), token(0, 0), type(Type::UNASSIGNED)
{

}

SymbolTableEntry::SymbolTableEntry(std::string name, int tok, int attr) : name(name), token(tok, attr), type(Type::UNASSIGNED)
{
}

SymbolTableEntry::SymbolTableEntry(const SymbolTableEntry& entry) : name(entry.name), token(entry.token), type(entry.type)
{

}

SymbolTableEntry::~SymbolTableEntry()
{
}

bool SymbolTableEntry::addType(Type::TYPE t, std::string* err)
{
	if (type == Type::UNASSIGNED)
	{
		type = t;
		return true; // no error
	}
	else
	{
		*err = "SEM ERROR: " + name + " already has type " + Type::typeToString(type) + ". Attempting to assign type " + Type::typeToString(type);
		return false; 
	}

}