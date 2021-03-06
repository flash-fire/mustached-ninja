#include "Project2.h"
#include "Project1.h"
#include <iostream>
#include "MiscFunc.h"
#include "Token.h"
#include "ParseNode.h"
#include "Type.h"

#define TTI(arg) Type::typeToInt(Type::arg)
#define ITT(arg) Type::intToType(arg)

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
bool Project2::Match(int T, Token** retTok)
{
	int C = lookAhead.token;
	Token* ret = new Token(lookAhead); // might cause a memory leak. Don't care.
	*retTok = ret; // This is the point in which I say lol I can't believe this worked but I also believe it. First legit use of double pointers!
	if (T == C && C != EOF)
	{
		//std::cout << "Matched: <" << p->TokenToGrammar(T) << ">\n";
		lookAhead = p->nextToken();
		return true;
	}
	else if (T == C && C == EOF)
	{
		std::cout << "Matched EOF! YAY!";
		return true;
	}
	else
	{
		return false;
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
	ParseNode* root = new ParseNode(NULL, "prgm", vars["prgm_1"]);
	lookAhead = p->nextToken();
	prgm(root);
	MatchEOF();
	std::ofstream file;
	file.open("./output/Tree.txt", std::ios::out | std::ios::trunc);
	ParseNode::WriteUndecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &file);
	std::ofstream decFile;
	decFile.open("./output/DecTree.txt", std::ios::out | std::ios::trunc);
	ParseNode::WriteDecoratedTree(ParseNode::Wrap(root, ParseNode::DEF_INSTANCE), &decFile);
}

void Project2::prgm(ParseNode* prgm_1 ) {
	std::string nt = "prgm";
	std::string exp = "program";
	ParseNode* ref = prgm_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("program")) {
		ParseNode* idList_1 = new ParseNode(prgm_1,"idList", vars["idList_1"]);
		prgm_1->appendChild(idList_1);
		ParseNode* prgmLF1_1 = new ParseNode(prgm_1,"prgmLF1", vars["prgmLF1_1"]);
		prgm_1->appendChild(prgmLF1_1);
		 if (!Match(p->GTT("program") , &currTok)) goto prgmError;
		prgm_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto prgmError;
		prgm_1->appendToken( currTok, ref);
					currScope = new Scope(NULL, prgm_1->findT("id_1")->lex);
			std::cout << prgm_1->findT("id_1")->lex;

		 if (!Match(p->GTT("(") , &currTok)) goto prgmError;
		prgm_1->appendToken( currTok, ref);
		ref = idList_1;
		idList(idList_1);
		 if (!Match(p->GTT(")") , &currTok)) goto prgmError;
		prgm_1->appendToken( currTok, ref);
		 if (!Match(p->GTT(";") , &currTok)) goto prgmError;
		prgm_1->appendToken( currTok, ref);
		ref = prgmLF1_1;
		prgmLF1(prgmLF1_1);
			prgm_1->set("totalSize", prgmLF1_1->get("totalSize"));

		return;
	}
	prgmError:

	SynErrorTok(nt, exp);
}

void Project2::prgmLF1(ParseNode* prgmLF1_1 ) {
	std::string nt = "prgmLF1";
	std::string exp = "procedure var begin";
	ParseNode* ref = prgmLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(prgmLF1_1,"subprgm_decs", vars["subprgm_decs_1"]);
		prgmLF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1_1,"comp_stmt", vars["comp_stmt_1"]);
		prgmLF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		 if (!Match(p->GTT(".") , &currTok)) goto prgmLF1Error;
		prgmLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1_1,"comp_stmt", vars["comp_stmt_1"]);
		prgmLF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		 if (!Match(p->GTT(".") , &currTok)) goto prgmLF1Error;
		prgmLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = new ParseNode(prgmLF1_1,"decs", vars["decs_1"]);
		prgmLF1_1->appendChild(decs_1);
		ParseNode* prgmLF1LF1_1 = new ParseNode(prgmLF1_1,"prgmLF1LF1", vars["prgmLF1LF1_1"]);
		prgmLF1_1->appendChild(prgmLF1LF1_1);
			decs_1->set("offset", 0);

		ref = decs_1;
		decs(decs_1);
		ref = prgmLF1LF1_1;
		prgmLF1LF1(prgmLF1LF1_1);
			prgmLF1_1->set("totalSize", decs_1->get("totalSize"));

		return;
	}
	prgmLF1Error:
			prgmLF1_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::prgmLF1LF1(ParseNode* prgmLF1LF1_1 ) {
	std::string nt = "prgmLF1LF1";
	std::string exp = "procedure begin";
	ParseNode* ref = prgmLF1LF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(prgmLF1LF1_1,"subprgm_decs", vars["subprgm_decs_1"]);
		prgmLF1LF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1LF1_1,"comp_stmt", vars["comp_stmt_1"]);
		prgmLF1LF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		 if (!Match(p->GTT(".") , &currTok)) goto prgmLF1LF1Error;
		prgmLF1LF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1LF1_1,"comp_stmt", vars["comp_stmt_1"]);
		prgmLF1LF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		 if (!Match(p->GTT(".") , &currTok)) goto prgmLF1LF1Error;
		prgmLF1LF1_1->appendToken( currTok, ref);
		return;
	}
	prgmLF1LF1Error:

	SynErrorTok(nt, exp);
}

void Project2::idList(ParseNode* idList_1 ) {
	std::string nt = "idList";
	std::string exp = "id";
	ParseNode* ref = idList_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* idListLR1_1 = new ParseNode(idList_1,"idListLR1", vars["idListLR1_1"]);
		idList_1->appendChild(idListLR1_1);
		 if (!Match(p->GTT("id") , &currTok)) goto idListError;
		idList_1->appendToken( currTok, ref);
		ref = idListLR1_1;
		idListLR1(idListLR1_1);
		return;
	}
	idListError:

	SynErrorTok(nt, exp);
}

