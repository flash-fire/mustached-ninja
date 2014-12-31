#ifndef __MISCFUNC_H_INCLUDED__
#define __MISCFUNC_H_INCLUDED__

#include <vector>
#include <string>
#include <map>

// TODO: MOVE THIS  DEFINE SOMEWHERE BETTER
#define ID 42
#define NUM 45
//#define LEXERR 200


class MiscFunc
{
public:
	MiscFunc();
	~MiscFunc();

	// Taken from http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c [++]
	static std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::vector<std::string> split(const std::string &s, char delim);
};

#endif