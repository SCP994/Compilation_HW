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
    bool sign = true;
    while (sign)
    {
        sign = false;
        for (auto& i : grammarNums)
        {
            for (auto& j : i.second)
                if (get<1>(numsToLetters[j.front()]) == 0 && firstSet[i.first].find(j.front()) == firstSet[i.first].end())
                {
                    sign = true;
                    firstSet[i.first].insert(j.front());
                }
                else if (get<1>(numsToLetters[j.front()]) == 1)
                    for (auto& k : firstSet[j.front()])
                        if (firstSet[i.first].find(k) == firstSet[i.first].end())
                        {
                            sign = true;
                            firstSet[i.first].insert(k);
                        }
        }
    }
    return true;
}

bool GrammarAnalysis::getFirstRight()
{
    set<int> tempToInsert;
    size_t len = 0;
    int sign = 0;
    for (auto& i : grammarNumsVector)
    {
        for (auto& j : i.second)
            if (j.front() == 0)
            {
                tempToInsert.insert(0);
                firstRightSet[i.first].push_back(pair<set<int>, int>(tempToInsert, 1));
                tempToInsert.clear();
            }
            else
            {
                len = j.size();
                for (int k = 0; k < len; ++k)
                    if (get<2>(numsToLetters[j[k]]) == 1)
                        for (auto& l : firstSet[j[k]])
                        {
                            tempToInsert.insert(l);
                            if (k == len - 1)
                                sign = 1;
                        }
                    else
                    {
                        if (get<1>(numsToLetters[j[k]]) == 1)
                            for (auto& l : firstSet[j[k]])
                                tempToInsert.insert(l);
                        else
                            tempToInsert.insert(j[k]);
                        break;
                    }
                firstRightSet[i.first].push_back(pair<set<int>, int>(tempToInsert, sign));
                tempToInsert.clear();
                sign = 0;
            }
    }

    return true;
}

void GrammarAnalysis::setEndSymbol()
{
    lettersToNums["$"] = tuple<int, int, int>{ count + 1, 0, 0 };
    numsToLetters[++count] = tuple<string, int, int>{ "$", 0, 0 };
}

bool GrammarAnalysis::getFollow()
{
    followSet[1].insert(count);

    bool sign = true;
    size_t len = 0;
    while (sign)
    {
        sign = false;
        for (auto& i : grammarNumsVector)
            for (auto& j : i.second)
            {
                len = j.size();
                for (int k = 0; k < len; ++k)
                    if (get<1>(numsToLetters[j[k]]) == 1)
                        if (k == len - 1)   // A -> aB
                        {
                            for (auto& l : followSet[i.first])
                                if (followSet[j[k]].find(l) == followSet[j[k]].end())
                                {
                                    sign = true;
                                    followSet[j[k]].insert(l);
                                }
                        }   // 删掉这大括号，下面 else 会匹配错 if
                        else
                            for (int l = k + 1; l < len; ++l) // A -> aBc
                            {
                                if (get<2>(numsToLetters[j[l]]) != 1)   // 将 c 中不能推出空集的符号之前的非终结符 first 集加到 A 的 follow 集中
                                {
                                    for (int m = k + 1; m < l; ++m)
                                        for (auto& n : firstSet[j[m]])
                                            if (followSet[j[k]].find(n) == followSet[j[k]].end())
                                            {
                                                sign = true;
                                                followSet[j[k]].insert(n);
                                            }
                                    if (get<1>(numsToLetters[j[l]]) == 1)   // c 是不能推出空集的非终结符
                                    {
                                        for (auto& n : firstSet[j[l]])
                                            if (followSet[j[k]].find(n) == followSet[j[k]].end())
                                            {
                                                sign = true;
                                                followSet[j[k]].insert(n);
                                            }
                                    }
                                    else if (followSet[j[k]].find(j[l]) == followSet[j[k]].end())   // c 是终结符
                                    {
                                        sign = true;
                                        followSet[j[k]].insert(j[l]);
                                    }
                                    break;
                                }
                                if (l == len - 1)   // A -> aBc; c -> ^
                                {
                                    for (int m = k + 1; m < len; ++m)
                                        for (auto& n : firstSet[j[m]])
                                            if (followSet[j[k]].find(n) == followSet[j[k]].end())
                                            {
                                                sign = true;
                                                followSet[j[k]].insert(n);
                                            }
                                    for (auto& m : followSet[i.first])
                                        if (followSet[j[k]].find(m) == followSet[j[k]].end())
                                        {
                                            sign = true;
                                            followSet[j[k]].insert(m);
                                        }
                                }
                            }
            }
    }
    for (auto& i : followSet)   // 删除添加进去的空集
        i.second.erase(0);

    return true;
}

