﻿#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <tuple>
#include <set>
#include <stack>
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

	void listToVector();	// 语法规则 list 转换成 vector，使用 list 还是 vector 视情况而定

	bool getFirst();

	bool getFirstRight();

	void setEndSymbol();

	bool getFollow();

	bool getSelect();

	bool generateLL1Table();

	bool grammarAnalyseLL1(list<pair<int, string> >&);

	bool grammarAnalyseSLR1(list<pair<int, string> >&);

	void print();

	void printString();

	void printFirstSet();

	void printFirstRightSet();

	void printFollowSet();

	void printSelectSet();

	void printLL1Table();

	void printNumAsString(int);

	set<pair<int, list<int> > > closure(set<pair<int, list<int> > >);

	set<pair<int, list<int> > > go(int, int);

	void getItemSet();

	int itemSetFind(set<pair<int, list<int> > >&);

	bool generateSLR1Table();

	void printItemSet();

	void printSLR1Table();

	void printGoMap();

private:
	string grammarFilepath;	// 语法文件地址

	int count;	// 终结符和非终结符数量，不计算空集

	int start = 1;	// 开始符号

	unordered_map<string, tuple<int, int, int> > lettersToNums;	// 后两个 int 第一个指是否为非终结符，第二个指能否推出空集

	unordered_map<int, tuple<string, int, int> > numsToLetters; // 后两个 int 的作用同上

	unordered_map<string, list<string> > grammarString;

	unordered_map<int, list<list<int> > > grammarNums;

	unordered_map<int, list<vector<int> > > grammarNumsVector;

	unordered_map<int, set<int> > firstSet;

	unordered_map<int, list<pair<set<int>, int> > > firstRightSet;  // unordered_map<非终结符号, list<pair<First 集, 能否推出空集的标志位> > >

	unordered_map<int, set<int> > followSet;

	unordered_map<int, list<set<int> > > selectSet;

	unordered_map<int, unordered_map<int, list<int> > > LL1Table;

	unordered_map<int, set<pair<int, list<int> > > > itemSet;

	unordered_map<int, unordered_map<int, int> > goMap;

	// 项目集数量
	int stateCount = 0;

	unordered_map<int, unordered_map<int, tuple<int, int, list<int> > > > SLR1Table;



};
