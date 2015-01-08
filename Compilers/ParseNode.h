#ifndef __PARSE_NODE_H_INCLUDED__
#define __PARSE_NODE_H_INCLUDED__

#include <string>
#include <list>
#include <map>
#include <fstream>

#include "Token.h"

class ParseNode
{
public:
	static const int DEF_INSTANCE = 1;
	static const int DEF_NOT_INSTANCE = -1;
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
	void appendChild(ParseNode* child, int debugTargInstance = DEF_NOT_INSTANCE);
	std::list<Wrapper> getChildren() { return children; };

	static void WriteUndecoratedTree(ParseNode* node, std::ofstream* out, int level = 0);
	
	ParseNode(ParseNode* parent, std::string nt, std::list<std::string> varNames);
	~ParseNode();


private:
	bool locSet(const std::string varName,const int newVal); // Sets a variable. Creates new variable if one doesn't already exist.
	int	locGet(const std::string varName, std::string* errorMsg); // gets a variable. Returns error message if var doesn't exist
	
	// Please note that nonLocSet also does local sets; however, I for debugging purposes try to avoid using nonLocSet unless necessary
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