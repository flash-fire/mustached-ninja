#include "ReservedWord.h"


ReservedWord::ReservedWord(std::string name, Token tok) : lex(name), token(tok)
{
}

ReservedWord::ReservedWord(std::string name, int tok, int attr) : lex(name), token(tok, attr)
{
}

ReservedWord::ReservedWord(const ReservedWord&res) : lex(res.lex), token(res.token)
{
}

ReservedWord::~ReservedWord()
{
}
