#include "Project2.h"
#include "Project1.h"
#include<iostream>
#include "MiscFunc.h"
#include "Token.h"

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

Token Project2::Match(int T, std::string nt, std::string exp)
{
	int C = lookAhead.token;
	if (T == C && C != EOF)
	{
		std::cout << "Matched: <" << p->TokenToGrammar(T) << ">\n";
		Token ret = lookAhead;
		lookAhead = p->nextToken();
		return Token(ret);

	}
	else if (T == C && C == EOF)
	{
		return Token(lookAhead); // end parse
	}
	else
	{
		SynErrorTok(nt, exp);
		Token ret = Token();
		ret.line = lookAhead.line; // just in case i need it later.
		return ret;
	}
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
	ParseNode* root = &ParseNode(NULL, "prgm", std::list<std::string>());
	lookAhead = p->nextToken();
	prgm(root);
	MatchEOF();
}
void Project2::prgm(ParseNode* parent ) {
	std::string nt = "prgm";
	std::string exp = "program";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("program")) {
		ParseNode* idList_1 = &ParseNode(parent,"idList", std::list<std::string>());
		parent->appendChild(idList_1);
		ParseNode* prgmLF1_1 = &ParseNode(parent,"prgmLF1", std::list<std::string>());
		parent->appendChild(prgmLF1_1);
		parent->appendToken(&Match(p->GTT("program") ,nt, "program"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		parent->appendToken(&Match(p->GTT("(") ,nt, "("), ref);
		ref = idList_1;
		idList(idList_1);
		parent->appendToken(&Match(p->GTT(")") ,nt, ")"), ref);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		ref = prgmLF1_1;
		prgmLF1(prgmLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1(ParseNode* parent ) {
	std::string nt = "prgmLF1";
	std::string exp = "var begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = &ParseNode(parent,"subprgm_decs", std::list<std::string>());
		parent->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		parent->appendToken(&Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		parent->appendToken(&Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = &ParseNode(parent,"decs", std::list<std::string>());
		parent->appendChild(decs_1);
		ParseNode* prgmLF1LF1_1 = &ParseNode(parent,"prgmLF1LF1", std::list<std::string>());
		parent->appendChild(prgmLF1LF1_1);
		ref = decs_1;
		decs(decs_1);
		ref = prgmLF1LF1_1;
		prgmLF1LF1(prgmLF1LF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1LF1(ParseNode* parent ) {
	std::string nt = "prgmLF1LF1";
	std::string exp = "begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = &ParseNode(parent,"subprgm_decs", std::list<std::string>());
		parent->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		parent->appendToken(&Match(p->GTT(".") ,nt, "."), ref);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		parent->appendToken(&Match(p->GTT(".") ,nt, "."), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::idList(ParseNode* parent ) {
	std::string nt = "idList";
	std::string exp = "id";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* idListLR1_1 = &ParseNode(parent,"idListLR1", std::list<std::string>());
		parent->appendChild(idListLR1_1);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = idListLR1_1;
		idListLR1(idListLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::idListLR1(ParseNode* parent ) {
	std::string nt = "idListLR1";
	std::string exp = ", )";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(",")) {
		ParseNode* idListLR1_2 = &ParseNode(parent,"idListLR1", std::list<std::string>());
		parent->appendChild(idListLR1_2);
		parent->appendToken(&Match(p->GTT(",") ,nt, ","), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = idListLR1_2;
		idListLR1(idListLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decs(ParseNode* parent ) {
	std::string nt = "decs";
	std::string exp = "var";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = &ParseNode(parent,"type", std::list<std::string>());
		parent->appendChild(type_1);
		ParseNode* decsLR1_1 = &ParseNode(parent,"decsLR1", std::list<std::string>());
		parent->appendChild(decsLR1_1);
		parent->appendToken(&Match(p->GTT("var") ,nt, "var"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		parent->appendToken(&Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		ref = decsLR1_1;
		decsLR1(decsLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decsLR1(ParseNode* parent ) {
	std::string nt = "decsLR1";
	std::string exp = "var begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("var")) {
		ParseNode* type_1 = &ParseNode(parent,"type", std::list<std::string>());
		parent->appendChild(type_1);
		ParseNode* decsLR1_2 = &ParseNode(parent,"decsLR1", std::list<std::string>());
		parent->appendChild(decsLR1_2);
		parent->appendToken(&Match(p->GTT("var") ,nt, "var"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		parent->appendToken(&Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		ref = decsLR1_2;
		decsLR1(decsLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("begin") || lookAhead.token == p->GTT("procedure")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::type(ParseNode* parent ) {
	std::string nt = "type";
	std::string exp = "real array integer";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("integer")) {
		ParseNode* std_type_1 = &ParseNode(parent,"std_type", std::list<std::string>());
		parent->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
		return;
	}

	if(lookAhead.token == p->GTT("array")) {
		ParseNode* std_type_1 = &ParseNode(parent,"std_type", std::list<std::string>());
		parent->appendChild(std_type_1);
		parent->appendToken(&Match(p->GTT("array") ,nt, "array"), ref);
		parent->appendToken(&Match(p->GTT("[") ,nt, "["), ref);
		parent->appendToken(&Match(p->GTT("num") ,nt, "num"), ref);
		parent->appendToken(&Match(p->GTT("..") ,nt, ".."), ref);
		parent->appendToken(&Match(p->GTT("num") ,nt, "num"), ref);
		parent->appendToken(&Match(p->GTT("]") ,nt, "]"), ref);
		parent->appendToken(&Match(p->GTT("of") ,nt, "of"), ref);
		ref = std_type_1;
		std_type(std_type_1);
		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		ParseNode* std_type_1 = &ParseNode(parent,"std_type", std::list<std::string>());
		parent->appendChild(std_type_1);
		ref = std_type_1;
		std_type(std_type_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::std_type(ParseNode* parent ) {
	std::string nt = "std_type";
	std::string exp = "real integer";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("integer")) {
		parent->appendToken(&Match(p->GTT("integer") ,nt, "integer"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("real")) {
		parent->appendToken(&Match(p->GTT("real") ,nt, "real"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decs(ParseNode* parent ) {
	std::string nt = "subprgm_decs";
	std::string exp = "procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = &ParseNode(parent,"subprgm_dec", std::list<std::string>());
		parent->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_1 = &ParseNode(parent,"subprgm_decsLR1", std::list<std::string>());
		parent->appendChild(subprgm_decsLR1_1);
		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		ref = subprgm_decsLR1_1;
		subprgm_decsLR1(subprgm_decsLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decsLR1(ParseNode* parent ) {
	std::string nt = "subprgm_decsLR1";
	std::string exp = "begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_dec_1 = &ParseNode(parent,"subprgm_dec", std::list<std::string>());
		parent->appendChild(subprgm_dec_1);
		ParseNode* subprgm_decsLR1_2 = &ParseNode(parent,"subprgm_decsLR1", std::list<std::string>());
		parent->appendChild(subprgm_decsLR1_2);
		ref = subprgm_dec_1;
		subprgm_dec(subprgm_dec_1);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		ref = subprgm_decsLR1_2;
		subprgm_decsLR1(subprgm_decsLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_dec(ParseNode* parent ) {
	std::string nt = "subprgm_dec";
	std::string exp = "procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_head_1 = &ParseNode(parent,"subprgm_head", std::list<std::string>());
		parent->appendChild(subprgm_head_1);
		ParseNode* subprgm_decLF1_1 = &ParseNode(parent,"subprgm_decLF1", std::list<std::string>());
		parent->appendChild(subprgm_decLF1_1);
		ref = subprgm_head_1;
		subprgm_head(subprgm_head_1);
		ref = subprgm_decLF1_1;
		subprgm_decLF1(subprgm_decLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1(ParseNode* parent ) {
	std::string nt = "subprgm_decLF1";
	std::string exp = "var begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = &ParseNode(parent,"subprgm_decs", std::list<std::string>());
		parent->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("var")) {
		ParseNode* decs_1 = &ParseNode(parent,"decs", std::list<std::string>());
		parent->appendChild(decs_1);
		ParseNode* subprgm_decLF1LF1_1 = &ParseNode(parent,"subprgm_decLF1LF1", std::list<std::string>());
		parent->appendChild(subprgm_decLF1LF1_1);
		ref = decs_1;
		decs(decs_1);
		ref = subprgm_decLF1LF1_1;
		subprgm_decLF1LF1(subprgm_decLF1LF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1LF1(ParseNode* parent ) {
	std::string nt = "subprgm_decLF1LF1";
	std::string exp = "begin procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_decs_1 = &ParseNode(parent,"subprgm_decs", std::list<std::string>());
		parent->appendChild(subprgm_decs_1);
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = subprgm_decs_1;
		subprgm_decs(subprgm_decs_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_head(ParseNode* parent ) {
	std::string nt = "subprgm_head";
	std::string exp = "procedure";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("procedure")) {
		ParseNode* subprgm_headLF1_1 = &ParseNode(parent,"subprgm_headLF1", std::list<std::string>());
		parent->appendChild(subprgm_headLF1_1);
		parent->appendToken(&Match(p->GTT("procedure") ,nt, "procedure"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = subprgm_headLF1_1;
		subprgm_headLF1(subprgm_headLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_headLF1(ParseNode* parent ) {
	std::string nt = "subprgm_headLF1";
	std::string exp = "( ;";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(";")) {
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* args_1 = &ParseNode(parent,"args", std::list<std::string>());
		parent->appendChild(args_1);
		ref = args_1;
		args(args_1);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::args(ParseNode* parent ) {
	std::string nt = "args";
	std::string exp = "(";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* param_list_1 = &ParseNode(parent,"param_list", std::list<std::string>());
		parent->appendChild(param_list_1);
		parent->appendToken(&Match(p->GTT("(") ,nt, "("), ref);
		ref = param_list_1;
		param_list(param_list_1);
		parent->appendToken(&Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_list(ParseNode* parent ) {
	std::string nt = "param_list";
	std::string exp = "id";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* type_1 = &ParseNode(parent,"type", std::list<std::string>());
		parent->appendChild(type_1);
		ParseNode* param_listLR1_1 = &ParseNode(parent,"param_listLR1", std::list<std::string>());
		parent->appendChild(param_listLR1_1);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		parent->appendToken(&Match(p->GTT(":") ,nt, ":"), ref);
		ref = type_1;
		type(type_1);
		ref = param_listLR1_1;
		param_listLR1(param_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_listLR1(ParseNode* parent ) {
	std::string nt = "param_listLR1";
	std::string exp = ") ;";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(";")) {
		ParseNode* type_1 = &ParseNode(parent,"type", std::list<std::string>());
		parent->appendChild(type_1);
		ParseNode* param_listLR1_2 = &ParseNode(parent,"param_listLR1", std::list<std::string>());
		parent->appendChild(param_listLR1_2);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		parent->appendToken(&Match(p->GTT(":") ,nt, ":"), ref);
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

void Project2::comp_stmt(ParseNode* parent ) {
	std::string nt = "comp_stmt";
	std::string exp = "begin";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmtLF1_1 = &ParseNode(parent,"comp_stmtLF1", std::list<std::string>());
		parent->appendChild(comp_stmtLF1_1);
		parent->appendToken(&Match(p->GTT("begin") ,nt, "begin"), ref);
		ref = comp_stmtLF1_1;
		comp_stmtLF1(comp_stmtLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::comp_stmtLF1(ParseNode* parent ) {
	std::string nt = "comp_stmtLF1";
	std::string exp = "begin id if while call end";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* opt_stmts_1 = &ParseNode(parent,"opt_stmts", std::list<std::string>());
		parent->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* opt_stmts_1 = &ParseNode(parent,"opt_stmts", std::list<std::string>());
		parent->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* opt_stmts_1 = &ParseNode(parent,"opt_stmts", std::list<std::string>());
		parent->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* opt_stmts_1 = &ParseNode(parent,"opt_stmts", std::list<std::string>());
		parent->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* opt_stmts_1 = &ParseNode(parent,"opt_stmts", std::list<std::string>());
		parent->appendChild(opt_stmts_1);
		ref = opt_stmts_1;
		opt_stmts(opt_stmts_1);
		parent->appendToken(&Match(p->GTT("end") ,nt, "end"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::opt_stmts(ParseNode* parent ) {
	std::string nt = "opt_stmts";
	std::string exp = "begin while call id if";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_list_1 = &ParseNode(parent,"stmt_list", std::list<std::string>());
		parent->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_list_1 = &ParseNode(parent,"stmt_list", std::list<std::string>());
		parent->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_list_1 = &ParseNode(parent,"stmt_list", std::list<std::string>());
		parent->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_list_1 = &ParseNode(parent,"stmt_list", std::list<std::string>());
		parent->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_list_1 = &ParseNode(parent,"stmt_list", std::list<std::string>());
		parent->appendChild(stmt_list_1);
		ref = stmt_list_1;
		stmt_list(stmt_list_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_list(ParseNode* parent ) {
	std::string nt = "stmt_list";
	std::string exp = "begin while call id if";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_1 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_1);
		ref = stmt_1;
		stmt(stmt_1);
		ref = stmt_listLR1_1;
		stmt_listLR1(stmt_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_listLR1(ParseNode* parent ) {
	std::string nt = "stmt_listLR1";
	std::string exp = "end ;";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(";")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		ParseNode* stmt_listLR1_2 = &ParseNode(parent,"stmt_listLR1", std::list<std::string>());
		parent->appendChild(stmt_listLR1_2);
		parent->appendToken(&Match(p->GTT(";") ,nt, ";"), ref);
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

void Project2::stmt(ParseNode* parent ) {
	std::string nt = "stmt";
	std::string exp = "begin while call id if";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("begin")) {
		ParseNode* comp_stmt_1 = &ParseNode(parent,"comp_stmt", std::list<std::string>());
		parent->appendChild(comp_stmt_1);
		ref = comp_stmt_1;
		comp_stmt(comp_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variable_1 = &ParseNode(parent,"variable", std::list<std::string>());
		parent->appendChild(variable_1);
		ParseNode* assignop_1 = &ParseNode(parent,"assignop", std::list<std::string>());
		parent->appendChild(assignop_1);
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ref = variable_1;
		variable(variable_1);
		ref = assignop_1;
		assignop(assignop_1);
		ref = expr_1;
		expr(expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* stmt_2 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_2);
		ParseNode* stmtLF1_1 = &ParseNode(parent,"stmtLF1", std::list<std::string>());
		parent->appendChild(stmtLF1_1);
		parent->appendToken(&Match(p->GTT("if") ,nt, "if"), ref);
		ref = expr_1;
		expr(expr_1);
		parent->appendToken(&Match(p->GTT("then") ,nt, "then"), ref);
		ref = stmt_2;
		stmt(stmt_2);
		ref = stmtLF1_1;
		stmtLF1(stmtLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmt_1 = &ParseNode(parent,"proc_stmt", std::list<std::string>());
		parent->appendChild(proc_stmt_1);
		ref = proc_stmt_1;
		proc_stmt(proc_stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* stmt_2 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_2);
		parent->appendToken(&Match(p->GTT("while") ,nt, "while"), ref);
		ref = expr_1;
		expr(expr_1);
		parent->appendToken(&Match(p->GTT("do") ,nt, "do"), ref);
		ref = stmt_2;
		stmt(stmt_2);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmtLF1(ParseNode* parent ) {
	std::string nt = "stmtLF1";
	std::string exp = "else end ;";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("else")) {
		ParseNode* stmt_1 = &ParseNode(parent,"stmt", std::list<std::string>());
		parent->appendChild(stmt_1);
		parent->appendToken(&Match(p->GTT("else") ,nt, "else"), ref);
		ref = stmt_1;
		stmt(stmt_1);
		return;
	}

	if(lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT(";")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variable(ParseNode* parent ) {
	std::string nt = "variable";
	std::string exp = "id";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("id")) {
		ParseNode* variableLF1_1 = &ParseNode(parent,"variableLF1", std::list<std::string>());
		parent->appendChild(variableLF1_1);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = variableLF1_1;
		variableLF1(variableLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variableLF1(ParseNode* parent ) {
	std::string nt = "variableLF1";
	std::string exp = "[ :=";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		parent->appendToken(&Match(p->GTT("[") ,nt, "["), ref);
		ref = expr_1;
		expr(expr_1);
		parent->appendToken(&Match(p->GTT("]") ,nt, "]"), ref);
		return;
	}

	if(lookAhead.token == p->GTT(":=")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmt(ParseNode* parent ) {
	std::string nt = "proc_stmt";
	std::string exp = "call";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("call")) {
		ParseNode* proc_stmtLF1_1 = &ParseNode(parent,"proc_stmtLF1", std::list<std::string>());
		parent->appendChild(proc_stmtLF1_1);
		parent->appendToken(&Match(p->GTT("call") ,nt, "call"), ref);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = proc_stmtLF1_1;
		proc_stmtLF1(proc_stmtLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmtLF1(ParseNode* parent ) {
	std::string nt = "proc_stmtLF1";
	std::string exp = "else ( end ;";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_list_1 = &ParseNode(parent,"expr_list", std::list<std::string>());
		parent->appendChild(expr_list_1);
		parent->appendToken(&Match(p->GTT("(") ,nt, "("), ref);
		ref = expr_list_1;
		expr_list(expr_list_1);
		parent->appendToken(&Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT(";")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_list(ParseNode* parent ) {
	std::string nt = "expr_list";
	std::string exp = "- id + ( not num";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("num")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_1 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_1);
		ref = expr_1;
		expr(expr_1);
		ref = expr_listLR1_1;
		expr_listLR1(expr_listLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_listLR1(ParseNode* parent ) {
	std::string nt = "expr_listLR1";
	std::string exp = ", )";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(",")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		ParseNode* expr_listLR1_2 = &ParseNode(parent,"expr_listLR1", std::list<std::string>());
		parent->appendChild(expr_listLR1_2);
		parent->appendToken(&Match(p->GTT(",") ,nt, ","), ref);
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

void Project2::expr(ParseNode* parent ) {
	std::string nt = "expr";
	std::string exp = "- id + ( not num";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("num")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ParseNode* exprLF1_1 = &ParseNode(parent,"exprLF1", std::list<std::string>());
		parent->appendChild(exprLF1_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		ref = exprLF1_1;
		exprLF1(exprLF1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::exprLF1(ParseNode* parent ) {
	std::string nt = "exprLF1";
	std::string exp = "else >= , then ; = ] > < ) <> <= do end";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(">=")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		ParseNode* relop_1 = &ParseNode(parent,"relop", std::list<std::string>());
		parent->appendChild(relop_1);
		ParseNode* simple_expr_1 = &ParseNode(parent,"simple_expr", std::list<std::string>());
		parent->appendChild(simple_expr_1);
		ref = relop_1;
		relop(relop_1);
		ref = simple_expr_1;
		simple_expr(simple_expr_1);
		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::term(ParseNode* parent ) {
	std::string nt = "term";
	std::string exp = "( not num id";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("num")) {
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_1 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_1 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_1 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_1 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_1;
		termLR1(termLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::termLR1(ParseNode* parent ) {
	std::string nt = "termLR1";
	std::string exp = "/ and else >= , * ; = > < <> <= do end - mod + then div or ] )";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("/")) {
		ParseNode* mulop_1 = &ParseNode(parent,"mulop", std::list<std::string>());
		parent->appendChild(mulop_1);
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_2 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		ParseNode* mulop_1 = &ParseNode(parent,"mulop", std::list<std::string>());
		parent->appendChild(mulop_1);
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_2 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		ParseNode* mulop_1 = &ParseNode(parent,"mulop", std::list<std::string>());
		parent->appendChild(mulop_1);
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_2 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		ParseNode* mulop_1 = &ParseNode(parent,"mulop", std::list<std::string>());
		parent->appendChild(mulop_1);
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_2 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		ParseNode* mulop_1 = &ParseNode(parent,"mulop", std::list<std::string>());
		parent->appendChild(mulop_1);
		ParseNode* factor_1 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_1);
		ParseNode* termLR1_2 = &ParseNode(parent,"termLR1", std::list<std::string>());
		parent->appendChild(termLR1_2);
		ref = mulop_1;
		mulop(mulop_1);
		ref = factor_1;
		factor(factor_1);
		ref = termLR1_2;
		termLR1(termLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_expr(ParseNode* parent ) {
	std::string nt = "simple_expr";
	std::string exp = "id - + ( not num";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("num")) {
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* sign_1 = &ParseNode(parent,"sign", std::list<std::string>());
		parent->appendChild(sign_1);
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* sign_1 = &ParseNode(parent,"sign", std::list<std::string>());
		parent->appendChild(sign_1);
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = sign_1;
		sign(sign_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_1 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_1;
		simple_exprLR1(simple_exprLR1_1);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_exprLR1(ParseNode* parent ) {
	std::string nt = "simple_exprLR1";
	std::string exp = "else >= , ; = > < <> <= do end - + then or ] )";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("or")) {
		ParseNode* addop_1 = &ParseNode(parent,"addop", std::list<std::string>());
		parent->appendChild(addop_1);
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		ParseNode* addop_1 = &ParseNode(parent,"addop", std::list<std::string>());
		parent->appendChild(addop_1);
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		ParseNode* addop_1 = &ParseNode(parent,"addop", std::list<std::string>());
		parent->appendChild(addop_1);
		ParseNode* term_1 = &ParseNode(parent,"term", std::list<std::string>());
		parent->appendChild(term_1);
		ParseNode* simple_exprLR1_2 = &ParseNode(parent,"simple_exprLR1", std::list<std::string>());
		parent->appendChild(simple_exprLR1_2);
		ref = addop_1;
		addop(addop_1);
		ref = term_1;
		term(term_1);
		ref = simple_exprLR1_2;
		simple_exprLR1(simple_exprLR1_2);
		return;
	}

	if(lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factor(ParseNode* parent ) {
	std::string nt = "factor";
	std::string exp = "( not num id";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("num")) {
		parent->appendToken(&Match(p->GTT("num") ,nt, "num"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		ParseNode* factor_2 = &ParseNode(parent,"factor", std::list<std::string>());
		parent->appendChild(factor_2);
		parent->appendToken(&Match(p->GTT("not") ,nt, "not"), ref);
		ref = factor_2;
		factor(factor_2);
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		ParseNode* factorLF1_1 = &ParseNode(parent,"factorLF1", std::list<std::string>());
		parent->appendChild(factorLF1_1);
		parent->appendToken(&Match(p->GTT("id") ,nt, "id"), ref);
		ref = factorLF1_1;
		factorLF1(factorLF1_1);
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		parent->appendToken(&Match(p->GTT("(") ,nt, "("), ref);
		ref = expr_1;
		expr(expr_1);
		parent->appendToken(&Match(p->GTT(")") ,nt, ")"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factorLF1(ParseNode* parent ) {
	std::string nt = "factorLF1";
	std::string exp = "/ and else >= , * ; = > < <> <= do end - mod + then div [ or ] )";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("[")) {
		ParseNode* expr_1 = &ParseNode(parent,"expr", std::list<std::string>());
		parent->appendChild(expr_1);
		parent->appendToken(&Match(p->GTT("[") ,nt, "["), ref);
		ref = expr_1;
		expr(expr_1);
		parent->appendToken(&Match(p->GTT("]") ,nt, "]"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("/") || lookAhead.token == p->GTT("and") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT("*") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT("mod") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("div") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::sign(ParseNode* parent ) {
	std::string nt = "sign";
	std::string exp = "+ -";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("-")) {
		parent->appendToken(&Match(p->GTT("-") ,nt, "-"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		parent->appendToken(&Match(p->GTT("+") ,nt, "+"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::addop(ParseNode* parent ) {
	std::string nt = "addop";
	std::string exp = "+ or -";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("or")) {
		parent->appendToken(&Match(p->GTT("or") ,nt, "or"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		parent->appendToken(&Match(p->GTT("-") ,nt, "-"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		parent->appendToken(&Match(p->GTT("+") ,nt, "+"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::assignop(ParseNode* parent ) {
	std::string nt = "assignop";
	std::string exp = ":=";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(":=")) {
		parent->appendToken(&Match(p->GTT(":=") ,nt, ":="), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::mulop(ParseNode* parent ) {
	std::string nt = "mulop";
	std::string exp = "div and / * mod";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT("/")) {
		parent->appendToken(&Match(p->GTT("/") ,nt, "/"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		parent->appendToken(&Match(p->GTT("and") ,nt, "and"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		parent->appendToken(&Match(p->GTT("*") ,nt, "*"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("mod")) {
		parent->appendToken(&Match(p->GTT("mod") ,nt, "mod"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		parent->appendToken(&Match(p->GTT("div") ,nt, "div"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::relop(ParseNode* parent ) {
	std::string nt = "relop";
	std::string exp = "= > < <> <= >=";
	ParseNode* ref = parent;
	if(lookAhead.token == p->GTT(">=")) {
		parent->appendToken(&Match(p->GTT(">=") ,nt, ">="), ref);
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		parent->appendToken(&Match(p->GTT("=") ,nt, "="), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		parent->appendToken(&Match(p->GTT("<") ,nt, "<"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		parent->appendToken(&Match(p->GTT("<>") ,nt, "<>"), ref);
		return;
	}

	if(lookAhead.token == p->GTT("<=")) {
		parent->appendToken(&Match(p->GTT("<=") ,nt, "<="), ref);
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		parent->appendToken(&Match(p->GTT(">") ,nt, ">"), ref);
		return;
	}
	SynErrorTok(nt, exp);
}

