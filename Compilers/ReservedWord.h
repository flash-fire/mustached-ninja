#ifndef __RESERVEDWORD_H_INCLUDED__
#define __RESERVEDWORD_H_INCLUDED__


#include <string>
#include "Token.h"

// Another possible struct later
class ReservedWord
{
public:
	ReservedWord(std::string lexeme, int token, int attr);
	ReservedWord(std::string lexeme, Token t);
	ReservedWord(const ReservedWord &res);
	~ReservedWord();

	std::string lex;
	Token token;
	
};

#endif
