#include "NFAGenerator.h"
#include <stdexcept>

bool NFAGenerator::isSym(char sym) {
	return (sym >= 'a' && sym <= 'z') || (sym >= '0' && sym <= '9');
}

NFAGenerator::NFAGenerator()
{
}
#include <iostream>
std::shared_ptr<NFA> NFAGenerator::generate(const std::string& line)
{
	pointer = 0;
	line_ = line.data();
	size = line.size();

	if (E() && pointer == size && nfa_stack.size() == 1) {
		return std::make_shared<NFA>(std::move(nfa_stack.top()));
	}
	else if (line.empty()) {
		return std::make_shared<NFA>(EMPTY_SYM);
	}
	else {
		std::cout << pointer << ' ' << size << ' ' << nfa_stack.size();
		throw std::runtime_error("Invalid regular expression");
	}
}

bool NFAGenerator::E()
{
	if (pointer >= size) return false;
	if (!E4()) return false;
	return true;
}

bool NFAGenerator::E4()
{
	if (pointer >= size) return false;
	if (!E3()) return false;
	if (!E44()) return false;
	return true;
}

bool NFAGenerator::E44()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '|') {
		pointer++;
		if (!E3()) return false;
		if (!E44()) return false;

		std::cout << "|\n";
		if (nfa_stack.size() >= 2) {
			NFA nfa1 = std::move(nfa_stack.top());
			nfa_stack.pop();
			NFA nfa0 = std::move(nfa_stack.top());
			nfa_stack.pop();

			nfa0.OR(nfa1);
			nfa_stack.push(std::move(nfa0));
		}
		else return false;
	}
	return true;
}

bool NFAGenerator::E3()
{
	if (pointer >= size) return false;
	if (!E2()) return false;
	if (!E33()) return false;
	return true;
}

bool NFAGenerator::E33()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '&') {
		pointer++;
		if (!E2()) return false;
		if (!E33()) return false;

		std::cout << "&\n";

		if (nfa_stack.size() >= 2) {
			NFA nfa1 = std::move(nfa_stack.top());
			nfa_stack.pop();
			NFA nfa0 = std::move(nfa_stack.top());
			nfa_stack.pop();

			nfa0.AND(nfa1);
			nfa_stack.push(std::move(nfa0));
		}
		else return false;
	}
	return true;
}

bool NFAGenerator::E2()
{
	if (pointer >= size) return false;
	if (!E1()) return false;
	if (!E22()) return false;
	return true;
}

bool NFAGenerator::E22()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '*') {
		pointer++;
		if (!E22()) return false;
		nfa_stack.top().CYCLE();
	}
	return true;
}

bool NFAGenerator::E1()
{
	if (isSym(line_[pointer])) {
		nfa_stack.push(line_[pointer]);
		pointer++;
	}
	else if (line_[pointer] == '(') {
		pointer++;
		if (!E()) return false;
		if (line_[pointer] == ')') {
			pointer++;
		}
		else return false;
	}
	else return false;
	return true;
}
