#include "pch.h"
#include "../regexp/SyntaxAnalyzer.cpp"
#include "../regexp/RPNGenerator.cpp"
#include "../regexp/NFA.cpp"

TEST(SyntaxAnalyzer, GoodData) {
	SyntaxAnalyzer analyzer;

	EXPECT_TRUE(analyzer.analyze(""));
	EXPECT_TRUE(analyzer.analyze("a&a&a&a&b&c&d&e&f&g&8&9&0"));
	EXPECT_TRUE(analyzer.analyze("a*"));
	EXPECT_TRUE(analyzer.analyze("a**"));
	EXPECT_TRUE(analyzer.analyze("(a*)"));
	EXPECT_TRUE(analyzer.analyze("(a)*"));
	EXPECT_TRUE(analyzer.analyze("(a)"));
	EXPECT_TRUE(analyzer.analyze("(a&b)"));
	EXPECT_TRUE(analyzer.analyze("a&b"));
	EXPECT_TRUE(analyzer.analyze("a*&b*"));
	EXPECT_TRUE(analyzer.analyze("a&b*"));
	EXPECT_TRUE(analyzer.analyze("(a&b|b&a)"));
	EXPECT_TRUE(analyzer.analyze("(a&b|b&a)*"));
	EXPECT_TRUE(analyzer.analyze("(a&b|b&a)**"));
	EXPECT_TRUE(analyzer.analyze("a|b|c|d|e|f**"));
	EXPECT_TRUE(analyzer.analyze("(a&b|(b&a)*)"));
	EXPECT_TRUE(analyzer.analyze("((a&a&a&(b)*)|(b&a)*)"));
	EXPECT_TRUE(analyzer.analyze("(a&b*)*"));
	EXPECT_TRUE(analyzer.analyze("((a*&b*)*)*"));
	EXPECT_TRUE(analyzer.analyze("a|b|c|d|e|f"));
	EXPECT_TRUE(analyzer.analyze("((a)|(b))|c"));
}

TEST(SyntaxAnalyzer, BadData) {
	SyntaxAnalyzer analyzer;
	EXPECT_TRUE(!analyzer.analyze("a|"));
	EXPECT_TRUE(!analyzer.analyze("|a"));
	EXPECT_TRUE(!analyzer.analyze("*"));
	EXPECT_TRUE(!analyzer.analyze("|"));
	EXPECT_TRUE(!analyzer.analyze("()"));
	EXPECT_TRUE(!analyzer.analyze(")"));
	EXPECT_TRUE(!analyzer.analyze("("));
	EXPECT_TRUE(!analyzer.analyze("(a&b&a&a&b&a))"));
	EXPECT_TRUE(!analyzer.analyze("(a&b|b&a|)"));
	EXPECT_TRUE(!analyzer.analyze("(a&b|(b&a)*))"));
	EXPECT_TRUE(!analyzer.analyze("((a&a&a&(b)*)|(b&a)*))"));
	EXPECT_TRUE(!analyzer.analyze("*(a&b*)*"));
	EXPECT_TRUE(!analyzer.analyze("((a*&b*)*)*|*"));
	EXPECT_TRUE(!analyzer.analyze("((a)|(b))|c()"));
}

TEST(NFA, Constructor) {
	{
		NFA nfa0('0');
		EXPECT_TRUE(nfa0.runOnLine("0"));
		for (char i = 'a'; i < 'z'; ++i) {
			EXPECT_TRUE(!nfa0.runOnLine(std::to_string(i)));
		}
	}
}

