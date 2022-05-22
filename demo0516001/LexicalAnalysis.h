#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
using namespace std;

class LexicalAnalysis
{
public:
	LexicalAnalysis() = delete;

	LexicalAnalysis(string, string = "");

	void run();

	list<pair<int, string>> getSequences() const;

private:
	string inputFilepath;

	string outputFilepath;

	vector<string> keywords, symbols;

	list<pair<int, string>> binarySequences;

	bool matchKeywords(const string&);

	bool matchIdentifiers(const string&);

	bool matchNum(const string&);

	bool matchSymbols(const string&);

	bool match(const string&);

};
