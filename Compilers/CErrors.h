#ifndef _SERRORS_H_
#define _SERRORS_H_

#include "Token.h"
#include <map>
#include <string>

class CErrors
{
public:
	enum ERROR_TYPES
	{
		LEXERR = 400
	};

	enum LEXERR
	{
		LINE_TOO_LONG,
		ID_TOO_LONG,
		HAS_LEADING_0,
		HAS_ENDING_0,
		INT_TOO_LONG,
		DEC_TOO_LONG,
		EXP_TOO_LONG,
		CAN_NOT_MATCH
	};
	typedef std::map<Token, std::string> OpMap;
	static OpMap errors;
};
#endif