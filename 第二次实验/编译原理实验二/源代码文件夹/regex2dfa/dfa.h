#ifndef DFA_H
#define DFA_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QVector>

#include "nfa.h"

class DFA
{
public:
    DFA();
    void fromNFA(NFA nfa);
    void fromDFA(DFA dfa);
    void clear();
    QString toCode();

    QHash<int, QSet<int>> mapping;
    QHash<int, QHash<QString, int>> G;
    int startState;
    QSet<int> endStates;
    int stateNum;                       // 表示状态数量

};

#endif // DFA_H
