#include "nfa.h"

#include <QVector>
#include <QString>
#include <QStack>
#include <QPair>
#include <QDebug>

#include "utils.h"

NFA::NFA(int begin, int end): startState(begin), endState(end), stateNum(0), maxStateNum(100) {
    G.resize(maxStateNum);
    for (int i = 0; i < maxStateNum; i++) {
        G[i].resize(maxStateNum);
    }
}

void NFA::clear()
{
    // 清空nfa
    for (int i = 0; i < maxStateNum; i++) {
        G[i].clear();
    }
    G.clear();
    maxStateNum = 100;
    stateNum = 0;
    startState = -1;
    endState = -1;
    G.resize(maxStateNum);
    for (int i = 0; i < maxStateNum; i++) {
        G[i].resize(maxStateNum);
    }
    stateSet.clear();
}

void NFA::fromRegex(QString re)     // 后缀表达式转NFA
{
    QStack<QPair<int, int>> s;
    for (int i = 0; i < re.size(); i++) {
        if (re[i] == '\\') {                        // 转义字符
            if (i < re.size() - 1) {
                allocateMemory(2);
                G[stateNum][stateNum + 1] = re[i + 1];  // 转义符不再出现
                s.append({stateNum, stateNum + 1}); // 入栈
                stateNum += 2;                      // 状态数量 + 2
                stateSet.insert(QString(re[i + 1]));
            }
            i++;
        } else if (re[i] == '*' || re[i] == '|' || re[i] == '.') {   // 运算符
            if (re[i] == '.') {
                if (s.size() >= 2) {                // 取出两个元素相连重新入栈
                    QPair<int, int> tail = s.top();
                    s.pop();
                    QPair<int, int> head = s.top();
                    s.pop();
                    G[head.second][tail.first] = "epsilon";     // 连接操作
                    s.append({head.first, tail.second});        // 重新入栈
                    stateSet.insert("epsilon");
                }
            } else if (re[i] == '|') {
                allocateMemory(2);
                if (s.size() >= 2) {                // 取出两个元素相连重新入栈
                    QPair<int, int> left = s.top();
                    s.pop();
                    QPair<int, int> right = s.top();
                    s.pop();
                    G[stateNum][left.first] = "epsilon";
                    G[stateNum][right.first] = "epsilon";
                    G[left.second][stateNum + 1] = "epsilon";
                    G[right.second][stateNum + 1] = "epsilon";
                    s.append({stateNum, stateNum + 1}); // 入栈
                    stateNum += 2;                      // 状态数量 + 2
                    stateSet.insert("epsilon");
                }
            } else {
                allocateMemory(2);
                if (!s.empty()) {                   // 闭包为单元运算符，因此只需要取出一个元素
                    QPair<int, int> item = s.top();
                    s.pop();
                    G[stateNum][stateNum + 1] = "epsilon";
                    G[stateNum][item.first] = "epsilon";
                    G[item.second][stateNum + 1] = "epsilon";
                    G[item.second][item.first] = "epsilon";
                    s.append({stateNum, stateNum + 1}); // 入栈
                    stateNum += 2;                      // 状态数量 + 2
                    stateSet.insert("epsilon");
                }
            }
        } else {
            if (re[i] == '#') {                     // epsilon
                allocateMemory(2);
                G[stateNum][stateNum + 1] = "epsilon";
                s.append({stateNum, stateNum + 1}); // 入栈
                stateNum += 2;                      // 状态数量 + 2
                stateSet.insert("epsilon");
            } else {                                // 其他非运算符
                allocateMemory(2);
                G[stateNum][stateNum + 1] = re[i];
                s.append({stateNum, stateNum + 1}); // 入栈
                stateNum += 2;                      // 状态数量 + 2
                stateSet.insert(QString(re[i]));
            }
        }
    }
    if (!s.empty()){
        QPair<int, int> statePair = s.top();
        startState = statePair.first;
        endState = statePair.second;
    }
}

void NFA::allocateMemory(int num)
{
    // 当状态数即将超过最大状态时，分配更大的空间
    while (stateNum + num >= maxStateNum) {
        QVector<QVector<QString>> newG(2 * maxStateNum);
        for (int i = 0; i < 2 * maxStateNum; i++) {
            newG[i].resize(2 * maxStateNum);
        }
        for (int i = 0; i < maxStateNum; i++) {
            for (int j = 0; j < maxStateNum; j++) {
                newG[i][j] = G[i][j];
            }
        }
        maxStateNum *= 2;
        G = newG;
    }
}

QSet<int> NFA::epsilonClosure(QSet<int> state)
{
    while (true) {
        QSet<int> tmpSet;
        for (int item: state) {
            for (int i = 0; i < maxStateNum; i++) {
                if (G[item][i] == "epsilon") {
                    if (!state.contains(i)) {
                        tmpSet.insert(i);
                    }
                }
            }
        }
        if (tmpSet.empty()) break;
        state = state.unite(tmpSet);
    }
    return state;
}

QSet<int> NFA::valueClosure(QSet<int> state, QString value)
{
    QSet<int> result;
    for (int item: state) {
        for (int i = 0; i < maxStateNum; i++) {
            if (G[item][i] == value) {
                result.insert(i);
            }
        }
    }
    return result;
}
