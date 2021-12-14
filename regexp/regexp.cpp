#include <iostream>
#include "SyntaxAnalyzer.h"
#include "NFA.h"
#include "NFAGenerator.h"

int main()
{
	std::string line;
	std::cin >> line;
	line = correct(line);
	SyntaxAnalyzer analyzer;
	if (analyzer.analyze(line)) {
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate(line);
		std::cin >> line;
		std::cout << nfa->runOnLine(line);
	}
	else {
		std::cout << "Invalid expression!";
	}
	return 0;
}