TEST(NFA, AND) {
	{
		NFA nfa0('0');
		NFA nfa1('1');
		nfa0.AND(nfa1);
		EXPECT_TRUE(nfa0.runOnLine("01"));
		EXPECT_TRUE(!nfa0.runOnLine("010"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('1');
		NFA nfa3('1');
		nfa0.AND(nfa1);
		nfa0.AND(nfa2);
		nfa0.AND(nfa3);
		EXPECT_TRUE(nfa0.runOnLine("0111"));
		EXPECT_TRUE(!nfa0.runOnLine("01111"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('2');
		NFA nfa3('3');
		nfa0.AND(nfa1);
		nfa0.AND(nfa2);
		nfa0.AND(nfa3);
		EXPECT_TRUE(nfa0.runOnLine("0123"));
		EXPECT_TRUE(!nfa0.runOnLine("012"));
		EXPECT_TRUE(!nfa0.runOnLine("013"));
		EXPECT_TRUE(!nfa0.runOnLine("01234"));
	}
}

TEST(NFA, OR) {
	{
		NFA nfa0('0');
		NFA nfa1('1');
		nfa0.OR(nfa1);
		EXPECT_TRUE(!nfa0.runOnLine("01"));
		EXPECT_TRUE(!nfa0.runOnLine("11"));
		EXPECT_TRUE(!nfa0.runOnLine("00"));
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(nfa0.runOnLine("1"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('2');
		nfa0.OR(nfa1);
		nfa0.OR(nfa2);
		// 0|1|2
		EXPECT_TRUE(!nfa0.runOnLine("02"));
		EXPECT_TRUE(!nfa0.runOnLine("012"));
		EXPECT_TRUE(nfa0.runOnLine("2"));
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(nfa0.runOnLine("1"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('2');
		nfa0.OR(nfa1);
		nfa0.AND(nfa2);
		// (0|1)&2
		EXPECT_TRUE(nfa0.runOnLine("12"));
		EXPECT_TRUE(nfa0.runOnLine("02"));
		EXPECT_TRUE(!nfa0.runOnLine("120"));
		EXPECT_TRUE(!nfa0.runOnLine("020"));
		EXPECT_TRUE(!nfa0.runOnLine("00"));
		EXPECT_TRUE(!nfa0.runOnLine("01"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('2');
		NFA nfa3('3');
		nfa0.OR(nfa1);
		nfa0.AND(nfa2);
		nfa0.OR(nfa3);
		// ((0|1)&2)|3
		EXPECT_TRUE(nfa0.runOnLine("12"));
		EXPECT_TRUE(nfa0.runOnLine("02"));
		EXPECT_TRUE(nfa0.runOnLine("3"));
		EXPECT_TRUE(!nfa0.runOnLine("120"));
		EXPECT_TRUE(!nfa0.runOnLine("020"));
		EXPECT_TRUE(!nfa0.runOnLine("00"));
		EXPECT_TRUE(!nfa0.runOnLine("01"));
	}
}

TEST(NFA, Cycle) {
	{
		NFA nfa0('0');
		nfa0.CYCLE();
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(nfa0.runOnLine("000"));
		EXPECT_TRUE(nfa0.runOnLine("000000000000"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		nfa0.OR(nfa1);
		nfa0.CYCLE();
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(nfa0.runOnLine("1"));
		EXPECT_TRUE(nfa0.runOnLine("1"));
		EXPECT_TRUE(nfa0.runOnLine("01010110"));
		EXPECT_TRUE(nfa0.runOnLine("000000000000"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		nfa1.CYCLE();
		nfa0.OR(nfa1);
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(!nfa0.runOnLine("01"));
		EXPECT_TRUE(!nfa0.runOnLine("011"));
		EXPECT_TRUE(nfa0.runOnLine("111111"));
		EXPECT_TRUE(nfa0.runOnLine("1"));
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(!nfa0.runOnLine("00"));
		EXPECT_TRUE(nfa0.runOnLine("11"));
	}
	{
		NFA nfa0('0');
		nfa0.CYCLE();
		nfa0.CYCLE();
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(nfa0.runOnLine("0"));
		EXPECT_TRUE(nfa0.runOnLine("000"));
		EXPECT_TRUE(nfa0.runOnLine("000000000000"));
	}
	{
		NFA nfa0('0');
		NFA nfa1('1');
		NFA nfa2('2');
		nfa0.AND(nfa1);
		nfa0.AND(nfa2);
		nfa0.CYCLE();
		EXPECT_TRUE(nfa0.runOnLine("012"));
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(nfa0.runOnLine("012012012"));
		EXPECT_TRUE(!nfa0.runOnLine("01"));
		EXPECT_TRUE(!nfa0.runOnLine("0"));
		EXPECT_TRUE(!nfa0.runOnLine("2"));
		EXPECT_TRUE(!nfa0.runOnLine("0120"));
		EXPECT_TRUE(!nfa0.runOnLine("01201"));
		EXPECT_TRUE(nfa0.runOnLine("012012"));
	}
}