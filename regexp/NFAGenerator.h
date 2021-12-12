#pragma once
#include <string>
#include "NFA.h"
#include <stack>

// Grammar:
// E -> E4
// E -> e
// E4 -> E4 | E3
// E4 -> E3
// E3 -> E3 E2
// E3 -> E2
// E2 -> E2*
// E2 -> E1
// E1 -> sym
// E1 -> (E)

// Grammar (Simplify)
// E -> E4
// E -> e
// E4 -> E3 E44
// E44 -> | E3 E44
// E44 -> e
// E3 -> E2 E33
// E33 -> & E2 E33
// E2 -> E1 E22
// E22 -> * E22
// E22 -> e
// E1 -> sym
// E1 -> (E)
// 
class NFAGenerator
{
public:
	NFAGenerator();
	std::shared_ptr<NFA> generate(const std::string& line);

private:
	bool isSym(char sym);
	bool E();
	bool E4();
	bool E44();
	bool E3();
	bool E33();
	bool E2();
	bool E22();
	bool E1();

	std::stack<NFA> nfa_stack;
	const char* line_ = nullptr;
	int pointer = 0;
	int size = 0;
};