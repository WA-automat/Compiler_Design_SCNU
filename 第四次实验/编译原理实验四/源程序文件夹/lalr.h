#ifndef LALR_H
#define LALR_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QHash>

// 单条规则
typedef struct Item {
    QString name;
    QStringList rule;
    QSet<QString> next;
    int pos;

    static bool haveSameCore(Item i, Item j);

    friend bool operator==(const Item a, const Item b) {
        return a.name == b.name && a.rule == b.rule && a.next == b.next && a.pos == b.pos;
    }

    friend bool operator<(const Item a, const Item b) {
        if (a.name == b.name) {
            if (a.rule == b.rule) {
                if (a.pos == b.pos) {
                    return a.pos < b.pos;
                }
                return a.pos < b.pos;
            }
            return a.rule < b.rule;
        }
        return a.name < b.name;
    }
}Item;

uint qHash(const Item& key);

// 单个状态
typedef struct State {
    QSet<Item> st;
    static State closure(State I, QHash<QString, QSet<QStringList>> grammars, QVector<QString> nonFinalizers, QHash<QString, QSet<QString>> firstSet);
    static State change(State I, QString X, QHash<QString, QSet<QStringList>> grammars, QVector<QString> nonFinalizers, QHash<QString, QSet<QString>> firstSet);

    static bool haveSameCore(State i, State j);

    friend bool operator==(const State a, const State b) {
        return a.st == b.st;
    }

}State;

uint qHash(const State& key);

// 分析表结点
typedef struct LALR1TableItem {
    int kind;   // 1 表示移进、2 表示规约、3 表示非终结符移进、4 表示接受
    int idx;
}LALR1TableItem;

typedef struct AnalysisItem {
    int kind;
    int state;
    QString ch;
}AnalysisItem;

// LALR结构
class LALR
{
public:
    LALR();

    // 成员变量
    int size;
    QHash<State, int> stateHash;
    QHash<int, QHash<QString, int>> changeHash;

    // 构建LR1
    void buildLR1(State faState, QHash<QString, QSet<QStringList>> grammars, QVector<QString> nonFinalizers,
                  QHash<QString, QSet<QString>> firstSet, QHash<QString, QSet<QString>> followSet);

    // 构建LALR1
    void buildLALR1(LALR lr1);

};

#endif // LALR_H