void Project2::idListLR1(ParseNode* idListLR1_1 ) {
	std::string nt = "idListLR1";
	std::string exp = ", )";
	ParseNode* ref = idListLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT(",")) {
		ParseNode* idListLR1_2 = new ParseNode(idListLR1_1,"idListLR1", vars["idListLR1_1"]);
		idListLR1_1->appendChild(idListLR1_2);
		 if (!Match(p->GTT(",") , &currTok)) goto idListLR1Error;
		idListLR1_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto idListLR1Error;
		idListLR1_1->appendToken( currTok, ref);
		ref = idListLR1_2;
		idListLR1(idListLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	idListLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::decs(ParseNode* decs_1 ) {
	std::string nt = "decs";
	std::string exp = "var";
	ParseNode* ref = decs_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = new ParseNode(decs_1,"type", vars["type_1"]);
		decs_1->appendChild(type_1);
		ParseNode* decsLR1_1 = new ParseNode(decs_1,"decsLR1", vars["decsLR1_1"]);
		decs_1->appendChild(decsLR1_1);
		 if (!Match(p->GTT("var") , &currTok)) goto decsError;
		decs_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto decsError;
		decs_1->appendToken( currTok, ref);
		 if (!Match(p->GTT(":") , &currTok)) goto decsError;
		decs_1->appendToken( currTok, ref);
		ref = type_1;
		type(type_1);
					decsLR1_1->set("offset", decs_1->get("offset") + type_1->get("width")); //# debug later
			Token* idTok = decs_1->findT("id_1");
			std::string ret = "";
			std::string* err = &ret;
			if (currScope->addVar(idTok->lex, ITT(type_1->get("t")), decs_1->get("offset"), err) == false)
			{
				target << *err << " VAR NAME: " << idTok->lex << " LINE # " << idTok->line << "\n";
			}

		 if (!Match(p->GTT(";") , &currTok)) goto decsError;
		decs_1->appendToken( currTok, ref);
		ref = decsLR1_1;
		decsLR1(decsLR1_1);
			decs_1->set("t", decsLR1_1->get("t"));
			decs_1->set("totalSize", decsLR1_1->get("totalSize"));

		return;
	}
	decsError:
		decs_1->set("t", TTI(ERROR));
		decs_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::decsLR1(ParseNode* decsLR1_1 ) {
	std::string nt = "decsLR1";
	std::string exp = "procedure var begin";
	ParseNode* ref = decsLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = new ParseNode(decsLR1_1,"type", vars["type_1"]);
		decsLR1_1->appendChild(type_1);
		ParseNode* decsLR1_2 = new ParseNode(decsLR1_1,"decsLR1", vars["decsLR1_1"]);
		decsLR1_1->appendChild(decsLR1_2);
		 if (!Match(p->GTT("var") , &currTok)) goto decsLR1Error;
		decsLR1_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto decsLR1Error;
		decsLR1_1->appendToken( currTok, ref);
		 if (!Match(p->GTT(":") , &currTok)) goto decsLR1Error;
		decsLR1_1->appendToken( currTok, ref);
		ref = type_1;
		type(type_1);
					decsLR1_2->set("offset", decsLR1_1->get("offset") + type_1->get("width")); //# ADDRESS
			Token* idTok = decsLR1_1->findT(1); //# SCOPE
			std::string ret = "";
			std::string* err = &ret;
			if (currScope->addVar(idTok->lex, ITT(type_1->get("t")), decsLR1_1->get("offset"), err) == false)
			{
				target << *err << " VAR NAME: " << idTok->lex << " LINE # " << idTok->line << "\n";
			}

		 if (!Match(p->GTT(";") , &currTok)) goto decsLR1Error;
		decsLR1_1->appendToken( currTok, ref);
		ref = decsLR1_2;
		decsLR1(decsLR1_2);
			decsLR1_1->set("t", decsLR1_2->get("t"));
			decsLR1_1->set("totalSize", decsLR1_2->get("totalSize"));

		return;
	}

	if(lookAhead.token == p->GTT("procedure") || lookAhead.token == p->GTT("begin")) {
			decsLR1_1->set("totalSize", decsLR1_1->get("offset"));
			decsLR1_1->set("t", TTI(NONE));
		return;
	}
	decsLR1Error:
		decsLR1_1->set("t", TTI(ERROR));
		decsLR1_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::type(ParseNode* type_1 ) {
	std::string nt = "type";
	std::string exp = "integer array real";
	ParseNode* ref = type_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("integer")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", vars["std_type_1"]);
		type_1->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
			type_1->set("t", std_type_1->get("t"));
			type_1->set("width", std_type_1->get("width"));

		return;
	}

	if(lookAhead.token == p->GTT("array")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", vars["std_type_1"]);
		type_1->appendChild(std_type_1);
		 if (!Match(p->GTT("array") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("[") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("num") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("..") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("num") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("]") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("of") , &currTok)) goto typeError;
		type_1->appendToken( currTok, ref);
		ref = std_type_1;
		std_type(std_type_1);
			Token* tok1 = type_1->findT("num_1"); //# Yes this may seem unsafe, but I only have to do this for arrays so I'm okay with it.
			Token* tok2 = type_1->findT("num_2");
			type_1->set("t",TTI(ERROR)); //# Defaulting to error unless changed
			type_1->set("width", 0);
			int min = -1;
			int max = -1;
			int arraySize = 0;
			int i = 0; //# Terrible hack is great hack!
			for (Token* currTok : {tok1, tok2})
			{
				if (currTok->isInt())
				{
					if (i == 0)
						min = currTok->val();
					else if (i == 1)
						max = currTok->val();
					else
						std::cout << "WTF in array terrible hack";
				}
				else if (currTok->isReal())
				{
					TypeErrorTok(nt, Type::INT, Type::REAL, *currTok);
				}
				else
				{ //# I highly doubt this line will be reached because currTok must be valid syntactically for this to be reached.
				  //# Therefore, I added this just in case I get it. IT's a nice "MY COMPILER HAS A BUG" output.
				TypeErrorTok(nt, Type::INT, Type::UNASSIGNED, *currTok);
				}
				i++;
			}
			arraySize = max - min;
			if (max == -1 || min == -1)
			{ // error would be already reported at above
			}
			else if (arraySize <= 0) //# if bad args
			{
				target << "SEMERR: Array parameters improper. Min >= Max with " << tok1->lex << " and " << tok2->lex << "\n\n";
			}
			else 
			{
				int t_ = std_type_1->get("t");
				if (t_ == TTI(INT))
				{
					type_1->set("t", TTI(AINT));
					type_1->set("width", std_type_1->get("width") * arraySize);
				}
				if (t_ == TTI(REAL))
				{
					type_1->set("t", TTI(AREAL));
					type_1->set("width", std_type_1->get("width") * arraySize);
				}
				 //# FINALLY all the silly errors are done
			}
			std::cout << "\nMade array of type " << Type::typeToString(ITT(type_1->get("t"))) << " width " << type_1->get("width");

		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", vars["std_type_1"]);
		type_1->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
			type_1->set("t", std_type_1->get("t"));
			type_1->set("width", std_type_1->get("width"));

		return;
	}
	typeError:
		type_1->set("t", TTI(ERROR));
		type_1->set("width", 0);

	SynErrorTok(nt, exp);
}

void Project2::std_type(ParseNode* std_type_1 ) {
	std::string nt = "std_type";
	std::string exp = "integer real";
	ParseNode* ref = std_type_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("integer")) {
		 if (!Match(p->GTT("integer") , &currTok)) goto std_typeError;
		std_type_1->appendToken( currTok, ref);
			std_type_1->set("width", 4);
			std_type_1->set("t", TTI(INT));

		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		 if (!Match(p->GTT("real") , &currTok)) goto std_typeError;
		std_type_1->appendToken( currTok, ref);
			std_type_1->set("width", 8);
			std_type_1->set("t", TTI(REAL));

		return;
	}
	std_typeError:
		std_type_1->set("t", TTI(ERROR));
		std_type_1->set("width", 0);

	SynErrorTok(nt, exp);
}

void Project2::subprgm_decs(ParseNode* subprgm_decs_1 ) {
	std::string nt = "subprgm_decs";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_decs_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = new ParseNode(subprgm_decs_1,"subprgm_dec", vars["subprgm_dec_1"]);
		subprgm_decs_1->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_1 = new ParseNode(subprgm_decs_1,"subprgm_decsLR1", vars["subprgm_decsLR1_1"]);
		subprgm_decs_1->appendChild(subprgm_decsLR1_1);
			Scope* dummyScope = new Scope(currScope, "~~DUMMY~~"); //# SCOPE
			scopeDepth++;
			std::string* err = &std::string();
			currScope->addChild(dummyScope, err); //# This call will never error. Thank god.
			if (*err != "")
			{
				std::cout  << " ANGRY ANGRY ANGRY IMPOSSIBLE ASSERTION LIKE THING ENCOUNTERED. DUMMY SCOPE! DUMMYSCOPE! FEEL ANGER YOU SHUOLD! YOU ANGRISH!\n";
			}
			currScope = dummyScope;

		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		 if (!Match(p->GTT(";") , &currTok)) goto subprgm_decsError;
		subprgm_decs_1->appendToken( currTok, ref);
		ref = subprgm_decsLR1_1;
		subprgm_decsLR1(subprgm_decsLR1_1);
		return;
	}
	subprgm_decsError:

	SynErrorTok(nt, exp);
}

