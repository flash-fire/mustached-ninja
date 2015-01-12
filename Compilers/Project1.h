#ifndef __PROJECT1_H_INCLUDED__
#define __PROJECT1_H_INCLUDED__

#include "Token.h"
#include "Trie.h"
#include "SymbolTableEntry.h"
#include "SymbolTable.h"
#include "CErrors.h"
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <list>


class Project1
{
public:
	SymbolTable table;
	static const char * const sourceNames[];
	static const std::string SOURCE_PATH; // imput file
	static const std::string TARGET_PATH; // output file
	static const std::string MISC_FILE_PATH; // path to everything else
	static const std::string RES_FILE_PATH; //  path to reserved
	static const std::string TOKEN_FILE_PATH; // path to file containing tokens from this file. This is mostly created for convenience of debugging.

	Project1();
	~Project1();
	Token nextToken();
	int GTT(std::string str); // Grammar To Token
	std::string TokenToGrammar(int tok);
	bool hasToken();

	void HandleLexing(); // Performs lexing
	static const int BUFFER_SIZE = 72; // Max size of buffer
	static const int MAX_EXP = 2; // exp
	static const int MAX_LEFT = 5; // left dec
	static const int MAX_RIGHT = 5; // right dec
private:
	void ReadReservedWordTable(); // Reads in reserved words from RES_FILE_PATH. Should reuse code but I'm lazy.
	void ReadOpWordTable(); // Reads in reserved words from RES_FILE_PATH. Should reuse code but I'm lazy.
	//void NumberLines();			// Because shenoi wanted line numbers lol.

	void HandleCurrentLine(std::string& currLine);	// handles one line for lexer

	void SkipWhiteSpace(const std::string& str);
	Token ReadIdentifier(const std::string& str);
	Token ReadLongReal(const std::string& str);
	Token ReadDecReal(const std::string& str);
	Token ReadInteger(const std::string& str);
	Token ReadOther(const std::string& str);
	Token CheckReservedAndID(const std::string& str);
	int MatchDigitStar(const std::string& str);

	void PrettyPrintToken(Token tok);
	// Used in lexing
	std::ifstream source;
	std::ofstream target;
	std::ofstream tokenFile;

	int lineNum;
	int front; // front point
	int back; // back pointer

	std::map<Token, std::string> tokenToCommon; // maps tokens to common value;
	std::map<std::string, Token> commonToToken; // Maps common names to tokens
	std::map <std::string, Token> codeToToken; // Maps everything but reserved words to token
	Trie ntMatcher; // Trie containing what needs to be matched for non terminals that are not reserved words [ie. Things that conflict with lexical things]
	std::map<std::string, Token> Res; // reserved words
	std::list<Token> tokens;
};

#endif 