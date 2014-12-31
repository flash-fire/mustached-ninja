#include "Token.h"
#include "MiscFunc.h"

Token::Token() : token(0), attr(0), line(0), lex("")
{ // This shouldn't be fucking necessary. But I get the weirdest compile error without it.
}


Token::Token(int tok, int attribute) : token(tok), attr(attribute), line(0)
{

}

Token::Token(int tok, int attribute, std::string lexeme) : token(tok), attr(attribute), lex(lexeme), line(0)
{

}

Token::Token(int tok, int attribute, std::string lexeme, int lineNum) : token(tok), attr(attribute), line(lineNum), lex(lexeme)
{

}

Token::Token(const Token& tok) : token(tok.token), attr(tok.attr), line(tok.line), lex(tok.lex), charNum(tok.charNum)
{
}

Token::~Token()
{
}

std::string Token::gram()
{
	if (token == ID)
	{
		return "id";
	}
	if (token == NUM)
	{
		return "num";
	}
	if (token == EOF)
	{
		return "$";
	}
	if (token == 32)
	{
		return "mulop";
	}
	if (token == 31)
	{
		return "addop";
	}
	if (token == 30)
	{
		return "relop";
	}
	if (token == 33)
	{
		return "assignop";
	}
	return lex;
}

bool Token::isValid(const Token& tok)
{
	return tok.attr != 0 || tok.token != 0;
}

bool Token::isValid()
{
	return attr != 0 || token != 0;
}