void Project2::subprgm_decsLR1(ParseNode* subprgm_decsLR1_1 ) {
	std::string nt = "subprgm_decsLR1";
	std::string exp = "procedure begin";
	ParseNode* ref = subprgm_decsLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = new ParseNode(subprgm_decsLR1_1,"subprgm_dec", vars["subprgm_dec_1"]);
		subprgm_decsLR1_1->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_2 = new ParseNode(subprgm_decsLR1_1,"subprgm_decsLR1", vars["subprgm_decsLR1_1"]);
		subprgm_decsLR1_1->appendChild(subprgm_decsLR1_2);
		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		 if (!Match(p->GTT(";") , &currTok)) goto subprgm_decsLR1Error;
		subprgm_decsLR1_1->appendToken( currTok, ref);
		ref = subprgm_decsLR1_2;
		subprgm_decsLR1(subprgm_decsLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
			currScope = currScope->getParent(); //# SCOPE
		return;
	}
	subprgm_decsLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::subprgm_dec(ParseNode* subprgm_dec_1 ) {
	std::string nt = "subprgm_dec";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_dec_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_head_1 = new ParseNode(subprgm_dec_1,"subprgm_head", vars["subprgm_head_1"]);
		subprgm_dec_1->appendChild(subprgm_head_1);
		ParseNode* subprgm_decLF1_1 = new ParseNode(subprgm_dec_1,"subprgm_decLF1", vars["subprgm_decLF1_1"]);
		subprgm_dec_1->appendChild(subprgm_decLF1_1);
		ref = subprgm_head_1;
		subprgm_head(subprgm_head_1);
					subprgm_decLF1_1->set("totalSize", subprgm_head_1->get("totalSize")); //# pass back address totalSize for params

		ref = subprgm_decLF1_1;
		subprgm_decLF1(subprgm_decLF1_1);
		return;
	}
	subprgm_decError:

	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1(ParseNode* subprgm_decLF1_1 ) {
	std::string nt = "subprgm_decLF1";
	std::string exp = "procedure var begin";
	ParseNode* ref = subprgm_decLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(subprgm_decLF1_1,"subprgm_decs", vars["subprgm_decs_1"]);
		subprgm_decLF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1_1,"comp_stmt", vars["comp_stmt_1"]);
		subprgm_decLF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1_1,"comp_stmt", vars["comp_stmt_1"]);
		subprgm_decLF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = new ParseNode(subprgm_decLF1_1,"decs", vars["decs_1"]);
		subprgm_decLF1_1->appendChild(decs_1);
		ParseNode* subprgm_decLF1LF1_1 = new ParseNode(subprgm_decLF1_1,"subprgm_decLF1LF1", vars["subprgm_decLF1LF1_1"]);
		subprgm_decLF1_1->appendChild(subprgm_decLF1LF1_1);
			decs_1->set("offset", subprgm_decLF1_1->get("totalSize")); //# receive address offset for params

		ref = decs_1;
		decs(decs_1);
		ref = subprgm_decLF1LF1_1;
		subprgm_decLF1LF1(subprgm_decLF1LF1_1);
		return;
	}
	subprgm_decLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1LF1(ParseNode* subprgm_decLF1LF1_1 ) {
	std::string nt = "subprgm_decLF1LF1";
	std::string exp = "procedure begin";
	ParseNode* ref = subprgm_decLF1LF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(subprgm_decLF1LF1_1,"subprgm_decs", vars["subprgm_decs_1"]);
		subprgm_decLF1LF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1LF1_1,"comp_stmt", vars["comp_stmt_1"]);
		subprgm_decLF1LF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1LF1_1,"comp_stmt", vars["comp_stmt_1"]);
		subprgm_decLF1LF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}
	subprgm_decLF1LF1Error:

	SynErrorTok(nt, exp);
}

void Project2::subprgm_head(ParseNode* subprgm_head_1 ) {
	std::string nt = "subprgm_head";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_head_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_headLF1_1 = new ParseNode(subprgm_head_1,"subprgm_headLF1", vars["subprgm_headLF1_1"]);
		subprgm_head_1->appendChild(subprgm_headLF1_1);
		 if (!Match(p->GTT("procedure") , &currTok)) goto subprgm_headError;
		subprgm_head_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto subprgm_headError;
		subprgm_head_1->appendToken( currTok, ref);
					Token* tok = subprgm_head_1->findT("id_1");
			Scope* currSib = new Scope(currScope->getParent(), tok->lex);
			std::string* errMsg = &std::string();
			//#std::cout << std::string(scopeDepth * 2, ' ') << tok->lex << "\n" << "\n";
			if (currScope->addSibling(currSib, errMsg) == false)
			{
				target << *errMsg << "\n\ton line " << tok->line << " and collumn " << tok->charNum  << "\n";
			}
			currScope = currSib;

		ref = subprgm_headLF1_1;
		subprgm_headLF1(subprgm_headLF1_1);
			subprgm_head_1->set("totalSize", subprgm_headLF1_1->get("totalSize")); //# pass back address address offset for params
			subprgm_head_1->set("t", subprgm_headLF1_1->get("t"));

		return;
	}
	subprgm_headError:
			subprgm_head_1->set("t", TTI(ERROR));
			subprgm_head_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::subprgm_headLF1(ParseNode* subprgm_headLF1_1 ) {
	std::string nt = "subprgm_headLF1";
	std::string exp = "; (";
	ParseNode* ref = subprgm_headLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* args_1 = new ParseNode(subprgm_headLF1_1,"args", vars["args_1"]);
		subprgm_headLF1_1->appendChild(args_1);
		ref = args_1;
		args(args_1);
		 if (!Match(p->GTT(";") , &currTok)) goto subprgm_headLF1Error;
		subprgm_headLF1_1->appendToken( currTok, ref);
			subprgm_headLF1_1->set("totalSize", args_1->get("totalSize"));
			subprgm_headLF1_1->set("t", args_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT(";")) {
		 if (!Match(p->GTT(";") , &currTok)) goto subprgm_headLF1Error;
		subprgm_headLF1_1->appendToken( currTok, ref);
			subprgm_headLF1_1->set("totalSize", 0);
			subprgm_headLF1_1->set("t", TTI(NONE));

		return;
	}
	subprgm_headLF1Error:
		subprgm_headLF1_1->set("t", TTI(ERROR));
		subprgm_headLF1_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::args(ParseNode* args_1 ) {
	std::string nt = "args";
	std::string exp = "(";
	ParseNode* ref = args_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* param_list_1 = new ParseNode(args_1,"param_list", vars["param_list_1"]);
		args_1->appendChild(param_list_1);
		 if (!Match(p->GTT("(") , &currTok)) goto argsError;
		args_1->appendToken( currTok, ref);
		ref = param_list_1;
		param_list(param_list_1);
		 if (!Match(p->GTT(")") , &currTok)) goto argsError;
		args_1->appendToken( currTok, ref);
			args_1->set("totalSize", param_list_1->get("totalSize"));
			args_1->set("t", param_list_1->get("t"));

		return;
	}
	argsError:
			args_1->set("t", TTI(ERROR));
			args_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::param_list(ParseNode* param_list_1 ) {
	std::string nt = "param_list";
	std::string exp = "id";
	ParseNode* ref = param_list_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* type_1 = new ParseNode(param_list_1,"type", vars["type_1"]);
		param_list_1->appendChild(type_1);
		ParseNode* param_listLR1_1 = new ParseNode(param_list_1,"param_listLR1", vars["param_listLR1_1"]);
		param_list_1->appendChild(param_listLR1_1);
		 if (!Match(p->GTT("id") , &currTok)) goto param_listError;
		param_list_1->appendToken( currTok, ref);
		 if (!Match(p->GTT(":") , &currTok)) goto param_listError;
		param_list_1->appendToken( currTok, ref);
		ref = type_1;
		type(type_1);
					param_listLR1_1->set("offset", param_list_1->get("offset") + type_1->get("width")); //# OFFSET STARTS AT 0 BECAUSE PARAMS
			Token* idTok = param_list_1->findT("id_1");
			std::string ret = "";
			std::string* err = &ret;
			if (currScope->addParam(idTok->lex, ITT(type_1->get("t")), param_list_1->get("offset"), err) == false)
			{
				target << *err << " VAR NAME: " << idTok->lex << " LINE # " << idTok->line << "\n";
			}

		ref = param_listLR1_1;
		param_listLR1(param_listLR1_1);
			param_list_1->set("t", param_listLR1_1->get("t"));
			param_list_1->set("totalSize", param_listLR1_1->get("totalSize"));

		return;
	}
	param_listError:

	SynErrorTok(nt, exp);
}

