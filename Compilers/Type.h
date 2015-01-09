#ifndef __TYPE_H_INCLUDED__
#define __TYPE_H_INCLUDED__

#include <string>
class Type
{
public:
	enum TYPE
	{
		ERROR = 0,
		OLD_ERROR,
		UNASSIGNED, // Initial value
		NONE, // Type that is of nothing
		BOOL, // Boolean
		INT, // Integer
		REAL, // Real
		AINT, // Array integer
		AREAL, // array real		
	};

	Type(const Type& t);
	Type(TYPE t);
	Type();
	~Type();

	static bool isArrayType(TYPE t);
	static bool isSame(Type t1, Type t2);
	static int typeToInt(TYPE t);
	static TYPE promoteToArray(TYPE t);
	static TYPE intToType(int t);
	static std::string typeToString(TYPE t);
	TYPE type;
};
#endif