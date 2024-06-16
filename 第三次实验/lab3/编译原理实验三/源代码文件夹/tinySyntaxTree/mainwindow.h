#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QString>
#include <QTreeWidgetItem>

#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct SyntaxNode {
    QString nodeStr;
    QVector<SyntaxNode *> children;
    QVector<SyntaxNode *> brother;

    SyntaxNode(QString str): nodeStr(str), children(QVector<SyntaxNode *>()), brother(QVector<SyntaxNode *>()) {}
    SyntaxNode(): SyntaxNode("") {}
}SyntaxNode, * SyntaxTree;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<QPair<QString, LexerInfo>> tokenList;
    QVector<ErrorInfo> errorList;
    QString TOKEN;
    int curType, curRow, curCol;
    int tokenIndex;
    SyntaxTree root;

private:
    Ui::MainWindow *ui;

    /**
     * @brief getToken
     * 获取下一个token
     */
    void getToken();

    // 匹配需要的token或者指定类型的token
    void match(QString expectedToken);
    void match(int expectedTokenType);

    /**
     * @brief ERROR
     * 错误函数
     */
    void ERROR(int errorType);

    // 程序系列
    SyntaxTree program();           // 完整程序
    SyntaxTree stmt_sequence();     // 语句序列
    SyntaxTree statement();         // 语句

    // 语句系列
    SyntaxTree if_stmt();           // if 语句
    SyntaxTree repeat_stmt();       // repeat 语句
    SyntaxTree for_stmt();          // for 语句
    SyntaxTree while_stmt();        // while 语句
    SyntaxTree assign_stmt();       // 赋值语句
    SyntaxTree read_stmt();         // 读语句
    SyntaxTree write_stmt();        // 写语句
    SyntaxTree write_sub_stmt();        // 写子语句
    SyntaxTree assign_sub_stmt();   // 赋值语句子语句（区分算术表达式与正则表达式赋值）

    // 正则表达式系列
    SyntaxTree re();
    SyntaxTree orre();
    SyntaxTree conre();
    SyntaxTree repre();

    // 算术表达式系列
    SyntaxTree exp();
    SyntaxTree simple_exp();
    SyntaxTree term();
    SyntaxTree factor();
    SyntaxTree selfexp();
    SyntaxTree resexp();

    // 重置程序
    void reset();

    // 清除语法树
    void remove(SyntaxTree tr);

    // 展示语法树
    void showTree(QTreeWidgetItem* item, SyntaxTree tr);

};
#endif // MAINWINDOW_H
