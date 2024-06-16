#ifndef UTILS_H
#define UTILS_H

#include <QVector>
#include <QString>

/**
 * @brief The LexerInfo struct
 * 记录分词信息
 * 类型：
 * 0：分隔符号
 * 1：字符
 * 2：数字
 * 3：标识符
 * 4：关键字
 * 5: 运算符
 * 6：注释
 */
struct LexerInfo {
    int lexerType;  // 分词类别
    int row;        // 行
    int column;     // 列
    LexerInfo(int type, int r, int c): lexerType(type), row(r), column(c) {}
    LexerInfo(): LexerInfo(0, 0, 0) {}
};

struct ErrorInfo {
    QString info;
    int row, column;
    ErrorInfo(QString msg, int r, int c): info(msg), row(r), column(c) {}
    ErrorInfo(): ErrorInfo("", 0, 0) {}
};

QVector<QPair<QString, LexerInfo>> lexer(QString content);
QString getTypeString(int type);

#endif // UTILS_H
