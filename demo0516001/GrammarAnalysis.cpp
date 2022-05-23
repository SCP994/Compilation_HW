#include "GrammarAnalysis.h"

GrammarAnalysis::GrammarAnalysis(string str)
    : grammarFilepath{ str }, count{ 0 } {}

bool GrammarAnalysis::readAndSaveGrammar()
{
    fstream newfile;
    newfile.open(grammarFilepath, ios::in);

    if (newfile.is_open())
    {
        string str = "", arrowDelimiter = "->", orDelimiter = "|";
        list<string> line;
        size_t pos = 0;
        cout << "Grammar Input: " << endl;
        while (getline(newfile, str))   // 将文法录入为字符串，并将所有非终结符转换为数字
        {
            cout << str << endl;

            while ((pos = str.find(arrowDelimiter)) != string::npos)
            {
                line.push_back(str.substr(0, pos));
                str.erase(0, pos + arrowDelimiter.length());
            }

            while ((pos = str.find(orDelimiter)) != string::npos)   // 此时 str 保存的是 -> 后面的字符串
            {
                line.push_back(str.substr(0, pos));
                str.erase(0, pos + orDelimiter.length());
            }
            line.push_back(str);

            if (lettersToNums.find(line.front()) == lettersToNums.end())    // 重复的非终结符不用再添加
            {
                lettersToNums[line.front()] = tuple<int, int, int>{++count, 1, 0};
                numsToLetters[count] = tuple<string, int, int>{ line.front(), 1, 0};
            }

            for (auto it = ++line.begin(); it != line.end(); ++it)
            {
                if (*it == "e") // 非终结符能推出空集的情况
                {
                    get<2>(lettersToNums[line.front()]) = 1;
                    get<2>(numsToLetters[count]) = 1;
                }
                grammarString[line.front()].push_back(*it);
            }
            line.clear();
        }
        cout << endl;

        int len = 0;
        for (auto& i : grammarString)   // 将所有终结符转换为数字
            for (auto& j : i.second)
                if (j == "e" && lettersToNums.find("e") == lettersToNums.end())
                {
                    lettersToNums["e"] = tuple<int, int, int>{ 0, 0, 0 };
                    numsToLetters[0] = tuple<string, int, int>{ "e", 0, 0 };
                }
                else
                {
                    len = j.size();
                    for (int k = 0; k < len; ++k)
                        if (k < len - 1 && lettersToNums.find(j.substr(k, 2)) != lettersToNums.end())  // 跳过类似 E' 的字符
                            ++k;
                        else if (lettersToNums.find(j.substr(k, 1)) == lettersToNums.end())
                        {
                            lettersToNums[j.substr(k, 1)] = tuple<int, int, int>{ ++count, 0, 0 };
                            numsToLetters[count] = tuple<string, int, int>{ j.substr(k, 1), 0, 0 };
                        }
                }

        list<int> tempVec;
        for (auto& i : grammarString) // 将文法转换为数字
            for (auto& j : i.second)
            {
                if (j == "e")
                    tempVec.push_back(get<0>(lettersToNums[j]));
                else
                {
                    len = j.size();
                    for (int k = 0; k < len; ++k)
                        if (k < len - 1 && lettersToNums.find(j.substr(k, 2)) != lettersToNums.end())
                        {
                            tempVec.push_back(get<0>(lettersToNums[j.substr(k, 2)]));
                            ++k;
                        }
                        else
                            tempVec.push_back(get<0>(lettersToNums[j.substr(k, 1)]));
                }
                grammarNums[get<0>(lettersToNums[i.first])].push_back(tempVec);
                tempVec.clear();
            }
        newfile.close();
    }
    return true;
}

