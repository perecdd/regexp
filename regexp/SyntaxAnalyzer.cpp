#include "SyntaxAnalyzer.h"

bool SyntaxAnalyzer::isSym(char sym) {
	return (sym >= 'a' && sym <= 'z') || (sym >= '0' && sym <= '9');
}

std::string correct(const std::string& line) {
	int pointer = 0;
	int size = line.size();
	std::string result;

	while (size > pointer) {
		char first = line[pointer];
		result.push_back(first);
		pointer++;

		if (!SyntaxAnalyzer::isSym(first) && first != ')') {
			continue;
		}

		if (size <= pointer) break;
		char second = line[pointer];

		if (SyntaxAnalyzer::isSym(second) || second == '(') {
			result.push_back('&');
		}
		else if (second == '*') {
			while (size > pointer) {
				result.push_back('*');
				pointer++;
				second = line[pointer];
				if (SyntaxAnalyzer::isSym(second) || second == '(') {
					result.push_back('&');
					break;
				}
				else if (second == ')') {
					break;
				}
			}
		}
	}
	return result;
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
	return true;
}
