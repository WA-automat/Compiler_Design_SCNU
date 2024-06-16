#ifndef NFA_H
#define NFA_H

#include <QString>
#include <QVector>
#include <QSet>

class NFA
{
public:
    // 构造函数
    NFA(int begin = -1, int end = -1);
    void clear();                       // 清空NFA
    void fromRegex(QString re);         // 从后缀表达式构造NFA
    void allocateMemory(int num);       // 分配G内存的函数
    QSet<int> epsilonClosure(QSet<int> state);  // 计算epsilon闭包
    QSet<int> valueClosure(QSet<int> state, QString value); // 计算某个集合状态能通过value转移到的状态集合

    // 成员变量
    QVector<QVector<QString>> G;        // 邻接矩阵
    QSet<QString> stateSet;             // 存储所有状态类型
    int startState;                     // 表示起始状态
    int endState;                       // 表示终止状态
    int stateNum;                       // 表示状态数量
    int maxStateNum;                    // 表示当前最多存储的状态数量
};

#endif // NFA_H
