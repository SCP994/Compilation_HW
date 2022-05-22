#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
using namespace std;

class GrammarAnalysis
{
public:
	GrammarAnalysis() = delete;

	GrammarAnalysis(string);

	bool generateAnalysisTable();

private:
	string grammarFilepath;

	unordered_map<string, int> mapping;

	vector<pair<string, string>> grammar;

};