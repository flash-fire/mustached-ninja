#ifndef __PARSE_NODE_H_INCLUDED__
#define __PARSE_NODE_H_INCLUDED__

#include <string>
#include <list>
#include <map>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "Token.h"

class ParseNode
{
public:
	static const int DEF_INSTANCE = 1;
	static const int DEF_NOT_INSTANCE = -1;

	typedef struct Wrapper {
		Wrapper(Token* tok, int instance) : val(Val(tok)), instanceNum(instance), isNode(false) {};
		Wrapper(ParseNode* node, int instance) : val(Val(node)), instanceNum(instance), isNode(true) {};

		union Val
		{
			Val(Token* tok) : tok(tok) { };
			Val(ParseNode* node) : node(node) {};
			Token* tok;
			ParseNode* node;
		};

		Val val;
		bool isNode;
		int instanceNum;
	} Wrap;

	ParseNode* getParent() { return parent; };
	void appendChild(ParseNode* child, int debugTargInstance = DEF_NOT_INSTANCE);
	void appendToken(Token* child, ParseNode* targ, int debugTargInstance = DEF_NOT_INSTANCE);

	std::vector<Wrap> getChildren() { return children; };

	static void WriteUndecoratedTree(Wrap node, std::ostream* out, int level = 0);
	static void WriteDecoratedTree(Wrap node, std::ostream* out, int level = 0);

	static std::string name(Wrap wrap, bool isRHS = false);

	bool set(const std::string varName, const int newVal); // Sets a variable. Creates new variable if one doesn't already exist.
	int	get(const std::string varName); // gets a variable. Returns error message if var doesn't exist
	ParseNode* findN(const std::string targ); 	// Finds Wrap with that name. Good for retrieving tokens.
	Token* findT(const std::string targ); // Finds child with that name. I should never use this thing.
	Token* findT(int loc); // Finds child at that location. I only use this because otherwise if there's a synerr, findT will break.

	ParseNode(ParseNode* parent, std::string nt, std::vector<std::string> varNames);
	ParseNode(const ParseNode& node);

	~ParseNode();


private:	
	// Please note that nonLocSet also does local sets; however, I for debugging purposes try to avoid using nonLocSet unless necessary
	bool nonLocSet(const std::string targ, const std::string var, const int val, std::string* errorMsg); // Non local setting can only occur with either parent, or with siblings.
	int nonLocGet(const std::string targ, const std::string var, std::string* errorMsg); // Non local getting can only occur with either parent, or with siblings.
	std::string nt; // Nonterminal name
	ParseNode* parent;
	std::vector<std::string> varNames; // This keeps track of variables that can be used. Main use is for debugging purposes
	std::map<std::string, int> vars;
	std::vector<Wrap> children;
};

#endif