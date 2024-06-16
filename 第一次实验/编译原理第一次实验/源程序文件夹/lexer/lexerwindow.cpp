#include "lexerwindow.h"
#include "ui_lexerwindow.h"

#include <QScreen>
#include <QRect>
#include <QFile>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

LexerWindow::LexerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LexerWindow)
{
    ui->setupUi(this);

    // 基本布局信息
    this->setWindowTitle("编译原理实验一：分词器");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int newWidth = screenWidth * 0.64;
    int newHeight = screenHeight * 0.48;
    this->resize(newWidth, newHeight);
    ui->pushButton->setFixedHeight(200);
    ui->xPushButton->setFixedHeight(200);
    ui->uploadButton->setFixedHeight(200);

    // 设置表格窗口
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "token" << "类型");
    ui->tableWidget->setColumnWidth(0, newWidth / 4);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自适应宽度
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 判断分词类型的变量
    int type = 1;

    // 按钮事件: 打开某文件获取其中的所有文本
    connect(ui->uploadButton, &QPushButton::clicked, this, [&]() {
        type = 1;
        QString fileName = QFileDialog::getOpenFileName(this, "选择C++文件");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                in.setCodec("UTF-8"); // 设置编码格式为 UTF-8
                ui->fileBrowser->setText(in.readAll());
                file.close();
            }
        }
    });
    connect(ui->xPushButton, &QPushButton::clicked, this, [&]() {
        type = 2;
        QString fileName = QFileDialog::getOpenFileName(this, "选择XC++文件");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                in.setCodec("UTF-8"); // 设置编码格式为 UTF-8
                ui->fileBrowser->setText(in.readAll());
                file.close();
            }
        }
    });

    // 按钮事件：分词
    connect(ui->pushButton, &QPushButton::clicked, this, [&]() {
        QString text = ui->fileBrowser->toPlainText();
        QVector<QPair<QString, QString>> list = LexerWindow::lexer(text, type);
        ui->tableWidget->setRowCount(list.size());
        for (int i = 0; i < list.size(); i++) {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(list[i].first));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(list[i].second));
        }
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *item = ui->tableWidget->item(row, 1);
            if (item) item->setTextAlignment(Qt::AlignCenter);
        }
        ui->tableWidget->show();
    });

}

LexerWindow::~LexerWindow()
{
    delete ui;
}

