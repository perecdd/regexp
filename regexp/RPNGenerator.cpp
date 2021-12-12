#include "RPNGenerator.h"
#include <stdexcept>

bool RPNGenerator::isSym(char sym) {
	return (sym >= 'a' && sym <= 'z') || (sym >= '0' && sym <= '9');
}

RPNGenerator::RPNGenerator()
{
}

NFA RPNGenerator::generate(const std::string& line)
{
	pointer = 0;
	line_ = line.data();
	size = line.size();
	if (line.empty() || E() && pointer == size) {
		return NFA('0');
	}
	else {
		throw std::runtime_error("Invalid regular expression");
	}
}

bool RPNGenerator::E()
{
	if (pointer >= size) return false;
	if (!E4()) return false;
	return true;
}

bool RPNGenerator::E4()
{
	if (pointer >= size) return false;
	if (!E3()) return false;
	if (!E44()) return false;
	return true;
}

bool RPNGenerator::E44()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '|') {
		pointer++;
		if (!E3()) return false;
		if (!E44()) return false;
	}
	return true;
}

bool RPNGenerator::E3()
{
	if (pointer >= size) return false;
	if (!E2()) return false;
	if (!E33()) return false;
	return true;
}

bool RPNGenerator::E33()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '&') {
		pointer++;
		if (!E2()) return false;
		if (!E33()) return false;
	}
	return true;
}

bool RPNGenerator::E2()
{
	if (pointer >= size) return false;
	if (!E1()) return false;
	if (!E22()) return false;
	return true;
}

bool RPNGenerator::E22()
{
	if (pointer >= size) return true;
	if (line_[pointer] == '*') {
		pointer++;
		if (!E22()) return false;
	}
	return true;
}

bool RPNGenerator::E1()
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
	return true;
}
