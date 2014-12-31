#include "MiscFunc.h"
#include <sstream>

MiscFunc::MiscFunc()
{
}


MiscFunc::~MiscFunc()
{
}

/*
 *	Splits strings via delim. Note: splits can be empty!
*/
std::vector<std::string>& MiscFunc::split(const std::string& s, char delim, std::vector<std::string> &elems) 
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> MiscFunc::split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