bool GrammarAnalysis::getSelect()
{
    set<int> tempToInsert;

    auto i = grammarNumsVector.begin();
    for (;i != grammarNumsVector.end(); ++i)
    {
        auto j = (*i).second.begin();
        auto k = firstRightSet[(*i).first].begin(); // 注意保持两张表的一致，哈希表遍历没有顺序
        for (; j != (*i).second.end(); ++j, ++k)
        {
            for (auto& m : (*k).first)  // 将产生式右部 first 集加进去
                if (m != 0)
                    tempToInsert.insert(m);

            if ((*k).second != 1)   // 产生式右部不能推出空集
                selectSet[(*i).first].push_back(tempToInsert);
            else
            {
                for (auto& m : followSet[(*i).first])   // 将产生式左边非终结符 follow 集加进去
                    tempToInsert.insert(m);
                selectSet[(*i).first].push_back(tempToInsert);
                tempToInsert.clear();
            }
            tempToInsert.clear();
        }
    }

    return true;
}

bool GrammarAnalysis::generateLL1Table()
{
    for (auto& i : selectSet)
    {
        auto j = i.second.begin();
        auto k = grammarNums[i.first].begin();
        for (; j != i.second.end(); ++j, ++k)
            for (auto& l : *j)
            {
                if (LL1Table[i.first][l].size() > 0)
                {
                    cout << "This is not LL1 grammar." << endl;
                    return false;   // 不是 LL1 文法
                }
                LL1Table[i.first][l] = *k;
            }
    }

    return true;
}

bool GrammarAnalysis::grammarAnalyseLL1(list<pair<int, string> >& list)
{
    stack<int> inputString, tempStack;
    inputString.push(get<0>(lettersToNums["$"]));
    tempStack.push(get<0>(lettersToNums["$"]));
    tempStack.push(start);   // 将开始符号压入栈中

    auto i = list.end();
    do
    {
        --i;
        if ((*i).first == 2)
            inputString.push(get<0>(lettersToNums["i"]));
        else if ((*i).first == 4)
            inputString.push(get<0>(lettersToNums[(*i).second]));
        else
        {
            cout << "Error: " << "input string" << endl;
            return false;
        }

    } while (i != list.begin());

    int row, col;
    bool sign = true;
    while (tempStack.size() > 1)
    {
        row = tempStack.top();
        col = inputString.top();

        if (LL1Table[row].find(col) == LL1Table[row].end())
        {
            cout << "Error: ";
            printNumAsString(row);
            cout << " ";
            printNumAsString(col);
            cout << " " << endl;
            inputString.pop();
            sign = false;
        }
        else
        {
            tempStack.pop();
            if (LL1Table[row][col].front() != 0)
            {
                auto i = LL1Table[row][col].end();
                do
                {
                    --i;
                    tempStack.push(*i);
                } while (i != LL1Table[row][col].begin());
            }
        }

        while (tempStack.top() == inputString.top() && tempStack.top() != get<0>(lettersToNums["$"]))
        {
            cout << "Matched: ";
            printNumAsString(tempStack.top());
            cout << " " << endl;
            tempStack.pop();
            inputString.pop();
        }
    }

    if (sign)
        cout << "Match successfully!" << endl;
    else
        cout << "Failed to match." << endl;
    return sign;
}

