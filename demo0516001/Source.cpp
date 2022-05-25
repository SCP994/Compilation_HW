#include "LexicalAnalysis.h"
#include "GrammarAnalysis.h"
using namespace std;

void test()
{
    LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input.txt");
    lexicalAnalysis.run();
    list<pair<int, string>> list = lexicalAnalysis.getSequences();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        cout << "key: " << (*it).first << ", value: " << (*it).second << endl;
    }

    GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar.txt");
    grammarAnalysis.readAndSaveGrammar();
    //grammarAnalysis.eliminateLeftRecursion();
    //grammarAnalysis.simplifyGrammar();
    //grammarAnalysis.extractLeftCommonFactor();
    grammarAnalysis.listToVector();
    grammarAnalysis.printString();
    grammarAnalysis.getFirst();
    grammarAnalysis.getFirstRight();
    grammarAnalysis.printFirstSet();
    grammarAnalysis.printFirstRightSet();
    grammarAnalysis.setEndSymbol();
    grammarAnalysis.getFollow();
    grammarAnalysis.printFollowSet();
    grammarAnalysis.getSelect();
    grammarAnalysis.printSelectSet();
    if (grammarAnalysis.generateLL1Table())
        grammarAnalysis.printLL1Table();

    //cout << endl;
    //grammarAnalysis.grammarAnalyse(list);

    grammarAnalysis.getItemSet();
    grammarAnalysis.generateSLR1Table();
}

int main()
{
    test();

	return 0;
}
