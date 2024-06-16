#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <QRect>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QColor>
#include <QStack>

#include "lalr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 基本布局信息
    this->setWindowTitle("20212821020 林泽勋 编译原理实验四：LALR(1)分析生成器");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int newWidth = screenWidth * 0.9;
    int newHeight = screenHeight * 0.9;
    this->resize(newWidth, newHeight);
    this->showMaximized();

    // 允许分析句子
    bool canSentence = false;

    // 打开文法文件
    connect(ui->openButton, &QPushButton::clicked, this, [&]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择文法规则文件");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                in.setCodec("UTF-8");
                ui->grammarEdit->setText(in.readAll());
                file.close();
            }
        }
    });

    // 保存文法文件
    connect(ui->saveButton, &QPushButton::clicked, this, [=]() {
        QString fileName = QString("grammar") + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + QString(".txt");
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->grammarEdit->toPlainText();
            file.close();
            QMessageBox::information(this, "提示", "文件保存为：" + fileName + "成功！", QMessageBox::Yes);
        } else {
            QMessageBox::warning(this, "提示", "文件保存失败！", QMessageBox::Yes);
        }
    });

    // 文法分析
    connect(ui->analysisButton, &QPushButton::clicked, this, [&]() {

        // 清空内容
        nonFinalizers.clear();
        grammars.clear();
        firstSet.clear();
        followSet.clear();
        recursion.clear();
        LALR1_table.clear();
        ui->firstTableWidget->clearContents();
        ui->followTableWidget->clearContents();
        ui->lrTableWidget->clearContents();
        ui->lalrTableWidget->clearContents();
        ui->lalrAnalysisTableWidget->clearContents();
        ui->resultTableWidget->clearContents();
        ui->resultTableWidget->setRowCount(0);

        // 获取文法规则
        QString content = ui->grammarEdit->toPlainText();
        QStringList grammarList = content.split("\n");

        // 存储文法规则
        // 获取非终结符
        for (const auto &grammar: qAsConst(grammarList)) {
            QStringList wordList = grammar.split(" ");
            for (int i = 0; i < wordList.size(); i++) {
                if (wordList[i] == "") {
                    wordList.removeAt(i);
                }
            }
            if (wordList.size() < 3 || wordList[1] != "->") {
                QMessageBox::warning(this, "提示", "文法分析失败，左侧非终结符只能出现单个词，非终结符后需要有空格以及 -> 符号", QMessageBox::Yes);
                return;
            } else {
                nonFinalizers.push_back(wordList[0]);
            }
        }
        qDebug() << nonFinalizers;

        // 获取起始符
        QStringList firstGrammar = grammarList[0].split(" ");
        if (!firstGrammar.empty()) startString = firstGrammar[0];
        qDebug() << startString;

        // 分词
        for (const auto &grammar: qAsConst(grammarList)) {
            QStringList wordList = grammar.split(" ");
            for (int i = 0; i < wordList.size(); i++) {
                if (wordList[i] == "") {
                    wordList.removeAt(i);
                } else {
                    if (!nonFinalizers.contains(wordList[i]) && wordList[i] != "->") {
                        firstSet[wordList[i]].insert(wordList[i]);
                    }
                }
            }
            // 删除非终结符与->
            QString nonfinal = wordList[0];
            wordList.removeAt(0);
            wordList.removeAt(0);
            int lastIdx = 0;
            for (int i = 1; i < wordList.size() - 1; i++) {
                if (wordList[i] == "|") {
                    grammars[nonfinal].insert(wordList.mid(lastIdx, i - lastIdx));
                    lastIdx = i + 1;
                }
            }
            grammars[nonfinal].insert(wordList.mid(lastIdx, wordList.size() - lastIdx));
        }
        qDebug() << grammars;

        // 计算first与follow集合
        // 消除直接左递归
        int len = nonFinalizers.size();
        for (int i = 0; i < len; i++) {
            QString nonFinal = nonFinalizers[i];

            QSet<QStringList> leftGrammar, rightGrammar;
            for (QStringList grammar: grammars[nonFinal]) {
                if (grammar[0] == nonFinal) {
                    // 左递归文法
                    leftGrammar.insert(grammar.mid(1));
                } else {
                    // 非左递归文法
                    rightGrammar.insert(grammar);
                }
            }

            if (rightGrammar.size() == 0) {
                // 死递归
                QMessageBox::warning(this, "提示", "文法分析失败，包含死递归文法", QMessageBox::Yes);
                return;
            } else if (leftGrammar.size() != 0) {
                // 包含左递归，进行消除
                grammars[nonFinal].clear();
                for (auto rightSentence: rightGrammar) {
                    rightSentence.append(nonFinal + "_nonleft");
                    grammars[nonFinal].insert(rightSentence);
                }
                nonFinalizers.append(nonFinal + "_nonleft");
                for (auto leftSentence: leftGrammar) {
                    leftSentence.append(nonFinal + "_nonleft");
                    grammars[nonFinal + "_nonleft"].insert(leftSentence);
                }
                grammars[nonFinal + "_nonleft"].insert(QStringList() << "@");
            }
        }
        qDebug() << grammars;

        // 计算first集合
        std::function<QSet<QString>(QString)> getFirst = [&](QString s) {
            if (firstSet.contains(s)) {
                return firstSet[s];
            } else if (!nonFinalizers.contains(s)) {
                return firstSet[s] = QSet<QString>({s});
            } else {
                for (auto grammar: grammars[s]) {
                    int k = 0;
                    while (k < grammar.size()) {
                        firstSet[grammar[k]] = getFirst(grammar[k]);
                        bool mark = false;
                        if (firstSet[grammar[k]].contains("@")) mark = true;
                        firstSet[s].unite(firstSet[grammar[k]].subtract(QSet<QString>({"@"})));
                        if (mark) firstSet[grammar[k]].insert("@");
                        else break;
                        k++;
                    }
                    if (k == grammar.size()) firstSet[s].insert("@");
                }
                return firstSet[s];
            }
        };
        for (int i = 0; i < nonFinalizers.size(); i++) {
            firstSet[nonFinalizers[i]] = getFirst(nonFinalizers[i]);
        }
        qDebug() << firstSet;

        // 计算follow集合
        bool flag = true;   // 判断循环结束的标识
        followSet[startString].insert("$");
        while (flag) {
            flag = false;
            for (auto key: grammars.keys()) {
                for (auto grammar: grammars[key]) {
                    // 对每条文法进行分析
                    for (int i = 0; i < grammar.size(); i++) {
                        if (!nonFinalizers.contains(grammar[i])) continue;
                        int k = i + 1;
                        QSet<QString> tmpSet;
                        while (k < grammar.size()) {
                            bool sign = false;
                            if (firstSet[grammar[k]].contains("@")) sign = true;
                            tmpSet.unite(firstSet[grammar[k]].subtract(QSet<QString>({"@"})));
                            if (sign) firstSet[grammar[k]].insert("@");
                            else break;
                            k++;
                        }
                        if (k == grammar.size()) tmpSet.insert("@");
                        bool mark = false;
                        if (tmpSet.contains("@")) mark = true;
                        if (!followSet[grammar[i]].contains(tmpSet.subtract(QSet<QString>({"@"})))) {
                            // 将first(X_{i+1}…X_{n})加入follow(X_{i})
                            for (auto tmp: tmpSet) {
                                followSet[grammar[i]].insert(tmp);
                            }
                            flag = true;
                        }
                        if (mark && !followSet[grammar[i]].contains(followSet[key])) {
                            for (auto tmp: followSet[key]) {
                                followSet[grammar[i]].insert(tmp);
                            }
                            flag = true;
                        }
                    }
                }
            }
        }
        qDebug() << followSet;

        // 可视化first 与 follow集合
        ui->firstTableWidget->setColumnCount(2);
        ui->followTableWidget->setColumnCount(2);
        ui->firstTableWidget->setHorizontalHeaderLabels(QStringList() << "非终结符" << "first集合");
        ui->followTableWidget->setHorizontalHeaderLabels(QStringList() << "非终结符" << "follow集合");
        ui->firstTableWidget->horizontalHeader()->setStretchLastSection(true);
        ui->followTableWidget->horizontalHeader()->setStretchLastSection(true);
        ui->firstTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->followTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        int firstNonSize = 0;
        for (auto firstKey: firstSet.keys()) {
            if (nonFinalizers.contains(firstKey)) firstNonSize++;
        }
        ui->firstTableWidget->setRowCount(firstNonSize);
        int firstCount = 0;
        for (auto key : firstSet.keys()) {
            if (nonFinalizers.contains(key)) {
                ui->firstTableWidget->setItem(firstCount, 0, new QTableWidgetItem(key));
                ui->firstTableWidget->setItem(firstCount, 1, new QTableWidgetItem(firstSet[key].toList().join(",")));
                firstCount++;
            }
        }
        ui->followTableWidget->setRowCount(followSet.size());
        int followCount = 0;
        for (auto key : followSet.keys()) {
            if (nonFinalizers.contains(key)) {
                ui->followTableWidget->setItem(followCount, 0, new QTableWidgetItem(key));
                ui->followTableWidget->setItem(followCount, 1, new QTableWidgetItem(followSet[key].toList().join(",")));
                followCount++;
            }
        }

        // 计算LR(1)DFA图
        // 判断是否需要扩充文法
        firstSet["$"] = QSet<QString>({"$"});
        if (grammars[startString].size() > 1) {
            grammars[startString + "_new"].insert(QStringList() << startString);
            firstSet[startString + "_new"].insert(startString);
            followSet[startString + "_new"].insert("$");
            startString = startString + "_new";
        }
        Item firstItem;
        firstItem.name = startString;
        firstItem.rule = *grammars[startString].begin();
        firstItem.next = QSet<QString>({"$"});
        firstItem.pos = 0;
        State firstState;
        firstState.st.insert(firstItem);
        firstState = State::closure(firstState, grammars, nonFinalizers, firstSet);
//        for (auto item: firstState.st) {
//            qDebug() << item.name << ' ' << item.rule << ' ' << item.next << ' ' << item.pos;
//        }
        LALR lr1;
        lr1.stateHash[firstState] = lr1.size++;

        lr1.buildLR1(firstState, grammars, nonFinalizers, firstSet, followSet);

        // 测试转移
//        qDebug();
//        State nextState = State::change(firstState, "A", grammars, nonFinalizers, firstSet);
//        for (auto item: nextState.st) {
//            qDebug() << item.name << ' ' << item.rule << ' ' << item.next << ' ' << item.pos;
//        }

        // LR1可视化
        ui->lrTableWidget->setRowCount(lr1.size);
        QSet<QString> lrChangeState;
        for (auto state: lr1.stateHash.keys()) {
            for (auto changeMethod: lr1.changeHash[lr1.stateHash[state]].keys()) {
                lrChangeState.insert(changeMethod);
            }
        }
        ui->lrTableWidget->setColumnCount(lrChangeState.size());
        auto lrChangeList = lrChangeState.toList();

        ui->lrTableWidget->setHorizontalHeaderLabels(lrChangeList);
        int lrIdx = 0;
        for (auto state: lr1.stateHash.keys()) {
            QString itemString = QString::number(lrIdx) + ":\n";
            for (auto item: state.st) {
                itemString += item.name;
                itemString += " -> ";
                for (int i = 0; i < item.rule.size(); i++) {
                    if (i == item.pos) itemString += ". ";
                    itemString += item.rule[i] + " ";
                }
                if (item.pos == item.rule.size()) itemString += ". ";
                itemString += ", ";
                int i = 0;
                for (auto nextCh: item.next) {
                    itemString += nextCh;
                    if (i != item.next.size() - 1) itemString += "、";
                    i++;
                }
                itemString += "\n";
            }
            QTableWidgetItem *item = new QTableWidgetItem(itemString);
            if (state == firstState) item->setTextColor(QColor(255, 0, 0));
            ui->lrTableWidget->setVerticalHeaderItem(lrIdx, item);
            for (int i = 0; i < lrChangeList.size(); i++) {
                if (lr1.changeHash[lr1.stateHash[state]].contains(lrChangeList[i])) {
                    ui->lrTableWidget->setItem(lrIdx, i, new QTableWidgetItem(QString::number(lr1.changeHash[lr1.stateHash[state]][lrChangeList[i]])));
                }
            }
            lrIdx++;
        }
        ui->lrTableWidget->resizeRowsToContents();
        ui->lrTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // 判断 LR1 文法
//        for (auto state: lr1.stateHash.keys()) {
//            // 找到所有的归约项，判断前进字符是否有重复
//            QSet<QString> callback;
//            for (auto item: state.st) {
//                // 判断规约项
//                if (item.pos == item.rule.size()) {
//                    for (auto next: item.next) {
//                        if (callback.contains(next)) {
//                            QMessageBox::warning(this, "警告", "该文法出现了规约-规约冲突，出现了向前看符号有相同的规约项，不是 LR(1) 文法", QMessageBox::Yes);
//                            return;
//                        }
//                    }
//                    callback.unite(item.next);
//                }
//            }
//        }

        // 计算LALR(1)DFA图
        LALR lalr1;
        lalr1.buildLALR1(lr1);

        ui->lalrTableWidget->setRowCount(lalr1.size);
        QSet<QString> lalrChangeState;
        for (auto state: lalr1.stateHash.keys()) {
            for (auto changeMethod: lalr1.changeHash[lalr1.stateHash[state]].keys()) {
                lalrChangeState.insert(changeMethod);
            }
        }
        ui->lalrTableWidget->setColumnCount(lalrChangeState.size());
        auto lalrChangeList = lalrChangeState.toList();

        ui->lalrTableWidget->setHorizontalHeaderLabels(lalrChangeList);
        int lalrIdx = 0;
        for (auto state: lalr1.stateHash.keys()) {
            QString itemString = QString::number(lalrIdx) + ":\n";
            for (auto item: state.st) {
                itemString += item.name;
                itemString += " -> ";
                for (int i = 0; i < item.rule.size(); i++) {
                    if (i == item.pos) itemString += ". ";
                    itemString += item.rule[i] + " ";
                }
                if (item.pos == item.rule.size()) itemString += ". ";
                itemString += ", ";
                int i = 0;
                for (auto nextCh: item.next) {
                    itemString += nextCh;
                    if (i != item.next.size() - 1) itemString += "、";
                    i++;
                }
                itemString += "\n";
            }
            QTableWidgetItem *item = new QTableWidgetItem(itemString);
            if (State::haveSameCore(state, firstState)) item->setTextColor(QColor(255, 0, 0));
            ui->lalrTableWidget->setVerticalHeaderItem(lalrIdx, item);
            for (int i = 0; i < lalrChangeList.size(); i++) {
                if (lalr1.changeHash[lalr1.stateHash[state]].contains(lalrChangeList[i])) {
                    ui->lalrTableWidget->setItem(lalrIdx, i, new QTableWidgetItem(QString::number(lalr1.changeHash[lalr1.stateHash[state]][lalrChangeList[i]])));
                }
            }
            lalrIdx++;
        }
        ui->lalrTableWidget->resizeRowsToContents();
        ui->lalrTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // 判断 LALR1 文法：移进-规约冲突 与 规约-规约冲突
        for (auto state: lalr1.stateHash.keys()) {
            // 找到所有的归约项，判断前进字符是否有重复
            QSet<QString> callback;
            for (auto item: state.st) {
                // 判断规约项
                if (item.pos == item.rule.size()) {
                    for (auto next: item.next) {
                        if (callback.contains(next)) {
                            QMessageBox::warning(this, "警告", "该文法在 LALR(1) 中出现了规约-规约冲突，出现了向前看符号有相同的规约项，不是 LALR(1) 文法", QMessageBox::Yes);
                            return;
                        }
                    }
                    callback.unite(item.next);
                }
            }
            // 找到所有移进项
            QSet<QString> footIn;
            for (auto item: state.st) {
                // 判断移进项
                if (item.pos != item.rule.size()) {
                    footIn.insert(item.rule[item.pos]);
                }
            }
            // 判断是否有移进-规约冲突
            if (!footIn.intersect(callback).empty()) {
                QMessageBox::warning(this, "警告", "该文法在 LALR(1) 中出现了移进-规约冲突，出现了规约项的向前看符号与移进项的下一个字符相同的情况，不是 LALR(1) 文法，但后续会使用移进项进行分析，解决二义性", QMessageBox::Yes);
            }
        }

        // 计算LALR(1)分析表
        QStringList header;
        QSet<QString> finalString;
        for (int i = 0; i < lalr1.size; i++) {
            for (auto changeMethod: lalr1.changeHash[i].keys()) {
                if (!nonFinalizers.contains(changeMethod)) {
                    finalString.insert(changeMethod);
                }
            }
        }
        QList<QString> finalStringList = finalString.toList();
        QList<QString> nonFinalStringList = nonFinalizers.toList();
        qSort(finalStringList.begin(), finalStringList.end());
        finalStringList.append("$");
        qSort(nonFinalStringList.begin(), nonFinalStringList.end());
        header.append(finalStringList);
        header.append(nonFinalStringList);
        ui->lalrAnalysisTableWidget->setColumnCount(header.size());
        ui->lalrAnalysisTableWidget->setHorizontalHeaderLabels(header);
        ui->lalrAnalysisTableWidget->setRowCount(lalr1.size);

        // 可视化 LALR(1) 分析表
        QHash<int, State> revHash;
        for (auto state: lalr1.stateHash.keys()) {
            revHash[lalr1.stateHash[state]] = state;
        }
        QHash<QString, int> headerHash;
        int headerIdx = 0;
        for (QString head: header) {
            headerHash[head] = headerIdx++;
        }
        for (int i = 0; i < lalr1.size; i++) {

            // 渲染行表头
            auto headerItem = new QTableWidgetItem(QString::number(i));
            if (i == 0) headerItem->setTextColor(QColor(255, 0, 0));
            ui->lalrAnalysisTableWidget->setVerticalHeaderItem(i, headerItem);

            State state = revHash[i];
            for (Item item: state.st) {
                if (item.pos == item.rule.size()) {
                    // 规约项
                    if (item.name == startString) {
                        // 接受状态
                        for (auto next: item.next) {
                            if (next == "$") {
                                LALR1TableItem lalrItem;
                                lalrItem.kind = 4;
                                LALR1_table[i].insert(next, lalrItem);
                                ui->lalrAnalysisTableWidget->setItem(i, headerHash[next], new QTableWidgetItem("接受"));
                            } else if (recursion.contains(item)) {
                                LALR1TableItem lalrItem;
                                lalrItem.idx = recursion.indexOf(item);
                                lalrItem.kind = 2;
                                LALR1_table[i].insert(next, lalrItem);
                                ui->lalrAnalysisTableWidget->setItem(i, headerHash[next], new QTableWidgetItem("r" + QString::number(recursion.indexOf(item))));
                            } else {
                                recursion.push_back(item);
                                LALR1TableItem lalrItem;
                                lalrItem.idx = recursion.size() - 1;
                                lalrItem.kind = 2;
                                LALR1_table[i].insert(next, lalrItem);
                                ui->lalrAnalysisTableWidget->setItem(i, headerHash[next], new QTableWidgetItem("r" + QString::number(recursion.size() - 1)));
                            }
                        }
                    } else {
                        for (auto next: item.next) {
                            // 判断该规约规则是否已经出现过
                            if (recursion.contains(item)) {
                                LALR1TableItem lalrItem;
                                lalrItem.idx = recursion.indexOf(item);
                                lalrItem.kind = 2;
                                LALR1_table[i].insert(next, lalrItem);
                                ui->lalrAnalysisTableWidget->setItem(i, headerHash[next], new QTableWidgetItem("r" + QString::number(recursion.indexOf(item))));
                            } else {
                                recursion.push_back(item);
                                LALR1TableItem lalrItem;
                                lalrItem.idx = recursion.size() - 1;
                                lalrItem.kind = 2;
                                LALR1_table[i].insert(next, lalrItem);
                                ui->lalrAnalysisTableWidget->setItem(i, headerHash[next], new QTableWidgetItem("r" + QString::number(recursion.size() - 1)));
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < lalr1.size; i++) {
            // 移进项
            for (int j = 0; j < finalStringList.size(); j++) {
                auto changeMethod = finalStringList[j];
                if (lalr1.changeHash[i].contains(changeMethod)) {
                    LALR1TableItem lalrItem;
                    lalrItem.idx = lalr1.changeHash[i][changeMethod];
                    lalrItem.kind = 1;
                    LALR1_table[i].insert(changeMethod, lalrItem);
                    ui->lalrAnalysisTableWidget->setItem(i, j, new QTableWidgetItem("s" + QString::number(lalr1.changeHash[i][changeMethod])));
                }
            }
            // 规约后的回溯
            for (int j = 0; j < nonFinalStringList.size(); j++) {
                auto changeMethod = nonFinalStringList[j];
                if (lalr1.changeHash[i].contains(changeMethod)) {
                    LALR1TableItem lalrItem;
                    lalrItem.idx = lalr1.changeHash[i][changeMethod];
                    lalrItem.kind = 3;
                    LALR1_table[i].insert(changeMethod, lalrItem);
                    ui->lalrAnalysisTableWidget->setItem(i, j + finalStringList.size(), new QTableWidgetItem(QString::number(lalr1.changeHash[i][changeMethod])));
                }
            }
        }

        ui->lalrAnalysisTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        canSentence = true;
    });

    // 句子分析
    connect(ui->sentenceButton, &QPushButton::clicked, this, [&]() {

        if (!canSentence) {
            QMessageBox::warning(this, "警告", "还没有进行文法分析", QMessageBox::Yes);
            return;
        }

        ui->resultTableWidget->clearContents();
        ui->resultTableWidget->setColumnCount(2);
        ui->resultTableWidget->setHorizontalHeaderLabels(QStringList() << "分析栈" << "符号栈");
        ui->resultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // 获取待分析的句子
        QString sentence = ui->sentenceEdit->text();
        QString str = sentence;
        ui->resultTableWidget->setRowCount(sentence.size() * 3);
        if (sentence == "") {
            QMessageBox::warning(this, "警告", "待分析的句子不能为空", QMessageBox::Yes);
            return;
        }
        sentence += "$";

        // 符号栈栈顶
        int sentenceTop = 0;
        // 分析栈
        QStack<AnalysisItem> analysisStack;
        AnalysisItem firstItem;
        firstItem.kind = 1;
        firstItem.state = 0;
        analysisStack.push(firstItem);
        // 分析栈计数器
        int cnt = 0;

        // 打印函数
        std::function<void(void)> printAnalysis = [&]() {
            QString analysisString;
            for (auto item: analysisStack) {
                if (item.kind == 1) {
                    analysisString += QString::number(item.state) + " ";
                } else {
                    analysisString += item.ch + " ";
                }
            }
            ui->resultTableWidget->setRowCount(cnt + 1);
            ui->resultTableWidget->setItem(cnt, 0, new QTableWidgetItem(analysisString));
            ui->resultTableWidget->setItem(cnt, 1, new QTableWidgetItem(sentence.mid(sentenceTop)));
            cnt++;
            qDebug() << analysisString << " " << sentence.mid(sentenceTop);
        };

        // 利用分析表分析句子
        printAnalysis();
        while (true) {
//            if (sentenceTop == sentence.size()) sentence += "@";
            if (analysisStack.empty()) {
                QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                return;
            }
            AnalysisItem analysisItem = analysisStack.top();
            if (analysisItem.kind == 2) {
                QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                return;
            }
            if (!LALR1_table[analysisItem.state].contains(QString(sentence[sentenceTop]))) {
                sentence.insert(sentenceTop, '@');
            }
            LALR1TableItem lalr1TableItem = LALR1_table[analysisItem.state][QString(sentence[sentenceTop])];
            if (lalr1TableItem.kind == 1) {

                AnalysisItem chAnalysisItem;
                chAnalysisItem.kind = 2;
                chAnalysisItem.ch = QString(sentence[sentenceTop++]);

                AnalysisItem stAnalysisItem;
                stAnalysisItem.kind = 1;
                stAnalysisItem.state = lalr1TableItem.idx;

                analysisStack.push(chAnalysisItem);
                analysisStack.push(stAnalysisItem);
                printAnalysis();

            } else if (lalr1TableItem.kind == 2) {

                // 获取规约规则
                Item recursionItem = recursion[lalr1TableItem.idx];

                // 删除规约右侧元素
                int popCnt = recursionItem.pos * 2;
                while (!analysisStack.empty() && popCnt) {
                    analysisStack.pop();
                    popCnt--;
                }
                if (popCnt || analysisStack.empty()) {
                    QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                    return;
                }

                // 增加规约左侧元素
                AnalysisItem nextAnalysisItem = analysisStack.top();
                if (nextAnalysisItem.kind != 1) {
                    QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                    return;
                }
                if (!LALR1_table[nextAnalysisItem.state].contains(recursionItem.name)) {
                    sentence.insert(sentenceTop, '@');
                }
                AnalysisItem chAnalysisItem;
                chAnalysisItem.kind = 2;
                chAnalysisItem.ch = recursionItem.name;
                AnalysisItem stAnalysisItem;
                stAnalysisItem.kind = 1;
                stAnalysisItem.state = LALR1_table[nextAnalysisItem.state][recursionItem.name].idx;
                analysisStack.push(chAnalysisItem);
                analysisStack.push(stAnalysisItem);
                printAnalysis();

            } else if (lalr1TableItem.kind == 3) {
                QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                return;
            } else if (lalr1TableItem.kind == 4) {
                ui->resultTableWidget->resizeColumnsToContents();
                if (sentenceTop != sentence.size() - 1) {
                    QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                    return;
                }
                QMessageBox::warning(this, "提醒", "分析完毕，" + str + "属于该文法的句子", QMessageBox::Yes);
                return;
            } else {
                QMessageBox::warning(this, "警告", "待分析的句子与文法不匹配", QMessageBox::Yes);
                return;
            }
        }

    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

