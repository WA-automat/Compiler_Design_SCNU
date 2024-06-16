/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *browserWidget;
    QVBoxLayout *verticalLayout_5;
    QTextBrowser *textBrowser;
    QWidget *titleWidget;
    QVBoxLayout *verticalLayout_4;
    QLabel *title;
    QWidget *buttonWidget;
    QVBoxLayout *verticalLayout_2;
    QPushButton *analysisPushButton;
    QPushButton *nfaPushButton;
    QPushButton *dfaPushButton;
    QPushButton *minimumDfaPushButton;
    QPushButton *codePushButton;
    QPushButton *resetPushButton;
    QWidget *textWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *textEdit;
    QWidget *mainWidget;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelOfTable;
    QTableWidget *tableWidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        browserWidget = new QWidget(centralwidget);
        browserWidget->setObjectName(QString::fromUtf8("browserWidget"));
        verticalLayout_5 = new QVBoxLayout(browserWidget);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        textBrowser = new QTextBrowser(browserWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(10);
        textBrowser->setFont(font);
        textBrowser->setLineWrapColumnOrWidth(1);

        verticalLayout_5->addWidget(textBrowser);


        gridLayout->addWidget(browserWidget, 2, 1, 1, 1);

        titleWidget = new QWidget(centralwidget);
        titleWidget->setObjectName(QString::fromUtf8("titleWidget"));
        verticalLayout_4 = new QVBoxLayout(titleWidget);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        title = new QLabel(titleWidget);
        title->setObjectName(QString::fromUtf8("title"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(title->sizePolicy().hasHeightForWidth());
        title->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Times New Roman"));
        font1.setPointSize(18);
        title->setFont(font1);
        title->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(title);

        buttonWidget = new QWidget(titleWidget);
        buttonWidget->setObjectName(QString::fromUtf8("buttonWidget"));
        buttonWidget->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_2 = new QVBoxLayout(buttonWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        analysisPushButton = new QPushButton(buttonWidget);
        analysisPushButton->setObjectName(QString::fromUtf8("analysisPushButton"));

        verticalLayout_2->addWidget(analysisPushButton);

        nfaPushButton = new QPushButton(buttonWidget);
        nfaPushButton->setObjectName(QString::fromUtf8("nfaPushButton"));

        verticalLayout_2->addWidget(nfaPushButton);

        dfaPushButton = new QPushButton(buttonWidget);
        dfaPushButton->setObjectName(QString::fromUtf8("dfaPushButton"));

        verticalLayout_2->addWidget(dfaPushButton);

        minimumDfaPushButton = new QPushButton(buttonWidget);
        minimumDfaPushButton->setObjectName(QString::fromUtf8("minimumDfaPushButton"));

        verticalLayout_2->addWidget(minimumDfaPushButton);

        codePushButton = new QPushButton(buttonWidget);
        codePushButton->setObjectName(QString::fromUtf8("codePushButton"));

        verticalLayout_2->addWidget(codePushButton);

        resetPushButton = new QPushButton(buttonWidget);
        resetPushButton->setObjectName(QString::fromUtf8("resetPushButton"));

        verticalLayout_2->addWidget(resetPushButton);


        verticalLayout_4->addWidget(buttonWidget);


        gridLayout->addWidget(titleWidget, 0, 0, 1, 1);

        textWidget = new QWidget(centralwidget);
        textWidget->setObjectName(QString::fromUtf8("textWidget"));
        textWidget->setMinimumSize(QSize(0, 0));
        verticalLayout = new QVBoxLayout(textWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(textWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        textEdit = new QTextEdit(textWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);


        gridLayout->addWidget(textWidget, 2, 0, 1, 1);

        mainWidget = new QWidget(centralwidget);
        mainWidget->setObjectName(QString::fromUtf8("mainWidget"));
        mainWidget->setMinimumSize(QSize(0, 0));
        mainWidget->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_3 = new QVBoxLayout(mainWidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        labelOfTable = new QLabel(mainWidget);
        labelOfTable->setObjectName(QString::fromUtf8("labelOfTable"));
        labelOfTable->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(labelOfTable);

        tableWidget = new QTableWidget(mainWidget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout_3->addWidget(tableWidget);


        gridLayout->addWidget(mainWidget, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        textBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'\345\256\213\344\275\223'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">Regex2DFA</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\346\230\257\351\200\232\350\277\207\350\276\223\345\205\245\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\347\273\204\357\274\214\345\260\206\344\273\245&quot;_&quot;\345\274\200\345\244\264\347\232\204\345\217\230\351\207\217\346\211\200\344\273\243\350\241\250\347\232\204\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\350\275\254\346\215\242\344\270\272NFA\343\200\201DFA\343\200"
                        "\201\346\234\200\345\260\217\345\214\226DFA\347\232\204\350\275\257\344\273\266\343\200\202</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\344\273\245</span><span style=\" font-family:'SimSun'; font-size:11pt; color:#00ff00;\">\347\273\277\350\211\262</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\346\240\207\350\256\260\345\247\213\346\200\201\357\274\214\344\273\245</span><span style=\" font-family:'SimSun'; font-size:11pt; color:#ff0000;\">\347\272\242\350\211\262</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\346\240\207\350\256\260\347\273\210\346\200\201\357\274\211</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px"
                        "; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">\345\205\266\344\270\255\357\274\214\350\275\257\344\273\266\345\256\236\347\216\260\344\272\206\357\274\232</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">1. </span><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">\345\210\206\346\236\220</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\345\267\246\344\276\247\347\254\254\344\270\200\344\270\252\346\214\211\351\222\256\357\274\214\346\214\211\344\270\213\350\257\245\346\214\211\351\222\256\345\220\216\345\217\257\344\273\245\351\200\232\350\277\207\347\202\271\345\207\273\345\267\246\344\276\2472\343\200\2013\343\200\2014\346\214\211\351\222\256\346\230\276\347\244\272\344\270\215\345\220\214\345\206\205\345\256\271\357\274\211</span></p>\n"
""
                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">2. </span><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\350\275\254\344\270\272NFA</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\345\267\246\344\276\247\347\254\254\344\272\214\344\270\252\346\214\211\351\222\256\357\274\211</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">3. </span><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">NFA\350\275\254\344\270\272DFA</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\345\267\246\344\276\247\347\254\254\344\270\211\344\270\252\346\214\211\351\222\256\357\274\211</span></p>\n"
"<p style=\" margin-t"
                        "op:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">4. </span><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">DFA\346\234\200\345\260\217\345\214\226</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\345\267\246\344\276\247\347\254\254\345\233\233\344\270\252\346\214\211\351\222\256\357\274\211</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">5. </span><span style=\" font-family:'SimSun'; font-size:11pt; font-weight:600;\">\351\207\215\347\275\256</span><span style=\" font-family:'SimSun'; font-size:11pt;\">\357\274\210\345\217\257\344\273\245\351\207\215\346\226\260\350\276\223\345\205\245\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\357\274\211</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px;"
                        " margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">\347\202\271\345\207\273\345\210\206\346\236\220\346\214\211\351\222\256\357\274\214\345\256\236\347\216\260\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217-&gt;NFA-&gt;DFA-&gt;\346\234\200\345\260\217\345\214\226DFA</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">\346\263\250\357\274\232<br />1. \350\276\223\345\205\245\345\217\252\350\203\275\345\214\205\345\220\253\344\270\200\344\270\252\344\273\245&quot;_&quot;\345\274\200\345\244\264\347\232\204\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\357\274\214\345\220\246\345"
                        "\210\231\350\275\257\344\273\266\345\217\257\350\203\275\344\274\232\345\207\272\347\216\260\350\257\257\350\247\243\357\274\214\344\272\247\347\224\237\344\270\215\346\255\243\347\241\256\347\232\204NFA\343\200\201DFA\347\255\211\357\274\233<br />2. \347\255\211\345\217\267\345\267\246\344\276\247\347\232\204\346\240\207\350\257\206\347\254\246\344\273\205\345\214\205\345\220\253\345\255\227\346\257\215\343\200\201\346\225\260\345\255\227\344\270\216\344\270\213\345\210\222\347\272\277</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt;\">3. \350\275\257\344\273\266\344\273\245#\345\255\227\347\254\246\344\275\234\344\270\272NFA\350\275\254\346\215\242\347\232\204epsilon\346\240\207\350\257\206\357\274\214\345\233\240\346\255\244\357\274\214\345\270\214\346\234\233\350\276\223\345\205\245#\345\255\227\347\254\246\357\274\214\345\272\224\345\275\223\344\275\277\347\224\250\350"
                        "\275\254\344\271\211\347\254\246\345\217\267\357\274\214\350\276\223\345\205\245\\#\346\211\215\345\217\257\344\273\245\350\257\206\345\210\253\343\200\202</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:11pt; color:#aa0000;\">\350\276\203\351\225\277\347\232\204\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\345\217\257\350\203\275\344\274\232\345\210\206\346\236\220\350\276\203\344\271\205\357\274\214\350\257\267\346\202\250\350\200\220\345\277\203\347\255\211\345\276\205\357\274\214\350\260\242\350\260\242\343\200\202</span></p></body></html>", nullptr));
        title->setText(QApplication::translate("MainWindow", "Regex2DFA", nullptr));
        analysisPushButton->setText(QApplication::translate("MainWindow", "\345\210\206\346\236\220", nullptr));
        nfaPushButton->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272NFA", nullptr));
        dfaPushButton->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272DFA", nullptr));
        minimumDfaPushButton->setText(QApplication::translate("MainWindow", "\346\234\200\345\260\217\345\214\226DFA", nullptr));
        codePushButton->setText(QApplication::translate("MainWindow", "\347\224\237\346\210\220\345\214\271\351\205\215\344\273\243\347\240\201", nullptr));
        resetPushButton->setText(QApplication::translate("MainWindow", "\351\207\215\347\275\256\347\250\213\345\272\217", nullptr));
        label->setText(QApplication::translate("MainWindow", "\350\257\267\345\234\250\344\270\213\346\226\271\346\241\206\344\270\255\345\206\231\345\205\245\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        labelOfTable->setText(QApplication::translate("MainWindow", "NFA\343\200\201DFA\345\261\225\347\244\272\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
