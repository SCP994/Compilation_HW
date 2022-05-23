#include "GrammarAnalysis_.h"

GrammarAnalysis_::GrammarAnalysis_(string str)
{
	grammarFilepath = str;
}

bool GrammarAnalysis_::generateAnalysisTable()
{
    fstream newfile;
    newfile.open(grammarFilepath, ios::in);

    if (newfile.is_open())
    {
        string str;
        string arrowDelimiter = "->";
        string orDelimiter = "|";

        vector<string> words{};
        size_t pos = 0;
        vector<string> vec;
        int sign = 0;
        while (getline(newfile, str))
        {
            cout << "Input: " << str << endl;

            while ((pos = str.find(arrowDelimiter)) != string::npos)
            {
                words.push_back(str.substr(0, pos));
                str.erase(0, pos + arrowDelimiter.length());
            }
            words.push_back(str);

            if (words.size() != 2)
                throw "Grammar file format is not right.";

            str = words[1];
            words.pop_back();
            while ((pos = str.find(orDelimiter)) != string::npos)
            {
                words.push_back(str.substr(0, pos));
                str.erase(0, pos + orDelimiter.length());
            }
            words.push_back(str);

            for (auto it = ++words.begin(); it != words.end(); ++it)
            {
                if ((*it) == "e")
                    sign = 1;
                vec.push_back(*it);
            }
            v1_.push_back(words[0]);
            first.push_back(pair<string, vector<string>>(words[0], vector<string>()));
            first_[words[0]] = vector<string>();
            grammar.push_back(tuple<string, vector<string>, int>(words[0], vec, sign));
            grammar_[words[0]] = pair<vector<string>, int>(vec, sign);
            vec.clear();
            words.clear();
            sign = 0;







            //for (const auto& str : words)
            //{
            //    cout << "Matching \"" << str << "\": ";
            //}
            //cout << endl;
        }
        for (const auto& i : grammar)
        {
            cout << get<0>(i) << " " << get<1>(i).size() << " " << get<2>(i) << endl;
            mapping[get<0>(i)] = get<2>(i);

        }
        cout << endl;


        getFirst();






        newfile.close();
    }
    return true;
}

bool GrammarAnalysis_::insert(set<string>& destination, set<string> source)
{
    for (auto it = source.begin(); it != source.end(); ++it)
    {
        if (destination.find(*it) == destination.end())
        {
            destination.insert(source.begin(), source.end());
            return true;
        }
    }
    return false;
}


void GrammarAnalysis_::getFirst()
{
    int t = 0;
    while (true)
    {
        for (auto& item : grammar_)
        {
            for (auto& i : item.second.first)
            {
                if (i.size() > 1 && judge(i.substr(0, 2)))
                {
                    if (insert(firstSet[item.first], firstSet[i.substr(0, 2)]))
                    {
                        t = 1;
                    }
                }
                else if (judge(i.substr(0, 1)))
                {
                    if (insert(firstSet[item.first], firstSet[i.substr(0, 1)]))
                    {
                        t = 1;
                    }
                }
                else if (i == "e")
                {
                    //firstSet[item.first].insert("e");
                    if (insert(firstSet[item.first], set<string>{ "e" }))
                    {
                        t = 1;
                    }
                }
                else
                {
                    if (insert(firstSet[item.first], set<string>{ i.substr(0, 1) }))
                    {
                        t = 1;
                    }
                }


            }

        }
        if (t == 0)
            break;
        t = 0;
    }




    for (auto& t : firstSet)
    {
        cout << t.first << " start: " << endl;
        for (auto& i : t.second)
        {
            cout << i << " xyz " << endl;
        }
    }


}

bool GrammarAnalysis_::judge(string str)
{
    for (auto& item : v1_)
    {
        if (item == str)
            return true;
    }
    return false;
}