bool GrammarAnalysis::eliminateLeftRecursion()
{
    unordered_map<int, list<list<int>>> map6;
    list<int> listForMap6;
    list<int> temp, tem;
    list<list<int>> t;
    int sign = 0;



    list<list<int>> tempToRemove;
    list<int> tempToInsert;
    for (auto i = numsToLetters.begin(); get<1>((*i).second) == 1; ++i)   // unordered_map 按添加顺序遍历，最开始添加的都是非终结符
    {
        for (auto j = numsToLetters.begin(); j != i; ++j)
        {
            for (auto& k : grammarNums[(*i).first])
                if (k.front() == (*j).first)
                {
                    tempToRemove.push_back(k);
                    for (auto& l : grammarNums[(*j).first])
                    {
                        tempToInsert = l;
                        for (auto m = ++k.begin(); m != k.end(); ++m)
                            tempToInsert.push_back(*m);
                        grammarNums[(*i).first].push_back(tempToInsert);
                    }
                }
            for (auto& k : tempToRemove)
                grammarNums[(*i).first].remove(k);
            tempToRemove.clear();
        }
        for (auto& j : grammarNums[(*i).first])
        {
            if ((*i).first == j.front())
            {
                tempToRemove.push_back(j);


            }
        }



    }













    //for (auto i = grammarNums.begin(); i != grammarNums.end(); ++i)
    //{
    //    for (auto j = grammarNums.begin(); j != i; ++j)
    //    {
    //        for (auto k = (*i).second.begin(); k != (*i).second.end(); ++k)
    //        {
    //            if ((*k).front() == (*j).first)
    //            {
    //                for (auto& l : (*j).second)
    //                {
    //                    if (l.front() != 0)
    //                    {
    //                        for (auto& m : l)
    //                        {
    //                            temp.push_back(m);
    //                        }
    //                    }
    //                    for (auto n = ++(*k).begin(); n != (*k).end(); ++n)
    //                    {
    //                        temp.push_back(*n);
    //                    }
    //                    t.push_back(temp);
    //                    temp.clear();
    //                }
    //                sign = 1;
    //                tem = *k;
    //            }
    //        }
    //        if (sign)
    //        {
    //            (*i).second.remove(tem);

    //            auto o = --t.end();
    //            for (; o != t.begin(); --o)
    //            {
    //                (*i).second.push_front(*o);
    //            }
    //            (*i).second.push_front(*o);

    //            t.clear();
    //            sign = 0;
    //            tem.clear();
    //        }
    //    }
    //    list<list<int>> tt;
    //    for (auto& p : (*i).second)
    //    {
    //        if ((*i).first == p.front())
    //        {
    //            tt.push_back(p);
    //            for (auto it = ++p.begin(); it != p.end(); ++it)
    //            {
    //                listForMap6.push_back(*it);
    //            }
    //            listForMap6.push_back(count + 1);
    //        }
    //        if (listForMap6.size() > 0)
    //        {
    //            map6[count + 1].push_back(listForMap6);
    //        }

    //        listForMap6.clear();
    //    }
    //    if (map6.find(count + 1) != map6.end())
    //    {

    //        for (auto& item : tt)
    //        {
    //            (*i).second.remove(item);
    //        }
    //        for (auto& item : (*i).second)
    //        {
    //            item.push_back(count + 1);
    //        }
    //        map6[count + 1].push_back(list<int>{ 0 });
    //        ++count;
    //    }
    //    tt.clear();

    //}
    //for (auto& item : map6)
    //{
    //    grammarNums[item.first] = item.second;
    //}
    return true;
}

bool GrammarAnalysis::extractLeftCommonFactor()
{
    unordered_map<int, list<list<int>>> map6;

    for (auto& i : grammarNums)
    {
        for (auto& j : i.second)
        {

        }


    }




    return true;
}

void GrammarAnalysis::print()
{
    cout << "*****************************************" << endl;
    cout << "Letters To Numbers: " << endl;
    for (auto& item : lettersToNums)
        cout << "key: " << item.first << ", value: " << get<0>(item.second) << " "
            << get<1>(item.second) << " " << get<2>(item.second) << endl;

    cout << endl << "Numbers to Letters: " << endl;
    for (auto& item : numsToLetters)
        cout << "key: " << item.first << ", value: " << get<0>(item.second) << " "
            << get<1>(item.second) << " " << get<2>(item.second) << endl;

    cout << endl << "Grammar: " << endl;
    for (auto& item : grammarNums)
    {
        cout << "key: " << item.first << ", value: " << endl;
        for (auto& j : item.second)
        {
            for (auto& k : j)
                cout << k << " ";
            cout << endl;
        }
    }
    cout << "*****************************************" << endl;
}
