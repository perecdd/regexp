#include "SyntaxAnalyzer.h"

bool isSym(char sym) {
	return (sym >= 'a' && sym <= 'z') || (sym >= '0' && sym <= '9');
}

SyntaxAnalyzer::SyntaxAnalyzer()
{
}

bool SyntaxAnalyzer::analyze(const std::string& line)
{
	pointer = 0;
	line_ = line.data();
	size = line.size();
	return line.empty() || E() && pointer == size;
}

bool SyntaxAnalyzer::E()
{
	if (pointer >= size) return false;
	if (!E4()) return false;
	return true;
}

bool SyntaxAnalyzer::E4()
{
	if (pointer >= size) return false;
	if (!E3()) return false;
	if (!E44()) return false;
	return true;
}

bool SyntaxAnalyzer::E44()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '|') {
		pointer++;
		if (!E3()) return false;
		if (!E44()) return false;
	}
	return true;
}

bool SyntaxAnalyzer::E3()
{
	if (pointer >= size) return false;
	if (!E2()) return false;
	if (!E33()) return false;
	return true;
}

bool SyntaxAnalyzer::E33()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '&') {
		pointer++;
		if (!E2()) return false;
		if (!E33()) return false;
	}
	return true;
}

bool SyntaxAnalyzer::E2()
{
	if (pointer >= size) return false;
	if (!E1()) return false;
	if (!E22()) return false;
	return true;
}

bool SyntaxAnalyzer::E22()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '*') {
		pointer++;
		if (!E22()) return false;
	}
	return true;
}

bool SyntaxAnalyzer::E1()
{
	if (isSym(line_[pointer])) {
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
	//else if (!E()) return false;
	return true;
}
