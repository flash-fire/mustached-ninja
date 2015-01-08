#ifndef __PARSE_NODE_H_INCLUDED__
#define __PARSE_NODE_H_INCLUDED__

#include <string>
#include <list>
#include <map>

#include "Token.h"

class ParseNode
{
public:
	struct Wrapper
	{
		Wrapper(Token* tok) : val(tok), isNode(false) {};
		Wrapper(ParseNode* node) : val(node), isNode(true) {};
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

	ParseNode* getParent() { return parent; };
	void setInstance(const int newInstance);
	int getInstance();
	std::string getName();
	void appendChild(const ParseNode* child);

	ParseNode(ParseNode* parent, std::string nt, std::list<std::string> varNames);
	~ParseNode();


private:
	ParseNode(ParseNode* parent, std::string nt, int iteration, std::list<std::string> varNames); // used in add children

	bool locSet(const std::string varName,const int newVal); // Sets a variable. Creates new variable if one doesn't already exist.
	int	locGet(const std::string varName, std::string* errorMsg); // gets a variable. Returns error message if var doesn't exist
	
	bool nonLocSet(const std::string targNT, const int instance, const std::string var, const int val, std::string* errorMsg); // Non local setting can only occur with either parent, or with siblings.
	int nonLocGet(const std::string targNT, const int instance, const std::string var, std::string* errorMsg); // Non local getting can only occur with either parent, or with siblings.
	ParseNode* findChild(const std::string targ, const int instance); // Finds node with said properties [child or parent]

	std::string nt; // Nonterminal name
	int instanceNumber; // Which iteration is this node in the production.  LHS ==> RHS :: LHS is instance 0.  RHS counts from there. 
	ParseNode* parent;
	std::list<std::string> varNames; // This keeps track of variables that can be used. Main use is for debugging purposes
	std::map<std::string, int> vars;
	std::list<Wrapper> children;
};

#endif