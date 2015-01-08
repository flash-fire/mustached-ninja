#include "ParseNode.h"
#include<algorithm>
#include<iostream>

ParseNode::ParseNode(ParseNode* par, std::string nonTerminal, std::list<std::string> varNames) : parent(par), nt(nonTerminal), instanceNumber(1), varNames(varNames)
{
	for (auto& it : varNames) {
		locSet(it, 0);
	}
}

ParseNode::~ParseNode()
{
}

void ParseNode::WriteUndecoratedTree(ParseNode* node, std::ofstream* fileToWrite, int level)
{
	if (node == NULL)
	{	// We have root
		return;
	}

	std::string out = "";
	if (level > 0)
	{
		out = std::string("\t", level);
	}
	out += node->getName() + " " + std::to_string(node->getInstance()) + "\n";
	*fileToWrite << out;

	for (auto& wrap : node->getChildren())
	{
		if (wrap.isNode)
		{
			ParseNode* curr = wrap.val.node;
			if (!curr)
			{	// null is error condition so it's okay.
				std::cout << "[writeUndecoratedTree] Impossible condition :: isNode is supposed to be a node, but is null!!!! " << node->getName() << "\n";
			}
			else
			{
				WriteUndecoratedTree(curr, fileToWrite, level + 1);
			}
		}
	}
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

void ParseNode::appendChild(ParseNode* child, int debugTargInstance)
{
	int instanceFound = 1;
	for (auto& wrap : children)
	{
		if (wrap.isNode)
		{
			ParseNode* curr = wrap.val.node;
			if (!curr)
			{	// null is error condition so it's okay.
				std::cout << "[append child] Impossible condition :: isNode is supposed to be a node, but is null!!!! " << nt << "\n";
			}
			if (curr->getName() == child->getName())
			{
				instanceFound++;
			}
		}
	}

	if (debugTargInstance != -1)
	{
		if (debugTargInstance != instanceFound)
		{
			std::cout << "[appen child] Wrong target instance found when appending a child. This is obviously so bad that everyone will die brutally.\n"
				<< "Iter Found: " << instanceFound << " iter desired " << debugTargInstance;
		}
	}

	child->setInstance(instanceFound);
	children.push_back(Wrapper(child));
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
	std::string debug = "";

	for (auto& wrap : children)
	{
		if (wrap.isNode)
		{
			ParseNode* curr = wrap.val.node;
			if (!curr)
			{	// null is error condition so it's okay.
				std::cout << "[FildChild] Impossible condition :: isNode is supposed to be a node, but is null!!!! " << nt << "\n";
				return curr;
			}

			if (curr->getName() == targ)
			{
				if (curr->getInstance() == instance)
				{
					return curr;
				}
				else
				{
					debug += "Found node with same name, but different instance. Want : " + std::to_string(instance) + " found : " + std::to_string(curr->getInstance()) + "\n";
				}
			}
		}
	}
	std::cout << "IN findChild and we have sad news about the finding of our target node.";
	std::cout << debug;
	return NULL;
}
