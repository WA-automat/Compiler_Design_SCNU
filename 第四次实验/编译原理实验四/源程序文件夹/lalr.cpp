#include "lalr.h"

#include <QDebug>

LALR::LALR(): size(0)
{

}

void LALR::buildLR1(State faState, QHash<QString, QSet<QStringList>> grammars, QVector<QString> nonFinalizers,
                    QHash<QString, QSet<QString> > firstSet, QHash<QString, QSet<QString> > followSet)
{
    int faStateId = stateHash[faState];

    // 找到可能的转移方法
    QStringList changeMethods;
    for (auto faItem: faState.st) {
        if (faItem.pos < faItem.rule.size()) {
            changeMethods.append(faItem.rule[faItem.pos]);
        }
    }

    // 深搜每个转移
    for (QString changeMethod: changeMethods) {
        State sonState = State::change(faState, changeMethod, grammars, nonFinalizers, firstSet);
        if (sonState.st.empty()) continue;
        if (stateHash.contains(sonState)) {
            // 该状态已经存在
            int sonStateId = stateHash[sonState];
            changeHash[faStateId].insert(changeMethod, sonStateId);
        } else {
            // 该状态不存在
            stateHash[sonState] = size++;
            changeHash[faStateId].insert(changeMethod, stateHash[sonState]);
            buildLR1(sonState, grammars, nonFinalizers, firstSet, followSet);
        }

        qDebug() << faStateId << ' ' << changeMethod << ' ' << stateHash[sonState];
    }

}

void LALR::buildLALR1(LALR lr1)
{
    // 并查集(合并同心项)
    int* p = new int[lr1.size];
    for (int i = 0; i < lr1.size; i++) {
        p[i] = i;
    }
    std::function<int(int)> find = [&](int x) {
        if (p[x] != x) p[x] = find(p[x]);
        return p[x];
    };
    QHash<int, State> revlr1StateHash;
    for (auto state: lr1.stateHash.keys()) {
        revlr1StateHash[lr1.stateHash[state]] = state;
    }

    // 状态合并
    for (int i = 0; i < lr1.size - 1; i++) {
        for (int j = i + 1; j < lr1.size; j++) {
            if (State::haveSameCore(revlr1StateHash[i], revlr1StateHash[j])) {
                p[find(j)] = find(i);
            }
        }
    }

    // LR1到LALR1状态映射
    QHash<int, int> cntChangeSet;
    int idx = 0;
    for (int i = 0; i < lr1.size; i++) {
        if (!cntChangeSet.contains(find(i))) {
            cntChangeSet[find(i)] = idx++;
        }
    }
    size = cntChangeSet.size();

    // 建立LALR状态集
    QHash<int, State> revlalr1StateHash;
    for (int i = 0; i < lr1.size; i++) {
        if (!revlalr1StateHash.contains(cntChangeSet[find(i)])) {
            revlalr1StateHash[cntChangeSet[find(i)]] = revlr1StateHash[i];
        } else {
            for (auto item: revlr1StateHash[i].st) {
                for (auto lalrItem: revlalr1StateHash[cntChangeSet[find(i)]].st) {
                    auto tmpItem = lalrItem;
                    if (Item::haveSameCore(item, lalrItem)) {
                        lalrItem.next.unite(item.next);
                    }
                    revlalr1StateHash[cntChangeSet[find(i)]].st.remove(tmpItem);
                    revlalr1StateHash[cntChangeSet[find(i)]].st.insert(lalrItem);
                }
            }
        }
    }
//    for (auto state: revlalr1StateHash) {
//        for (auto item: state.st) {
//            qDebug() << item.name << ' ' << item.rule << ' ' << item.next << ' ' << item.pos;
//        }
//        qDebug();
//    }
    for (auto idx: revlalr1StateHash.keys()) {
        stateHash[revlalr1StateHash[idx]] = idx;
    }

    // 建立LALR转移集
    for (int i = 0; i < lr1.size; i++) {
        for (auto changeMethod: lr1.changeHash[i].keys()) {
            if (lr1.changeHash[i].contains(changeMethod)) {
                changeHash[cntChangeSet[find(i)]].insert(changeMethod, cntChangeSet[find(lr1.changeHash[i][changeMethod])]);
            }
        }
    }
}

