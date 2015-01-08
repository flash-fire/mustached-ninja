#ifndef __WRAPPER_H_INCLUDED__
#define __WRAPPER_H_INCLUDED__

#include "Token.h"
#include "ParseNode.h"

class Wrapper
{
public:
	Wrapper(Token* tok);
	Wrapper(ParseNode* node);
	~Wrapper();
	union Val
	{
		Val(Token* tok) : tok(tok) {};
		Val(ParseNode* node) : node(node) {};
		Token* tok;
		ParseNode* node;
	};
	Val val;
	bool isNode;
};

#endif