bool GrammarAnalysis::grammarAnalyseSLR1(list<pair<int, string> >& list)
{
    //	unordered_map<int, unordered_map<int, tuple<int, int, list<int> > > > SLR1Table;

    stack<int> inputString, tempStack, temp;
    inputString.push(get<0>(lettersToNums["$"]));
    tempStack.push(0);   // 将初始状态符号 0 压入栈中

    auto i = list.end();
    do
    {
        --i;
        if ((*i).first == 2)
            inputString.push(get<0>(lettersToNums["i"]));
        else if ((*i).first == 4)
            inputString.push(get<0>(lettersToNums[(*i).second]));
        else
        {
            cout << "Error: " << "input string" << endl;
            return false;
        }

    } while (i != list.begin());

    int row, col;
    while (tempStack.size() > 0)
    {
        row = tempStack.top();
        col = inputString.top();
        if (get<0>(SLR1Table[row][col]) == -1)  // Acc
        {
            cout << "Match successfully!" << endl;
            return true;
        }
        else if (get<0>(SLR1Table[row][col]) == 0)
        {
            cout << 0 << endl;
            tempStack.push(inputString.top());  // 压入输入串第一个字符
            inputString.pop();
            tempStack.push(get<1>(SLR1Table[row][col]));    // 压入状态
        }
        else if (get<0>(SLR1Table[row][col]) == 2)
        {
            cout << 2 << endl;
            for (int i = 0; i < get<2>(SLR1Table[row][col]).size() - 1; ++i)    // list<int> 存了产生式左边的符号和右边的符号，所以这里 size 要减 1
            {
                tempStack.pop();    // 弹出状态数字
                tempStack.pop();    // 弹出终结符号或非终结符号
            }
            col = get<2>(SLR1Table[row][col]).front();
            row = tempStack.top();
            tempStack.push(col);
            tempStack.push(get<1>(SLR1Table[row][col]));
        }
        else
            break;
    }

    cout << "Match failed" << endl;
    return false;
}

