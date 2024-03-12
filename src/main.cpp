#include "lexerwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LexerWindow w;
    w.show();
    return a.exec();
}
