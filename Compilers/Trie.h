#pragma once
//TODO: Redo to my satisfaction of being better code, and to know DS better

#include <iostream>
#include <vector>

class Node {
public:
	Node() { mContent = ' '; mMarker = false; }

	~Node() {}
	bool hasChildren() { return mChildren.size() != 0; }
	char content() { return mContent; }
	void setContent(char c) { mContent = c; }
	bool wordMarker() { return mMarker; }
	void setWordMarker() { mMarker = true; }
	Node* findChild(char c);
	void appendChild(Node* child) { mChildren.push_back(child); }
	std::vector<Node*> children() { return mChildren; }
private:
	char mContent;
	bool mMarker;
	std::vector<Node*> mChildren;

};

class Trie {
public:
	Trie();
	~Trie();
	void addWord(std::string s);
	int searchWord(std::string s);
	void deleteWord(std::string s);
private:
	Node* root;
};