#include "ParseNode.h"
#include<algorithm>
#include<iostream>

ParseNode::ParseNode(ParseNode* par, std::string nonTerminal, std::vector<std::string> varNames) : parent(par), nt(nonTerminal), varNames(varNames)
{
	children = std::list<Wrap>();
	for (auto& it : varNames) {
		locSet(it, 0);
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

void ParseNode::WriteUndecoratedTree(Wrap wrap, std::ofstream* fileToWrite, int level)
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
		out += ParseNode::name(wrap) + "\n";
	}
	*fileToWrite << out;
	//std::cout << out;

	if (wrap.isNode)
	{
		ParseNode* node = wrap.val.node;
		for (auto& wrap : node->getChildren())
		{
			WriteUndecoratedTree(wrap, fileToWrite, level + 1);
		}
	}
}

void ParseNode::WriteDecoratedTree(Wrap wrap, std::ofstream* fileToWrite, int level)
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
				out += tab + "   " + "<<" + var + ">> : " + std::to_string(node->locGet(var)) + "\n";
				if (err != "")
				{
					std::cout << err;
				}
			}
		}
		*fileToWrite << out;

		for (auto& wrap : node->getChildren())
		{
			WriteDecoratedTree(wrap, fileToWrite, level + 1);
		}
	}
	else
	{
		out += tab + ParseNode::name(wrap) + "\n";
		*fileToWrite << out;
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
bool ParseNode::locSet(const std::string varName, const int newVal)
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
int	ParseNode::locGet(const std::string varName)
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
	Wrap targ = find(targNT, errorMsg);
	if (*errorMsg != "")
	{
		return false;
	}
	return targ.isNode & targ.val.node->locSet(var, val);
}

int ParseNode::nonLocGet(const std::string targNT, const std::string var, std::string* errorMsg)
{
	Wrap targ = find(targNT, errorMsg);
	if (*errorMsg != "")
	{
		return false;
	}
	return targ.isNode && targ.val.node->locGet(var);
}

ParseNode::Wrapper ParseNode::find(const std::string targ, std::string* errorMsg)
{
	if (ParseNode::name(Wrap(this, DEF_INSTANCE)) == targ)
	{
		return Wrapper(this, DEF_INSTANCE);
	}

	for (auto& wrap : children)
	{
		if (ParseNode::name(wrap) == targ)
		{
			return wrap;
		}
	}

	*errorMsg += "IN findChild and we have sad news about the finding of our target node.\n";
	return Wrapper(this, DEF_INSTANCE);
}