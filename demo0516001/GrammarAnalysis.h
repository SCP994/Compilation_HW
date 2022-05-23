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


	bool generateAnalysisTable();

	bool eliminateLeftRecursion();

	bool extractLeftCommonFactor();

	void print();







private:
	string grammarFilepath;	// 语法文件地址

	//string start;

	int count;	// 终结符和非终结符数量，不计算空集

	unordered_map<string, tuple<int, int, int>> lettersToNums;	// 后两个 int 第一个指是否为非终结符，第二个指能否推出空集

	unordered_map<int, tuple<string, int, int>> numsToLetters;

	unordered_map<string, list<string>> grammarString;

	unordered_map<int, list<list<int>>> grammarNums;

};
