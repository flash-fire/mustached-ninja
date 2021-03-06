
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
	if (t < 0 || t > Type::NUM_TYPES) {
		std::cout << "Invalid int passed into type. WTF?" << t << "You are stupid noob and should be ashamed of yourself, Robbie.\n";
		return Type::ERROR;
	}
	return (TYPE)t;
}

Type::TYPE Type::promoteToArray(TYPE t)
{
	if (t == REAL)
	{
		return Type::AREAL;
	}
	if (t == INT)
	{
		return Type::AINT;
	}
	return Type::ERROR;
}

std::string Type::typeToString(TYPE t)
{
	switch (t){
	case NONE:
		return "NONE";
	case INT:
		return "INT";
	case REAL:
		return "REAL";
	case AINT:
		return "ARRAY_INT";
	case AREAL:
		return "ARRAY_REAL";
	case BOOL:
		return "BOOLEAN";
	case UNASSIGNED:
		return "UNASSIGNED";
	case ERROR:
		return "ERROR";
	case OLD_ERROR:
		return "ERROR*";
	case IGNORE:
		return "IgnoreExprCheck";
	default:
		return "ERROR?";
	}
}