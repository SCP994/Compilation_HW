#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <tuple>
#include <set>
using namespace std;

class GrammarAnalysis
{
public:
	GrammarAnalysis() = delete;

	GrammarAnalysis(string);

	bool readAndSaveGrammar();

	bool eliminateLeftRecursion();

	bool simplifyGrammar();

	bool extractLeftCommonFactor();

	bool getFirst();

	void print();

	void printString();

private:
	string grammarFilepath;	// 语法文件地址

	int count;	// 终结符和非终结符数量，不计算空集

	unordered_map<string, tuple<int, int, int>> lettersToNums;	// 后两个 int 第一个指是否为非终结符，第二个指能否推出空集

	unordered_map<int, tuple<string, int, int>> numsToLetters;

	unordered_map<string, list<string>> grammarString;

	unordered_map<int, list<list<int>>> grammarNums;

};
