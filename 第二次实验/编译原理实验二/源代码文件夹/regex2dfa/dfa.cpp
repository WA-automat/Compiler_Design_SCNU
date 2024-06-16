#include "dfa.h"

#include <QQueue>
#include <QDebug>

DFA::DFA()
{

}

void DFA::fromNFA(NFA nfa)
{
    QHash<QSet<int>, int> revMapping;
    // 获取始态
    QSet<int> startSet;
    startSet.insert(nfa.startState);
    startSet = nfa.epsilonClosure(startSet);
    mapping[startState] = startSet;
    revMapping[startSet] = startState;
    if (startSet.contains(nfa.endState)) {
        endStates.insert(startState);
    }
    stateNum++;

    QSet<QSet<int>> vis;        // 判断某个状态是否被找到
    QQueue<int> q;              // 循环队列
    q.push_back(startState);
    while (!q.empty()) {

        // 取出队头
        int stateItem = q.front();
        q.pop_front();
        QSet<int> nfaStateSet = mapping[stateItem];

        // 未曾出现过的状态集合才需要查找
        if (!vis.contains(nfaStateSet)) {

            // 查询当前状态集合的转移
            for (QString changeItem: nfa.stateSet) {
                if (changeItem != "epsilon") {

                    // 获取对应转移到的epsilon闭包
                    QSet<int> changeClosure = nfa.valueClosure(nfaStateSet, changeItem);
                    QSet<int> changeEpsilon = nfa.epsilonClosure(changeClosure);

                    // 找不到集合
                    if (changeEpsilon.empty()) continue;

                    // 之前找到过这个状态
                    if (revMapping.contains(changeEpsilon)) {
                        int nextItem = revMapping[changeEpsilon];

                        G[stateItem][changeItem] = nextItem;
                        if (changeEpsilon.contains(nfa.endState)) {
                            endStates.insert(nextItem);
                        }

                        if (!vis.contains(changeEpsilon)) {
                            q.push_back(nextItem);
                        }
                    } else {
                        int nextItem = stateNum;
                        mapping[nextItem] = changeEpsilon;
                        revMapping[changeEpsilon] = nextItem;

                        G[stateItem][changeItem] = nextItem;
                        if (changeEpsilon.contains(nfa.endState)) {
                            endStates.insert(nextItem);
                        }
                        stateNum++;

                        if (!vis.contains(changeEpsilon)) {
                            q.push_back(nextItem);
                        }
                    }
                }
            }
            vis.insert(nfaStateSet);
        }
    }
}

