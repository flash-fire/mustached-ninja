#include "Project1.h"
#include "MiscFunc.h"
#include "CErrors.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

const char * const Project1::sourceNames[] = 
{ ".\\data\\source\\Over9000Fuzz.txt", ".\\data\\source\\BasicLexTest2.txt", ".\\data\\source\\Fuzz1.txt", ".\\data\\source\\IfTest.txt", ".\\data\\source\\ShenoiLexTest.txt"};
const std::string Project1::SOURCE_PATH = sourceNames[0];
const std::string Project1::TARGET_PATH = ".\\output\\Target.txt";
const std::string Project1::MISC_FILE_PATH =  ".\\data\\MiscTokens.txt";
const std::string Project1::RES_FILE_PATH	= ".\\data\\ReservedWords.txt";
const std::string Project1::TOKEN_FILE_PATH = ".\\output\\Tokens.txt";

Project1::Project1() : lineNum(0), table(), front(0), back(0), ntMatcher()
{
	target.open(TARGET_PATH, std::ios::out | std::ios::trunc);
	tokenFile.open(TOKEN_FILE_PATH, std::ios::out | std::ios::trunc);

	if (target.is_open() == false)
	{
		std::cout << "Unable to open file for target\n";
	}

	if (tokenFile.is_open() == false)
	{
		std::cout << "Unable to open file for token\n";
	}
	ReadReservedWordTable();
	ReadOpWordTable();
	// Exceptions to the table
	tokenToCommon[Token(EOF, 0)] = "EOF";
	// id, int are only exceptions left
	PrettyPrintToken(Token(-2, 0)); // Prints pretty printing first line
}

Project1::~Project1() { }

// I could define somethign to allow code reuse. Oh well. LAZY and don't want to make another class just for special loading.. Or a function I suppose. Meh.
// TODO: Generalize this thing.
void Project1::ReadReservedWordTable()
{
	std::string line;
	std::ifstream resFile(Project1::RES_FILE_PATH);
	if (resFile.is_open())
	{
		while (std::getline(resFile, line))
		{
			if (line.size() == 0 || (line.size() > 1 && line.at(0) == '#'))
			{ // skip lines that are comments or empty
				continue;
			}
			std::vector<std::string> split = MiscFunc::split(line, '\t');
			Token tok = Token(std::stoi(split.at(1)), std::stoi(split.at(2)), split.at(0));
			Res[split.at(0)] = tok;
			tokenToCommon[tok] = split.at(3);
		}
		resFile.close();
	}
	else std::cout << "Unable to open file for reserved table\n";
}

// Reads in the none reserved words that are non terminals
void Project1::ReadOpWordTable()
{
	std::string line;
	std::ifstream resFile(Project1::MISC_FILE_PATH);
	if (resFile.is_open())
	{
		while (std::getline(resFile, line))
		{
			if (line.size() == 0 || (line.size() > 1 && line.at(0) == '#'))
			{ // skip lines that are comments or empty
				continue;
			}
			std::vector<std::string> split = MiscFunc::split(line, '\t');
			if (split.size() != 4) {
				std::cout << "DEBUG WARNING: Splitted op in misc op table is of wrong size: " << line << "\n";
			}
			Token targ = Token(std::stoi(split.at(1)), std::stoi(split.at(2)), split.at(0));
			ntMatcher.addWord(split.at(0)); // Ensures that the nonterminal matcher has the code for the terminals I want to match
			codeToToken[split.at(0)] = targ; // Used to more easily convert what is matched.
			commonToToken[split.at(3)] = targ; // Used to make programming easier later
			tokenToCommon[targ] = split.at(3); // Used to make programming easier later
		}
		for (auto it = commonToToken.cbegin(); it != commonToToken.cend(); ++it)
		{
			//std::cout << it->first << " " << it->second << "\n";
		}
		resFile.close();
	}
	else std::cout << "DEBUG ERROR: Unable to open file for misc word table\n";
}

void Project1::HandleLexing()
{

	auto paddify = [](std::string a, int pad) { int max = std::max(pad - static_cast<int>(a.length()), 1); return std::string(a).append(max, ' '); };

	std::ifstream source(Project1::SOURCE_PATH);
	std::string currentLine;
	if (source.is_open())
	{
		while (getline(source, currentLine))
		{
			//std::cout << lineNum << " " << currentLine << "\n";
			target << paddify(std::to_string(lineNum), 10) << currentLine << "\n";
			HandleCurrentLine(currentLine);
			lineNum++;
		}
		tokens.push_back(Token(EOF, 0, "$", lineNum));
		PrettyPrintToken(Token(EOF, 0, "\EOF", lineNum));
		//TODO: ADD TOKEN FOR EOF
	}
	else std::cout << "Unable to open file for source\n";
	source.close();
	target.close();
	tokenFile.close();
}

