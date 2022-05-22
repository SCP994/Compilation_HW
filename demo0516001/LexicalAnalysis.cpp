#include "LexicalAnalysis.h"

LexicalAnalysis::LexicalAnalysis(string input, string output)
{
	inputFilepath = input;
	outputFilepath = output;
    keywords = { "begin", "end", "if", "then", "else", "for", "while", "do", "and", "or", "not" };
    symbols = { "+", "-", "*", "/", ">", "<", "=", ":=", ">=", "<=", "<>", "++", "--", "(", ")", ";", "#",
    "," };
}

void LexicalAnalysis::run()
{
    fstream newfile;
    newfile.open(inputFilepath, ios::in);

    if (newfile.is_open())
    {
        string str;
        string space_delimiter = " ";

        vector<string> words{};
        size_t pos = 0;
        while (getline(newfile, str))
        {
            cout << "Input: " << str << endl;

            pos = 0;
            while ((pos = str.find(space_delimiter)) != string::npos)
            {
                words.push_back(str.substr(0, pos));
                str.erase(0, pos + space_delimiter.length());
            }
            words.push_back(str);


            for (const auto& str : words)
            {
                cout << "Matching \"" << str << "\": ";
                cout << match(str) << endl;
            }
            words.clear();
        }

        newfile.close();
    }
}

list<pair<int, string>> LexicalAnalysis::getSequences() const
{
    return binarySequences;
}

bool LexicalAnalysis::matchKeywords(const string& str)
{
    for (auto iter = keywords.begin(); iter != keywords.end(); ++iter)
        if (*iter == str)
            return true;

    return false;
}

bool LexicalAnalysis::matchIdentifiers(const string& str)
{
    if (str[0] < 'A' || str[0] > 'Z' && str[0] < 'a' && str[0] != '_' || str[0] > 'z')
        return false;

    int len = str.size();
    char c = ' ';
    for (int i = 1; i < len; ++i)
    {
        c = str[i];
        if (c < '0' || c > '9' && c < 'A' || c > 'Z' && c < 'a' && c != '_' || c > 'z')
            return false;
    }

    return true;
}

bool LexicalAnalysis::matchNum(const string& str)
{
    int len = str.size();
    for (int i = 0; i < len; ++i)
        if (str[i] > '9' || str[i] < '0')
            return false;

    return true;
}

bool LexicalAnalysis::matchSymbols(const string& str)
{
    for (auto iter = symbols.begin(); iter != symbols.end(); ++iter)
        if (*iter == str)
            return true;

    return false;
}

bool LexicalAnalysis::match(const string& str)
{
    int len = str.size();
    int start = 0;
    for (int i = 0; i < len; ++i)
    {
        if ((i < len - 1 && matchSymbols(str.substr(i, 2))) || matchSymbols(str.substr(i, 1)))
        {
            if (start != i)
                if (matchKeywords(str.substr(start, i - start)))
                    binarySequences.push_back(pair<int, string>(1, str.substr(start, i - start)));

                else if (matchIdentifiers(str.substr(start, i - start)))
                    binarySequences.push_back(pair<int, string>(2, str.substr(start, i - start)));

                else if (matchNum(str.substr(start, i - start)))
                    binarySequences.push_back(pair<int, string>(3, str.substr(start, i - start)));

                else
                    return false;

            if (i < len - 1 && matchSymbols(str.substr(i, 2)))
            {
                binarySequences.push_back(pair<int, string>(4, str.substr(i, 2)));
                start = i + 2;
            }
            else
            {
                binarySequences.push_back(pair<int, string>(4, str.substr(i, 1)));
                start = i + 1;
            }
        }
        else if (i == len - 1)
            if (matchKeywords(str.substr(start, i - start + 1)))
                binarySequences.push_back(pair<int, string>(1, str.substr(start, i - start + 1)));

            else if (matchIdentifiers(str.substr(start, i - start + 1)))
                binarySequences.push_back(pair<int, string>(2, str.substr(start, i - start + 1)));

            else if (matchNum(str.substr(start, i - start + 1)))
                binarySequences.push_back(pair<int, string>(3, str.substr(start, i - start + 1)));

            else
                return false;
    }
    return true;
}
