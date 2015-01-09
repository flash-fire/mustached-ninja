#ifndef __PROJECT2_H_INCLUDED__
#define __PROJECT2_H_INCLUDED__
#include "Project1.h"
#include "ParseNode.h"
#include "Type.h"

class Project2 {

public:
	Project2(Project1* p);
	~Project2();
	void LoadSynch();
	void Parse();
	Token* Match(int tok, std::string nt, std::string exp);
	void MatchEOF();
	void SynErrorTok(std::string nt, std::string exp);
	void TypeErrorTok(std::string nt, Type::TYPE exp, Type::TYPE rec, Token tok);
	void UniteSynchronization(std::string nt);

	std::map<std::string, std::string> synchSet;
private:
