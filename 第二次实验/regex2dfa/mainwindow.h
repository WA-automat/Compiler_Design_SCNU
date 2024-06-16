#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "nfa.h"
#include "dfa.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    NFA nfa;
    DFA dfa;
    DFA miniDFA;
    QString code;

    static QString regexListPreprocessing(QStringList allRegex);
    static QString regexToPostFix(QString re);

    void tableRemoveAll(void);
};
#endif // MAINWINDOW_H
