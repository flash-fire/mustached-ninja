#include "Project2.h"
#include "Project1.h"
#include<iostream>
#include "MiscFunc.h"
#include "Token.h"

const std::string Project2::SYNCH_PATH = "synch.txt";

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


void Project2::Match(int T, std::string nt, std::string exp)
{
	int C = lookAhead.token;
	if (T == C && C != EOF)
	{
		std::cout << "Matched: <" << p->TokenToGrammar(T) << ">\n";
		lookAhead = p->nextToken();
	}
	else if (T == C && C == EOF)
	{
		return; // end parse
	}
	else
	{
		SynErrorTok(nt, exp);
	}
}

void Project2::SynErrorTok(std::string nt, std::string exp)
{
	target << "SYNERR at line: " << lookAhead.line << " char pos " << lookAhead.charNum
		<< " NonTerminal: <" << nt
		<< "> \nEncountered: <" << lookAhead.lex << "> Expected one of: <"
		<< exp << ">\n\n";
	target.flush();
	if (exp == "EOF")
	{
		return;
	}
	UniteSynchronization(nt);
}

void Project2::UniteSynchronization(std::string nt)
{
	bool recover = false;
	std::cout << "In UniteSynchronization attempting to match one of <" << synchSet[nt] << ">\n";
	while (p->hasToken())
	{
		if (p->GTT(lookAhead.gram()) == p->GTT("+") && contains(synchSet[nt], "addop"))
		{
			recover = true;
			break;
		}
		else if (!contains(synchSet[nt], lookAhead.gram()))
		{
			lookAhead = p->nextToken();
		}
		else
		{
			recover = true;
			break;
		}
	}
	if (recover == false && !contains(synchSet[nt], lookAhead.gram()))
	{
		target << "Reached EOF while trying to synchronize NT <" << nt
			<< "> \nExpecting one of following <" << synchSet[nt] << ">\n";
		target.flush();
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
	lookAhead = p->nextToken();
	prgm();
	MatchEOF();
}
void Project2::prgm() {
	std::string nt = "prgm";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("program")) {
		Match(p->GTT("program") ,nt, "program");
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT("(") ,nt, "(");
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT(")") ,nt, ")");
		Match(p->GTT(";") ,nt, ";");
		prgmLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1() {
	std::string nt = "prgmLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("var")) {
		decs();
		prgmLF1LF1();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		comp_stmt();
		Match(p->GTT(".") ,nt, ".");
		return;
	}

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_decs();
		comp_stmt();
		Match(p->GTT(".") ,nt, ".");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::prgmLF1LF1() {
	std::string nt = "prgmLF1LF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("begin")) {
		comp_stmt();
		Match(p->GTT(".") ,nt, ".");
		return;
	}

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_decs();
		comp_stmt();
		Match(p->GTT(".") ,nt, ".");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decs() {
	std::string nt = "decs";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("var")) {
		Match(p->GTT("var") ,nt, "var");
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT(":") ,nt, ":");
		type();
		Match(p->GTT(";") ,nt, ";");
		decsLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::decsLR1() {
	std::string nt = "decsLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("var")) {
		Match(p->GTT("var") ,nt, "var");
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT(":") ,nt, ":");
		type();
		Match(p->GTT(";") ,nt, ";");
		decsLR1();
		return;
	}

	if(lookAhead.token == p->GTT("procedure") || lookAhead.token == p->GTT("begin")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::type() {
	std::string nt = "type";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("real")) {
		std_type();
		return;
	}

	if(lookAhead.token == p->GTT("array")) {
		Match(p->GTT("array") ,nt, "array");
		Match(p->GTT("[") ,nt, "[");
		Match(p->GTT("num") ,nt, "num");
		Match(p->GTT("..") ,nt, "..");
		Match(p->GTT("num") ,nt, "num");
		Match(p->GTT("]") ,nt, "]");
		Match(p->GTT("of") ,nt, "of");
		std_type();
		return;
	}

	if(lookAhead.token == p->GTT("integer")) {
		std_type();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::std_type() {
	std::string nt = "std_type";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("real")) {
		Match(p->GTT("real") ,nt, "real");
		return;
	}

	if(lookAhead.token == p->GTT("integer")) {
		Match(p->GTT("integer") ,nt, "integer");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decs() {
	std::string nt = "subprgm_decs";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_dec();
		Match(p->GTT(";") ,nt, ";");
		subprgm_decsLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decsLR1() {
	std::string nt = "subprgm_decsLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_dec();
		Match(p->GTT(";") ,nt, ";");
		subprgm_decsLR1();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_dec() {
	std::string nt = "subprgm_dec";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_head();
		subprgm_decLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1() {
	std::string nt = "subprgm_decLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("var")) {
		decs();
		subprgm_decLF1LF1();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		comp_stmt();
		return;
	}

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_decs();
		comp_stmt();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_decLF1LF1() {
	std::string nt = "subprgm_decLF1LF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("begin")) {
		comp_stmt();
		return;
	}

	if(lookAhead.token == p->GTT("procedure")) {
		subprgm_decs();
		comp_stmt();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_head() {
	std::string nt = "subprgm_head";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("procedure")) {
		Match(p->GTT("procedure") ,nt, "procedure");
		Match(p->GTT("id") ,nt, "id");
		subprgm_headLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::subprgm_headLF1() {
	std::string nt = "subprgm_headLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT(";")) {
		Match(p->GTT(";") ,nt, ";");
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		args();
		Match(p->GTT(";") ,nt, ";");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::args() {
	std::string nt = "args";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("(")) {
		Match(p->GTT("(") ,nt, "(");
		param_list();
		Match(p->GTT(")") ,nt, ")");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_list() {
	std::string nt = "param_list";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT(":") ,nt, ":");
		type();
		param_listLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::param_listLR1() {
	std::string nt = "param_listLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT(";")) {
		Match(p->GTT(";") ,nt, ";");
		Match(p->GTT("id") ,nt, "id");
		Match(p->GTT(":") ,nt, ":");
		type();
		param_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::comp_stmt() {
	std::string nt = "comp_stmt";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("begin")) {
		Match(p->GTT("begin") ,nt, "begin");
		comp_stmtLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::comp_stmtLF1() {
	std::string nt = "comp_stmtLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		opt_stmts();
		Match(p->GTT("end") ,nt, "end");
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		opt_stmts();
		Match(p->GTT("end") ,nt, "end");
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		opt_stmts();
		Match(p->GTT("end") ,nt, "end");
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		opt_stmts();
		Match(p->GTT("end") ,nt, "end");
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		opt_stmts();
		Match(p->GTT("end") ,nt, "end");
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		Match(p->GTT("end") ,nt, "end");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::opt_stmts() {
	std::string nt = "opt_stmts";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		stmt_list();
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		stmt_list();
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		stmt_list();
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		stmt_list();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		stmt_list();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_list() {
	std::string nt = "stmt_list";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		stmt();
		stmt_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		stmt();
		stmt_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		stmt();
		stmt_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		stmt();
		stmt_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		stmt();
		stmt_listLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt_listLR1() {
	std::string nt = "stmt_listLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT(";")) {
		Match(p->GTT(";") ,nt, ";");
		stmt();
		stmt_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmt() {
	std::string nt = "stmt";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		variable();
		assignop();
		expr();
		return;
	}

	if(lookAhead.token == p->GTT("if")) {
		Match(p->GTT("if") ,nt, "if");
		expr();
		Match(p->GTT("then") ,nt, "then");
		stmt();
		stmtLF1();
		return;
	}

	if(lookAhead.token == p->GTT("while")) {
		Match(p->GTT("while") ,nt, "while");
		expr();
		Match(p->GTT("do") ,nt, "do");
		stmt();
		return;
	}

	if(lookAhead.token == p->GTT("call")) {
		proc_stmt();
		return;
	}

	if(lookAhead.token == p->GTT("begin")) {
		comp_stmt();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::stmtLF1() {
	std::string nt = "stmtLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("else")) {
		Match(p->GTT("else") ,nt, "else");
		stmt();
		return;
	}

	if(lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variable() {
	std::string nt = "variable";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("id")) {
		Match(p->GTT("id") ,nt, "id");
		variableLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::variableLF1() {
	std::string nt = "variableLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("[")) {
		Match(p->GTT("[") ,nt, "[");
		expr();
		Match(p->GTT("]") ,nt, "]");
		return;
	}

	if(lookAhead.token == p->GTT(":=")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmt() {
	std::string nt = "proc_stmt";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("call")) {
		Match(p->GTT("call") ,nt, "call");
		Match(p->GTT("id") ,nt, "id");
		proc_stmtLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::proc_stmtLF1() {
	std::string nt = "proc_stmtLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("(")) {
		Match(p->GTT("(") ,nt, "(");
		expr_list();
		Match(p->GTT(")") ,nt, ")");
		return;
	}

	if(lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("end")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_list() {
	std::string nt = "expr_list";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("num")) {
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		expr();
		expr_listLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr_listLR1() {
	std::string nt = "expr_listLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT(",")) {
		Match(p->GTT(",") ,nt, ",");
		expr();
		expr_listLR1();
		return;
	}

	if(lookAhead.token == p->GTT(")")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::expr() {
	std::string nt = "expr";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("num")) {
		simple_expr();
		exprLF1();
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		simple_expr();
		exprLF1();
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		simple_expr();
		exprLF1();
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		simple_expr();
		exprLF1();
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		simple_expr();
		exprLF1();
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		simple_expr();
		exprLF1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::exprLF1() {
	std::string nt = "exprLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("<=")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT(">=")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		relop();
		simple_expr();
		return;
	}

	if(lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT(",")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::term() {
	std::string nt = "term";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("num")) {
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		factor();
		termLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::termLR1() {
	std::string nt = "termLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("mod")) {
		mulop();
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("/")) {
		mulop();
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		mulop();
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		mulop();
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		mulop();
		factor();
		termLR1();
		return;
	}

	if(lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("<")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_expr() {
	std::string nt = "simple_expr";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("num")) {
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("-")) {
		sign();
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		sign();
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		term();
		simple_exprLR1();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::simple_exprLR1() {
	std::string nt = "simple_exprLR1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("-")) {
		addop();
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		addop();
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("or")) {
		addop();
		term();
		simple_exprLR1();
		return;
	}

	if(lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("<")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factor() {
	std::string nt = "factor";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("num")) {
		Match(p->GTT("num") ,nt, "num");
		return;
	}

	if(lookAhead.token == p->GTT("id")) {
		Match(p->GTT("id") ,nt, "id");
		factorLF1();
		return;
	}

	if(lookAhead.token == p->GTT("(")) {
		Match(p->GTT("(") ,nt, "(");
		expr();
		Match(p->GTT(")") ,nt, ")");
		return;
	}

	if(lookAhead.token == p->GTT("not")) {
		Match(p->GTT("not") ,nt, "not");
		factor();
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::factorLF1() {
	std::string nt = "factorLF1";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("[")) {
		Match(p->GTT("[") ,nt, "[");
		expr();
		Match(p->GTT("]") ,nt, "]");
		return;
	}

	if(lookAhead.token == p->GTT("mod") || lookAhead.token == p->GTT("]") || lookAhead.token == p->GTT("<=") || lookAhead.token == p->GTT("<>") || lookAhead.token == p->GTT("=") || lookAhead.token == p->GTT(">") || lookAhead.token == p->GTT("-") || lookAhead.token == p->GTT(";") || lookAhead.token == p->GTT("do") || lookAhead.token == p->GTT("/") || lookAhead.token == p->GTT("+") || lookAhead.token == p->GTT("else") || lookAhead.token == p->GTT("then") || lookAhead.token == p->GTT("div") || lookAhead.token == p->GTT(")") || lookAhead.token == p->GTT("or") || lookAhead.token == p->GTT(",") || lookAhead.token == p->GTT(">=") || lookAhead.token == p->GTT("end") || lookAhead.token == p->GTT("and") || lookAhead.token == p->GTT("<") || lookAhead.token == p->GTT("*")) {
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::sign() {
	std::string nt = "sign";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("-")) {
		Match(p->GTT("-") ,nt, "-");
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		Match(p->GTT("+") ,nt, "+");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::addop() {
	std::string nt = "addop";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("-")) {
		Match(p->GTT("-") ,nt, "-");
		return;
	}

	if(lookAhead.token == p->GTT("+")) {
		Match(p->GTT("+") ,nt, "+");
		return;
	}

	if(lookAhead.token == p->GTT("or")) {
		Match(p->GTT("or") ,nt, "or");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::assignop() {
	std::string nt = "assignop";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT(":=")) {
		Match(p->GTT(":=") ,nt, ":=");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::mulop() {
	std::string nt = "mulop";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("mod")) {
		Match(p->GTT("mod") ,nt, "mod");
		return;
	}

	if(lookAhead.token == p->GTT("/")) {
		Match(p->GTT("/") ,nt, "/");
		return;
	}

	if(lookAhead.token == p->GTT("and")) {
		Match(p->GTT("and") ,nt, "and");
		return;
	}

	if(lookAhead.token == p->GTT("div")) {
		Match(p->GTT("div") ,nt, "div");
		return;
	}

	if(lookAhead.token == p->GTT("*")) {
		Match(p->GTT("*") ,nt, "*");
		return;
	}
	SynErrorTok(nt, exp);
}

void Project2::relop() {
	std::string nt = "relop";
	std::string debugLEX = lookAhead.lex;
	int debugLineNum = lookAhead.line+1;
	int debugCharCol = lookAhead.charNum+1;
	std::string exp = " ";

	if(lookAhead.token == p->GTT("<=")) {
		Match(p->GTT("<=") ,nt, "<=");
		return;
	}

	if(lookAhead.token == p->GTT("<>")) {
		Match(p->GTT("<>") ,nt, "<>");
		return;
	}

	if(lookAhead.token == p->GTT("=")) {
		Match(p->GTT("=") ,nt, "=");
		return;
	}

	if(lookAhead.token == p->GTT(">")) {
		Match(p->GTT(">") ,nt, ">");
		return;
	}

	if(lookAhead.token == p->GTT(">=")) {
		Match(p->GTT(">=") ,nt, ">=");
		return;
	}

	if(lookAhead.token == p->GTT("<")) {
		Match(p->GTT("<") ,nt, "<");
		return;
	}
	SynErrorTok(nt, exp);
}