// Handles lexing on the current line
void Project1::HandleCurrentLine(std::string& currLine)
{
	if (currLine.length() > Project1::BUFFER_SIZE)
	{
		tokenFile << "WARNING: line " << lineNum << " is larger than buffer size. Will truncate. Sizes: " << currLine.length() << " vs " << BUFFER_SIZE << ".";
		currLine = currLine.substr(0, Project1::BUFFER_SIZE);
	}
	//int front, back;
	front = back = 0;
	//tokenFile << "\n" << lineNum << " " << currLine << "\n";
	while (front < currLine.length())
	{
		Token tok;
		int i = 0;
		int frontInit = front;
		bool match = true;

		while (!tok.isValid() && frontInit == front && match)
		{
			switch (i)
			{
			case 0:
				SkipWhiteSpace(currLine);
				break;
			case 1:
				tok = ReadIdentifier(currLine);
				break;
			case 2:
				tok = ReadLongReal(currLine);
				break;
			case 3:
				tok = ReadDecReal(currLine);
				break;
			case 4:
				tok = ReadInteger(currLine);
				break;
			case 5:
				tok = ReadOther(currLine);
				break;
			default:
				tok = Token();
				match = false;
				break;
			}
			i++;
		}
		if (tok.isValid())
		{
			//PrettyPrintToken(tok);
			tok.lex = currLine.substr(frontInit, front - frontInit);
			tok.charNum = frontInit;
			tok.line = lineNum;
			PrettyPrintToken(tok);
			tokens.push_back(tok);
			frontInit = front;
		}
		else if (!match)
		{
			char c = currLine.at(frontInit);
			Token tok = Token(CErrors::LEXERR, CErrors::CAN_NOT_MATCH );
			tok.lex = currLine.substr(frontInit, front+1 - frontInit);
			tok.charNum = frontInit;
			tok.line = lineNum;
			tokens.push_back(tok);
			PrettyPrintToken(tok);
			front++;
			back++;
			frontInit = front;
		} // else whitespace
	}
}

// This is overengineered for the purpose of handling essentially ANY conceivable change I can think of to the operands. 
// Essentially as long as you don't make an operator like "32<xcs" it should be okay.

/*
	if result is 3
	done. We found definitively the largest match (word matches and no further exists)
	if result is 2
	we need to search further to see if another match exists as this matches and further match exists (word matches but further match exists)
	if result is 1
	we need to search further to see if match exists. Because even though this location isn't valid, some longer string may be. (word doesn't match but further match could exist)
	if result is 0
	we need to roll back until we find the first 2. (word doesn't match and further does not exists)
	if we find no two, then we return null token.

	if we reach end of string we also need to roll back as though it were 0.
*/
Token Project1::ReadOther(const std::string& str)
{
	int lastTwoOffset = -1;
	char next = str.at(front);
	std::string word = std::string(1, next);
	int result = ntMatcher.searchWord(word);
	front++;
	while (result == 2 || result == 1) {
		if (result == 2) {
			lastTwoOffset = front - back; // index offset : or I could do back = lastTwo since that will be a valid match
		}
		if (front >= str.length()) {
			result = 0;
			break;
		}
		word = word.append(1, str.at(front)); // append next character
		result = ntMatcher.searchWord(word);
		front += 1;
	}

	if (result == 3) {
		back = front;
		//std::cout << " Value of code to token at " << word << " is " << codeToToken[word] << " or in common called " << tokenToCommon[codeToToken[word]] << "\n\n";
		return codeToToken[word];
	}
	else{
		if (lastTwoOffset == -1) { // Nothing ever found;
//		std:cout << "\nDEBUG WARNING: No match found for: #" << word << "# where front is: " << front << " and back: " << back << "\nCONTEXT#?*?#: " 
//			<< str.substr(0,back-1) << "#?" << str.substr(back, front-back) << "?#" << str.substr(front) << "\n";
			front = back;
			return Token();
		}
		else {
			word = word.substr(0, lastTwoOffset);
			back = front = back + lastTwoOffset;
			return codeToToken[word];
		}
	}
}

