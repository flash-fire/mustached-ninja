#include "Trie.h"


Node* Node::findChild(char c) {
	for (int i = 0; i < mChildren.size(); i++) {
		Node* tmp = mChildren.at(i);
		if (tmp->content() == c) {
			return tmp;
		}
	}
	return NULL;
}

Trie::Trie() {
	root = new Node();
}
Trie::~Trie() {
	// Free memory
}

void Trie::addWord(std::string s) {
	Node* current = root;
	if (s.length() == 0) {
		current->setWordMarker(); // an empty word return;
	}
	for (int i = 0; i < s.length(); i++) {
		Node* child = current->findChild(s[i]);
		if (child != NULL) {
			current = child;
		}
		else {
			Node* tmp = new Node();
			tmp->setContent(s[i]);
			current->appendChild(tmp);
			current = tmp;
		} if (i == s.length() - 1)
			current->setWordMarker();
	}
}

/* IF we don't match -- 0
 * IF we don't match a word BUT there exists child that could match further -- match = 1
 * If we match AND there exists child that could match -- 2
 * IF we match AND there does NOT exist child that could match further-- 3

  Saves a lot of work trying to write all the automata. Yo! */
int Trie::searchWord(std::string s) {
	if (root == NULL) {
		return 0;
	}
	Node* current = root;
	for (int i = 0; i < s.length(); i++) {
		Node* tmp = current->findChild(s[i]);
		if (tmp == NULL)
			return 0; // No match
		current = tmp;
	}
	if (current->wordMarker()) {
		if (current->hasChildren()) {
			return 2; // match and exists further match
		}
		else {
			return 3; // match and no further match
		}
	}
	else {
		return 1; // No match to a word now, but further there could exist a match.
	}
	return 0;
}
// Test program 

// Adapted from http://www.sourcetricks.com/2011/06/c-tries.html#.VIujbHsYEWg
// Not very efficient but works. I could handle datastructures better. In any case, it makes automata so much easier.