#include "SymbolTableEntry.h"
#include "MiscFunc.h"

SymbolTableEntry::SymbolTableEntry() : name(std::string()), token(0,0)
{

}

SymbolTableEntry::SymbolTableEntry(std::string name, int tok, int attr) : name(name), token(tok,attr)
{
}

SymbolTableEntry::SymbolTableEntry(const SymbolTableEntry& entry) : name(entry.name), token(entry.token)
{

}

SymbolTableEntry::~SymbolTableEntry()
{
}
