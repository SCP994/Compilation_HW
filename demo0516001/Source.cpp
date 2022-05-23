#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include <set>
#include "LexicalAnalysis.h"
#include "GrammarAnalysis.h"
using namespace std;

// 1 key, 2 identifier, 3 num, 4 sign


//vector<pair<string, string>> prediction[7][8]{};
pair<string, string> pred[7][8]{};
void construct()
{
    pred[0][0] = pair<string, string>("E", "TE'");
    pred[0][2] = pair<string, string>("E", "TE'");

    pred[1][1] = (pair<string, string>("E'", "null"));
    pred[1][3] = (pair<string, string>("E'", "ATE'"));
    pred[1][4] = (pair<string, string>("E'", "ATE'"));
    pred[1][7] = (pair<string, string>("E'", "null"));

    pred[2][0] = (pair<string, string>("T", "FT'"));
    pred[2][2] = (pair<string, string>("T", "FT'"));

    pred[3][5] = (pair<string, string>("T'", "MFT'"));
    pred[3][6] = (pair<string, string>("T'", "MFT'"));

    pred[3][3] = (pair<string, string>("T'", "null"));
    pred[3][4] = (pair<string, string>("T'", "null"));

    pred[3][1] = (pair<string, string>("T'", "null"));
    pred[3][7] = (pair<string, string>("T'", "null"));

    pred[4][0] = (pair<string, string>("F", "(E)"));
    pred[4][2] = (pair<string, string>("F", "i"));

    pred[5][3] = (pair<string, string>("A", "+"));
    pred[5][4] = (pair<string, string>("A", "-"));

    pred[6][5] = (pair<string, string>("M", "*"));
    pred[6][6] = (pair<string, string>("M", "/"));

}

int getColumn(string str)
{
    string strs[8]{ "(", ")", "i", "+", "-", "*", "/", "$" };

    for (int i = 0; i < 8; ++i)
    {
        if (strs[i] == str)
            return i;
    }
    return -1;
}

int getRow(string str)
{
    string strs[7]{ "E", "E'", "T", "T'", "F", "A", "M" };

    for (int i = 0; i < 7; ++i)
    {
        if (strs[i] == str)
            return i;
    }
    return -1;
}

bool analyze(list<pair<int, string>>& myVector)
{
    int row = 0, col = 0;
    size_t len = 0;
    string s = "";

    stack<string> myStack;
    myStack.push("E");
    for (auto it = myVector.begin(), end = --myVector.end(); ; )
    {

        row = getRow(myStack.top());
        //cout << (*it).second << endl;
        if ((*it).first == 4)
        {
            col = getColumn((*it).second);
        }
        else if ((*it).first == 2)
        {
            col = getColumn("i");
        }
        s = pred[row][col].second;

        if (s == "null")
        {
            myStack.pop();
        }
        else
        {
            len = size(s);
            myStack.pop();
            for (int l = len - 1; l >= 0; --l)
            {
                if (s[l] != '\'')
                {
                    myStack.push(s.substr(l, 1));
                }
                else
                {
                    myStack.push(s.substr(l - 1, 2));
                    --l;
                }
            }

        }


        if (myStack.empty())
            return true;

        while (myStack.top() == (*it).second || myStack.top() == "i")
        {
            myStack.pop();
            if (it != end)
                ++it;
        }



    }


    return false;
}

bool matchKeyWords(string str)
{
    list<string> keyList = { "begin", "end", "if", "then", "else", "for", "while", "do", "and", "or", "not" };

    for (auto iter = keyList.begin(); iter != keyList.end(); ++iter)
        if (*iter == str)
            return true;

    return false;
}

bool matchSigns(string str)
{
    list<string> keyList = { "+", "-", "*", "/", ">", "<", "=", ":=", ">=", "<=", "<>", "++", "--", "(", ")", ";", "#",
    ","};

    for (auto iter = keyList.begin(); iter != keyList.end(); ++iter)
        if (*iter == str)
            return true;

    return false;
}

bool matchIdentifier(string str)
{
    if (str[0] < 'A' || str[0] > 'Z' && str[0] < 'a' && str[0] != '_' || str[0] > 'z')
        return false;
    size_t len = size(str);
    char c = 'a';
    for (int i = 1; i < len; ++i)
    {
        c = str[i];
        if (c != '_' && c < '0' || c > '9' && c < 'A' || c > 'Z' && c < 'a' || c > 'z')
            return false;
    }

    return true;
}

bool matchNum(string str)
{
    size_t t = size(str);
    for (int i = 0; i < t; ++i)
        if (str[i] > '9' || str[i] < '0')
            return false;

    return true;
}

