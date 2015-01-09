#ifndef __TYPE_H_INCLUDED__
#define __TYPE_H_INCLUDED__

class Type
{
public:
	enum TYPE
	{
		ERROR = 0,
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
	TYPE type;
};
#endif