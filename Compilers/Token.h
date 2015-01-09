#ifndef __TOKEND_H_INCLUDED__
#define __TOKEND_H_INCLUDED__

#include <ostream>
// Probably make a struct of this later.
class Token
{
public:
	Token();
	Token(int token, int attr);
	Token(int token, int attr, std::string lex);
	Token(int token, int attr, std::string lex, int lineNum);
	~Token();
	Token(const Token &tok);
	
	static bool isValid(const Token& t);
	bool isValid();
	std::string gram();
	bool isInt();
	int val();

	friend std::ostream &operator<<(std::ostream& out, const Token &tok)
	{
		return out << "(" << tok.token << " , " << tok.attr << ")";
	}


	friend bool operator<(const Token& foo, const Token& foo1)
	{ 
		if (foo.token < foo1.token) 
			return true;
		if (foo.token == foo1.token)
		{
			if (foo.attr < foo1.attr)
				return true;
		}
		return false;
	}

	friend bool operator>(const Token& foo, const Token& foo1) { 
		if (foo.token > foo1.token)
			return true;
		if (foo.token == foo1.token)
		{
			if (foo.attr > foo1.attr)
			{
				return true;
			}
		}
		return false;
	}


	friend bool operator==(const Token& foo, const Token& foo1) {
		return foo.token == foo1.token && foo.attr == foo1.attr;
	}
	int token;
	int attr;
	int line;
	int charNum;
	std::string lex;
	//std::string lexeme;
};


#endif