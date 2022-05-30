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

    //GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar.txt");
    GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar1.txt");
    //GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar2.txt");
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
    //grammarAnalysis.grammarAnalyseLL1(list);

    grammarAnalysis.getItemSet();
    grammarAnalysis.generateSLR1Table();
    grammarAnalysis.grammarAnalyseSLR1(list);

}

int main()
{
    test();

	return 0;
}
