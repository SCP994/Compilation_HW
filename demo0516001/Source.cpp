#include "LexicalAnalysis.h"
#include "GrammarAnalysis.h"
using namespace std;

void testLexicalAnalysis()
{
    LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input3.txt");

    lexicalAnalysis.run();
    list<pair<int, string>> list = lexicalAnalysis.getSequences();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        cout << "key: " << (*it).first << ", value: " << (*it).second << endl;
    }
}



void test()
{
    LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input0.txt");    // SLR1
    //LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input1.txt");  // SLR1
    //LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input2.txt");    // LL1
    //LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input3.txt");

    // 词法分析
    lexicalAnalysis.run();
    list<pair<int, string>> list = lexicalAnalysis.getSequences();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        cout << "key: " << (*it).first << ", value: " << (*it).second << endl;
    }

    GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar0.txt");
    //GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar1.txt");
    //GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar2.txt");

    // 读取并保存文法
    grammarAnalysis.readAndSaveGrammar();

    // 消除左递归、化简文法、提取左公共因子，分析SLR1文法用不到这三个
    //grammarAnalysis.eliminateLeftRecursion();
    //grammarAnalysis.simplifyGrammar();
    //grammarAnalysis.extractLeftCommonFactor();

    grammarAnalysis.listToVector();
    grammarAnalysis.printString();

    // 获取非终结符号的 First 集
    grammarAnalysis.getFirst();

    // 获取产生式右部的 First 集
    grammarAnalysis.getFirstRight();
    grammarAnalysis.printFirstSet();
    grammarAnalysis.printFirstRightSet();

    // 将结束符号 $ 保存到非终结符号中
    grammarAnalysis.setEndSymbol();

    // 获取 Follow 集
    grammarAnalysis.getFollow();
    grammarAnalysis.printFollowSet();

    // 获取 Select 集
    grammarAnalysis.getSelect();
    grammarAnalysis.printSelectSet();


    // LL1文法分析
    //if (grammarAnalysis.generateLL1Table())
    //    grammarAnalysis.printLL1Table();

    //grammarAnalysis.grammarAnalyseLL1(list);


    // SLR1文法分析
    grammarAnalysis.getItemSet();
    grammarAnalysis.generateSLR1Table();
    grammarAnalysis.grammarAnalyseSLR1(list);

}

int main()
{
    test();

	return 0;
}
