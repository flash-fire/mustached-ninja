#include "Token.h"
#include "MiscFunc.h"

Token::Token() : token(0), attr(0), line(0), lex(""), charNum(0)
{ // This shouldn't be fucking necessary. But I get the weirdest compile error without it.
}


Token::Token(int tok, int attribute) : token(tok), attr(attribute), line(0), charNum(0)
{

}

Token::Token(int tok, int attribute, std::string lexeme) : token(tok), attr(attribute), lex(lexeme), line(0), charNum(0)
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

bool Token::isInt()
{
	return token == NUM && attr == 0;
}

bool Token::isReal()
{
	return token == NUM && attr == 1;
}

int Token::val()
{
	if (isInt())
	{
		return std::stoi(lex);
	}
	return 9001; // arbitrary number chosen to designate an error. Never will be used so I don't really care.
}