vector<pair<int, string>> myVector{};
map<int, string> myMap{};
bool match(string str)
{
    size_t len = size(str);
    int start = 0;
    for (int i = 0; i < len; ++i)
    {
        if (i < len - 1)
        {
            if (matchSigns(str.substr(i, 2)))
            {
                if (start != i)
                {
                    if (matchKeyWords(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(1, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(1, str.substr(start, i - start)));
                    }
                    if (matchIdentifier(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(2, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(2, str.substr(start, i - start)));
                    }
                    else if (matchNum(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(3, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(3, str.substr(start, i - start)));
                    }
                    else
                        return false;
                }
                myVector.push_back(pair<int, string>(4, str.substr(i, 2)));
                //myMap.insert(pair<int, string>(4, str.substr(i, 2)));
                start = i + 2;
            }
            else if (matchSigns(str.substr(i, 1)))
            {
                if (start != i)
                {
                    if (matchKeyWords(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(1, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(1, str.substr(start, i - start)));
                    }
                    if (matchIdentifier(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(2, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(2, str.substr(start, i - start)));
                    }
                    else if (matchNum(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(3, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(3, str.substr(start, i - start)));
                    }
                    else
                        return false;
                }
                myVector.push_back(pair<int, string>(4, str.substr(i, 1)));
                //myMap.insert(pair<int, string>(4, str.substr(i, 1)));
                start = i + 1;
            }
        }
        else
        {
            if (matchSigns(str.substr(i, 1)))
            {
                if (start != i)
                {
                    if (matchKeyWords(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(1, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(1, str.substr(start, i - start)));
                    }
                    if (matchIdentifier(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(2, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(2, str.substr(start, i - start)));
                    }
                    else if (matchNum(str.substr(start, i - start)))
                    {
                        myVector.push_back(pair<int, string>(3, str.substr(start, i - start)));
                        //myMap.insert(pair<int, string>(3, str.substr(start, i - start)));
                    }
                    else
                        return false;
                }
                myVector.push_back(pair<int, string>(4, str.substr(i, 1)));
                //myMap.insert(pair<int, string>(4, str.substr(i, 1)));
                start = i + 1;
            }
            else
            {
                if (matchKeyWords(str.substr(start, i - start + 1)))
                {
                    myVector.push_back(pair<int, string>(1, str.substr(start, i - start + 1)));
                    //myMap.insert(pair<int, string>(1, str.substr(start, i - start + 1)));
                }
                if (matchIdentifier(str.substr(start, i - start + 1)))
                {
                    myVector.push_back(pair<int, string>(2, str.substr(start, i - start + 1)));
                    //myMap.insert(pair<int, string>(2, str.substr(start, i - start + 1)));
                }
                else if (matchNum(str.substr(start, i - start + 1)))
                {
                    myVector.push_back(pair<int, string>(3, str.substr(start, i - start + 1)));
                    //myMap.insert(pair<int, string>(3, str.substr(start, i - start + 1)));
                }
                else
                    return false;
            }
        }
    }
    return true;
}

void testt()
{
    fstream newfile;

    newfile.open("C:/Users/Ohh/Desktop/compiler/input.txt", ios::in); //open a file to perform read operation using file object
    if (newfile.is_open())
    {   //checking whether the file is open
        string tp;

        string space_delimiter = " ";

        vector<string> words{};
        size_t pos = 0;
        while (getline(newfile, tp))
        { //read data from file object and put it into string.
            cout << "Input: " << tp << endl << endl; //print the data of the string

            pos = 0;
            while ((pos = tp.find(space_delimiter)) != string::npos)
            {
                words.push_back(tp.substr(0, pos));
                tp.erase(0, pos + space_delimiter.length());
            }
            words.push_back(tp);


            for (const auto& str : words)
            {
                //cout << str << endl;
                match(str);
            }
        }
        newfile.close(); //close the file object.
    }
    
    //newfile.open("tpoint.txt", ios::out);  // open a file to perform write operation using file object
    //if (newfile.is_open()) //checking whether the file is open
    //{
    //    newfile << "Tutorials point \n";   //inserting text
    //    newfile.close();    //close the file object
    //}
    
}

void test()
{
    LexicalAnalysis lexicalAnalysis("C:/Users/Ohh/Desktop/compiler/input.txt");
    lexicalAnalysis.run();
    list<pair<int, string>> list = lexicalAnalysis.getSequences();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        cout << "key: " << (*it).first << ", value: " << (*it).second << endl;
    }

    construct();
    cout << analyze(list) << " answer" << endl;
    cout << (pred[0][3].first == "") << endl;
    cout << pred[0][0].first << endl;
}

int main()
{
    //test();

    GrammarAnalysis grammarAnalysis("C:/Users/Ohh/Desktop/compiler/grammar.txt");
    grammarAnalysis.readAndSaveGrammar();
    grammarAnalysis.printString();
    grammarAnalysis.eliminateLeftRecursion();
    grammarAnalysis.printString();
    grammarAnalysis.simplifyGrammar();
    grammarAnalysis.printString();
    grammarAnalysis.extractLeftCommonFactor();
    grammarAnalysis.printString();






	return 0;
}
