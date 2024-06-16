#ifndef LEXERWINDOW_H
#define LEXERWINDOW_H

#include <QMainWindow>
#include <QPair>
#include <QVector>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class LexerWindow; }
QT_END_NAMESPACE

class LexerWindow : public QMainWindow
{
    Q_OBJECT

public:
    LexerWindow(QWidget *parent = nullptr);
    ~LexerWindow();

private:

    // 工具函数，用于分词
    static QVector<QPair<QString, QString>> lexer(QString content, int type);
    static bool isType(QString s);

    Ui::LexerWindow *ui;
};
#endif // LEXERWINDOW_H