void Project2::param_listLR1(ParseNode* param_listLR1_1 ) {
	std::string nt = "param_listLR1";
	std::string exp = "; )";
	ParseNode* ref = param_listLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT(";")) {
		ParseNode* type_1 = new ParseNode(param_listLR1_1,"type", vars["type_1"]);
		param_listLR1_1->appendChild(type_1);
		ParseNode* param_listLR1_2 = new ParseNode(param_listLR1_1,"param_listLR1", vars["param_listLR1_1"]);
		param_listLR1_1->appendChild(param_listLR1_2);
		 if (!Match(p->GTT(";") , &currTok)) goto param_listLR1Error;
		param_listLR1_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto param_listLR1Error;
		param_listLR1_1->appendToken( currTok, ref);
		 if (!Match(p->GTT(":") , &currTok)) goto param_listLR1Error;
		param_listLR1_1->appendToken( currTok, ref);
		ref = type_1;
		type(type_1);
					param_listLR1_2->set("offset", param_listLR1_1->get("offset") + type_1->get("width")); //# ADDRESS
			Token* idTok = param_listLR1_1->findT(1); //# SCOPE
			std::string ret = "";
			std::string* err = &ret;
			if (currScope->addParam(idTok->lex, ITT(type_1->get("t")), param_listLR1_1->get("offset"), err) == false)
			{
				target << *err << " VAR NAME: " << idTok->lex << " LINE # " << idTok->line << "\n";
			}

		ref = param_listLR1_2;
		param_listLR1(param_listLR1_2);
			param_listLR1_1->set("t", param_listLR1_2->get("t"));
			param_listLR1_1->set("totalSize", param_listLR1_2->get("totalSize"));

		return;
	}

	if(lookAhead.token == p->GTT(")")) {
			param_listLR1_1->set("totalSize", param_listLR1_1->get("offset"));
			param_listLR1_1->set("t", TTI(NONE));
		return;
	}
	param_listLR1Error:
		param_listLR1_1->set("t", TTI(ERROR));
		param_listLR1_1->set("totalSize", 0);

	SynErrorTok(nt, exp);
}

void Project2::comp_stmt(ParseNode* comp_stmt_1 ) {
	std::string nt = "comp_stmt";
	std::string exp = "begin";
	ParseNode* ref = comp_stmt_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmtLF1_1 = new ParseNode(comp_stmt_1,"comp_stmtLF1", vars["comp_stmtLF1_1"]);
		comp_stmt_1->appendChild(comp_stmtLF1_1);
		 if (!Match(p->GTT("begin") , &currTok)) goto comp_stmtError;
		comp_stmt_1->appendToken( currTok, ref);
		ref = comp_stmtLF1_1;
		comp_stmtLF1(comp_stmtLF1_1);
		return;
	}
	comp_stmtError:

	SynErrorTok(nt, exp);
}

void Project2::comp_stmtLF1(ParseNode* comp_stmtLF1_1 ) {
	std::string nt = "comp_stmtLF1";
	std::string exp = "call if begin while id end";
	ParseNode* ref = comp_stmtLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", vars["opt_stmts_1"]);
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", vars["opt_stmts_1"]);
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", vars["opt_stmts_1"]);
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", vars["opt_stmts_1"]);
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", vars["opt_stmts_1"]);
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		 if (!Match(p->GTT("end") , &currTok)) goto comp_stmtLF1Error;
		comp_stmtLF1_1->appendToken( currTok, ref);
		return;
	}
	comp_stmtLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::opt_stmts(ParseNode* opt_stmts_1 ) {
	std::string nt = "opt_stmts";
	std::string exp = "call while id if begin";
	ParseNode* ref = opt_stmts_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", vars["stmt_list_1"]);
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", vars["stmt_list_1"]);
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", vars["stmt_list_1"]);
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", vars["stmt_list_1"]);
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", vars["stmt_list_1"]);
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}
	opt_stmtsError:

	SynErrorTok(nt, exp);
}

void Project2::stmt_list(ParseNode* stmt_list_1 ) {
	std::string nt = "stmt_list";
	std::string exp = "call while id if begin";
	ParseNode* ref = stmt_list_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", vars["stmt_1"]);
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", vars["stmt_1"]);
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", vars["stmt_1"]);
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", vars["stmt_1"]);
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", vars["stmt_1"]);
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}
	stmt_listError:

	SynErrorTok(nt, exp);
}

