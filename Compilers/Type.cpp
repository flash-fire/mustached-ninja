#include "Type.h"

Type::Type() : type(UNASSIGNED)
{
}

Type::Type(TYPE t) : type(t)
{
}

Type::Type(const Type& t) : type(t.type)
{
}

Type::~Type()
{
}

bool Type::isArrayType(TYPE t)
{
	return t == AINT || t == AREAL;
}

bool Type::isSame(Type a, Type b)
{
	return a.type == b.type;
}