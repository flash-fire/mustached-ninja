#include "Type.h"
#include <iostream>

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

int Type::typeToInt(TYPE t)
{
	return (int)t;
}

Type::TYPE Type::intToType(int t)
{
	if (t < 0 || t > 7) {
		std::cout << "Invalid int passed into type. WTF?" << t << "You are stupid noob and should be ashamed of yourself, Robbie.\n";
		return Type::ERROR;
	}
	return (TYPE)t;
}