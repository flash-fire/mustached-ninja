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
	void Project2::prgm();
	void Project2::prgmLF1();
	void Project2::prgmLF1LF1();
	void Project2::idList();
	void Project2::idListLR1();
	void Project2::decs();
	void Project2::decsLR1();
	void Project2::type();
	void Project2::std_type();
	void Project2::subprgm_decs();
	void Project2::subprgm_decsLR1();
	void Project2::subprgm_dec();
	void Project2::subprgm_decLF1();
	void Project2::subprgm_decLF1LF1();
	void Project2::subprgm_head();
	void Project2::subprgm_headLF1();
	void Project2::args();
	void Project2::param_list();
	void Project2::param_listLR1();
	void Project2::comp_stmt();
	void Project2::comp_stmtLF1();
	void Project2::opt_stmts();
	void Project2::stmt_list();
	void Project2::stmt_listLR1();
	void Project2::stmt();
	void Project2::stmtLF1();
	void Project2::variable();
	void Project2::variableLF1();
	void Project2::proc_stmt();
	void Project2::proc_stmtLF1();
	void Project2::expr_list();
	void Project2::expr_listLR1();
	void Project2::expr();
	void Project2::exprLF1();
	void Project2::term();
	void Project2::termLR1();
	void Project2::simple_expr();
	void Project2::simple_exprLR1();
	void Project2::factor();
	void Project2::factorLF1();
	void Project2::sign();
	void Project2::addop();
	void Project2::assignop();
	void Project2::mulop();
	void Project2::relop();
	Token lookAhead;
	Project1* p;
	std::ofstream target;

	static const std::string Project2::SYNCH_PATH;
	std::ifstream synch;
	bool contains(std::string str, std::string targ);

};
#endif