void DFA::fromDFA(DFA dfa)
{
    QSet<int> notEndStates;
    for (int i = 0; i < dfa.stateNum; i++) {
        if (!dfa.endStates.contains(i)) {
            notEndStates.insert(i);
        }
    }

    // 找到原始DFA的转移类型
    QSet<QString> stateSet;
    for (int i = 0; i < dfa.stateNum; i++) {
        for (QString changeItem: dfa.G[i].keys()) {
            stateSet.insert(changeItem);
        }
    }

    QQueue<QSet<int>> q;
    if (!notEndStates.empty()) q.push_back(notEndStates);
    if (!dfa.endStates.empty()) q.push_back(dfa.endStates);

    int lastQueLen = 0, cnt = 0;
    if (!notEndStates.empty()) lastQueLen++;
    if (!dfa.endStates.empty()) lastQueLen++;

    while (true) {

        QSet<int> stateItem = q.front();

        // 划分集合
        bool flag = true;
        for (QString changeItem: stateSet) {
            QSet<QSet<int>> vis;
            QHash<QSet<int>, QSet<int>> setHash;        // 当前选出
            for (int beginItem: stateItem) {
                if (!dfa.G.contains(beginItem)) continue;
                if (!dfa.G[beginItem].contains(changeItem)) {
                    vis.insert(QSet<int>());
                    setHash[QSet<int>()].insert(beginItem);
                    continue;
                }
                for (QSet<int> queueItem: q) {
                    if (queueItem.contains(dfa.G[beginItem][changeItem])){
                        vis.insert(queueItem);
                        setHash[queueItem].insert(beginItem);
                        break;
                    }
                }
            }
            if (vis.size() <= 1) {
                continue;
            } else {
                for (QSet<int> visItem: vis) {
                    q.push_back(setHash[visItem]);
                }
                flag = false;
                break;
            }
        }

        q.pop_front();
        if (flag) q.push_back(stateItem);

        // 判断“上一个”队列是否全被扫过一次
        cnt++;
        if (lastQueLen == cnt) {
            if (lastQueLen == q.size()) {
                break;
            }
            lastQueLen = q.size();
            cnt = 0;
        }
    }

    int idx = 0;
    // 设置新编号
    QHash<QSet<int>, int> revMapping;
    for (QSet<int> queueItem: q) {
        if (queueItem.empty()) continue;
        mapping[idx] = queueItem;
        revMapping[queueItem] = idx;
        idx++;
    }
    stateNum = idx;

    // 找到始态和终态
    for (QSet<int> queueItem: q) {
        if (queueItem.empty()) continue;
        // 找始态
        if (queueItem.contains(dfa.startState)) {
            startState = revMapping[queueItem];
        }
        // 找终态
        for (int endState: dfa.endStates) {
            if (queueItem.contains(endState)) {
                endStates.insert(revMapping[queueItem]);
                break;
            }
        }
    }

    // 构建新的G
    for (QSet<int> queueItem: q) {
        if (queueItem.empty()) continue;
        int stateItem = *queueItem.begin();
        for (QString changeItem: stateSet) {
            if (!dfa.G[stateItem].contains(changeItem)) continue;
            int endItem = dfa.G[stateItem][changeItem];
            for (QSet<int> endQueueItem: q) {
                if (endQueueItem.contains(endItem)) {
                    G[revMapping[queueItem]][changeItem] = revMapping[endQueueItem];
                    break;
                }
            }
        }
    }
}

void DFA::clear()
{
    mapping.clear();
    G.clear();
    endStates.clear();
    stateNum = 0;
    startState = 0;
}

QString DFA::toCode()
{
    QString code = "bool matchFunction(string s) {\n";
    code += "\tint idx = 0, state = ";
    code += QString::number(startState) + ";\n";
    code += "\twhile (state < " + QString::number(stateNum) + " && idx < s.size()) {\n";
    code += "\t\tswitch (state) {\n";
    for (int i = 0; i < stateNum; i++) {
        if (G[i].keys().empty()) continue;
        code += "\t\tcase " + QString::number(i) + ":\n";
        code += "\t\t\tswitch (s[idx]) {\n";
        for (QString changeItem: G[i].keys()) {
            code += "\t\t\tcase \'" + changeItem + "\':\n";
            code += "\t\t\t\tstate = " + QString::number(G[i][changeItem]) + ";\n";
            code += "\t\t\t\tbreak;\n";
        }
        code += "\t\t\tdefault:\n";
        code += "\t\t\t\tstate = " + QString::number(stateNum) + ";\n";
        code += "\t\t\t\tbreak;\n";
        code += "\t\t\t}\n";
        code += "\t\t\tbreak;\n";
    }
    code += "\t\tdefault:\n";
    code += "\t\t\tstate = " + QString::number(stateNum) + ";\n";
    code += "\t\t\tbreak;\n";
    code += "\t\t}\n";
    code += "\t\tidx++\n";
    code += "\t}\n";
    code += "\tif (";
    int cnt = 0;
    for (int i: endStates) {
        code += "state == " + QString::number(i);
        cnt++;
        if (cnt != endStates.size()) code += "||";
    }
    code += ") return true;\n";
    code += "\telse return false;\n";
    code += "}";
    return code;
}
