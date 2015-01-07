#include "SymbolTableEntry.h"
#include "MiscFunc.h"

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