Token Project1::ReadIdentifier(const std::string& str)
{
	if (isalpha(str.at(front)))
	{
		front++;
		while (front < str.length())
		{

			if (isalnum(str.at(front)) == false)
			{
				break;
			}
			front++;
		}
	}
	else // If first isn't letter, than no point doing anything else.
	{
		return Token();
	}

	int length = front - back;
	std::string id = str.substr(back, length);
	Token tok = CheckReservedAndID(id);
	back = front;
	if (length > 10)
	{
		return Token((int)CErrors::LEXERR, (int)CErrors::ID_TOO_LONG);
	}
	else if (tok.isValid())
	{
		//tokenFile << id;
		return tok;
	}
	else { // Need new ID
		return Token(ID, (int) table.addEntry(id));
	}
}

void Project1::SkipWhiteSpace(const std::string& str)
{
	while (front < str.length())
	{
		if (!isblank(str.at(front)) && str.at(front) != '\t')
		{
			break;
		}
		front++;
	}
	back = front;
}

Token Project1::CheckReservedAndID(const std::string& str)
{
	if (Res.count(str) > 0)
	{
		return Res[str];
	}
	else if (table.hasEntry(str).isValid())
	{
		return table.get(str);
	}
	else
	{
		return Token();
	}
}

int Project1::MatchDigitStar(const std::string& str)
{
	int numMatch = 0;
	while (front < str.length() && isdigit(str.at(front)))
	{
		front++;
		numMatch++;
	}
	return numMatch;
}

Token Project1::ReadInteger(const std::string& str)
{
	int frontInit = front;
	int len = MatchDigitStar(str);
	if (len == 0)
	{
		return Token();
	}
	else
	{
		if (str.at(frontInit) == '0' && len != 1)
		{
			back = front;  // error so just stop and return. No going past go. No collecting 200 dollars. ?:frontInit+1?
			return Token((int)CErrors::LEXERR, (int)CErrors::HAS_LEADING_0);
		}
		if (len > 10)
		{
			back = front;
			return Token((int)CErrors::LEXERR, (int)CErrors::INT_TOO_LONG);
		}
		else
		{
			Token ret =  Token(NUM, 0); // null because we have silly compiler that ignores the values.
			back = front;
			return ret;
		}
	}
}

Token Project1::ReadLongReal(const std::string& str)
{
	// [0-9]+.[0-9]+E[+|-][0-9]+
	int expLen = 0;
	int decLen = 0;
	int numLen = 0;
	int frontInit = front;
	numLen += MatchDigitStar(str);
	if (numLen >= 1) 
	{
		if (lineNum == 20)
		{
			lineNum = lineNum;
		}
		if (str.at(frontInit) == '0' && numLen != 1)
		{
			back = front; // ?: frontInit+1?
			return Token((int)CErrors::LEXERR, (int)CErrors::HAS_LEADING_0);
		}
		if (numLen > MAX_LEFT) {
			back = front;
			return Token((int)CErrors::LEXERR, (int)CErrors::INT_TOO_LONG);
		}
		if (front < str.length() && str.at(front) == '.')
		{
			front++;
			decLen += MatchDigitStar(str);
			if (decLen > MAX_RIGHT) {
				back = front;
				return Token((int)CErrors::LEXERR, (int)CErrors::DEC_TOO_LONG);
			}
			if (decLen >= 1)
			{
				if (str.at(front - 1) == '0' && decLen != 1) // check ending 0
				{
					back = front;
					return Token((int)CErrors::LEXERR, (int)CErrors::HAS_ENDING_0);
				}
			handleExp:
				if (front < str.length())
				{
					front = front;
					if (str.at(front) == 'E')
					{
						front = front;
					}
				}
				if (front < str.length() && (str.at(front) == 'E' || str.at(front) == 'e'))
				{
					front++;
					if (front < str.length() && (str.at(front) == '+' || str.at(front) == '-'))
					{
						front++;
					} // we want to check for exponent even with epsilon as +-
					frontInit = front; // just to make checking leading 0s easier.
					expLen += MatchDigitStar(str);
					if (expLen >= 1)
					{
						if (str.at(frontInit) == '0' && expLen != 1)
						{
							back = front; // ?: frontInit+1
							return Token((int)CErrors::LEXERR, (int)CErrors::HAS_LEADING_0);
						}
						if (expLen > MAX_EXP) {
							back = front;
							return Token((int)CErrors::LEXERR, (int)CErrors::EXP_TOO_LONG);
						}
						back = front;
						return Token(NUM, 1);
					}
				}
			} // dig+. followed by not digit
		}
		else
		{ // Handle the optional decimal before exponent
			goto handleExp; // I'm going to hell for this
		}
	} // num len is 0
	front = back;
	return Token();
}

