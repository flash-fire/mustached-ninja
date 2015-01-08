#ifndef __PROJECT2_H_INCLUDED__
#define __PROJECT2_H_INCLUDED__
#include "Project1.h"
#include "ParseNode.h"

class Project2 {

public:
	Project2(Project1* p);
	~Project2();
	void LoadSynch();
	void Parse();
	Token Match(int tok, std::string nt, std::string exp);
	void MatchEOF();
	void SynErrorTok(std::string nt, std::string exp);
	void UniteSynchronization(std::string nt);

	std::map<std::string, std::string> synchSet;
private:
	void Project2::prgm(ParseNode* par);
	void Project2::prgmLF1(ParseNode* par);
	void Project2::prgmLF1LF1(ParseNode* par);
	void Project2::idList(ParseNode* par);
	void Project2::idListLR1(ParseNode* par);
	void Project2::decs(ParseNode* par);
	void Project2::decsLR1(ParseNode* par);
	void Project2::type(ParseNode* par);
	void Project2::std_type(ParseNode* par);
	void Project2::subprgm_decs(ParseNode* par);
	void Project2::subprgm_decsLR1(ParseNode* par);
	void Project2::subprgm_dec(ParseNode* par);
	void Project2::subprgm_decLF1(ParseNode* par);
	void Project2::subprgm_decLF1LF1(ParseNode* par);
	void Project2::subprgm_head(ParseNode* par);
	void Project2::subprgm_headLF1(ParseNode* par);
	void Project2::args(ParseNode* par);
	void Project2::param_list(ParseNode* par);
	void Project2::param_listLR1(ParseNode* par);
	void Project2::comp_stmt(ParseNode* par);
	void Project2::comp_stmtLF1(ParseNode* par);
	void Project2::opt_stmts(ParseNode* par);
	void Project2::stmt_list(ParseNode* par);
	void Project2::stmt_listLR1(ParseNode* par);
	void Project2::stmt(ParseNode* par);
	void Project2::stmtLF1(ParseNode* par);
	void Project2::variable(ParseNode* par);
	void Project2::variableLF1(ParseNode* par);
	void Project2::proc_stmt(ParseNode* par);
	void Project2::proc_stmtLF1(ParseNode* par);
	void Project2::expr_list(ParseNode* par);
	void Project2::expr_listLR1(ParseNode* par);
	void Project2::expr(ParseNode* par);
	void Project2::exprLF1(ParseNode* par);
	void Project2::term(ParseNode* par);
	void Project2::termLR1(ParseNode* par);
	void Project2::simple_expr(ParseNode* par);
	void Project2::simple_exprLR1(ParseNode* par);
	void Project2::factor(ParseNode* par);
	void Project2::factorLF1(ParseNode* par);
	void Project2::sign(ParseNode* par);
	void Project2::addop(ParseNode* par);
	void Project2::assignop(ParseNode* par);
	void Project2::mulop(ParseNode* par);
	void Project2::relop(ParseNode* par);
	Token lookAhead;
	Project1* p;
	std::ofstream target;

	static const std::string Project2::SYNCH_PATH;
	std::ifstream synch;
	bool contains(std::string str, std::string targ);

};
#endif