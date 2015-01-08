#include "Wrapper.h"


Wrapper::Wrapper(Token* tok) : val(tok)
{
}

Wrapper::Wrapper(ParseNode* node) : val(node)
{
}

Wrapper::~Wrapper()
{
}