Token Project1::ReadDecReal(const std::string& str)
{
	int decLen = 0;
	int numLen = 0;
	int frontInit = front;
	numLen += MatchDigitStar(str);
	if (numLen >= 1)
	{
		if (str.at(frontInit) == '0' && numLen != 1)
		{
			back = front; // ?:
			return Token((int)CErrors::LEXERR, (int)CErrors::HAS_LEADING_0);
		}
		if (numLen > MAX_LEFT) {
			back = front;
			return Token((int)CErrors::LEXERR, (int)CErrors::INT_TOO_LONG);
		}
		if (front < str.length() && str.at(front) == '.')
		{
			front++;
			decLen += MatchDigitStar(str);

			if (str.at(front - 1) == '0' && decLen != 1) // check ending 0
			{
				back = front; // ?:
				return Token((int)CErrors::LEXERR, (int)CErrors::HAS_ENDING_0);
			}

			if (decLen > MAX_RIGHT) {
				back = front;
				return Token((int)CErrors::LEXERR, (int)CErrors::DEC_TOO_LONG);
			}
			if (decLen >= 1)
			{
				back = front;
				return Token(NUM, 2); // we have a real
			}
		}
	}
	front = back;
	return Token();
}

void Project1::PrettyPrintToken(Token tok)
{
	auto paddify = [](std::string a, int pad) { int max = std::max(pad - static_cast<int>(a.length()), 1); return std::string(a).append(max, ' '); };

	std::string commonName = "";
	std::string description = "NULL";
	if (tok.token == -2) // initialize
	{
		tokenFile << paddify("Line No.",10) << paddify("LEXEME", 10)
			<< paddify("(TOKEN-TYPE)", 10) << "\t" << paddify("ATTRIBUTE", 9) << "\n\n";
		return;
	}
	if (!tok.isValid())
	{
		std::cout << "Invalid Token" << "\n";
	}
	else if (tokenToCommon.count(tok) > 0)
	{
		commonName = tokenToCommon[tok];
	}
	else if (tok.token == (int)CErrors::LEXERR)
	{
		commonName = "LEXERR";
		description = CErrors::errors[tok];
		target << paddify(commonName + ":", 10) << paddify(CErrors::errors[tok] + ":", 30) << tok.lex << "\n";
	}
	else if (table.getID(tok) != "")
	{
		commonName = "ID";
		description = "Pointer to Sym Tab";
	}
	 if (tok.token == NUM)
	{
		 if (tok.attr == 0)
		 {
			 commonName = "INT";
		 }
		 else
		 {
			 commonName = "REAL";
		 }
		description = "value";
	}

	 tokenFile << paddify(std::to_string(tok.line), 10) << paddify(tok.lex, 10) << "\t" << paddify(std::to_string(tok.token), 4)
		 	<< paddify("(" + commonName + ")", 10) << "\t" << paddify(std::to_string(tok.attr), 9) << "( " << tok.gram() << " )\n";
}


Token Project1::nextToken()
{
	Token ret = Token(tokens.front());
	tokens.pop_front();
	return ret;
}

bool Project1::hasToken()
{
	return tokens.empty() == false;
}

int Project1::GTT(std::string str)
{
	if (Res.count(str) > 0) {
		return Res[str].token;
	}
	if (codeToToken.count(str) > 0) {
		return codeToToken[str].token;
	}
	if (str == "id")
	{
		return ID;
	}
	if (str == "num")
	{
		return NUM;
	}

	if (str == "$")
	{
		return EOF;
	}
//std::cout << "Invalid string fed to STRING to TOK";
	return -1;
}

std::string Project1::TokenToGrammar(int token)
{
	if (token == ID)
	{
		return "id";
	}
	if (token == NUM)
	{
		return "num";
	}
	if (token == EOF)
	{
		return "$";
	}
	for (auto it = Res.cbegin(); it != Res.cend(); ++it)
	{
		//std::cout << it->first << "\n" << it->second << "\n";
		if (it->second.token == token)
		{
			return it->first;
		}
	}
	if (tokenToCommon.count(Token(token, 0)) > 0)
	{
		return commonToToken[tokenToCommon[Token(token, 0)]].lex; // I thought it was funny to exploit that a token has its lex
	}
	return "";
}