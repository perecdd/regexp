#include "pch.h"
#include "../regexp/SyntaxAnalyzer.cpp"

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