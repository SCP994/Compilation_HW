#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <tuple>
#include <set>
using namespace std;



class GrammarAnalysis_
{
public:
	struct Node;

	GrammarAnalysis_() = delete;

	GrammarAnalysis_(string);

	bool generateAnalysisTable();

	void getFirst();

	bool judge(string);

	bool insert(set<string>&, set<string>);






private:
	string grammarFilepath;

	unordered_map<string, int> mapping;

	vector<tuple<string, vector<string>, int>> grammar;









	unordered_map<string, pair<vector<string>, int>> grammar_;
	unordered_map<string, set<string>> firstSet;

	unordered_map<string, set<string>> followSet;


	vector<string> v1_;
	vector<pair<string, vector<string>>> first;
	unordered_map<string, vector<string>> first_;
	vector<string> v2;

};
