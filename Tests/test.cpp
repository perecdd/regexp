#include "pch.h"
#include "../regexp/SyntaxAnalyzer.cpp"
#include "../regexp/NFAGenerator.cpp"
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
	EXPECT_TRUE(analyzer.analyze("(a|b)&c"));
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

TEST(Corrector, Data) {
	EXPECT_EQ("", correct(""));
	EXPECT_EQ("a&b", correct("ab"));
	EXPECT_EQ("a&b&c&d&e", correct("abcde"));
	EXPECT_EQ("a|b", correct("a|b"));
	EXPECT_EQ("a*&b", correct("a*b"));
	EXPECT_EQ("a**&b", correct("a**b"));
	EXPECT_EQ("(a)&(b)", correct("(a)(b)"));
	EXPECT_EQ("(a)&((c)&b)", correct("(a)((c)b)"));
	EXPECT_EQ("(a)&((c)|b)", correct("(a)((c)|b)"));
	EXPECT_EQ("(a)&((c)**&b)", correct("(a)((c)**b)"));
	EXPECT_EQ("a|b&c|e", correct("a|bc|e"));
	EXPECT_EQ("a&(b&c)*&e", correct("a(bc)*e"));
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

TEST(NFA, HardExpressions) {
	{
		//(a&b&c)|c*
		NFA nfa0('a');
		NFA nfa1('b');
		NFA nfa2('c');
		NFA nfa3('c');
		nfa3.CYCLE();
		nfa0.AND(nfa1);
		nfa0.AND(nfa2);
		nfa0.OR(nfa3);
		EXPECT_TRUE(nfa0.runOnLine("abc"));
		EXPECT_TRUE(!nfa0.runOnLine("ab"));
		EXPECT_TRUE(!nfa0.runOnLine("a"));
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(nfa0.runOnLine("c"));
		EXPECT_TRUE(nfa0.runOnLine("ccccc"));
	}

	{
		//((a&b&c)|c*)*
		NFA nfa0('a');
		NFA nfa1('b');
		NFA nfa2('c');
		NFA nfa3('c');
		nfa3.CYCLE();
		nfa0.AND(nfa1);
		nfa0.AND(nfa2);
		nfa0.OR(nfa3);
		nfa0.CYCLE();

		EXPECT_TRUE(nfa0.runOnLine("abc"));
		EXPECT_TRUE(!nfa0.runOnLine("ab"));
		EXPECT_TRUE(!nfa0.runOnLine("a"));
		EXPECT_TRUE(nfa0.runOnLine("abcabc"));
		EXPECT_TRUE(nfa0.runOnLine(""));
		EXPECT_TRUE(nfa0.runOnLine("c"));
		EXPECT_TRUE(nfa0.runOnLine("abcabcccccccc"));
		EXPECT_TRUE(nfa0.runOnLine("ccccc"));
		EXPECT_TRUE(nfa0.runOnLine("ccabccccabc"));
		EXPECT_TRUE(nfa0.runOnLine("cabc"));
	}
}

TEST(NFAGenerator, GoodData) {
	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("a"));
		EXPECT_TRUE(!nfa->runOnLine("aa"));
	}

	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a&b");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(!nfa->runOnLine("a"));
		EXPECT_TRUE(!nfa->runOnLine("aa"));
		EXPECT_TRUE(nfa->runOnLine("ab"));
		EXPECT_TRUE(!nfa->runOnLine("ba"));
	}

	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a|b");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("a"));
		EXPECT_TRUE(!nfa->runOnLine("aa"));
		EXPECT_TRUE(!nfa->runOnLine("ab"));
		EXPECT_TRUE(!nfa->runOnLine("ba"));
		EXPECT_TRUE(nfa->runOnLine("b"));
	}

	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a*");
		EXPECT_TRUE(nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("a"));
		EXPECT_TRUE(nfa->runOnLine("aa"));
		EXPECT_TRUE(nfa->runOnLine("aaaaaaaa"));
		EXPECT_TRUE(!nfa->runOnLine("ab"));
		EXPECT_TRUE(!nfa->runOnLine("ba"));
		EXPECT_TRUE(!nfa->runOnLine("b"));
	}

	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a|b&c");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("bc"));
		EXPECT_TRUE(nfa->runOnLine("a"));
		EXPECT_TRUE(!nfa->runOnLine("abc"));
		EXPECT_TRUE(!nfa->runOnLine("ab"));
		EXPECT_TRUE(!nfa->runOnLine("ac"));
	}

	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("(a|b)&c");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("bc"));
		EXPECT_TRUE(nfa->runOnLine("ac"));
		EXPECT_TRUE(!nfa->runOnLine("abc"));
		EXPECT_TRUE(!nfa->runOnLine("ab"));
	}
	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("a**&c");
		EXPECT_TRUE(!nfa->runOnLine(""));
		EXPECT_TRUE(nfa->runOnLine("c"));
		EXPECT_TRUE(nfa->runOnLine("ac"));
		EXPECT_TRUE(nfa->runOnLine("aaaaaac"));
		EXPECT_TRUE(!nfa->runOnLine("abc"));
		EXPECT_TRUE(!nfa->runOnLine("ab"));
	}
}

TEST(NFAGenerator, BadData) {
	{
		NFAGenerator generator;
		std::shared_ptr<NFA> nfa = generator.generate("");
		EXPECT_TRUE(nfa->runOnLine(""));
		EXPECT_TRUE(!nfa->runOnLine("a"));
		EXPECT_TRUE(!nfa->runOnLine("0"));
		EXPECT_TRUE(!nfa->runOnLine("8fd"));
	}
	{
		NFAGenerator generator;

		bool error = false;
		try {
			std::shared_ptr<NFA> nfa = generator.generate("ac");
		}
		catch (const std::exception&) {
			error = true;
		}
		EXPECT_TRUE(error);
	}
	{
		NFAGenerator generator;

		bool error = false;
		try {
			std::shared_ptr<NFA> nfa = generator.generate("a&&c");
		}
		catch (const std::exception&) {
			error = true;
		}
		EXPECT_TRUE(error);
	}
	{
		NFAGenerator generator;

		bool error = false;
		try {
			std::shared_ptr<NFA> nfa = generator.generate("a**c");
		}
		catch (const std::exception&) {
			error = true;
		}
		EXPECT_TRUE(error);
	}
	{
		NFAGenerator generator;

		bool error = false;
		try {
			std::shared_ptr<NFA> nfa = generator.generate("a**|&c");
		}
		catch (const std::exception&) {
			error = true;
		}
		EXPECT_TRUE(error);
	}
	{
		NFAGenerator generator;

		bool error = false;
		try {
			std::shared_ptr<NFA> nfa = generator.generate("a*c*");
		}
		catch (const std::exception&) {
			error = true;
		}
		EXPECT_TRUE(error);
	}
}