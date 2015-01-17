#ifndef __PROJECT2_H_INCLUDED__
#define __PROJECT2_H_INCLUDED__
#include "Project1.h"
#include "ParseNode.h"
#include "Type.h"
#include "Scope.h"

class Project2 {

public:
	Project2(Project1* p);
	~Project2();
	void LoadSynch();
	void Parse();
	bool Match(int tok, Token** ret);
	void MatchEOF();
	void SynErrorTok(std::string nt, std::string exp);
	void TypeErrorTok(std::string nt, Type::TYPE exp, Type::TYPE rec, Token tok);
	void UniteSynchronization(std::string nt);

	std::map<std::string, std::string> synchSet;
	Scope* currScope; // I'm not using a stack. I'm too rebellious to use a stack.
private:
	std::map<std::string, std::vector<std::string>> vars = makeMap();
	std::map<std::string, std::vector<std::string>> makeMap()
	{
		return {
			{ "prgm_1", { "totalSize"} },
			{ "prgmLF1_1", { "totalSize"} },
			{ "prgmLF1LF1_1", { std::vector<std::string>()}},
			{ "idList_1", { std::vector<std::string>()}},
			{ "idListLR1_1", { std::vector<std::string>()}},
			{ "decs_1", { "offset", "t", "totalSize"} },
			{ "decsLR1_1", { "offset", "t", "totalSize"} },
			{ "type_1", { "width", "t"} },
			{ "std_type_1", { "width", "t"} },
			{ "subprgm_decs_1", { std::vector<std::string>()}},
			{ "subprgm_decsLR1_1", { std::vector<std::string>()}},
			{ "subprgm_dec_1", { "first"} },
			{ "subprgm_decLF1_1", { "totalSize"} },
			{ "subprgm_decLF1LF1_1", { std::vector<std::string>()}},
			{ "subprgm_head_1", { "t", "totalSize"} },
			{ "subprgm_headLF1_1", { "t", "totalSize"} },
			{ "args_1", { "offset", "t", "totalSize"} },
			{ "param_list_1", { "offset", "t", "totalSize"} },
			{ "param_listLR1_1", { "offset", "t", "totalSize"} },
			{ "comp_stmt_1", { std::vector<std::string>()}},
			{ "comp_stmtLF1_1", { std::vector<std::string>()}},
			{ "opt_stmts_1", { std::vector<std::string>()}},
			{ "stmt_list_1", { std::vector<std::string>()}},
			{ "stmt_listLR1_1", { std::vector<std::string>()}},
			{ "stmt_1", { std::vector<std::string>()}},
			{ "stmtLF1_1", { std::vector<std::string>()}},
			{ "variable_1", { std::vector<std::string>()}},
			{ "variableLF1_1", { std::vector<std::string>()}},
			{ "proc_stmt_1", { std::vector<std::string>()}},
			{ "proc_stmtLF1_1", { "t"} },
			{ "expr_list_1", { "t", "ind"} },
			{ "expr_listLR1_1", { std::vector<std::string>()}},
			{ "expr_1", { std::vector<std::string>()}},
			{ "exprLF1_1", { std::vector<std::string>()}},
			{ "term_1", { std::vector<std::string>()}},
			{ "termLR1_1", { std::vector<std::string>()}},
			{ "simple_expr_1", { std::vector<std::string>()}},
			{ "simple_exprLR1_1", { std::vector<std::string>()}},
			{ "factor_1", { "t"} },
			{ "factorLF1_1", { "i"} },
			{ "sign_1", { std::vector<std::string>()}},
			{ "addop_1", { std::vector<std::string>()}},
			{ "assignop_1", { std::vector<std::string>()}},
			{ "mulop_1", { std::vector<std::string>()}},
			{ "relop_1", { std::vector<std::string>()}},
		};
	};

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
	int scopeDepth = 1;
	Token lookAhead;
	Project1* p;
	std::ofstream target;

	static const std::string Project2::SYNCH_PATH;
	std::ifstream synch;
	bool contains(std::string str, std::string targ);

};
#endif