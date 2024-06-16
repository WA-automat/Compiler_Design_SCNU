#include "codedialog.h"
#include "ui_codedialog.h"

#include <QScreen>
#include <QRect>

CodeDialog::CodeDialog(QWidget *parent, QString code) :
    QDialog(parent),
    ui(new Ui::CodeDialog)
{
    ui->setupUi(this);
    ui->codeBrowser->setText(code);

    this->setWindowTitle("匹配代码");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int newWidth = screenWidth * 0.6;
    int newHeight = screenHeight * 0.6;
    this->resize(newWidth, newHeight);
}

CodeDialog::~CodeDialog()
{
    delete ui;
}
