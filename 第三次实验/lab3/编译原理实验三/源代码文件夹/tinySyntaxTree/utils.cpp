#include "utils.h"

#include <QFile>
#include <QHash>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

QVector<QPair<QString, LexerInfo> > lexer(QString content)
{

    // 获取关键字与运算符
    QFile file(":/mapping.json");
    QHash<QString, QString> hash;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

        if (!jsonDocument.isNull()) {
            QJsonObject jsonObject = jsonDocument.object();

            for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
                QString key = it.key();
                QString value = it.value().toString();
                hash.insert(key, value);
            }
        }
    }

    // 开始分词
    QVector<QPair<QString, LexerInfo>> tokenList;
    QStringList lines = content.split("\n");
    bool finishCom = false;
    int cachePos = 0;
    for (int index = 0; index < lines.size(); index++) {
        const QString& line = lines[index];
        int pos = finishCom ? cachePos : 0;
        int len = line.size();

        finishCom = false;

        // 跳过空格与tab
        while (pos < len && (line[pos] == ' ' || line[pos] == '\t')) pos++;
        if (pos == len) continue;

        // 遍历单行
        while (pos < len) {
//            qDebug() << line[pos] << ' ' << hash.contains(QString(line[pos])) << endl;
            if (hash.contains(line[pos]) && hash[line[pos]] == "separator") {
                // 判断 (、)、; 等分隔符号
                tokenList.append(qMakePair(QString(line[pos]), LexerInfo(0, index, pos)));
                pos++;
            } else if (pos != len && line[pos] == '\'') {
                // 字符类型
                int endIndex = line.indexOf(line[pos], pos + 1);
                if (endIndex != -1) {
                    while (endIndex != -1 && line[endIndex - 1] == '\\') endIndex = line.indexOf(line[pos], endIndex + 1);
                    if (endIndex == -1) {
                        break;
                    }
                    QString str = line.mid(pos, endIndex - pos + 1);
                    tokenList.append(qMakePair(str, LexerInfo(1, index, pos)));
                    pos = endIndex + 1;
                } else {
                    // 出现错误
                    break;
                }
            } else if (line[pos] == '+' || line[pos] == '-' || line[pos].isDigit()) {
                // 数字类型或运算符
                if (line[pos] == '+' || line[pos] == '-') {
                    // 判断 + 与 - 是运算符还是数字的一部分

                    if (tokenList.back().second.lexerType != 5) {
                        // 在tiny语言中，上一个token不为运算符，则当前+-必定为运算符
                        tokenList.append(qMakePair(QString(line[pos]), LexerInfo(5, index, pos)));
                        pos++;
                        continue;
                    }
//                    qDebug() << tokenList.back().first << ' ' << line[pos];
                    if (tokenList.back().first == line[pos] && tokenList.size() > 1 && tokenList[tokenList.size() - 2].second.lexerType != 2) {
//                        qDebug() << pos << endl;
                        tokenList.pop_back();
                        tokenList.append(qMakePair(QString(line[pos] == '+' ? "++" : "--"), LexerInfo(5, index, pos - 1)));
                        pos++;
                        continue;
                    }
                    if (pos == len - 1 || !line[pos + 1].isDigit()) {
                        // + 与 - 位于最后一个位置或后续字符不为数字
                        tokenList.append(qMakePair(QString(line[pos]), LexerInfo(5, index, pos)));
                        pos++;
                        continue;
                    }
                }

                // 完整数字匹配
                QString number = ((line[pos] == '+' || line[pos] == '-' ) ? line[pos] : QString(""));
                if (line[pos] == '+' || line[pos] == '-') pos++;
                while (pos < len && line[pos].isDigit()) number += line[pos++];
                if (pos < len - 1 && line[pos] == '.' && line[pos + 1].isDigit()) number += line[pos++];
                while (pos < len && line[pos].isDigit()) number += line[pos++];
                if (pos < len - 1 && (line[pos] == 'E' || line[pos] == 'e') && line[pos + 1].isDigit()) number += line[pos++];
                while (pos < len && line[pos].isDigit()) number += line[pos++];
                tokenList.append(qMakePair(QString(number), LexerInfo(2, index, pos - number.size())));

            } else if (line[pos].isLetter() || line[pos] == '_') {
                // 标识符或关键字
                QString identifier;
                while (pos < len && (line[pos].isDigit() || line[pos].isLetter() || line[pos] == '_')) identifier += line[pos++];

                // 判断是否为关键词
                if (hash.contains(identifier) && hash[identifier] == "keyword") {
                    // 关键词
                    tokenList.append(qMakePair(identifier, LexerInfo(4, index, pos - identifier.size())));
                } else {
                    // 标识符
                    tokenList.append(qMakePair(identifier, LexerInfo(3, index, pos - identifier.size())));
                }

            } else if (line[pos] == '{') {
                // 注释
                int indexCnt = index, posCnt = pos;
                while (indexCnt < lines.size()) {

                    // 查找注释结束位置
                    int endCom = lines[indexCnt].indexOf('}', pos);
                    if (endCom != -1) {
                        tokenList.append(qMakePair(lines[indexCnt].mid(posCnt, endCom - posCnt + 1), LexerInfo(6, indexCnt, posCnt)));
                        posCnt = endCom + 1;
                        break;
                    }

                    // 没找到就找下一行
                    tokenList.append(qMakePair(lines[indexCnt].right(lines[indexCnt].size() - posCnt), LexerInfo(6, indexCnt, posCnt)));
                    posCnt = 0;
                    indexCnt++;

                }
                index = indexCnt - 1;
                cachePos = posCnt;
                finishCom = true;
                break;

            } else {
                // 运算符
                if (pos < len - 1 && hash.contains(line.mid(pos, 2)) && hash[line.mid(pos, 2)] == "operator") {
                    // 双字符运算符
                    tokenList.append(qMakePair(line.mid(pos, 2), LexerInfo(5, index, pos)));
                    pos += 2;
                } else if (hash.contains(line[pos]) && hash[line[pos]] == "operator") {
                    // 单字符运算符
                    tokenList.append(qMakePair(line[pos], LexerInfo(5, index, pos)));
                    pos++;
                } else {
                    // 其他
                    pos++;
                }
            }

        }
    }

//    for (const auto &token : tokenList) {
//        qDebug() << token.first << ' ' << token.second.lexerType;
//    }

    return tokenList;
}

QString getTypeString(int type)
{
    switch (type) {
    case 0:
        return "分隔符";
    case 1:
        return "字符";
    case 2:
        return "数字";
    case 3:
        return "标识符";
    case 4:
        return "关键字";
    case 5:
        return "运算符";
    case 6:
        return "注释";
    default:
        return "";
    }
}
