#include "ParseNode.h"
#include<algorithm>
#include<iostream>
#include "Type.h"

ParseNode::ParseNode(ParseNode* par, std::string nonTerminal, std::vector<std::string> varNames) : parent(par), nt(nonTerminal), varNames(varNames)
{
	children = std::vector<Wrap>();
	for (auto& it : varNames) {
		set(it, 0);
	}
}

ParseNode::ParseNode(const ParseNode& node) : parent(node.parent), nt(node.nt), varNames(node.varNames), children(node.children)
{

}

ParseNode::~ParseNode()
{
}

std::string ParseNode::name(Wrap wrap, bool isRHS)
{
	std::string str = "";
	if (wrap.isNode)
	{
		if (isRHS)
		{
			return wrap.val.node->nt + "_" + std::to_string(DEF_INSTANCE);
		}
		else
		{
			str += wrap.val.node->nt + "_" + std::to_string(wrap.instanceNum);
			return str;
		}
	}
	else
	{
		str += wrap.val.tok->gram() + "_" + std::to_string(wrap.instanceNum);
		return str;
	}
}

void ParseNode::WriteUndecoratedTree(Wrap wrap, std::ostream* os, int level)
{
	std::string out = "";
	if (level > 0)
	{
		out = std::string(level*2, ' '); // switching level with ' ' is hilariously beepy
		//out = std::string(' ', level);
	}
	if (wrap.isNode)
	{
		out += "<" + ParseNode::name(wrap) + ">\n";
	}
	else
	{
		out += ParseNode::name(wrap) + " : " + wrap.val.tok->lex + "\n";
	}
	*os << out;
	std::cout << out;

	if (wrap.isNode)
	{
		ParseNode* node = wrap.val.node;
		for (auto& wrap : node->getChildren())
		{
			WriteUndecoratedTree(wrap, os, level + 1);
		}
	}
}

static int debug = 0;
// The purpose of this method and the write decorated tree method is to debug any of my decorated grammar.
void ParseNode::WriteDecoratedTree(Wrap wrap, std::ostream* os, int level)
{
	std::string out = "";
	std::string tab = "";
	if (level > 0)
	{
		tab = std::string(level * 2, ' '); // switching level with ' ' is hilariously beepy
		//out = std::string(' ', level);
	}



	if (wrap.isNode)
	{
		ParseNode* node = wrap.val.node;
		out += tab + "<" + ParseNode::name(wrap) + ">\n";

		if (node->varNames.empty() == false)
		{
			out += tab + "  " + "<<VARS>>\n";
			for (std::string var : node->varNames)
			{
				std::string err = "";
				if (var == "t" || var == "i")
				{
					auto checkName = ParseNode::name(wrap);
					Type::TYPE check = Type::intToType(node->get(var));
					std::string check2 = Type::typeToString(check);
					if (check == Type::ERROR)
					{
						debug++;
						//std::cout << " DBUG CHECK #$#$" << debug << "\n"; // Used when a seemingly valid program fails to count the failures.
					}
					out += tab + "   " + "<<" + var + ">> : " + check2 + "\n";
				}
				else
					out += tab + "   " + "<<" + var + ">> : " + std::to_string(node->get(var)) + "\n";
				if (err != "")
				{
					std::cout << err;
				}
			}
		}
		*os << out;

		for (auto& wrap : node->getChildren())
		{
			WriteDecoratedTree(wrap, os, level + 1);
		}
	}
	else
	{
		//out += tab + ParseNode::name(wrap) + "\n";
		out += tab + ParseNode::name(wrap) + " : " + wrap.val.tok->lex + "\n";
		*os << out;
	}
}


void ParseNode::appendChild(ParseNode* child, int debugTargInstance)
{
	int instanceFound = ParseNode::DEF_INSTANCE;
	if (child->nt == nt)
	{
		instanceFound++;
	}
	if (children.empty())
	{
		children.push_back(ParseNode::Wrap(child, instanceFound));
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		Wrap wrap = *it;
		if (wrap.isNode)
		{
			ParseNode* curr = wrap.val.node;
			if (curr->nt == child->nt)
			{
				instanceFound++;
			}
		}
	}

	if (debugTargInstance != ParseNode::DEF_NOT_INSTANCE)
	{
		if (debugTargInstance != instanceFound)
		{
			std::cout << "[appen child] Wrong target instance found when appending a child. This is obviously so bad that everyone will die brutally.\n"
				<< "Iter Found: " << instanceFound << " iter desired " << debugTargInstance;
		}
	}

	//child->setInstance(instanceFound);
	children.push_back(Wrap(child, instanceFound));
}

