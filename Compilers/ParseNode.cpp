#include "ParseNode.h"
#include<algorithm>
#include<iostream>

ParseNode::ParseNode(ParseNode* par, std::string nonTerminal, std::list<std::string> varNames) : parent(par), nt(nonTerminal), instanceNumber(0), varNames(varNames)
{
	for (auto& it : varNames) {
		locSet(it, 0);
	}
}

ParseNode::ParseNode(ParseNode* par, std::string nt, int instanceNum, std::list<std::string> varNames) : parent(par), nt(nt), instanceNumber(instanceNum), varNames(varNames)
{
	for (auto& it : varNames) {
		locSet(it, 0);
	}
}

ParseNode::~ParseNode()
{
}

int ParseNode::getInstance()
{
	return instanceNumber;
}

void ParseNode::setInstance(const int newInstance)
{
	instanceNumber = newInstance;
}

std::string ParseNode::getName()
{
	return nt;
}

// Error occurred if returns false
bool ParseNode::locSet(const std::string varName, const int newVal)
{
	if (std::find(std::begin(varNames), std::end(varNames), varName) == std::end(varNames))
	{
		return false;
	}
	vars[varName] = newVal;
	return true;
}
// gets a variable. Returns error message if var doesn't exist
int	ParseNode::locGet(const std::string varName, std::string* errorMsg)
{
	if (vars.count(varName) != 0)
	{
		return vars[varName];
	}
	else
	{
		*errorMsg = "Cannot find variable " + varName + " in ";
		for (auto iter = vars.begin(); iter != vars.end(); ++iter) {
			*errorMsg = *errorMsg + (" " + iter->first);
		}
		*errorMsg = *errorMsg + "\n";
		return 0;
	}
}

// Non local setting can only occur with either parent, or with siblings.
bool ParseNode::nonLocSet(const std::string targNT, const int instance, const std::string var, const int val, std::string* errorMsg)
{
	ParseNode* targ = findChild(targNT, instance);
	if (targ) // not nullzies
	{
		return targ->locSet(var, val);
	}
	else
	{
		*errorMsg = "[nonLocSET] Cannot find variable " + var + " in " + targNT + " or in parents/siblings. In other words :( .\n";
		return false;
	}
}

int ParseNode::nonLocGet(const std::string targNT, const int instance, const std::string var, std::string* errorMsg)
{
	ParseNode* targ = findChild(targNT, instance);
	if (targ) // not nullzies
	{
		return targ->locGet(var, errorMsg);
	}
	else
	{
		*errorMsg = "[nonLocGET] Cannot find variable " + var + " in " + targNT + " or in parents/siblings. In other words :(\n";
		return 0;
	}
}

ParseNode* ParseNode::findChild(const std::string targ, const int instance)
{
	if (nt == targ && getInstance() == instance)
	{
		return this; // check current node first
	}

	for (auto& wrap : children)
	{
		if (wrap.isNode)
		{
			ParseNode* curr = wrap.val.node;
			if (!curr)
			{	// null is error condition so it's okay.
				std::cout << "Impossible condition :: isNode is supposed to be a node, but is null!!!! " << nt << "\n";
				return curr;
			}

			if (curr->getName() == targ && curr->getInstance() == instance)
			{
				return curr;
			}
		}
	}

	return NULL;
}