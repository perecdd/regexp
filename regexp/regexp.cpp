#include <iostream>
#include "SyntaxAnalyzer.h"
#include "NFA.h"
#include "NFAGenerator.h"

int main()
{
	NFAGenerator generator;
	std::shared_ptr<NFA> nfa = generator.generate("a");
	std::cout << nfa->runOnLine("a");
	return 0;
}