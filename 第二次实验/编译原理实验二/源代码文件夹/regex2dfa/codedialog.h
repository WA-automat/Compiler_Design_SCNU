#ifndef CODEDIALOG_H
#define CODEDIALOG_H

#include <QDialog>

namespace Ui {
class CodeDialog;
}

class CodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeDialog(QWidget *parent = nullptr, QString code = "");
    ~CodeDialog();

private:
    Ui::CodeDialog *ui;
};

#endif // CODEDIALOG_H
