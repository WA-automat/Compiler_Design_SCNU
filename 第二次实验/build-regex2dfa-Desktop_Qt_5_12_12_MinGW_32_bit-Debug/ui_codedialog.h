/********************************************************************************
** Form generated from reading UI file 'codedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CODEDIALOG_H
#define UI_CODEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_CodeDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QTextBrowser *codeBrowser;

    void setupUi(QDialog *CodeDialog)
    {
        if (CodeDialog->objectName().isEmpty())
            CodeDialog->setObjectName(QString::fromUtf8("CodeDialog"));
        CodeDialog->resize(648, 496);
        horizontalLayout = new QHBoxLayout(CodeDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        codeBrowser = new QTextBrowser(CodeDialog);
        codeBrowser->setObjectName(QString::fromUtf8("codeBrowser"));

        horizontalLayout->addWidget(codeBrowser);


        retranslateUi(CodeDialog);

        QMetaObject::connectSlotsByName(CodeDialog);
    } // setupUi

    void retranslateUi(QDialog *CodeDialog)
    {
        CodeDialog->setWindowTitle(QApplication::translate("CodeDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CodeDialog: public Ui_CodeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CODEDIALOG_H
