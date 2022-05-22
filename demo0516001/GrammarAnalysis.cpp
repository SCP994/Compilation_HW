#include "GrammarAnalysis.h"

GrammarAnalysis::GrammarAnalysis(string str)
{
	grammarFilepath = str;
}

bool GrammarAnalysis::generateAnalysisTable()
{
    fstream newfile;
    newfile.open(grammarFilepath, ios::in);

    if (newfile.is_open())
    {
        string str;
        string delimiter = "->";

        vector<string> words{};
        size_t pos = 0;
        while (getline(newfile, str))
        {
            cout << "Input: " << str << endl;

            pos = 0;
            while ((pos = str.find(delimiter)) != string::npos)
            {
                words.push_back(str.substr(0, pos));
                str.erase(0, pos + delimiter.length());
            }
            words.push_back(str);

            if (words.size() != 2)
                throw "Grammar file format is not right.";

            for (const auto& str : words)
            {
                cout << "Matching \"" << str << "\": ";
            }
            cout << endl;
            words.clear();
        }

        newfile.close();
    }
    return true;
}