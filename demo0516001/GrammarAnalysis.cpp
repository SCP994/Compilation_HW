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
                if (*it == "^") // 非终结符能推出空集的情况
                {
                    get<2>(lettersToNums[line.front()]) = 1;
                    get<2>(numsToLetters[count]) = 1;
                }
                grammarString[line.front()].push_back(*it);
            }
            line.clear();
        }
        cout << endl;

        size_t len = 0;
        for (auto& i : grammarString)   // 将所有终结符转换为数字
            for (auto& j : i.second)
                if (j == "^" && lettersToNums.find("^") == lettersToNums.end())
                {
                    lettersToNums["^"] = tuple<int, int, int>{ 0, 0, 0 };
                    numsToLetters[0] = tuple<string, int, int>{ "^", 0, 0 };
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
                if (j == "^")
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
    list<list<int>> tempToRemove;
    list<int> tempToInsert;
    for (auto i = numsToLetters.begin(); i != numsToLetters.end(); ++i)   // unordered_map 按添加顺序遍历，最开始添加的都是非终结符
    {
        if (get<1>((*i).second) != 1)
            continue;
        for (auto j = numsToLetters.begin(); j != i; ++j)
        {
            if (get<1>((*j).second) != 1)
                continue;
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

        for (auto& j : grammarNums[(*i).first]) // 消除直接左递归
            if ((*i).first == j.front())
                tempToRemove.push_back(j);
        if (tempToRemove.size() > 0)
        {
            for (auto& j : tempToRemove)
                grammarNums[(*i).first].remove(j);
            for (auto& j : grammarNums[(*i).first])
            {
                if (j.front() == 0)
                    j.pop_front();
                j.push_back(count + 1);
            }
            for (auto& j : tempToRemove)
            {
                j.pop_front();
                j.push_back(count + 1);
                grammarNums[count + 1].push_back(j);
            }
            if (numsToLetters.find(0) == numsToLetters.end())   //若之前文法中没有空集，此时添加
            {
                numsToLetters[0] = tuple<string, int, int>{ "^", 0, 0 };
                lettersToNums["^"] = tuple<int, int, int>{ 0, 0, 0 };
            }
            grammarNums[count + 1].push_back(list<int>{ 0 });
            numsToLetters[++count] = tuple<string, int, int>{ "___", 1, 1 };
            tempToRemove.clear();
        }
    }

    return true;
}

bool GrammarAnalysis::simplifyGrammar()
{
    set<int> usedSymbols;
    usedSymbols.insert(1);  // 假设 1 是开始符号

    bool sign = true;
    while (sign)
    {
        sign = false;
        for (auto& i : usedSymbols)
            for (auto& j : grammarNums[i])
                for (auto& k : j)
                    if (get<1>(numsToLetters[k]) == 1 && usedSymbols.find(k) == usedSymbols.end())
                    {
                        sign = true;
                        usedSymbols.insert(k);
                    }
    }
    
    list<int> tempToDelete;
    for (auto i = numsToLetters.begin(); i != numsToLetters.end(); ++i)
        if (get<1>((*i).second) == 1 && usedSymbols.find((*i).first) == usedSymbols.end())
        {
            grammarNums.erase(grammarNums.find((*i).first));
            lettersToNums.erase(lettersToNums.find(get<0>(numsToLetters[(*i).first])));
            tempToDelete.push_back((*i).first); // 正在被遍历对象的不能被删掉
        }
    for (auto& i : tempToDelete)
        numsToLetters.erase(numsToLetters.find(i));
 
    return true;
}

bool GrammarAnalysis::extractLeftCommonFactor()
{
    size_t len = 0;
    int sign = 0;
    list<list<int>> tempToChange;
    for (auto& i : grammarNums)
        for (auto j = i.second.begin(); j != i.second.end(); ++j)
            if ((*j).front() != 0)
            {
                tempToChange.push_back(*j);
                for (auto k = j; k != i.second.end(); ++k)  // 注意不能写 k = ++j;
                {
                    if (k == j)
                        continue;
                    if ((*k).front() == (*j).front())
                        tempToChange.push_back(*k);
                }

                if (tempToChange.size() > 1)
                {
                    *j = list<int>{ (*j).front() };
                    (*j).push_back(count + 1);

                    for (auto& k : tempToChange)
                    {
                        i.second.remove(k);
                        k.pop_front();
                        if (k.size() != 0)
                            grammarNums[count + 1].push_back(k);
                        else
                        {
                            sign = 1;   // 判断能否推出空集
                            grammarNums[count + 1].push_back(list<int>{ 0 });
                        }
                    }
                    numsToLetters[++count] = tuple<string, int, int>{ "___", 1, sign };
                    sign = 0;
                }
                tempToChange.clear();
            }

    return true;
}

bool GrammarAnalysis::getFirst()
{





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

void GrammarAnalysis::printString()
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
        cout << "key: ";
        if (get<0>(numsToLetters[item.first]) == "___")
            cout << item.first << ", value: " << endl;
        else
            cout << get<0>(numsToLetters[item.first]) << ", value: " << endl;

        for (auto& j : item.second)
        {
            for (auto& k : j)
                if (get<0>(numsToLetters[k]) == "___")
                    cout << k << " ";
                else
                    cout << get<0>(numsToLetters[k]) << " ";
            cout << endl;
        }
    }
    cout << "*****************************************" << endl;
}