void Project2::stmt_listLR1(ParseNode* stmt_listLR1_1 ) {
	std::string nt = "stmt_listLR1";
	std::string exp = "; end";
	ParseNode* ref = stmt_listLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT(";")) {
		ParseNode* stmt_1 = new ParseNode(stmt_listLR1_1,"stmt", vars["stmt_1"]);
		stmt_listLR1_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_2 = new ParseNode(stmt_listLR1_1,"stmt_listLR1", vars["stmt_listLR1_1"]);
		stmt_listLR1_1->appendChild(stmt_listLR1_2);
		 if (!Match(p->GTT(";") , &currTok)) goto stmt_listLR1Error;
		stmt_listLR1_1->appendToken( currTok, ref);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_2;
		stmt_listLR1(stmt_listLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		return;
	}
	stmt_listLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::stmt(ParseNode* stmt_1 ) {
	std::string nt = "stmt";
	std::string exp = "call while id if begin";
	ParseNode* ref = stmt_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmt_1 = new ParseNode(stmt_1,"proc_stmt", vars["proc_stmt_1"]);
		stmt_1->appendChild(proc_stmt_1);
		ref = proc_stmt_1;
		proc_stmt(proc_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variable_1 = new ParseNode(stmt_1,"variable", vars["variable_1"]);
		stmt_1->appendChild(variable_1);
		ParseNode* assignop_1 = new ParseNode(stmt_1,"assignop", vars["assignop_1"]);
		stmt_1->appendChild(assignop_1);
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", vars["expr_1"]);
		stmt_1->appendChild(expr_1);
		ref = variable_1;
		variable(variable_1);
		ref = assignop_1;
		assignop(assignop_1);
					expr_1->set("i", variable_1->get("t"));

		ref = expr_1;
		expr(expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(stmt_1,"comp_stmt", vars["comp_stmt_1"]);
		stmt_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", vars["expr_1"]);
		stmt_1->appendChild(expr_1);
		ParseNode* stmt_2 = new ParseNode(stmt_1,"stmt", vars["stmt_1"]);
		stmt_1->appendChild(stmt_2);
		ParseNode* stmtLF1_1 = new ParseNode(stmt_1,"stmtLF1", vars["stmtLF1_1"]);
		stmt_1->appendChild(stmtLF1_1);
			expr_1->set("i", TTI(BOOL));

		 if (!Match(p->GTT("if") , &currTok)) goto stmtError;
		stmt_1->appendToken( currTok, ref);
		ref = expr_1;
		expr(expr_1);
		 if (!Match(p->GTT("then") , &currTok)) goto stmtError;
		stmt_1->appendToken( currTok, ref);
		ref = stmt_2;
		stmt(stmt_2);
		ref = stmtLF1_1;
		stmtLF1(stmtLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", vars["expr_1"]);
		stmt_1->appendChild(expr_1);
		ParseNode* stmt_2 = new ParseNode(stmt_1,"stmt", vars["stmt_1"]);
		stmt_1->appendChild(stmt_2);
			expr_1->set("i", TTI(BOOL));

		 if (!Match(p->GTT("while") , &currTok)) goto stmtError;
		stmt_1->appendToken( currTok, ref);
		ref = expr_1;
		expr(expr_1);
		 if (!Match(p->GTT("do") , &currTok)) goto stmtError;
		stmt_1->appendToken( currTok, ref);
		ref = stmt_2;
		stmt(stmt_2);
		return;
	}
	stmtError:

	SynErrorTok(nt, exp);
}

void Project2::stmtLF1(ParseNode* stmtLF1_1 ) {
	std::string nt = "stmtLF1";
	std::string exp = "; end else";
	ParseNode* ref = stmtLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("else")) {
		ParseNode* stmt_1 = new ParseNode(stmtLF1_1,"stmt", vars["stmt_1"]);
		stmtLF1_1->appendChild(stmt_1);
		 if (!Match(p->GTT("else") , &currTok)) goto stmtLF1Error;
		stmtLF1_1->appendToken( currTok, ref);
		ref = stmt_1;
		stmt(stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end")) {
		return;
	}
	stmtLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::variable(ParseNode* variable_1 ) {
	std::string nt = "variable";
	std::string exp = "id";
	ParseNode* ref = variable_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variableLF1_1 = new ParseNode(variable_1,"variableLF1", vars["variableLF1_1"]);
		variable_1->appendChild(variableLF1_1);
		 if (!Match(p->GTT("id") , &currTok)) goto variableError;
		variable_1->appendToken( currTok, ref);
					Token* tok = variable_1->findT("id_1");
			std::string lex = tok->lex;
			if (currScope->isVarInScope(lex))
			{
				std::string* err = &std::string();
				Type::TYPE t1 = currScope->getTypeOfVar(lex, err);
				variableLF1_1->set("i", t1);
			}
			else
			{
				target << "SEMERR: Attempting to use variable " << lex << " before it was initialized on line " << tok->line << " and collumn " << tok->charNum;
				variableLF1_1->set("i", TTI(ERROR));
			}

		ref = variableLF1_1;
		variableLF1(variableLF1_1);
			variable_1->set("t", variableLF1_1->get("t"));

		return;
	}
	variableError:

	SynErrorTok(nt, exp);
}

void Project2::variableLF1(ParseNode* variableLF1_1 ) {
	std::string nt = "variableLF1";
	std::string exp = "[ :=";
	ParseNode* ref = variableLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = new ParseNode(variableLF1_1,"expr", vars["expr_1"]);
		variableLF1_1->appendChild(expr_1);
		 if (!Match(p->GTT("[") , &currTok)) goto variableLF1Error;
		variableLF1_1->appendToken( currTok, ref);
					expr_1->set("i", TTI(INT));

		ref = expr_1;
		expr(expr_1);
		 if (!Match(p->GTT("]") , &currTok)) goto variableLF1Error;
		variableLF1_1->appendToken( currTok, ref);
			int t_i = variableLF1_1->get("i");
			if (false == (t_i == TTI(AINT) || t_i == TTI(AREAL) || t_i == TTI(ERROR)))
			{
				target << "TYPERR: ~Expected array, but received " << Type::typeToString(ITT(t_i)) << " on line " << lookAhead.line << ".\n";
				variableLF1_1->set("t", TTI(ERROR));
			}
			else
			{	
				if (expr_1->get("t") == TTI(INT))
				{
					int t_exp = 0;
					if (t_i == TTI(AINT)) t_exp = TTI(INT);
					if (t_i == TTI(AREAL)) t_exp = TTI(REAL);
					variableLF1_1->set("t", t_exp);
				}
				else
				{
					variableLF1_1->set("t", TTI(ERROR));
				}
			}

		return;
	}

	if(lookAhead.token == p->GTT(":=")) {
			variableLF1_1->set("t", variableLF1_1->get("i"));
		return;
	}
	variableLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::proc_stmt(ParseNode* proc_stmt_1 ) {
	std::string nt = "proc_stmt";
	std::string exp = "call";
	ParseNode* ref = proc_stmt_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmtLF1_1 = new ParseNode(proc_stmt_1,"proc_stmtLF1", vars["proc_stmtLF1_1"]);
		proc_stmt_1->appendChild(proc_stmtLF1_1);
		 if (!Match(p->GTT("call") , &currTok)) goto proc_stmtError;
		proc_stmt_1->appendToken( currTok, ref);
		 if (!Match(p->GTT("id") , &currTok)) goto proc_stmtError;
		proc_stmt_1->appendToken( currTok, ref);
				proc_stmtLF1_1->leSigh = &proc_stmt_1->findT("id_1")->lex; //# Making a one time use pointer in ParseNode just to make this decorating easier and safer.

		ref = proc_stmtLF1_1;
		proc_stmtLF1(proc_stmtLF1_1);
		return;
	}
	proc_stmtError:

	SynErrorTok(nt, exp);
}

void Project2::proc_stmtLF1(ParseNode* proc_stmtLF1_1 ) {
	std::string nt = "proc_stmtLF1";
	std::string exp = "; end ( else";
	ParseNode* ref = proc_stmtLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_list_1 = new ParseNode(proc_stmtLF1_1,"expr_list", vars["expr_list_1"]);
		proc_stmtLF1_1->appendChild(expr_list_1);
			expr_list_1->leSigh = proc_stmtLF1_1->leSigh;
			expr_list_1->set("ind",0);

		 if (!Match(p->GTT("(") , &currTok)) goto proc_stmtLF1Error;
		proc_stmtLF1_1->appendToken( currTok, ref);
		ref = expr_list_1;
		expr_list(expr_list_1);
		 if (!Match(p->GTT(")") , &currTok)) goto proc_stmtLF1Error;
		proc_stmtLF1_1->appendToken( currTok, ref);
			proc_stmtLF1_1->set("t", expr_list_1->get("t")); // set to exprList type

		return;
	}

	if(lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("else")) {
			std::string procID = *proc_stmtLF1_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				if (currScope->getParams(procID).size() == 0)
				{
					proc_stmtLF1_1->set("t", TTI(NONE));
					goto endCode;
				}
				else
				{
					target << "SEMERR: Expected " << std::to_string(currScope->getParams(procID).size()) << " parameters, but received none on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}
			proc_stmtLF1_1->set("t", TTI(ERROR));
			endCode:
		return;
	}
	proc_stmtLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::expr_list(ParseNode* expr_list_1 ) {
	std::string nt = "expr_list";
	std::string exp = "not + num ( id -";
	ParseNode* ref = expr_list_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", vars["expr_1"]);
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_list_1->appendChild(expr_listLR1_1);
			expr_listLR1_1->leSigh = expr_list_1->leSigh;
			std::string procID = *expr_list_1->leSigh;
			if (currScope->isProcCallable(procID))
			{
				std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
				if (expr_list_1->get("ind") < params.size())
				{
					expr_listLR1_1->set("ind", expr_list_1->get("ind") + 1);
					expr_1->set("i", params[expr_list_1->get("ind")].type);
				}
				else
				{
					expr_listLR1_1->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Expected ";
					for (Scope::VAR_WRAP wrap : params)
					{
						target << Type::typeToString(wrap.type) << " , ";
					} 
					target << " but received additional parameters on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_1->set("i", TTI(ERROR));
				expr_1->set("i", TTI(IGNORE));
				target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
			}

		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_1->set("i", TTI(ERROR));
			}
			else
			{
				expr_listLR1_1->set("i", TTI(NONE));
			}

		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
			expr_list_1->set("t", expr_listLR1_1->get("t"));

		return;
	}
	expr_listError:

	SynErrorTok(nt, exp);
}

void Project2::expr_listLR1(ParseNode* expr_listLR1_1 ) {
	std::string nt = "expr_listLR1";
	std::string exp = ", )";
	ParseNode* ref = expr_listLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT(",")) {
		ParseNode* expr_1 = new ParseNode(expr_listLR1_1,"expr", vars["expr_1"]);
		expr_listLR1_1->appendChild(expr_1);
		ParseNode* expr_listLR1_2 = new ParseNode(expr_listLR1_1,"expr_listLR1", vars["expr_listLR1_1"]);
		expr_listLR1_1->appendChild(expr_listLR1_2);
			expr_listLR1_2->leSigh = expr_listLR1_1->leSigh;
			std::string procID = *expr_listLR1_1->leSigh;
			if (expr_listLR1_1->get("i") != TTI(ERROR))
			{
				if (currScope->isProcCallable(procID))
				{
					std::vector<Scope::VAR_WRAP> params = currScope->getParams(procID);
					if (expr_listLR1_1->get("ind") < params.size())
					{
						expr_listLR1_2->set("ind", expr_listLR1_1->get("ind") + 1);
						expr_1->set("i", params[expr_listLR1_1->get("ind")].type);
					}
					else
					{
						expr_listLR1_2->set("i", TTI(ERROR));
						expr_1->set("i", TTI(IGNORE));
						target << "SEMERR: Expected ";
						for (Scope::VAR_WRAP wrap : params)
						{
							target << Type::typeToString(wrap.type) << " , ";
						} 
						target << " but received additional parameters on line " << lookAhead.line << "\n";
					}
				}
				else
				{
					expr_listLR1_2->set("i", TTI(ERROR));
					expr_1->set("i", TTI(IGNORE));
					target << "SEMERR: Procedure " << procID << " is not in scope on line " << lookAhead.line << "\n";
				}
			}
			else
			{
				expr_listLR1_2->set("i", TTI(ERROR));
			}

		 if (!Match(p->GTT(",") , &currTok)) goto expr_listLR1Error;
		expr_listLR1_1->appendToken( currTok, ref);
		ref = expr_1;
		expr(expr_1);
					if (expr_1->get("t") == TTI(ERROR))
			{
				expr_listLR1_2->set("t", TTI(ERROR));
			}
			else
			{
				expr_listLR1_2->set("t", TTI(NONE));
			}

		ref = expr_listLR1_2;
		expr_listLR1(expr_listLR1_2);
			expr_listLR1_1->set("t", expr_listLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT(")")) {
			expr_listLR1_1->set("t", expr_listLR1_1->get("i"));
		return;
	}
	expr_listLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::expr(ParseNode* expr_1 ) {
	std::string nt = "expr";
	std::string exp = "not + num ( id -";
	ParseNode* ref = expr_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", vars["simple_expr_1"]);
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", vars["exprLF1_1"]);
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					exprLF1_1->set("i", simple_expr_1->get("t"));

		ref = exprLF1_1;
		exprLF1(exprLF1_1);
			int rec = exprLF1_1->get("t");
			int exp = expr_1->get("i");
			if ((exp == rec) == false && exp != TTI(IGNORE))
			{
				if ((exp == TTI(ERROR) || rec == TTI(ERROR)) == false)
				{
					target << "TYPERR: Expected type " << Type::typeToString(ITT(exp)) << " but receive " << Type::typeToString(ITT(rec))
					<< "\non line " << lookAhead.line << " and near column " << lookAhead.charNum;
				}
				expr_1->set("t", TTI(ERROR));
			}
			else
			{
				expr_1->set("t", exprLF1_1->get("t"));
			}

		return;
	}
	exprError:

	SynErrorTok(nt, exp);
}

void Project2::exprLF1(ParseNode* exprLF1_1 ) {
	std::string nt = "exprLF1";
	std::string exp = "then <> < , ] > ) = >= ; end <= do else";
	ParseNode* ref = exprLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("<>")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT(">=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", vars["relop_1"]);
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", vars["simple_expr_1"]);
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
					int t_1 = simple_expr_1->get("t");
			int t_2 = exprLF1_1->get("i");
			if (t_1 == t_2 && (t_2 == TTI(REAL) || t_2 == TTI(INT)))
			{
				exprLF1_1->set("t",TTI(BOOL));
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				Token* rel = exprLF1_1->findT("relop_1");
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << rel->lex << " on line " << rel->line << " and column " << rel->charNum << "\n";
				exprLF1_1->set("t", TTI(ERROR));
			}
			else
			{
				exprLF1_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("then")) {
			exprLF1_1->set("t", exprLF1_1->get("i"));
		return;
	}
	exprLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::term(ParseNode* term_1 ) {
	std::string nt = "term";
	std::string exp = "num id ( not";
	ParseNode* ref = term_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", vars["factor_1"]);
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", vars["termLR1_1"]);
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
					termLR1_1->set("i", factor_1->get("t"));

		ref = termLR1_1;
		termLR1(termLR1_1);
			term_1->set("t", termLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", vars["factor_1"]);
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", vars["termLR1_1"]);
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
					termLR1_1->set("i", factor_1->get("t"));

		ref = termLR1_1;
		termLR1(termLR1_1);
			term_1->set("t", termLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", vars["factor_1"]);
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", vars["termLR1_1"]);
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
					termLR1_1->set("i", factor_1->get("t"));

		ref = termLR1_1;
		termLR1(termLR1_1);
			term_1->set("t", termLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", vars["factor_1"]);
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", vars["termLR1_1"]);
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
					termLR1_1->set("i", factor_1->get("t"));

		ref = termLR1_1;
		termLR1(termLR1_1);
			term_1->set("t", termLR1_1->get("t"));

		return;
	}
	termError:

	SynErrorTok(nt, exp);
}

void Project2::termLR1(ParseNode* termLR1_1 ) {
	std::string nt = "termLR1";
	std::string exp = "/ <> < mod ] > = >= do then else + , div and * ) or ; end <= -";
	ParseNode* ref = termLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("/")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", vars["mulop_1"]);
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", vars["factor_1"]);
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", vars["termLR1_1"]);
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
					int t_1 = termLR1_1->get("i");
			int t_2 = factor_1->get("t");
			Token* mul = termLR1_1->findN("mulop_1")->findT(0);
			if (t_1 == t_2)
			{
				std::string op = mul->lex;
				if (op == "*" || op == "/" || op == "div" || op == "mod")
				{
					if (t_1 == TTI(INT) || t_1 == TTI(REAL))
					{
						termLR1_2->set("i", t_1);
					}
					else if(t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching numeral types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
				else if (op == "and")
				{
					if (t_1 == TTI(BOOL))
					{
						termLR1_2->set("i", TTI(BOOL));
					}
					else if (t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching boolean types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
			}
			else if (false == ( t_1 == TTI(ERROR) || t_2 == TTI(ERROR)))
			{
				target << "TYPERR: Expecting matching types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
				termLR1_2->set("i", TTI(ERROR));
			}

		ref = termLR1_2;
		termLR1(termLR1_2);
					termLR1_1->set("t", termLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", vars["mulop_1"]);
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", vars["factor_1"]);
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", vars["termLR1_1"]);
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
					int t_1 = termLR1_1->get("i");
			int t_2 = factor_1->get("t");
			Token* mul = termLR1_1->findN("mulop_1")->findT(0);
			if (t_1 == t_2)
			{
				std::string op = mul->lex;
				if (op == "*" || op == "/" || op == "div" || op == "mod")
				{
					if (t_1 == TTI(INT) || t_1 == TTI(REAL))
					{
						termLR1_2->set("i", t_1);
					}
					else if(t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching numeral types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
				else if (op == "and")
				{
					if (t_1 == TTI(BOOL))
					{
						termLR1_2->set("i", TTI(BOOL));
					}
					else if (t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching boolean types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
			}
			else if (false == ( t_1 == TTI(ERROR) || t_2 == TTI(ERROR)))
			{
				target << "TYPERR: Expecting matching types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
				termLR1_2->set("i", TTI(ERROR));
			}

		ref = termLR1_2;
		termLR1(termLR1_2);
					termLR1_1->set("t", termLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", vars["mulop_1"]);
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", vars["factor_1"]);
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", vars["termLR1_1"]);
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
					int t_1 = termLR1_1->get("i");
			int t_2 = factor_1->get("t");
			Token* mul = termLR1_1->findN("mulop_1")->findT(0);
			if (t_1 == t_2)
			{
				std::string op = mul->lex;
				if (op == "*" || op == "/" || op == "div" || op == "mod")
				{
					if (t_1 == TTI(INT) || t_1 == TTI(REAL))
					{
						termLR1_2->set("i", t_1);
					}
					else if(t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching numeral types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
				else if (op == "and")
				{
					if (t_1 == TTI(BOOL))
					{
						termLR1_2->set("i", TTI(BOOL));
					}
					else if (t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching boolean types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
			}
			else if (false == ( t_1 == TTI(ERROR) || t_2 == TTI(ERROR)))
			{
				target << "TYPERR: Expecting matching types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
				termLR1_2->set("i", TTI(ERROR));
			}

		ref = termLR1_2;
		termLR1(termLR1_2);
					termLR1_1->set("t", termLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", vars["mulop_1"]);
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", vars["factor_1"]);
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", vars["termLR1_1"]);
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
					int t_1 = termLR1_1->get("i");
			int t_2 = factor_1->get("t");
			Token* mul = termLR1_1->findN("mulop_1")->findT(0);
			if (t_1 == t_2)
			{
				std::string op = mul->lex;
				if (op == "*" || op == "/" || op == "div" || op == "mod")
				{
					if (t_1 == TTI(INT) || t_1 == TTI(REAL))
					{
						termLR1_2->set("i", t_1);
					}
					else if(t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching numeral types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
				else if (op == "and")
				{
					if (t_1 == TTI(BOOL))
					{
						termLR1_2->set("i", TTI(BOOL));
					}
					else if (t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching boolean types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
			}
			else if (false == ( t_1 == TTI(ERROR) || t_2 == TTI(ERROR)))
			{
				target << "TYPERR: Expecting matching types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
				termLR1_2->set("i", TTI(ERROR));
			}

		ref = termLR1_2;
		termLR1(termLR1_2);
					termLR1_1->set("t", termLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", vars["mulop_1"]);
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", vars["factor_1"]);
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", vars["termLR1_1"]);
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
					int t_1 = termLR1_1->get("i");
			int t_2 = factor_1->get("t");
			Token* mul = termLR1_1->findN("mulop_1")->findT(0);
			if (t_1 == t_2)
			{
				std::string op = mul->lex;
				if (op == "*" || op == "/" || op == "div" || op == "mod")
				{
					if (t_1 == TTI(INT) || t_1 == TTI(REAL))
					{
						termLR1_2->set("i", t_1);
					}
					else if(t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching numeral types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
				else if (op == "and")
				{
					if (t_1 == TTI(BOOL))
					{
						termLR1_2->set("i", TTI(BOOL));
					}
					else if (t_1 != TTI(ERROR))
					{
						target << "TYPERR: Expecting matching boolean types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
						<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
						termLR1_2->set("i", TTI(ERROR));
					}
				}
			}
			else if (false == ( t_1 == TTI(ERROR) || t_2 == TTI(ERROR)))
			{
				target << "TYPERR: Expecting matching types, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor mulop " << mul->lex << " on line " << mul->line << " and column " << mul->charNum << "\n";
				termLR1_2->set("i", TTI(ERROR));
			}

		ref = termLR1_2;
		termLR1(termLR1_2);
					termLR1_1->set("t", termLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("-")) {
			termLR1_1->set("t", termLR1_1->get("i"));
		return;
	}
	termLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::simple_expr(ParseNode* simple_expr_1 ) {
	std::string nt = "simple_expr";
	std::string exp = "- + num ( id not";
	ParseNode* ref = simple_expr_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* sign_1 = new ParseNode(simple_expr_1,"sign", vars["sign_1"]);
		simple_expr_1->appendChild(sign_1);
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* sign_1 = new ParseNode(simple_expr_1,"sign", vars["sign_1"]);
		simple_expr_1->appendChild(sign_1);
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", vars["term_1"]);
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
					simple_exprLR1_1->set("i", term_1->get("t"));

		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
			simple_expr_1->set("t", simple_exprLR1_1->get("t"));

		return;
	}
	simple_exprError:

	SynErrorTok(nt, exp);
}

void Project2::simple_exprLR1(ParseNode* simple_exprLR1_1 ) {
	std::string nt = "simple_exprLR1";
	std::string exp = "then + - <> < ] , > ) = >= ; or end <= do else";
	ParseNode* ref = simple_exprLR1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", vars["addop_1"]);
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", vars["term_1"]);
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
					Token* add = simple_exprLR1_1->findN("addop_1")->findT(0);
			int t_1 = simple_exprLR1_1->get("i");
			int t_2 = term_1->get("t");
			if (t_1 == t_2)
			{
				if ((t_2 == TTI(REAL) || t_2 == TTI(INT)) && add->lex != "or")
				{
					simple_exprLR1_2->set("i",t_2);
				}
				else if (t_2 == TTI(BOOL) && add->lex == "or")
				{
					simple_exprLR1_2->set("i",TTI(BOOL));
				}
				else if (t_1 != TTI(ERROR))
				{
					target << "TYPERR: invalid type arguments for " << add->lex << ".\n";
					if (add->lex == "or")
						target << "Expected both booleans; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					else
						target << "Expected both numeric types; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					target << " on line " << lookAhead.line << "\n";
				}
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << add->lex << " on line " << add->line << " and column " << add->charNum << "\n";
				simple_exprLR1_2->set("i", TTI(ERROR));
			}
			else
			{
				simple_exprLR1_2->set("i", TTI(ERROR));
			}

		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		simple_exprLR1_1->set("t", simple_exprLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("or")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", vars["addop_1"]);
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", vars["term_1"]);
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
					Token* add = simple_exprLR1_1->findN("addop_1")->findT(0);
			int t_1 = simple_exprLR1_1->get("i");
			int t_2 = term_1->get("t");
			if (t_1 == t_2)
			{
				if ((t_2 == TTI(REAL) || t_2 == TTI(INT)) && add->lex != "or")
				{
					simple_exprLR1_2->set("i",t_2);
				}
				else if (t_2 == TTI(BOOL) && add->lex == "or")
				{
					simple_exprLR1_2->set("i",TTI(BOOL));
				}
				else if (t_1 != TTI(ERROR))
				{
					target << "TYPERR: invalid type arguments for " << add->lex << ".\n";
					if (add->lex == "or")
						target << "Expected both booleans; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					else
						target << "Expected both numeric types; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					target << " on line " << lookAhead.line << "\n";
				}
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << add->lex << " on line " << add->line << " and column " << add->charNum << "\n";
				simple_exprLR1_2->set("i", TTI(ERROR));
			}
			else
			{
				simple_exprLR1_2->set("i", TTI(ERROR));
			}

		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		simple_exprLR1_1->set("t", simple_exprLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", vars["addop_1"]);
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", vars["term_1"]);
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", vars["simple_exprLR1_1"]);
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
					Token* add = simple_exprLR1_1->findN("addop_1")->findT(0);
			int t_1 = simple_exprLR1_1->get("i");
			int t_2 = term_1->get("t");
			if (t_1 == t_2)
			{
				if ((t_2 == TTI(REAL) || t_2 == TTI(INT)) && add->lex != "or")
				{
					simple_exprLR1_2->set("i",t_2);
				}
				else if (t_2 == TTI(BOOL) && add->lex == "or")
				{
					simple_exprLR1_2->set("i",TTI(BOOL));
				}
				else if (t_1 != TTI(ERROR))
				{
					target << "TYPERR: invalid type arguments for " << add->lex << ".\n";
					if (add->lex == "or")
						target << "Expected both booleans; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					else
						target << "Expected both numeric types; received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2)) << "\n";
					target << " on line " << lookAhead.line << "\n";
				}
			}
			else if (t_1 != TTI(ERROR) && t_2 != TTI(ERROR))
			{
				target << "TYPERR: Expecting matching types of either both being INT or REAL, but received " << Type::typeToString(ITT(t_1)) << " and " << Type::typeToString(ITT(t_2))
				<< "\nfor relop " << add->lex << " on line " << add->line << " and column " << add->charNum << "\n";
				simple_exprLR1_2->set("i", TTI(ERROR));
			}
			else
			{
				simple_exprLR1_2->set("i", TTI(ERROR));
			}

		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		simple_exprLR1_1->set("t", simple_exprLR1_2->get("t"));

		return;
	}

	if(lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("<=")) {
			simple_exprLR1_1->set("t", simple_exprLR1_1->get("i"));
		return;
	}
	simple_exprLR1Error:

	SynErrorTok(nt, exp);
}

void Project2::factor(ParseNode* factor_1 ) {
	std::string nt = "factor";
	std::string exp = "num id ( not";
	ParseNode* ref = factor_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factorLF1_1 = new ParseNode(factor_1,"factorLF1", vars["factorLF1_1"]);
		factor_1->appendChild(factorLF1_1);
		 if (!Match(p->GTT("id") , &currTok)) goto factorError;
		factor_1->appendToken( currTok, ref);
					Token* tok = factor_1->findT("id_1");
			std::string id = tok->lex;
			if (currScope->isVarInScope(id))
			{
				std::string* err = &std::string();
				Type::TYPE foundType = currScope->getTypeOfVar(id, err);
				int t = foundType;
				factorLF1_1->set("i", t);
				if (*err != "")
				{
					std::cout << " UNEXPECTED ERR IN FACTOR: " << *err;
				}
			}
			else
			{
				target << "SEMERR : attempting to use uninitialized variable: " << id << " on line " << std::to_string(tok->line) << " char position " << std::to_string(tok->charNum) << "\n";
			}

		ref = factorLF1_1;
		factorLF1(factorLF1_1);
			factor_1->set("t", factorLF1_1->get("t")); //# place holder

		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		 if (!Match(p->GTT("num") , &currTok)) goto factorError;
		factor_1->appendToken( currTok, ref);
					Token* num = factor_1->findT("num_1");
			if (num->isInt())
				factor_1->set("t", TTI(INT));
			else if (num->isReal())
				factor_1->set("t", TTI(REAL));
			else
				factor_1->set("t", TTI(ERROR));

		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_2 = new ParseNode(factor_1,"factor", vars["factor_1"]);
		factor_1->appendChild(factor_2);
		 if (!Match(p->GTT("not") , &currTok)) goto factorError;
		factor_1->appendToken( currTok, ref);
		ref = factor_2;
		factor(factor_2);
			if (factor_2->get("t") == TTI(BOOL))
			{
				factor_1->set("t", TTI(BOOL));
			}
			else if (factor_2->get("t") != TTI(ERROR))
			{
				target << "TYPERR: ~Expected BOOL but received " << Type::typeToString(ITT(factor_2->get("t"))) << " on line " << lookAhead.line << ".\n";
				factor_1->set("t", TTI(ERROR));
			}
			else
			{
				factor_1->set("t", TTI(ERROR));
			}

		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = new ParseNode(factor_1,"expr", vars["expr_1"]);
		factor_1->appendChild(expr_1);
			expr_1->set("i", TTI(IGNORE));

		 if (!Match(p->GTT("(") , &currTok)) goto factorError;
		factor_1->appendToken( currTok, ref);
		ref = expr_1;
		expr(expr_1);
		 if (!Match(p->GTT(")") , &currTok)) goto factorError;
		factor_1->appendToken( currTok, ref);
			factor_1->set("t", expr_1->get("t"));

		return;
	}
	factorError:

	SynErrorTok(nt, exp);
}

void Project2::factorLF1(ParseNode* factorLF1_1 ) {
	std::string nt = "factorLF1";
	std::string exp = "/ <> < mod ] [ > = >= do then else + , div and * ) or ; end <= -";
	ParseNode* ref = factorLF1_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = new ParseNode(factorLF1_1,"expr", vars["expr_1"]);
		factorLF1_1->appendChild(expr_1);
		 if (!Match(p->GTT("[") , &currTok)) goto factorLF1Error;
		factorLF1_1->appendToken( currTok, ref);
					expr_1->set("i",TTI(INT));

		ref = expr_1;
		expr(expr_1);
		 if (!Match(p->GTT("]") , &currTok)) goto factorLF1Error;
		factorLF1_1->appendToken( currTok, ref);
			int t_i = factorLF1_1->get("i");
			if (false == (t_i == TTI(AINT) || t_i == TTI(AREAL) || t_i == TTI(ERROR)))
			{
				target << "TYPERR: Expected array, but received " << Type::typeToString(ITT(t_i)) << " on line " << lookAhead.line << ".\n";
				factorLF1_1->set("t", TTI(ERROR));
			}
			else
			{	
				if (expr_1->get("t") == TTI(INT))
				{
					int t_exp = 0;
					if (t_i == TTI(AINT)) t_exp = TTI(INT);
					if (t_i == TTI(AREAL)) t_exp = TTI(REAL);
					factorLF1_1->set("t", t_exp);
				}
				else
				{
					factorLF1_1->set("t", TTI(ERROR));
				}
			}

		return;
	}

	if(lookAhead.token == p->GTT("/") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("mod") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT("div") || lookAhead.token == p->GTT("and") || lookAhead.token == p->GTT("*") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("-")) {
			int t_i = factorLF1_1->get("i");
			factorLF1_1->set("t", t_i);
		return;
	}
	factorLF1Error:

	SynErrorTok(nt, exp);
}

void Project2::sign(ParseNode* sign_1 ) {
	std::string nt = "sign";
	std::string exp = "+ -";
	ParseNode* ref = sign_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("+")) {
		 if (!Match(p->GTT("+") , &currTok)) goto signError;
		sign_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		 if (!Match(p->GTT("-") , &currTok)) goto signError;
		sign_1->appendToken( currTok, ref);
		return;
	}
	signError:

	SynErrorTok(nt, exp);
}

void Project2::addop(ParseNode* addop_1 ) {
	std::string nt = "addop";
	std::string exp = "+ or -";
	ParseNode* ref = addop_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("+")) {
		 if (!Match(p->GTT("+") , &currTok)) goto addopError;
		addop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("or")) {
		 if (!Match(p->GTT("or") , &currTok)) goto addopError;
		addop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		 if (!Match(p->GTT("-") , &currTok)) goto addopError;
		addop_1->appendToken( currTok, ref);
		return;
	}
	addopError:

	SynErrorTok(nt, exp);
}

void Project2::assignop(ParseNode* assignop_1 ) {
	std::string nt = "assignop";
	std::string exp = ":=";
	ParseNode* ref = assignop_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT(":=")) {
		 if (!Match(p->GTT(":=") , &currTok)) goto assignopError;
		assignop_1->appendToken( currTok, ref);
		return;
	}
	assignopError:

	SynErrorTok(nt, exp);
}

void Project2::mulop(ParseNode* mulop_1 ) {
	std::string nt = "mulop";
	std::string exp = "div mod and * /";
	ParseNode* ref = mulop_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("/")) {
		 if (!Match(p->GTT("/") , &currTok)) goto mulopError;
		mulop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		 if (!Match(p->GTT("div") , &currTok)) goto mulopError;
		mulop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		 if (!Match(p->GTT("and") , &currTok)) goto mulopError;
		mulop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		 if (!Match(p->GTT("*") , &currTok)) goto mulopError;
		mulop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		 if (!Match(p->GTT("mod") , &currTok)) goto mulopError;
		mulop_1->appendToken( currTok, ref);
		return;
	}
	mulopError:

	SynErrorTok(nt, exp);
}

void Project2::relop(ParseNode* relop_1 ) {
	std::string nt = "relop";
	std::string exp = "> = >= <> < <=";
	ParseNode* ref = relop_1;
	Token* currTok = &Token();
	bool hasParseErr = false;
	bool* addrErr = &hasParseErr;

	if(lookAhead.token == p->GTT("<>")) {
		 if (!Match(p->GTT("<>") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		 if (!Match(p->GTT("<") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		 if (!Match(p->GTT(">") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		 if (!Match(p->GTT("=") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT(">=")) {
		 if (!Match(p->GTT(">=") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		 if (!Match(p->GTT("<=") , &currTok)) goto relopError;
		relop_1->appendToken( currTok, ref);
		return;
	}
	relopError:

	SynErrorTok(nt, exp);
}