State State::closure(State I, QHash<QString, QSet<QStringList> > grammars, QVector<QString> nonFinalizers, QHash<QString, QSet<QString>> firstSet)
{
    State J = I;
    while (true) {
        for (Item item: J.st) {
            qDebug() << item.name << ' ' << item.rule << ' ' << item.next << ' ' << item.pos;
            // 下一个字符是非终结符
            if (item.pos < item.rule.size() && nonFinalizers.contains(item.rule[item.pos])) {
                for (auto grammar: grammars[item.rule[item.pos]]) {
                    qDebug() << grammar;

                    for (auto nextItem: item.next) {
                        qDebug() << nextItem;

                        QStringList betaRule = item.rule.mid(item.pos + 1);
                        qDebug() << betaRule;

                        if (nextItem != "@") betaRule.push_back(nextItem);

                        // 计算first集合
                        int k = 0;
                        QSet<QString> tmpSet;
                        while (k < betaRule.size()) {
                            bool sign = false;
                            if (firstSet[betaRule[k]].contains("@")) sign = true;
                            tmpSet.unite(firstSet[betaRule[k]].subtract(QSet<QString>({"@"})));
                            if (sign) firstSet[betaRule[k]].insert("@");
                            else break;
                            k++;
                        }
                        if (k == betaRule.size()) tmpSet.insert("@");

                        qDebug() << k << ' ' << tmpSet;

                        // 遍历first集合中的每个元素
                        for (auto b: tmpSet) {

                            Item newItem;
                            newItem.name = item.rule[item.pos];
                            newItem.rule = grammar;
                            newItem.next.insert(b);
                            newItem.pos = 0;

                            if (!J.st.contains(newItem)) {
                                bool mark = true;
                                for (auto it: J.st) {
                                    if (Item::haveSameCore(it, newItem)) {
                                        J.st.remove(it);
                                        it.next.unite(newItem.next);
                                        J.st.insert(it);
                                        mark = false;
                                        break;
                                    }
                                }
                                if (mark) {
                                    J.st.insert(newItem);
                                }
                            }
                        }

                    }

                }
            }
        }
        if (I == J) break;
        I = J;
    }
    qDebug();
    for (auto item: J.st) {
        qDebug() << item.name << ' ' << item.rule << ' ' << item.next << ' ' << item.pos;
    }
    return J;
}

State State::change(State I, QString X, QHash<QString, QSet<QStringList> > grammars, QVector<QString> nonFinalizers, QHash<QString, QSet<QString>> firstSet)
{
    State I_new;
    for (auto item: I.st) {
        if (item.pos >= item.rule.size() || item.rule[item.pos] != X) continue;

        Item newItem;
        newItem.name = item.name;
        newItem.rule = item.rule;
        newItem.next = item.next;
        newItem.pos = item.pos + 1;

        // 判断I_new中是否已经出现了newItem
        if (!I_new.st.contains(newItem)) {
            bool mark = true;
            for (auto it: I_new.st) {
                if (Item::haveSameCore(it, newItem)) {
                    I_new.st.remove(it);
                    it.next.unite(newItem.next);
                    I_new.st.insert(it);
                    mark = false;
                    break;
                }
            }
            if (mark) {
                I_new.st.insert(newItem);
            }
        }
    }
    if (I_new.st.empty()) return I_new;
    else return closure(I_new, grammars, nonFinalizers, firstSet);
}

bool State::haveSameCore(State i, State j)
{
    if (i.st.size() != j.st.size()) return false;
    auto iList = i.st.toList();
    auto jList = j.st.toList();
    qSort(iList.begin(), iList.end());
    qSort(jList.begin(), jList.end());
    for (int cnt = 0; cnt < i.st.size(); cnt++) {
        if (!Item::haveSameCore(iList[cnt], jList[cnt])) {
            return false;
        }
    }
    return true;
}

bool Item::haveSameCore(Item i, Item j)
{
    if (i.name == j.name && i.rule == j.rule && i.pos == j.pos) return true;
    else return false;
}

uint qHash(const Item& key)
{
    return qHash(key.name) + qHash(key.rule) + qHash(key.next) + qHash(key.pos);
}

uint qHash(const State &key)
{
    return qHash(key.st);
}
