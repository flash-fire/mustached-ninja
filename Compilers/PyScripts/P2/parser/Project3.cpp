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

void Project2::prgm(ParseNode* prgm_1 ) {
	std::string nt = "prgm";
	std::string exp = "program";
	ParseNode* ref = prgm_1;
	if(lookAhead.token == p->GTT("program")) {
		ParseNode* idList_1 = new ParseNode(prgm_1,"idList", std::vector<std::string>());
		prgm_1->appendChild(idList_1);
		ParseNode* prgmLF1_1 = new ParseNode(prgm_1,"prgmLF1", std::vector<std::string>());
		prgm_1->appendChild(prgmLF1_1);
		prgm_1->appendToken(Match(p->GTT("program") ,nt, "program"), ref);
		prgm_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		prgm_1->appendToken(Match(p->GTT("(") ,nt, "("), ref);
		ref = idList_1;
		idList(idList_1);
		prgm_1->appendToken(Match(p->GTT(")") ,nt, ")"), ref);
		prgm_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = prgmLF1_1;
		prgmLF1(prgmLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1(ParseNode* prgmLF1_1 ) {
	std::string nt = "prgmLF1";
	std::string exp = "var procedure begin";
	ParseNode* ref = prgmLF1_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(prgmLF1_1,"subprgm_decs", std::vector<std::string>());
		prgmLF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1_1,"comp_stmt", std::vector<std::string>());
		prgmLF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		prgmLF1_1->appendToken(Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1_1,"comp_stmt", std::vector<std::string>());
		prgmLF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		prgmLF1_1->appendToken(Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = new ParseNode(prgmLF1_1,"decs", std::vector<std::string>());
		prgmLF1_1->appendChild(decs_1);
		ParseNode* prgmLF1LF1_1 = new ParseNode(prgmLF1_1,"prgmLF1LF1", std::vector<std::string>());
		prgmLF1_1->appendChild(prgmLF1LF1_1);
		ref = decs_1;
		decs(decs_1);
		ref = prgmLF1LF1_1;
		prgmLF1LF1(prgmLF1LF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1LF1(ParseNode* prgmLF1LF1_1 ) {
	std::string nt = "prgmLF1LF1";
	std::string exp = "procedure begin";
	ParseNode* ref = prgmLF1LF1_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(prgmLF1LF1_1,"subprgm_decs", std::vector<std::string>());
		prgmLF1LF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1LF1_1,"comp_stmt", std::vector<std::string>());
		prgmLF1LF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		prgmLF1LF1_1->appendToken(Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(prgmLF1LF1_1,"comp_stmt", std::vector<std::string>());
		prgmLF1LF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		prgmLF1LF1_1->appendToken(Match(p->GTT(".") ,nt, "."), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::idList(ParseNode* idList_1 ) {
	std::string nt = "idList";
	std::string exp = "id";
	ParseNode* ref = idList_1;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* idListLR1_1 = new ParseNode(idList_1,"idListLR1", std::vector<std::string>());
		idList_1->appendChild(idListLR1_1);
		idList_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = idListLR1_1;
		idListLR1(idListLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::idListLR1(ParseNode* idListLR1_1 ) {
	std::string nt = "idListLR1";
	std::string exp = ") ,";
	ParseNode* ref = idListLR1_1;
	if(lookAhead.token == p->GTT(",")) {
		ParseNode* idListLR1_2 = new ParseNode(idListLR1_1,"idListLR1", std::vector<std::string>());
		idListLR1_1->appendChild(idListLR1_2);
		idListLR1_1->appendToken(Match(p->GTT(",") ,nt, ","), ref);
		idListLR1_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = idListLR1_2;
		idListLR1(idListLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decs(ParseNode* decs_1 ) {
	std::string nt = "decs";
	std::string exp = "var";
	ParseNode* ref = decs_1;
	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = new ParseNode(decs_1,"type", std::vector<std::string>());
		decs_1->appendChild(type_1);
		ParseNode* decsLR1_1 = new ParseNode(decs_1,"decsLR1", std::vector<std::string>());
		decs_1->appendChild(decsLR1_1);
		decs_1->appendToken(Match(p->GTT("var") ,nt, "var"), ref);
		decs_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		decs_1->appendToken(Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		decs_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = decsLR1_1;
		decsLR1(decsLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decsLR1(ParseNode* decsLR1_1 ) {
	std::string nt = "decsLR1";
	std::string exp = "procedure var begin";
	ParseNode* ref = decsLR1_1;
	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = new ParseNode(decsLR1_1,"type", std::vector<std::string>());
		decsLR1_1->appendChild(type_1);
		ParseNode* decsLR1_2 = new ParseNode(decsLR1_1,"decsLR1", std::vector<std::string>());
		decsLR1_1->appendChild(decsLR1_2);
		decsLR1_1->appendToken(Match(p->GTT("var") ,nt, "var"), ref);
		decsLR1_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		decsLR1_1->appendToken(Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		decsLR1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = decsLR1_2;
		decsLR1(decsLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("procedure") || lookAhead.token == p->GTT("begin")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::type(ParseNode* type_1 ) {
	std::string nt = "type";
	std::string exp = "integer real array";
	ParseNode* ref = type_1;
	if(lookAhead.token == p->GTT("array")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", std::vector<std::string>());
		type_1->appendChild(std_type_1);
		type_1->appendToken(Match(p->GTT("array") ,nt, "array"), ref);
		type_1->appendToken(Match(p->GTT("[") ,nt, "["), ref);
		type_1->appendToken(Match(p->GTT("num") ,nt, "num"), ref);
		type_1->appendToken(Match(p->GTT("..") ,nt, ".."), ref);
		type_1->appendToken(Match(p->GTT("num") ,nt, "num"), ref);
		type_1->appendToken(Match(p->GTT("]") ,nt, "]"), ref);
		type_1->appendToken(Match(p->GTT("of") ,nt, "of"), ref);
		ref = std_type_1;
		std_type(std_type_1);
		return;
	}

	if(lookAhead.token == p->GTT("integer")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", std::vector<std::string>());
		type_1->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
			type_1->locSet("t", std_type_1->locGet("t"));
			type_1->locSet("width", std_type_1->locGet("width"));
		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		ParseNode* std_type_1 = new ParseNode(type_1,"std_type", std::vector<std::string>());
		type_1->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
			type_1->locSet("t", std_type_1->locGet("t"));
			type_1->locSet("width", std_type_1->locGet("width"));
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::std_type(ParseNode* std_type_1 ) {
	std::string nt = "std_type";
	std::string exp = "integer real";
	ParseNode* ref = std_type_1;
	if(lookAhead.token == p->GTT("integer")) {
		std_type_1->appendToken(Match(p->GTT("integer") ,nt, "integer"), ref);
			std_type_1->locSet("width", 4);
			std_type_1->locSet("t", Type::typeToInt(Type::INT));
		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		std_type_1->appendToken(Match(p->GTT("real") ,nt, "real"), ref);
			std_type_1->locSet("width", 8);
			std_type_1->locSet("t", Type::typeToInt(Type::REAL));
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decs(ParseNode* subprgm_decs_1 ) {
	std::string nt = "subprgm_decs";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_decs_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = new ParseNode(subprgm_decs_1,"subprgm_dec", std::vector<std::string>());
		subprgm_decs_1->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_1 = new ParseNode(subprgm_decs_1,"subprgm_decsLR1", std::vector<std::string>());
		subprgm_decs_1->appendChild(subprgm_decsLR1_1);
		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		subprgm_decs_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = subprgm_decsLR1_1;
		subprgm_decsLR1(subprgm_decsLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decsLR1(ParseNode* subprgm_decsLR1_1 ) {
	std::string nt = "subprgm_decsLR1";
	std::string exp = "procedure begin";
	ParseNode* ref = subprgm_decsLR1_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = new ParseNode(subprgm_decsLR1_1,"subprgm_dec", std::vector<std::string>());
		subprgm_decsLR1_1->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_2 = new ParseNode(subprgm_decsLR1_1,"subprgm_decsLR1", std::vector<std::string>());
		subprgm_decsLR1_1->appendChild(subprgm_decsLR1_2);
		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		subprgm_decsLR1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = subprgm_decsLR1_2;
		subprgm_decsLR1(subprgm_decsLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_dec(ParseNode* subprgm_dec_1 ) {
	std::string nt = "subprgm_dec";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_dec_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_head_1 = new ParseNode(subprgm_dec_1,"subprgm_head", std::vector<std::string>());
		subprgm_dec_1->appendChild(subprgm_head_1);
		ParseNode* subprgm_decLF1_1 = new ParseNode(subprgm_dec_1,"subprgm_decLF1", std::vector<std::string>());
		subprgm_dec_1->appendChild(subprgm_decLF1_1);
		ref = subprgm_head_1;
		subprgm_head(subprgm_head_1);
		ref = subprgm_decLF1_1;
		subprgm_decLF1(subprgm_decLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1(ParseNode* subprgm_decLF1_1 ) {
	std::string nt = "subprgm_decLF1";
	std::string exp = "var procedure begin";
	ParseNode* ref = subprgm_decLF1_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(subprgm_decLF1_1,"subprgm_decs", std::vector<std::string>());
		subprgm_decLF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1_1,"comp_stmt", std::vector<std::string>());
		subprgm_decLF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1_1,"comp_stmt", std::vector<std::string>());
		subprgm_decLF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = new ParseNode(subprgm_decLF1_1,"decs", std::vector<std::string>());
		subprgm_decLF1_1->appendChild(decs_1);
		ParseNode* subprgm_decLF1LF1_1 = new ParseNode(subprgm_decLF1_1,"subprgm_decLF1LF1", std::vector<std::string>());
		subprgm_decLF1_1->appendChild(subprgm_decLF1LF1_1);
		ref = decs_1;
		decs(decs_1);
		ref = subprgm_decLF1LF1_1;
		subprgm_decLF1LF1(subprgm_decLF1LF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1LF1(ParseNode* subprgm_decLF1LF1_1 ) {
	std::string nt = "subprgm_decLF1LF1";
	std::string exp = "procedure begin";
	ParseNode* ref = subprgm_decLF1LF1_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = new ParseNode(subprgm_decLF1LF1_1,"subprgm_decs", std::vector<std::string>());
		subprgm_decLF1LF1_1->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1LF1_1,"comp_stmt", std::vector<std::string>());
		subprgm_decLF1LF1_1->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(subprgm_decLF1LF1_1,"comp_stmt", std::vector<std::string>());
		subprgm_decLF1LF1_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_head(ParseNode* subprgm_head_1 ) {
	std::string nt = "subprgm_head";
	std::string exp = "procedure";
	ParseNode* ref = subprgm_head_1;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_headLF1_1 = new ParseNode(subprgm_head_1,"subprgm_headLF1", std::vector<std::string>());
		subprgm_head_1->appendChild(subprgm_headLF1_1);
		subprgm_head_1->appendToken(Match(p->GTT("procedure") ,nt, "procedure"), ref);
		subprgm_head_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = subprgm_headLF1_1;
		subprgm_headLF1(subprgm_headLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_headLF1(ParseNode* subprgm_headLF1_1 ) {
	std::string nt = "subprgm_headLF1";
	std::string exp = "( ;";
	ParseNode* ref = subprgm_headLF1_1;
	if(lookAhead.token == p->GTT(";")) {
		subprgm_headLF1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* args_1 = new ParseNode(subprgm_headLF1_1,"args", std::vector<std::string>());
		subprgm_headLF1_1->appendChild(args_1);
		ref = args_1;
		args(args_1);
		subprgm_headLF1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::args(ParseNode* args_1 ) {
	std::string nt = "args";
	std::string exp = "(";
	ParseNode* ref = args_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* param_list_1 = new ParseNode(args_1,"param_list", std::vector<std::string>());
		args_1->appendChild(param_list_1);
		args_1->appendToken(Match(p->GTT("(") ,nt, "("), ref);
		ref = param_list_1;
		param_list(param_list_1);
		args_1->appendToken(Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_list(ParseNode* param_list_1 ) {
	std::string nt = "param_list";
	std::string exp = "id";
	ParseNode* ref = param_list_1;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* type_1 = new ParseNode(param_list_1,"type", std::vector<std::string>());
		param_list_1->appendChild(type_1);
		ParseNode* param_listLR1_1 = new ParseNode(param_list_1,"param_listLR1", std::vector<std::string>());
		param_list_1->appendChild(param_listLR1_1);
		param_list_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		param_list_1->appendToken(Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		ref = param_listLR1_1;
		param_listLR1(param_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_listLR1(ParseNode* param_listLR1_1 ) {
	std::string nt = "param_listLR1";
	std::string exp = ") ;";
	ParseNode* ref = param_listLR1_1;
	if(lookAhead.token == p->GTT(";")) {
		ParseNode* type_1 = new ParseNode(param_listLR1_1,"type", std::vector<std::string>());
		param_listLR1_1->appendChild(type_1);
		ParseNode* param_listLR1_2 = new ParseNode(param_listLR1_1,"param_listLR1", std::vector<std::string>());
		param_listLR1_1->appendChild(param_listLR1_2);
		param_listLR1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		param_listLR1_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		param_listLR1_1->appendToken(Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		ref = param_listLR1_2;
		param_listLR1(param_listLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::comp_stmt(ParseNode* comp_stmt_1 ) {
	std::string nt = "comp_stmt";
	std::string exp = "begin";
	ParseNode* ref = comp_stmt_1;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmtLF1_1 = new ParseNode(comp_stmt_1,"comp_stmtLF1", std::vector<std::string>());
		comp_stmt_1->appendChild(comp_stmtLF1_1);
		comp_stmt_1->appendToken(Match(p->GTT("begin") ,nt, "begin"), ref);
		ref = comp_stmtLF1_1;
		comp_stmtLF1(comp_stmtLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::comp_stmtLF1(ParseNode* comp_stmtLF1_1 ) {
	std::string nt = "comp_stmtLF1";
	std::string exp = "if call end begin while id";
	ParseNode* ref = comp_stmtLF1_1;
	if(lookAhead.token == p->GTT("while")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", std::vector<std::string>());
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", std::vector<std::string>());
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", std::vector<std::string>());
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", std::vector<std::string>());
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* opt_stmts_1 = new ParseNode(comp_stmtLF1_1,"opt_stmts", std::vector<std::string>());
		comp_stmtLF1_1->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		comp_stmtLF1_1->appendToken(Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::opt_stmts(ParseNode* opt_stmts_1 ) {
	std::string nt = "opt_stmts";
	std::string exp = "if begin while call id";
	ParseNode* ref = opt_stmts_1;
	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", std::vector<std::string>());
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", std::vector<std::string>());
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", std::vector<std::string>());
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", std::vector<std::string>());
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_list_1 = new ParseNode(opt_stmts_1,"stmt_list", std::vector<std::string>());
		opt_stmts_1->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_list(ParseNode* stmt_list_1 ) {
	std::string nt = "stmt_list";
	std::string exp = "if begin while call id";
	ParseNode* ref = stmt_list_1;
	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_1 = new ParseNode(stmt_list_1,"stmt", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = new ParseNode(stmt_list_1,"stmt_listLR1", std::vector<std::string>());
		stmt_list_1->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_listLR1(ParseNode* stmt_listLR1_1 ) {
	std::string nt = "stmt_listLR1";
	std::string exp = "; end";
	ParseNode* ref = stmt_listLR1_1;
	if(lookAhead.token == p->GTT(";")) {
		ParseNode* stmt_1 = new ParseNode(stmt_listLR1_1,"stmt", std::vector<std::string>());
		stmt_listLR1_1->appendChild(stmt_1);
		ParseNode* stmt_listLR1_2 = new ParseNode(stmt_listLR1_1,"stmt_listLR1", std::vector<std::string>());
		stmt_listLR1_1->appendChild(stmt_listLR1_2);
		stmt_listLR1_1->appendToken(Match(p->GTT(";") ,nt, ";"), ref);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_2;
		stmt_listLR1(stmt_listLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt(ParseNode* stmt_1 ) {
	std::string nt = "stmt";
	std::string exp = "if begin while call id";
	ParseNode* ref = stmt_1;
	if(lookAhead.token == p->GTT("while")) {
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", std::vector<std::string>());
		stmt_1->appendChild(expr_1);
		ParseNode* stmt_2 = new ParseNode(stmt_1,"stmt", std::vector<std::string>());
		stmt_1->appendChild(stmt_2);
		stmt_1->appendToken(Match(p->GTT("while") ,nt, "while"), ref);
		ref = expr_1;
		expr(expr_1);
		stmt_1->appendToken(Match(p->GTT("do") ,nt, "do"), ref);
		ref = stmt_2;
		stmt(stmt_2);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = new ParseNode(stmt_1,"comp_stmt", std::vector<std::string>());
		stmt_1->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", std::vector<std::string>());
		stmt_1->appendChild(expr_1);
		ParseNode* stmt_2 = new ParseNode(stmt_1,"stmt", std::vector<std::string>());
		stmt_1->appendChild(stmt_2);
		ParseNode* stmtLF1_1 = new ParseNode(stmt_1,"stmtLF1", std::vector<std::string>());
		stmt_1->appendChild(stmtLF1_1);
		stmt_1->appendToken(Match(p->GTT("if") ,nt, "if"), ref);
		ref = expr_1;
		expr(expr_1);
		stmt_1->appendToken(Match(p->GTT("then") ,nt, "then"), ref);
		ref = stmt_2;
		stmt(stmt_2);
		ref = stmtLF1_1;
		stmtLF1(stmtLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variable_1 = new ParseNode(stmt_1,"variable", std::vector<std::string>());
		stmt_1->appendChild(variable_1);
		ParseNode* assignop_1 = new ParseNode(stmt_1,"assignop", std::vector<std::string>());
		stmt_1->appendChild(assignop_1);
		ParseNode* expr_1 = new ParseNode(stmt_1,"expr", std::vector<std::string>());
		stmt_1->appendChild(expr_1);
		ref = variable_1;
		variable(variable_1);
		ref = assignop_1;
		assignop(assignop_1);
		ref = expr_1;
		expr(expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmt_1 = new ParseNode(stmt_1,"proc_stmt", std::vector<std::string>());
		stmt_1->appendChild(proc_stmt_1);
		ref = proc_stmt_1;
		proc_stmt(proc_stmt_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmtLF1(ParseNode* stmtLF1_1 ) {
	std::string nt = "stmtLF1";
	std::string exp = "else ; end";
	ParseNode* ref = stmtLF1_1;
	if(lookAhead.token == p->GTT("else")) {
		ParseNode* stmt_1 = new ParseNode(stmtLF1_1,"stmt", std::vector<std::string>());
		stmtLF1_1->appendChild(stmt_1);
		stmtLF1_1->appendToken(Match(p->GTT("else") ,nt, "else"), ref);
		ref = stmt_1;
		stmt(stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variable(ParseNode* variable_1 ) {
	std::string nt = "variable";
	std::string exp = "id";
	ParseNode* ref = variable_1;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variableLF1_1 = new ParseNode(variable_1,"variableLF1", std::vector<std::string>());
		variable_1->appendChild(variableLF1_1);
		variable_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = variableLF1_1;
		variableLF1(variableLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variableLF1(ParseNode* variableLF1_1 ) {
	std::string nt = "variableLF1";
	std::string exp = "[ :=";
	ParseNode* ref = variableLF1_1;
	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = new ParseNode(variableLF1_1,"expr", std::vector<std::string>());
		variableLF1_1->appendChild(expr_1);
		variableLF1_1->appendToken(Match(p->GTT("[") ,nt, "["), ref);
		ref = expr_1;
		expr(expr_1);
		variableLF1_1->appendToken(Match(p->GTT("]") ,nt, "]"), ref);
		return;
	}

	if(lookAhead.token == p->GTT(":=")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmt(ParseNode* proc_stmt_1 ) {
	std::string nt = "proc_stmt";
	std::string exp = "call";
	ParseNode* ref = proc_stmt_1;
	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmtLF1_1 = new ParseNode(proc_stmt_1,"proc_stmtLF1", std::vector<std::string>());
		proc_stmt_1->appendChild(proc_stmtLF1_1);
		proc_stmt_1->appendToken(Match(p->GTT("call") ,nt, "call"), ref);
		proc_stmt_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = proc_stmtLF1_1;
		proc_stmtLF1(proc_stmtLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmtLF1(ParseNode* proc_stmtLF1_1 ) {
	std::string nt = "proc_stmtLF1";
	std::string exp = "( else ; end";
	ParseNode* ref = proc_stmtLF1_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_list_1 = new ParseNode(proc_stmtLF1_1,"expr_list", std::vector<std::string>());
		proc_stmtLF1_1->appendChild(expr_list_1);
		proc_stmtLF1_1->appendToken(Match(p->GTT("(") ,nt, "("), ref);
		ref = expr_list_1;
		expr_list(expr_list_1);
		proc_stmtLF1_1->appendToken(Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_list(ParseNode* expr_list_1 ) {
	std::string nt = "expr_list";
	std::string exp = "+ num - not ( id";
	ParseNode* ref = expr_list_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* expr_1 = new ParseNode(expr_list_1,"expr", std::vector<std::string>());
		expr_list_1->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = new ParseNode(expr_list_1,"expr_listLR1", std::vector<std::string>());
		expr_list_1->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_listLR1(ParseNode* expr_listLR1_1 ) {
	std::string nt = "expr_listLR1";
	std::string exp = ") ,";
	ParseNode* ref = expr_listLR1_1;
	if(lookAhead.token == p->GTT(",")) {
		ParseNode* expr_1 = new ParseNode(expr_listLR1_1,"expr", std::vector<std::string>());
		expr_listLR1_1->appendChild(expr_1);
		ParseNode* expr_listLR1_2 = new ParseNode(expr_listLR1_1,"expr_listLR1", std::vector<std::string>());
		expr_listLR1_1->appendChild(expr_listLR1_2);
		expr_listLR1_1->appendToken(Match(p->GTT(",") ,nt, ","), ref);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_2;
		expr_listLR1(expr_listLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr(ParseNode* expr_1 ) {
	std::string nt = "expr";
	std::string exp = "+ num - not ( id";
	ParseNode* ref = expr_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* simple_expr_1 = new ParseNode(expr_1,"simple_expr", std::vector<std::string>());
		expr_1->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = new ParseNode(expr_1,"exprLF1", std::vector<std::string>());
		expr_1->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::exprLF1(ParseNode* exprLF1_1 ) {
	std::string nt = "exprLF1";
	std::string exp = "= ) end ] do <= then ; >= > else < <> ,";
	ParseNode* ref = exprLF1_1;
	if(lookAhead.token == p->GTT(">=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		ParseNode* relop_1 = new ParseNode(exprLF1_1,"relop", std::vector<std::string>());
		exprLF1_1->appendChild(relop_1);
		ParseNode* simple_expr_1 = new ParseNode(exprLF1_1,"simple_expr", std::vector<std::string>());
		exprLF1_1->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(",")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::term(ParseNode* term_1 ) {
	std::string nt = "term";
	std::string exp = "not num ( id";
	ParseNode* ref = term_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", std::vector<std::string>());
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", std::vector<std::string>());
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", std::vector<std::string>());
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", std::vector<std::string>());
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", std::vector<std::string>());
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", std::vector<std::string>());
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* factor_1 = new ParseNode(term_1,"factor", std::vector<std::string>());
		term_1->appendChild(factor_1);
		ParseNode* termLR1_1 = new ParseNode(term_1,"termLR1", std::vector<std::string>());
		term_1->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::termLR1(ParseNode* termLR1_1 ) {
	std::string nt = "termLR1";
	std::string exp = "end or do and / >= mod ; else = + ) ] div <= then - > * < <> ,";
	ParseNode* ref = termLR1_1;
	if(lookAhead.token == p->GTT("and")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", std::vector<std::string>());
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", std::vector<std::string>());
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", std::vector<std::string>());
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("/")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", std::vector<std::string>());
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", std::vector<std::string>());
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", std::vector<std::string>());
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", std::vector<std::string>());
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", std::vector<std::string>());
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", std::vector<std::string>());
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", std::vector<std::string>());
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", std::vector<std::string>());
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", std::vector<std::string>());
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		ParseNode* mulop_1 = new ParseNode(termLR1_1,"mulop", std::vector<std::string>());
		termLR1_1->appendChild(mulop_1);
		ParseNode* factor_1 = new ParseNode(termLR1_1,"factor", std::vector<std::string>());
		termLR1_1->appendChild(factor_1);
		ParseNode* termLR1_2 = new ParseNode(termLR1_1,"termLR1", std::vector<std::string>());
		termLR1_1->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT(",")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_expr(ParseNode* simple_expr_1 ) {
	std::string nt = "simple_expr";
	std::string exp = "+ num - not ( id";
	ParseNode* ref = simple_expr_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* sign_1 = new ParseNode(simple_expr_1,"sign", std::vector<std::string>());
		simple_expr_1->appendChild(sign_1);
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* sign_1 = new ParseNode(simple_expr_1,"sign", std::vector<std::string>());
		simple_expr_1->appendChild(sign_1);
		ParseNode* term_1 = new ParseNode(simple_expr_1,"term", std::vector<std::string>());
		simple_expr_1->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = new ParseNode(simple_expr_1,"simple_exprLR1", std::vector<std::string>());
		simple_expr_1->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_exprLR1(ParseNode* simple_exprLR1_1 ) {
	std::string nt = "simple_exprLR1";
	std::string exp = "end or do >= ; else = + ) ] <= then - > < <> ,";
	ParseNode* ref = simple_exprLR1_1;
	if(lookAhead.token == p->GTT("or")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", std::vector<std::string>());
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", std::vector<std::string>());
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", std::vector<std::string>());
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", std::vector<std::string>());
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", std::vector<std::string>());
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", std::vector<std::string>());
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* addop_1 = new ParseNode(simple_exprLR1_1,"addop", std::vector<std::string>());
		simple_exprLR1_1->appendChild(addop_1);
		ParseNode* term_1 = new ParseNode(simple_exprLR1_1,"term", std::vector<std::string>());
		simple_exprLR1_1->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = new ParseNode(simple_exprLR1_1,"simple_exprLR1", std::vector<std::string>());
		simple_exprLR1_1->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT(",")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factor(ParseNode* factor_1 ) {
	std::string nt = "factor";
	std::string exp = "not num ( id";
	ParseNode* ref = factor_1;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = new ParseNode(factor_1,"expr", std::vector<std::string>());
		factor_1->appendChild(expr_1);
		factor_1->appendToken(Match(p->GTT("(") ,nt, "("), ref);
		ref = expr_1;
		expr(expr_1);
		factor_1->appendToken(Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factorLF1_1 = new ParseNode(factor_1,"factorLF1", std::vector<std::string>());
		factor_1->appendChild(factorLF1_1);
		factor_1->appendToken(Match(p->GTT("id") ,nt, "id"), ref);
		ref = factorLF1_1;
		factorLF1(factorLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_2 = new ParseNode(factor_1,"factor", std::vector<std::string>());
		factor_1->appendChild(factor_2);
		factor_1->appendToken(Match(p->GTT("not") ,nt, "not"), ref);
		ref = factor_2;
		factor(factor_2);
		return;
	}

	if(lookAhead.token == p->GTT("num")) {
		factor_1->appendToken(Match(p->GTT("num") ,nt, "num"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factorLF1(ParseNode* factorLF1_1 ) {
	std::string nt = "factorLF1";
	std::string exp = "end [ or do and / >= mod ; else = + ) ] div <= then - > * < <> ,";
	ParseNode* ref = factorLF1_1;
	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = new ParseNode(factorLF1_1,"expr", std::vector<std::string>());
		factorLF1_1->appendChild(expr_1);
		factorLF1_1->appendToken(Match(p->GTT("[") ,nt, "["), ref);
		ref = expr_1;
		expr(expr_1);
		factorLF1_1->appendToken(Match(p->GTT("]") ,nt, "]"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("and") || lookAhead.token == p->GTT("/") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("mod") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("div") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("*") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT(",")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::sign(ParseNode* sign_1 ) {
	std::string nt = "sign";
	std::string exp = "+ -";
	ParseNode* ref = sign_1;
	if(lookAhead.token == p->GTT("+")) {
		sign_1->appendToken(Match(p->GTT("+") ,nt, "+"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		sign_1->appendToken(Match(p->GTT("-") ,nt, "-"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::addop(ParseNode* addop_1 ) {
	std::string nt = "addop";
	std::string exp = "+ or -";
	ParseNode* ref = addop_1;
	if(lookAhead.token == p->GTT("or")) {
		addop_1->appendToken(Match(p->GTT("or") ,nt, "or"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		addop_1->appendToken(Match(p->GTT("+") ,nt, "+"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		addop_1->appendToken(Match(p->GTT("-") ,nt, "-"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::assignop(ParseNode* assignop_1 ) {
	std::string nt = "assignop";
	std::string exp = ":=";
	ParseNode* ref = assignop_1;
	if(lookAhead.token == p->GTT(":=")) {
		assignop_1->appendToken(Match(p->GTT(":=") ,nt, ":="), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::mulop(ParseNode* mulop_1 ) {
	std::string nt = "mulop";
	std::string exp = "* and / div mod";
	ParseNode* ref = mulop_1;
	if(lookAhead.token == p->GTT("and")) {
		mulop_1->appendToken(Match(p->GTT("and") ,nt, "and"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("/")) {
		mulop_1->appendToken(Match(p->GTT("/") ,nt, "/"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		mulop_1->appendToken(Match(p->GTT("mod") ,nt, "mod"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		mulop_1->appendToken(Match(p->GTT("div") ,nt, "div"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		mulop_1->appendToken(Match(p->GTT("*") ,nt, "*"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::relop(ParseNode* relop_1 ) {
	std::string nt = "relop";
	std::string exp = "<= = >= > < <>";
	ParseNode* ref = relop_1;
	if(lookAhead.token == p->GTT(">=")) {
		relop_1->appendToken(Match(p->GTT(">=") ,nt, ">="), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		relop_1->appendToken(Match(p->GTT("<") ,nt, "<"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		relop_1->appendToken(Match(p->GTT("=") ,nt, "="), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		relop_1->appendToken(Match(p->GTT("<=") ,nt, "<="), ref);
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		relop_1->appendToken(Match(p->GTT(">") ,nt, ">"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		relop_1->appendToken(Match(p->GTT("<>") ,nt, "<>"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