void GrammarAnalysis::listToVector()
{
    vector<int> tempToInsert;
    for (auto& i : grammarNums)
        for (auto& j : i.second)
        {
            for (auto& k : j)
                tempToInsert.push_back(k);
            grammarNumsVector[i.first].push_back(tempToInsert); // list 转换成 vector，更方便随机访问
            tempToInsert.clear();
        }
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

void GrammarAnalysis::printFirstSet()
{
    cout << "***************************************** first set" << endl;
    for (auto& i : firstSet)
    {
        if (get<0>(numsToLetters[i.first]) == "___")
            cout << "key: " << i.first << ", value: " << endl;
        else
            cout << "key: " << get<0>(numsToLetters[i.first]) << ", value: " << endl;
        for (auto& j : i.second)
            if (get<0>(numsToLetters[j]) == "___")
                cout << j << endl;
            else
                cout << get<0>(numsToLetters[j]) << endl;
    }
    cout << "*****************************************" << endl;
}

void GrammarAnalysis::printFirstRightSet()
{
    cout << "***************************************** first right set" << endl;
    for (auto& i : firstRightSet)
    {
        if (get<0>(numsToLetters[i.first]) == "___")
            cout << "key: " << i.first << ", value: " << endl;
        else
            cout << "key: " << get<0>(numsToLetters[i.first]) << ", value: " << endl;
        for (auto& j : i.second)
        {
            for (auto& k : j.first)
                cout << get<0>(numsToLetters[k]) << " ";
            cout << "It can get to e: " << j.second << endl;
        }
    }
    cout << "*****************************************" << endl;
}

void GrammarAnalysis::printFollowSet()
{
    cout << "***************************************** follow set" << endl;
    for (auto& i : followSet)
    {
        if (get<0>(numsToLetters[i.first]) == "___")
            cout << "key: " << i.first << ", value: " << endl;
        else
            cout << "key: " << get<0>(numsToLetters[i.first]) << ", value: " << endl;
        for (auto& j : i.second)
            if (get<0>(numsToLetters[j]) == "___")
                cout << j << endl;
            else
                cout << get<0>(numsToLetters[j]) << endl;
    }
    cout << "*****************************************" << endl;
}

void GrammarAnalysis::printSelectSet()
{
    cout << "***************************************** select set" << endl;
    for (auto& i : selectSet)
    {
        if (get<0>(numsToLetters[i.first]) == "___")
            cout << "key: " << i.first << ", value: " << endl;
        else
            cout << "key: " << get<0>(numsToLetters[i.first]) << ", value: " << endl;
        for (auto& j : i.second)
        {
            for (auto& k : j)
                cout << get<0>(numsToLetters[k]) << " ";
            cout << endl;
        }
    }
    cout << "*****************************************" << endl;
}

void GrammarAnalysis::printLL1Table()
{
    cout << "***************************************** LL1 Table" << endl;
    for (auto& i : LL1Table)
    {
        cout << "row: ";
        printNumAsString(i.first);
        cout << endl;
        for (auto& j : i.second)
        {
            cout << "   column: ";
            printNumAsString(j.first);
            cout << endl << "   ";
            for (auto& k : j.second)
            {
                printNumAsString(k);
                cout << " ";
            }
            cout << endl;
        }
    }
    cout << "*****************************************" << endl;
}

void GrammarAnalysis::printNumAsString(int num)
{
    if (get<0>(numsToLetters[num]) == "___")
        cout << num;
    else if (num == -1)
        cout << "dot";
    else
        cout << get<0>(numsToLetters[num]);
}

set<pair<int, list<int> > > GrammarAnalysis::closure(set<pair<int, list<int> > > closure)
{
    pair<int, list<int> > tempToInsert;
    bool sign = true;
    while (sign)
    {
        sign = false;
        for (auto& i : closure)
            for (auto j = i.second.begin(); j != i.second.end(); ++j)
                if ((*j) == -1 && ++j != i.second.end())
                {
                    if (get<1>(numsToLetters[*j]) == 1)
                    {
                        tempToInsert.first = *j;
                        for (auto& k : grammarNums[*j])
                        {
                            tempToInsert.second.push_back(-1);
                            if (k.front() != 0)
                                for (auto& l : k)
                                    tempToInsert.second.push_back(l);

                            if (closure.find(tempToInsert) == closure.end()) // 未添加的产生式
                            {
                                sign = true;
                                closure.insert(tempToInsert);
                            }
                            tempToInsert.second.clear();
                        }
                    }
                    break;
                }
                else if (j == i.second.end())   // 注意一定要有这句
                    break;
    }
    return closure;
}

set<pair<int, list<int> > > GrammarAnalysis::go(int I, int symbol)
{
    set<pair<int, list<int> > > J;
    pair<int, list<int> > tempToInsert;
    for (auto& i : itemSet[I])
        for (auto j = i.second.begin(); j != i.second.end(); ++j)
        {
            tempToInsert.first = i.first;
            if (*j == -1 && ++j != i.second.end())
            {
                if (*j == symbol)
                {
                    for (auto k = i.second.begin(); k != i.second.end(); ++k)
                        if (*k != -1)
                            tempToInsert.second.push_back(*k);
                        else
                        {
                            tempToInsert.second.push_back(*++k);
                            tempToInsert.second.push_back(-1);
                        }
                    J.insert(tempToInsert);
                    tempToInsert.second.clear();
                }
                break;
            }
            else if (j == i.second.end())
                break;
        }

    return closure(J);
}

void GrammarAnalysis::getItemSet()
{
    set<pair<int, list<int> > > tempSet;
    list<int> tempList{ -1 };
    for (auto& i : grammarNums[start].front())
        tempList.push_back(i);

    tempSet.insert(pair<int, list<int> >{ start, tempList });
    itemSet[stateCount++] = closure(tempSet);
    bool sign = true;
    while (sign)
    {
        sign = false;
        for (auto& i : itemSet)
            for (auto& j : numsToLetters)
                if (j.first != 0)
                {
                    tempSet = go(i.first, j.first);
                    if (tempSet.size() > 0)
                    {
                        goMap[i.first][j.first] = itemSetFind(tempSet);
                        if (goMap[i.first][j.first] == -1) // 没有找到
                        {
                            sign = true;
                            goMap[i.first][j.first] = stateCount;
                            itemSet.insert(pair<int, set<pair<int, list<int> > > >{ stateCount++, tempSet });   // 此时插入的值不会被遍历到
                        }
                    }
                }
    }
    printItemSet();
}

int GrammarAnalysis::itemSetFind(set<pair<int, list<int> > >& set)
{
    for (auto& i : itemSet)
        if (i.second == set)
            return i.first;

    return -1;
}

void GrammarAnalysis::printItemSet()
{
    for (auto& i : itemSet)
    {
        cout << "State: I" << i.first << endl;
        for (auto& j : i.second)
        {
            printNumAsString(j.first);
            cout << " -> ";
            for (auto& k : j.second)
            {
                printNumAsString(k);
                cout << " ";
            }
            cout << "; ";
        }
        cout << endl;
    }
}

bool GrammarAnalysis::generateSLR1Table()
{
    cout << "Begin SLR1:" << endl;
    int endCol = get<0>(lettersToNums["$"]);
    int endRow = 0;

    set<int> tempFollowSet;
    for (int i = 0; i < stateCount; ++i)    // 判断文法的冲突能否解决
    {
        for (auto& k : itemSet[i])
        {
            if (*--(k.second.end()) == -1)
            {
                if (k.first == start)
                    endRow = i;
                for (auto& l : followSet[k.first])
                    if (tempFollowSet.find(l) != tempFollowSet.end())
                    {
                        cout << i << endl;
                        printNumAsString(l);
                        cout << "fail1" << endl;
                        return false;
                    }
                    else
                        tempFollowSet.insert(l);
            }
            else
                for (auto l = k.second.begin(); l != k.second.end(); ++l)
                    if (*l == -1)
                    {
                        if (get<1>(numsToLetters[*++l]) == 0)
                            if (tempFollowSet.find(*l) != tempFollowSet.end())
                            {
                                cout << "fail2" << endl;
                                return false;
                            }
                            else
                                tempFollowSet.insert(*l);
                        break;
                    }
        }
        tempFollowSet.clear();
    }

    list<int> tempList;
    for (int i = 0; i < stateCount; ++i)
    {
        for (auto& j : numsToLetters)
        {
            if (j.first == endCol && i == endRow)   // Acc 特殊处理
            {
                SLR1Table[i][j.first] = tuple<int, int, list<int> >{ -1, 0, list<int>{} };
                continue;
            }
            if (goMap[i].find(j.first) != goMap[i].end())
            {
                if (get<1>(numsToLetters[j.first]) == 1)
                    SLR1Table[i][j.first] = tuple<int, int, list<int> >{ 1, goMap[i][j.first], list<int>{} };
                else
                    SLR1Table[i][j.first] = tuple<int, int, list<int> >{ 0, goMap[i][j.first], list<int>{} };
                continue;
            }
            for (auto& k : itemSet[i])
                if (*--k.second.end() == -1)  // 归约项目
                    if (followSet[k.first].find(j.first) != followSet[k.first].end())
                    {
                        tempList = k.second;
                        tempList.push_front(k.first);
                        tempList.pop_back();    // 把结尾的 点 弹出！！！
                        SLR1Table[i][j.first] = tuple<int, int, list<int> >{ 2, 0, tempList };
                        break;
                    }
        }
    }

    printSLR1Table();
    cout << "SLR1Table generated." << endl;
    return true;
}

void GrammarAnalysis::printSLR1Table()
{
    //unordered_map<int, unordered_map<int, tuple<int, int, list<int>> > > SLR1Table;

    for (int i = 0; i < stateCount; ++i)
    {
        cout << "Row: " << i << endl;
        for (auto& i : SLR1Table[i])
        {
            cout << "   ";
            printNumAsString(i.first);
            cout << ": " << get<0>(i.second) << ", " << get<1>(i.second) << endl;
        }
    }
}

void GrammarAnalysis::printGoMap()
{
    for (int i = 0; i < stateCount; ++i)
    {
        for (auto& j : goMap[i])
        {
            cout << "state: " << i << " and ";
            printNumAsString(j.first);
            cout << " go to " << j.second << endl;

        }

    }


}
