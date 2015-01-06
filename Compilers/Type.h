#ifndef __TYPE_H_INCLUDED__
#define __TYPE_H_INCLUDED__

class Type
{
public:
	enum TYPE
	{
		UNASSIGNED, // Initial valud
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
	TYPE type;
};
#endif