QVector<QPair<QString, QString>> LexerWindow::lexer(QString content, int type)
{
    /*
     * 功能：将文本字符串content分词
     * 类别：关键词、运算符、数字类型、标识符，注释，分界符号
     * 返回：键值对序列（词：类型）
     */

    // 读取json文件：关键字，运算符，边界符号等
    QString mappingPath;
    if (type == 1) {
        mappingPath = ":/json/mapping.json";
    } else {
        mappingPath = ":/json/Xmapping.json";
    }

    qDebug() << mappingPath << endl;

    QFile file(mappingPath);
    QHash<QString, QString> hash;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

        if (!jsonDocument.isNull()) {
            QJsonObject jsonObject = jsonDocument.object();

            // 遍历 JSON 对象的所有键值对，并添加到哈希表中
            for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
                QString key = it.key();
                QString value = it.value().toString();
                hash.insert(key, value);
                qDebug() << key << ' ' << value << endl;
            }
        }
    }

    // 开始分词
    QVector<QPair<QString, QString>> tokenList(0);
    QStringList lines = content.split("\n");
    for (int index = 0; index < lines.size(); index++) {
        const QString& line = lines[index];
        int pos = 0;
        int len = line.size();

        // 跳过空格与tab
        while (pos < len && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        if (pos == len) continue;

        // 单行注释
        if (line[pos] == '/' && line[pos + 1] == '/') {
            tokenList.append(qMakePair(line.mid(pos), QString("注释")));
            continue;
        }

        // 遍历单行
        while (pos < len) {
//            qDebug() << index << ' ' << pos;
            if (hash.contains(line[pos]) && hash[line[pos]] == "SEPARATOR") {
                // 分隔符
                tokenList.append(qMakePair(QString(line[pos++]), QString("分界符号")));
            } else if (pos != len && (line[pos] == '\"' || line[pos] == '\'')) {
                // 字符串或字符
                int endIndex = line.indexOf(line[pos], pos + 1);
                if (endIndex != -1) {
                    while (endIndex != -1 && line[endIndex - 1] == '\\') endIndex = line.indexOf(line[pos], endIndex + 1);
                    if (endIndex == -1) {
                        break;
                    }
                    QString str = line.mid(pos, endIndex - pos + 1);
                    tokenList.append(qMakePair(str, line[pos] == '\"' ? QString("字符串") : QString("字符")));
                    pos = endIndex + 1;
                } else {
                    break;
                }
            } else if (line[pos].isDigit()) {
                // 数字类型
                QString number;
                // 读取前置数字
                while (pos < len && (line[pos].isDigit())) number += line[pos++];

                if (line[pos] == 'x') {
                    number += line[pos++];
                    while (pos < len && (line[pos].isDigit() || (line[pos].toLower() >= 'a' && line[pos].toLower() <= 'f'))) number += line[pos++];
                    tokenList.append(qMakePair(number, QString("十六进制整数")));
                } else if (line[pos] == 'b') {
                    number += line[pos++];
                    while (pos < len && (line[pos] == '0' || line[pos] == '1')) number += line[pos++];
                    tokenList.append(qMakePair(number, QString("二进制整数")));
                } else if (pos < len && (line[pos].toLower() == 'e' || line[pos] == '.')) {
                    number += line[pos++];
                    while (pos < len && (line[pos].isDigit())) number += line[pos++];
                    tokenList.append(qMakePair(number, QString("浮点数")));
                } else {
                    if (number[0] == '0') {
                        bool isOctal = true;
                        for (int i = 1; i < number.size(); i++) {
                            if (number[i] < '0' || number[i] > '8') {
                                isOctal = false;
                                break;
                            }
                        }
                        if (isOctal) {
                            tokenList.append(qMakePair(number, QString("八进制整数")));
                        } else {
                            tokenList.append(qMakePair(number, QString("整数")));
                        }
                    } else {
                        tokenList.append(qMakePair(number, QString("整数")));
                    }
                }
            } else if (line[pos].isLetter() || line[pos] == '_') {
                // 标识符、关键字类型
                QString identifier;
                while (pos < len && (line[pos].isLetter() || line[pos] == '_' || line[pos].isDigit())) identifier += line[pos++];

                if (hash.contains(identifier) && hash[identifier] == "KEYWORD") {
                    // 捕获指针类型
                    if (LexerWindow::isType(identifier)) {
                        while (pos < len && (line[pos] == ' ' || line[pos] == "*")) {
                            if (line[pos] == ' ') pos++;
                            else identifier += line[pos++];
                        }
                    }
                    tokenList.append(qMakePair(identifier, QString("关键字")));
                    if (identifier == "include" || identifier == "import") {
                        if (line.indexOf('<', pos) != -1) {
                            int beginHeaderIndex = line.indexOf('<', pos);
                            int endHeaderIndex = line.indexOf('>', pos);
                            tokenList.append(qMakePair(line.mid(beginHeaderIndex, endHeaderIndex - beginHeaderIndex + 1), QString("头文件")));
                            pos = endHeaderIndex + 1;
                        } else {
                            int beginHeaderIndex = line.indexOf('\"', pos);
                            int endHeaderIndex = line.indexOf('\"', beginHeaderIndex + 1);
                            tokenList.append(qMakePair(line.mid(beginHeaderIndex, endHeaderIndex - beginHeaderIndex + 1), QString("头文件")));
                            pos = endHeaderIndex + 1;
                        }
                    }
                } else {
                    tokenList.append(qMakePair(identifier, QString("标识符")));
                }
            } else if (hash.contains(line[pos]) && hash[line[pos]] == "OPERATOR") {
                // 运算符
                QString op;
                if (line[pos] == '/') {
                    // 判断是否为注释
                    if (pos != len && (line[pos + 1] == '/' || line[pos + 1] == '*')) {
                        if (line[pos + 1] == '/') {
                            tokenList.append(qMakePair(line.mid(pos), QString("注释")));
                            break;
                        } else {
                            int endStar = line.indexOf('*', pos + 2);
                            // 找第一行
                            while (endStar != -1 && endStar < len - 1) {
                                if (line[endStar + 1] == '/') {
                                    tokenList.append(qMakePair(line.mid(pos, endStar + 2 - pos), QString("注释")));
                                    pos = endStar + 2;
                                    break;
                                }
                                endStar = line.indexOf('*', endStar + 1);
                            }
                            if (endStar == -1 || endStar == len - 1) {
                                tokenList.append(qMakePair(line.mid(pos), QString("注释")));
                                // 接下来继续找
                                while (index + 1 < lines.size()) {
                                    bool flag = true;
                                    index++;
                                    QString newLine = lines[index];
                                    len = newLine.size();
                                    pos = 0;

                                    // 跳过空格与tab
                                    while (pos < len && (newLine[pos] == ' ' || newLine[pos] == '\t')) pos++;
                                    if (pos == len) continue;

                                    endStar = newLine.indexOf('*', pos);
                                    while (endStar != -1 && endStar < len - 1) {
                                        if (newLine[endStar + 1] == '/') {
                                            tokenList.append(qMakePair(newLine.mid(pos, endStar + 2 - pos), QString("注释")));
                                            pos = endStar + 2;
                                            flag = false;
                                            break;
                                        }
                                        endStar = newLine.indexOf('*', endStar + 1);
                                    }
                                    if (endStar == -1 || endStar == len - 1) {
                                        tokenList.append(qMakePair(newLine.mid(pos), QString("注释")));
                                    }
                                    if (!flag) break;
                                }
                            }
                        }
                    } else {
                        op += line[pos++];
                        tokenList.append(qMakePair(op, QString("运算符")));
                    }
                } else {
                    while (pos < len && (hash.contains(line[pos]) && hash[line[pos]] == "OPERATOR")) op += line[pos++];
                    tokenList.append(qMakePair(op, QString("运算符")));
                }
            } else {
                pos++;
            }
        }
    }

    return tokenList;
}

bool LexerWindow::isType(QString s)
{
    if (s == "int" || s == "short" || s == "long" || s == "bool" || s == "char" ||
            s == "void" || s == "float" || s == "double" || s == "signed" || s == "unsigned" || s == "auto") {
        return true;
    }
    return false;
}
