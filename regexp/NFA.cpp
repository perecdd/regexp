#include "NFA.h"

void Node::ConnectTo(std::shared_ptr<Node> node, char sym)
{
	connections.insert({ sym, node });
}

std::set<std::shared_ptr<Node>> Node::goTo(char sym)
{
	std::set<std::shared_ptr<Node>> set;
	auto pair = connections.equal_range(sym);
	for (auto it = pair.first; it != pair.second; it++) {
		set.insert(it->second);
	}
	return set;
}

NFA::NFA(char symbol)
{
	start_ = std::make_shared<Node>();
	end_ = std::make_shared<Node>();
	start_->ConnectTo(end_, symbol);
}

void NFA::AND(NFA nfa)
{
	end_->ConnectTo(nfa.start_, EMPTY_SYM);
	end_ = nfa.end_;
}

void NFA::OR(NFA nfa)
{
	std::shared_ptr<Node> start = std::make_shared<Node>();
	start->ConnectTo(start_, EMPTY_SYM);
	start->ConnectTo(nfa.start_, EMPTY_SYM);

	std::shared_ptr<Node> end = std::make_shared<Node>();
	end_->ConnectTo(end, EMPTY_SYM);
	nfa.end_->ConnectTo(end, EMPTY_SYM);

	start_ = start;
	end_ = end;
}

void NFA::CYCLE()
{
	end_->ConnectTo(start_, EMPTY_SYM);
	std::shared_ptr<Node> start = std::make_shared<Node>();
	std::shared_ptr<Node> end = std::make_shared<Node>();
	start->ConnectTo(end, EMPTY_SYM);
	start->ConnectTo(start_, EMPTY_SYM);
	end_->ConnectTo(end, EMPTY_SYM);
	start_ = start;
	end_ = end;
}

#include <iostream>

bool NFA::runOnLine(const std::string& line) {
	old_states.clear();
	states_.clear();

	std::set<std::shared_ptr<Node>> S = runOnEpsilon(start_);
	std::set<std::shared_ptr<Node>> newset;

	for (int i = 0; i < line.size(); i++) {
		old_states.clear(); // Clear de-cycle massive
		for (std::shared_ptr<Node> node : moveTo(S, line[i])) {
			newset.merge(runOnEpsilon(node));

			for (auto k : newset) {
				std::cout << k << ' ';
			}
			std::cout << std::endl;
		}
		S.swap(newset);
		newset.clear();
	}
	if (S.find(end_) != S.end()) return true;
	return false;
}

std::set<std::shared_ptr<Node>> NFA::moveTo(std::set<std::shared_ptr<Node>> set, char sym) {
	std::set<std::shared_ptr<Node>> result;
	for (std::shared_ptr<Node> node : set) {
		result.merge(node->goTo(sym));
	}
	return result;
}

std::set<std::shared_ptr<Node>> NFA::runOnEpsilon(std::shared_ptr<Node> state) {
	if (old_states.find(state) != old_states.end()) {
		return std::set<std::shared_ptr<Node>>();
	}
	std::set<std::shared_ptr<Node>> result;
	result.insert(state);
	old_states.insert(state); // de-cycle
	for (std::shared_ptr<Node> node : state->goTo(EMPTY_SYM)) {
		result.insert(node);
		result.merge(runOnEpsilon(node));
	}
	return result;
}