// Because I preinitialize my variables in my nodes [to debug], I need to add the nodes first. This finds where the tokens should be in any case. YAY NONLINeAR!
void ParseNode::appendToken(Token* tok, ParseNode* targ, int debugTargInstance)
{
	auto endLoc = children.begin(); // If target node isn't found, we are in current node -- add to beginning. Simple enough
	int instance = DEF_INSTANCE;

	for (auto it = children.begin(); it != children.end(); ++it) {
		// if the current index is needed:
		Wrap curr = *it;
		if (curr.isNode)
		{
			ParseNode* temp = curr.val.node;

			if (curr.val.node == targ)
			{
				endLoc = it;
				if (endLoc != children.end())
				{
					endLoc++;
				}
				break;
			}
		}
		else
		{
			if (curr.val.tok->gram() == tok->gram())
			{
				instance += 1;
			}
		}
	}

	
	// This makes sure the token is insert right before the last node entered.
	// Since tokens come in order, you can think of this as fixing the offset of the token w/r2 the target node.
	while (endLoc != children.end() && endLoc->isNode == false)
	{
		++endLoc;
	} // Finds last inserted token

	children.insert(endLoc, Wrap(tok, instance));
}

// Error occurred if returns false
bool ParseNode::set(const std::string varName, const int newVal)
{
	if (std::find(std::begin(varNames), std::end(varNames), varName) == std::end(varNames))
	{
		std::string err = "Cannot find variable " + varName + " in <<";
		for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
			err += (" " + iter->first);
		}
		err += ">> for nt : <" + nt + ">.\n";
		std::cout << err;
		return false;
	}
	vars[varName] = newVal;
	return true;
}
// gets a variable. Returns error message if var doesn't exist
int	ParseNode::get(const std::string varName)
{
	if (vars.count(varName) != 0)
	{
		return vars[varName];
	}
	else
	{
		std::string errorMsg = "Cannot find variable " + varName + " in ";
		for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
			errorMsg += (" " + iter->first);
		}
		errorMsg += errorMsg + "\n";
		std::cout << errorMsg;
		return 0;
	}
}

// Non local setting can only occur with either parent, or with siblings.
bool ParseNode::nonLocSet(const std::string targNT, const std::string var, const int val, std::string* errorMsg)
{
	ParseNode* node = findN(targNT);
	return node && node->set(var, val);
}

int ParseNode::nonLocGet(const std::string targNT, const std::string var, std::string* errorMsg)
{
	ParseNode* targ = findN(targNT);
	return targ && targ->get(var);
}

ParseNode* ParseNode::findN(const std::string targ)
{
	if (ParseNode::name(Wrap(this, DEF_INSTANCE)) == targ)
	{
		return this;
	}

	for (auto& wrap : children)
	{
		if (ParseNode::name(wrap) == targ)
		{
			if (wrap.isNode)
			{
				//std::cout << "Found " << targ << "!!";
				return wrap.val.node;
			}
			else
			{
				std::cout << "Found wrong type [token instead of node] for " << targ;
			}
		}
	}
	std::cout << "Find [node] failed for " << targ;
	return NULL;
}


// Yeah it's unsafe, but I don't feel like loaading the grammar the proper way and supporting indexing.
Token* ParseNode::findT(int loc)
{
	if (children.size() <= loc)
	{
		std::cout << "FINDT: Fail. We are OOB";
		return NULL;
	}
	Wrap targ = children[loc];
	if (targ.isNode)
	{
		std::cout << "FINDT: Fail. We found a node you idiot.";
	}
	return targ.val.tok;
}

Token* ParseNode::findT(const std::string targ)
{
	if (ParseNode::name(Wrap(this, DEF_INSTANCE)) == targ)
	{
		std::cout << "Find [token] tried to match current node.";
		return NULL;
	}

	for (auto& wrap : children)
	{
		std::string test = ParseNode::name(wrap);
		if (test == targ)
		{
			if (!wrap.isNode)
			{
				//std::cout << "Found " << targ << "!!";
				return wrap.val.tok;
			}
			else
			{
				std::cout << "Found wrong type [node instead of token] for " << targ;
			}
		}
	}
	std::cout << "Find [token] failed for " << targ;

	return NULL;
}