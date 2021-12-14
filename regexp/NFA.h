#pragma once
#include <memory>
#include <map>
#include <set>
#include <string>

const char EMPTY_SYM = ' ';

class Node {
public:
	void ConnectTo(std::shared_ptr<Node> node, char sym);
	std::set<std::shared_ptr<Node>> goTo(char sym);

private:
	std::multimap<char, std::shared_ptr<Node>> connections;
};

class NFA
{
public:
	NFA(char symbol);
	void AND(NFA nfa); // this & nfa
	void OR(NFA nfa);
	void CYCLE();
	bool runOnLine(const std::string& line);

private:
	std::set<std::shared_ptr<Node>> runOnEpsilon(std::shared_ptr<Node> state);
	std::set<std::shared_ptr<Node>> moveTo(std::set<std::shared_ptr<Node>> set, char sym);

	std::set<std::shared_ptr<Node>> old_states;
	std::set<std::shared_ptr<Node>> states_;

	std::shared_ptr<Node> start_ = nullptr;
	std::shared_ptr<Node> end_ = nullptr;
};