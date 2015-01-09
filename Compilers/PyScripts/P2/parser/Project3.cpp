#include "Project2.h"
#include "Project1.h"
#include <iostream>
#include "MiscFunc.h"
#include "Token.h"
#include "ParseNode.h"
#include "Type.h"

const std::string Project2::SYNCH_PATH = ".\\data\\synch.txt";

Project2::Project2(Project1* p1) : lookAhead(Token()), p(p1)
{
	synch.open(SYNCH_PATH);
	target.open(Project1::TARGET_PATH, std::ios::app);
	LoadSynch();
	Parse();
}

Project2::~Project2()
{
	synch.close();
}


int main()
{
	Project1 a;
	a.HandleLexing();
	Project2 b(&a);
	return 0;
}


void Project2::LoadSynch()
{
	std::string line1;
	if (synch.is_open())
	{
		while (std::getline(synch, line1))
		{
			std::string line2;
			if (std::getline(synch, line2))
			{
				synchSet[line1] = line2;
			}
		}
	}
}

void Project2::MatchEOF()
{
	if (lookAhead.token == EOF)
	{
		return; // end parse
	}
	else
	{
		SynErrorTok("EOF", "EOF"); // The lex matches what we want to match when we're matching an individual token.
	}
}

Token* Project2::Match(int T, std::string nt, std::string exp)
{
	int C = lookAhead.token;
	if (T == C && C != EOF)
	{
		std::cout << "Matched: <" << p->TokenToGrammar(T) << ">\n";
		Token ret = lookAhead;
		lookAhead = p->nextToken();
		return new Token(ret);

	}
	else if (T == C && C == EOF)
	{
		return new Token(lookAhead); // end parse
	}
	else
	{
		SynErrorTok(nt, exp);
		Token ret = Token();
		ret.line = lookAhead.line; // just in case i need it later.
		return new Token(ret);
	}
}

void Project2::TypeErrorTok(std::string nt, Type::TYPE exp, Type::TYPE rec, Token tok)
{
	target << "TYPERR found at line: " << tok.line << " char pos " << lookAhead.charNum
		<< " NonTerminal: <" << nt << "> \nEncountered: <" << Type::typeToString(rec) << "> Expected <"
		<< Type::typeToString(exp) << ">\n\n";
}

void Project2::SynErrorTok(std::string nt, std::string exp)
{
	target << "SYNERR at line: " << lookAhead.line << " char pos " << lookAhead.charNum
		<< " NonTerminal: <" << nt
		<< "> \nEncountered: <" << lookAhead.lex << "> Expected one of: <"
		<< exp << ">\n\n";
	target.flush();
	UniteSynchronization(nt);
}

void Project2::UniteSynchronization(std::string nt)
{
	bool recover = false;
	std::cout << "In UniteSynchronization attempting to match one of <" << synchSet[nt] << ">\n";
	while (p->hasToken())
	{
		if (!contains(synchSet[nt], lookAhead.gram()))
		{
			lookAhead = p->nextToken();
		}
		else
		{
			recover = true;
			break;
		}
	}
	if (recover == false)
	{
		target << "Reached EOF while trying to synchronize NT <" << nt
			<< "> \nExpecting one of following <" << synchSet[nt] << ">\n";
		target.flush();
	}
	else
	{
		target << "Resynchronized at " << lookAhead.line << " char pos " << lookAhead.charNum
			<< "> \nUsing: <" << lookAhead.lex << ">\n"
			<< "for synch set <" << synchSet[nt] << "> NT <" << nt << ">\n\n";
	}
}
// Lol. So inneficient. I ain't optimizing this shit unless I need too.
bool Project2::contains(std::string str, std::string targ)
{
	std::vector<std::string> split = MiscFunc::split(str, ' ');
	return (std::find(split.begin(), split.end(), targ) != split.end());
}
void Project2::Parse()
{
	ParseNode* root = new ParseNode(NULL, "prgm", std::vector<std::string>());
	lookAhead = p->nextToken();
	prgm(root);
	MatchEOF();
	std::ofstream file;
	file.open("Tree.txt", std::ios::out | std::ios::trunc);
	ParseNode::WriteUndecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &file);
	std::ofstream decFile;
	decFile.open("DecTree.txt", std::ios::out | std::ios::trunc);
	ParseNode::WriteDecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &decFile);
}

