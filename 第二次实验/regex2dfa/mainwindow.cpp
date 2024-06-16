#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <QRect>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QStack>
#include <QPalette>
#include <QColor>

#include "utils.h"
#include "nfa.h"
#include "dfa.h"
#include "codedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nfa()
{
    ui->setupUi(this);

    // 页面基本布局
    this->setWindowTitle("编译原理实验二：正则表达式转NFA、DFA");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int newWidth = screenWidth * 0.9;
    int newHeight = screenHeight * 0.9;
    this->resize(newWidth, newHeight);

    // 分析并产生NFA、DFA、最小化DFA
    connect(ui->analysisPushButton, &QPushButton::clicked, this, [=]() {
        this->nfa.clear();
        this->dfa.clear();
        this->miniDFA.clear();
        this->code = "";
        tableRemoveAll();

        QString allText = ui->textEdit->toPlainText();
        if (allText != "") {

            QStringList lines = allText.split('\n', QString::SkipEmptyParts);

            // 预处理正则表达式组
            QString mainRegex = regexListPreprocessing(lines);

            // 正则表达式转后缀表达式
            QString postFixRegex = regexToPostFix(mainRegex);

            // 后缀表达式转NFA
            this->nfa.fromRegex(postFixRegex);

            // NFA转DFA
            this->dfa.fromNFA(this->nfa);

            // DFA最小化
            this->miniDFA.fromDFA(this->dfa);

            // 转换为代码
            this->code = this->miniDFA.toCode();
        }

    });

    // 显示NFA
    connect(ui->nfaPushButton, &QPushButton::clicked, this, [=]() {
        // 清空原来的表格
        tableRemoveAll();

        ui->tableWidget->setRowCount(nfa.stateNum);
        ui->tableWidget->setColumnCount(nfa.stateSet.size());
        QStringList strListColumnHander;
        for (QString item : nfa.stateSet) {
            strListColumnHander << tr(item.toStdString().c_str());
        }
        ui->tableWidget->setHorizontalHeaderLabels(strListColumnHander);
        QStringList strListRowHander;
        for (int i = 0; i < nfa.stateNum; i++) {
            strListRowHander << tr(QString::number(i).toStdString().c_str());
        }
        ui->tableWidget->setVerticalHeaderLabels(strListRowHander);
        for (int i = 0; i < nfa.stateNum; i++) {
            int j = 0;
            for (QString stateChange: nfa.stateSet) {
                QString itemString = "";
                for (int k = 0; k < nfa.stateNum; k++) {
                    if (nfa.G[i][k] == stateChange) {
                        itemString += QString::number(k);
                        itemString += ",";
                    }
                }
                ui->tableWidget->setItem(i, j, new QTableWidgetItem(itemString.left(itemString.size() - 1)));
                j++;
            }
        }

        // 添加始态、终态颜色
        QTableWidgetItem *beginItem = ui->tableWidget->verticalHeaderItem(nfa.startState);
        QTableWidgetItem *endItem = ui->tableWidget->verticalHeaderItem(nfa.endState);
        beginItem->setTextColor(QColor(0, 255, 0));
        endItem->setTextColor(QColor(255, 0, 0));
    });

    // 显示DFA
    connect(ui->dfaPushButton, &QPushButton::clicked, this, [=]() {
        // 清空原来的表格
        tableRemoveAll();

        ui->tableWidget->setRowCount(dfa.stateNum);
        ui->tableWidget->setColumnCount(nfa.stateSet.size() - (nfa.stateSet.contains("epsilon") ? 1 : 0));

        QStringList strListColumnHander;
        for (QString item : nfa.stateSet) {
            if (item == "epsilon") continue;
            strListColumnHander << tr(item.toStdString().c_str());
        }
        ui->tableWidget->setHorizontalHeaderLabels(strListColumnHander);

        QStringList strListRowHander;
        for (int i = 0; i < dfa.stateNum; i++) {
            strListRowHander << tr(QString::number(i).toStdString().c_str());
        }
        ui->tableWidget->setVerticalHeaderLabels(strListRowHander);

        for (int i = 0; i < dfa.stateNum; i++) {
            if (!dfa.G.contains(i)) continue;
            int j = 0;
            for (QString stateChange: nfa.stateSet) {
                if (stateChange == "epsilon") continue;
                if (dfa.G[i].contains(stateChange)) {

                    int k = dfa.G[i][stateChange];
                    QString titleTr = QString::number(k) + ":{";
                    for (int item: dfa.mapping[k]) {
                        titleTr += QString::number(item);
                        titleTr += ",";
                    }
                    titleTr = titleTr.left(titleTr.size() - 1);
                    titleTr += "}";

                    ui->tableWidget->setItem(i, j, new QTableWidgetItem(titleTr));
                }
                j++;
            }
        }

        // 添加始态、终态颜色
        QTableWidgetItem *beginItem = ui->tableWidget->verticalHeaderItem(dfa.startState);
        beginItem->setTextColor(QColor(0, 255, 0));
        for (int endState: dfa.endStates) {
            QTableWidgetItem *endItem = ui->tableWidget->verticalHeaderItem(endState);
            endItem->setTextColor(QColor(255, 0, 0));
        }

    });

    // 显示最小化的DFA
    connect(ui->minimumDfaPushButton, &QPushButton::clicked, this, [=]() {
        // 清空原来的表格
        tableRemoveAll();

        ui->tableWidget->setRowCount(miniDFA.stateNum);
        ui->tableWidget->setColumnCount(nfa.stateSet.size() - (nfa.stateSet.contains("epsilon") ? 1 : 0));

        QStringList strListColumnHander;
        for (QString item : nfa.stateSet) {
            if (item == "epsilon") continue;
            strListColumnHander << tr(item.toStdString().c_str());
        }
        ui->tableWidget->setHorizontalHeaderLabels(strListColumnHander);

        QStringList strListRowHander;
        for (int i = 0; i < miniDFA.stateNum; i++) {
            strListRowHander << tr(QString::number(i).toStdString().c_str());
        }
        ui->tableWidget->setVerticalHeaderLabels(strListRowHander);

        for (int i = 0; i < miniDFA.stateNum; i++) {
            if (!miniDFA.G.contains(i)) continue;
            int j = 0;
            for (QString stateChange: nfa.stateSet) {
                if (stateChange == "epsilon") continue;
                if (miniDFA.G[i].contains(stateChange)) {

                    int k = miniDFA.G[i][stateChange];
                    QString titleTr = QString::number(k) + ":{";
                    for (int item: miniDFA.mapping[k]) {
                        titleTr += QString::number(item);
                        titleTr += ",";
                    }
                    titleTr = titleTr.left(titleTr.size() - 1);
                    titleTr += "}";

                    ui->tableWidget->setItem(i, j, new QTableWidgetItem(titleTr));
                }
                j++;
            }
        }

        // 添加始态、终态颜色
        QTableWidgetItem *beginItem = ui->tableWidget->verticalHeaderItem(miniDFA.startState);
        beginItem->setTextColor(QColor(0, 255, 0));
        for (int endState: miniDFA.endStates) {
            QTableWidgetItem *endItem = ui->tableWidget->verticalHeaderItem(endState);
            endItem->setTextColor(QColor(255, 0, 0));
        }
    });

    // 生成匹配代码
    connect(ui->codePushButton, &QPushButton::clicked, this, [=]() {
        CodeDialog* codeDialog = new CodeDialog(this, code);
        codeDialog->exec();
    });

    // 重置程序
    connect(ui->resetPushButton, &QPushButton::clicked, this, [=]() {
        this->nfa.clear();
        this->dfa.clear();
        this->miniDFA.clear();
        this->code = "";
        tableRemoveAll();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::regexListPreprocessing(QStringList regexList) {
    int lineSize = regexList.size();

    // 去除所有空格
    for (int i = 0; i < lineSize; i++) {
        regexList[i].replace(" ", "");
    }

    // 替换[]
    for (int i = 0; i < lineSize; i++) {
        int leftBracket = -1;    // 左右括号位置
        for (int j = 0; j < regexList[i].size(); j++) {
            if (regexList[i][j] == '\\') {             // 转义字符
                j++;
            } else if (regexList[i][j] == '[') {       // 匹配左括号
                leftBracket = j;
            } else if (regexList[i][j] == ']') {
                if (leftBracket == -1) {            // 出现了右括号，但没有出现左括号
                    continue;
                } else {
                    if (leftBracket == j - 1) continue; // 特判一下
                    QString bracketString = regexList[i].mid(leftBracket + 1, j - leftBracket - 1);
                    QString newBracketString = "";
                    int bracketLen = bracketString.size();
                    int groupNum = bracketLen / 3;
                    for (int k = 0; k < groupNum; k++) {
                        int beginIndex = 3 * k;
                        int endIndex = 3 * k + 2;
                        for (char l = bracketString[beginIndex].unicode(); l <= bracketString[endIndex].unicode(); l++) {
                            newBracketString.append(l);
                            newBracketString.append('|');
                        }
                    }
                    if (groupNum != 0) {
                        // 替换字符串
                        int newBracketStringLen = newBracketString.size();
                        newBracketString = newBracketString.mid(0, newBracketStringLen - 1);
                        regexList[i] = regexList[i].left(leftBracket) + "(" + newBracketString + ")" + regexList[i].right(regexList[i].size() - j - 1);
                        j = leftBracket + newBracketString.size() + 2 - 1;
                    }
                    leftBracket = -1;               // 后续需要重新匹配中括号
                }
            }
        }
    }

    // 替换正闭包+
    for (int i = 0; i < lineSize; i++) {
        int leftBracket = -1;
        for (int j = 0; j < regexList[i].size(); j++) {
            if (regexList[i][j] == '\\') {             // 转义字符
                j++;
            } else if (regexList[i][j] == '(') {       // 匹配左括号
                leftBracket = j;
            } else if (regexList[i][j] == '+') {        // 遇到加号
                if (j > 0) {
                    regexList[i][j] = '*';
                    if (regexList[i][j - 1] == ')') {
                        regexList[i].insert(leftBracket, regexList[i].mid(leftBracket, j - leftBracket));
                        j += j - leftBracket;
                    } else {
                        regexList[i].insert(j - 1, regexList[i][j - 1]);
                        j += 1;
                    }
                }
            }
        }
    }

    // 替换可选?
    for (int i = 0; i < lineSize; i++) {
        int leftBracket = -1;
        for (int j = 0; j < regexList[i].size(); j++) {
            if (regexList[i][j] == '\\') {             // 转义字符
                j++;
            } else if (regexList[i][j] == '(') {       // 匹配左括号
                leftBracket = j;
            } else if (regexList[i][j] == '?') {        // 遇到加号
                if (j > 0) {
                    regexList[i][j] = '|';
                    if (regexList[i][j - 1] == ')') {
                        regexList[i].insert(leftBracket, '(');
                        regexList[i].insert(j + 2, "#)");
                        j += 3;
                    } else {
                        regexList[i].insert(j - 1, '(');
                        regexList[i].insert(j + 2, "#)");
                        j += 3;
                    }
                }
            }
        }
    }

    // 获取要转换成为NFA的主正则表达式
    QString mainRegex;
    QHash<QString, QString> regexHash;
    for (int i = 0; i < lineSize; i++) {
        QString regexItem = regexList[i];
        if (regexItem[0] == '_') {   // 主正则表达式 (仅考虑有一个主正则表达式的情况)
            mainRegex = regexItem;
            regexList.removeAt(i);
        } else {                    // 非主正则表达式，获取其等号左侧的标识符
            int regexLen = regexItem.size();
            for (int j = 0; j < regexLen; j++) {
                if (regexItem[j] == '=') {
                    QString identifier = regexItem.left(j);
                    QString regexBody = regexItem.right(regexLen - j - 1);
                    regexHash.insert(identifier, regexBody);
                    break;          // 只捕获第一个等号(等号前的转义符不考虑)
                }
            }
        }
    }

    // 合并正则表达式
    for (auto key: regexHash.keys()) {
        QString replaceString = regexHash[key];
        if (replaceString[0] != '(' || replaceString[replaceString.size() - 1] != ')') {
            replaceString = "(" + replaceString + ")";
        }
        for (auto& value: regexHash.values()) {
            value.replace(key, replaceString);
        }
        mainRegex.replace(key, replaceString);
    }

    // 主正则表达式添加连接符
    mainRegex = mainRegex.right(mainRegex.size() - mainRegex.indexOf('=') - 1);
    for (int i = 0; i < mainRegex.size() - 1; i++) {
        if (mainRegex[i] == '\\') {
            i++;
            if (i < mainRegex.size() - 1) {
                if (mainRegex[i + 1] == '(' || !isOperator(mainRegex[i + 1].unicode())) {
                    mainRegex.insert(i + 1, '.');
                    i++;
                }
            }
        } else if (isOperator(mainRegex[i].unicode())) {
            if (mainRegex[i] == ')' || mainRegex[i] == '*') {
                if (!isOperator(mainRegex[i + 1].unicode()) || mainRegex[i + 1] == '(') {
                    mainRegex.insert(i + 1, '.');
                    i++;
                }
            }
        } else {
            if (!isOperator(mainRegex[i + 1].unicode()) || mainRegex[i + 1] == '(') {
                mainRegex.insert(i + 1, '.');
                i++;
            }
        }
    }

    return mainRegex;
}

QString MainWindow::regexToPostFix(QString re)
{
    QStack<QChar> s1;
    QStack<QString> s2;
    for (int i = 0; i < re.size(); i++) {
        if (re[i] == '\\') {
            if (i < re.size() - 1) {
                s2.push(re.mid(i, 2));
            }
            i++;
        }
        else if (isOperator(re[i].unicode())) {
            if (re[i] == '(') {
                s1.push(re[i]);                     // 左括号直接放入符号栈
            } else if (re[i] == ')') {
                while (!s1.empty() && s1.top() != '(') {                          // 右括号不断拿出符号栈内容，放入结果栈，直到遇到结果栈
                    s2.push(s1.top());
                    s1.pop();
                }
                if (!s1.empty() && s1.top() == '(') s1.pop();
            } else {
                int nowPriority = getPriority(re[i].unicode());
                while (!s1.empty() && getPriority(s1.top().unicode()) >= nowPriority) {
                    s2.push(s1.top());
                    s1.pop();
                }
                if (s1.empty() || getPriority(s1.top().unicode()) < nowPriority) {
                    s1.push(re[i]);
                }
            }
        } else {
            s2.push(re.mid(i, 1));
        }
    }
    while (!s1.empty()) {
        s2.push(s1.top());
        s1.pop();
    }
    QStack<QString> s3;
    QString postFixRegex = "";
    while (!s2.empty()) {
        s3.push(s2.top());
        s2.pop();
    }
    while (!s3.empty()) {
        postFixRegex += s3.top();
        s3.pop();
    }
    return postFixRegex;
}

void MainWindow::tableRemoveAll()
{
    ui->tableWidget->clear();
    ui->tableWidget->setHorizontalHeaderLabels(QStringList());
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
}

