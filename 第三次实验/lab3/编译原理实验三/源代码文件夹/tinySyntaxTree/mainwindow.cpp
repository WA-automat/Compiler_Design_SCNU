#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QRect>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , TOKEN(""), curType(6), curRow(0), curCol(0), tokenIndex(0), root(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 基本布局信息
    this->setWindowTitle("编译原理实验三：扩展Tiny语言的语法树生成");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int newWidth = screenWidth * 0.64;
    int newHeight = screenHeight * 0.48;
    this->resize(newWidth, newHeight);

    // 设置表格列
    ui->wordWidget->setColumnCount(2);
    ui->wordWidget->setHorizontalHeaderLabels(QStringList() << "token" << "类型");
    ui->wordWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自适应宽度
    ui->errorWidget->setColumnCount(3);
    ui->errorWidget->setHorizontalHeaderLabels(QStringList() << "错误说明" << "行" << "列");
    ui->errorWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自适应宽度
    ui->treeWidget->setColumnCount(1);

    // 上传文件
    connect(ui->uploadButton, &QPushButton::clicked, this, [&]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择Tiny源程序");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                in.setCodec("UTF-8");
                ui->textEdit->setText(in.readAll());
                file.close();
            }
        }
    });

    // 词法分析与语法分析
    connect(ui->syntaxButton, &QPushButton::clicked, this, [&]() {

        // 重置程序
        QString content = ui->textEdit->toPlainText();
        reset();
        ui->textEdit->setText(content);

        // 分词
        tokenList = lexer(content);

        // 分词展示
        ui->wordWidget->setRowCount(tokenList.size());
        ui->wordWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        for (int i = 0; i < tokenList.size(); i++) {
            ui->wordWidget->setItem(i, 0, new QTableWidgetItem(tokenList[i].first));
            ui->wordWidget->setItem(i, 1, new QTableWidgetItem(getTypeString(tokenList[i].second.lexerType)));
        }
        for (int row = 0; row < ui->wordWidget->rowCount(); ++row) {
            QTableWidgetItem *item = ui->wordWidget->item(row, 1);
            if (item) item->setTextAlignment(Qt::AlignCenter);
        }
        ui->wordWidget->show();

        // 去除所有注释token
        QVector<QPair<QString, LexerInfo>> tmpList = tokenList;
        tokenList.clear();
        for (const auto &pair: tmpList) {
            if (pair.second.lexerType != 6) {
                tokenList.append(pair);
            }
        }

        if (!tokenList.isEmpty()){

            // 语法树构造
            getToken();
            root = program();
            if (tokenIndex != tokenList.size() + 1) {
                ERROR(0);
            }

            // 错误展示
            ui->errorWidget->setRowCount(errorList.size());
            for (int i = 0; i < errorList.size(); i++) {
                ui->errorWidget->setItem(i, 0, new QTableWidgetItem(errorList[i].info));
                ui->errorWidget->setItem(i, 1, new QTableWidgetItem(QString::number(errorList[i].row)));
                ui->errorWidget->setItem(i, 2, new QTableWidgetItem(QString::number(errorList[i].column)));
            }
            for (int row = 0; row < ui->errorWidget->rowCount(); ++row) {
                QTableWidgetItem *item = ui->errorWidget->item(row, 0);
                if (item) item->setTextAlignment(Qt::AlignCenter);
                item = ui->errorWidget->item(row, 1);
                if (item) item->setTextAlignment(Qt::AlignCenter);
                item = ui->errorWidget->item(row, 2);
                if (item) item->setTextAlignment(Qt::AlignCenter);
            }

            // 语法树展示
            if (errorList.empty()) {
                ui->treeWidget->setHeaderLabels(QStringList()<<"语法树结点");
                QTreeWidgetItem* topItem = new QTreeWidgetItem(QStringList() << "start");
                ui->treeWidget->addTopLevelItem(topItem);
                if (!root->children.isEmpty()) {
                    showTree(topItem, root->children[0]);
                }
            } else {
                ui->treeWidget->setHeaderLabels(QStringList()<<"语法有误");
            }

        }
    });

    // 重置程序
    connect(ui->resetButton, &QPushButton::clicked, this, [&]() {
        reset();
    });

    // 保存源文件
    connect(ui->saveButton, &QPushButton::clicked, this, [=]() {
        QString fileName = QString("tiny") + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + QString(".txt");
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEdit->toPlainText();
            file.close();
            QMessageBox::information(this, "提示", "源文件保存为：" + fileName + "成功！", QMessageBox::Yes);
        } else {
            QMessageBox::warning(this, "提示", "源文件保存失败！", QMessageBox::Yes);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getToken()
{
    if (tokenIndex == tokenList.size()) {
        ++tokenIndex;
        TOKEN = "";
        curType = 6;
        return;
    } else if (tokenIndex == tokenList.size() + 1) {
        ERROR(2);
        return;
    }
    this->TOKEN = tokenList[tokenIndex].first;
    this->curRow = tokenList[tokenIndex].second.row;
    this->curCol = tokenList[tokenIndex].second.column;
    this->curType = tokenList[tokenIndex].second.lexerType;
    ++tokenIndex;
}

void MainWindow::match(QString expectedToken)
{
    if (TOKEN == expectedToken) getToken();
    else ERROR(0);
}

void MainWindow::match(int expectedTokenType)
{
    if (curType == expectedTokenType) getToken();
    else ERROR(1);
}

void MainWindow::ERROR(int errorType)
{
    switch (errorType) {
    case 0:
        errorList.append(ErrorInfo("意外的token：" + TOKEN + "(" + getTypeString(curType) + ")", curRow, curCol));
        break;
    case 1:
        errorList.append(ErrorInfo("意外的token类型：" + TOKEN + "(" + getTypeString(curType) + ")", curRow, curCol));
        break;
    case 2:
        errorList.append(ErrorInfo("多余的token", curRow, curCol));
        break;
    case 3:
        errorList.append(ErrorInfo("存在意外的语句类型", curRow, curCol));
        break;
    case 4:
        errorList.append(ErrorInfo("存在意外的赋值语句", curRow, curCol));
        break;
    default:
        errorList.append(ErrorInfo("未知错误", curRow, curCol));
        break;
    }
}

SyntaxTree MainWindow::program()
{
    SyntaxTree root = new SyntaxNode("start");
    root->children.append(stmt_sequence());
    return root;
}

SyntaxTree MainWindow::stmt_sequence()
{
    SyntaxTree tmp = statement();
    if (tmp == nullptr) return nullptr;
    while (TOKEN == ";") {
        match(";");
        tmp->brother.append(statement());
    }
    return tmp;
}

SyntaxTree MainWindow::statement()
{
    if (TOKEN == "if") {
        return if_stmt();
    } else if (TOKEN == "repeat") {
        return repeat_stmt();
    } else if (TOKEN == "for") {
        return for_stmt();
    } else if (TOKEN == "while") {
        return while_stmt();
    } else if (TOKEN == "read") {
        return read_stmt();
    } else if (TOKEN == "write") {
        return write_stmt();
    } else if (curType == 3) {      // 赋值语句：标识符
        return assign_stmt();
    } else {
        ERROR(0);
        ERROR(3);
        return nullptr;
    }
}

SyntaxTree MainWindow::if_stmt()
{
    match("if");
    SyntaxTree tmp = new SyntaxNode("if");
    match("(");
    tmp->children.append(exp());
    match(")");
    tmp->children.append(stmt_sequence());
    if (TOKEN == "else") {
        match("else");
        tmp->children.append(stmt_sequence());
    }
    match("endif");
    return tmp;
}

SyntaxTree MainWindow::repeat_stmt()
{
    match("repeat");
    SyntaxTree tmp = new SyntaxNode("repeat");
    tmp->children.append(stmt_sequence());
    match("until");
    tmp->children.append(exp());
    return tmp;
}

SyntaxTree MainWindow::for_stmt()
{
    match("for");
    SyntaxTree tmp = new SyntaxNode("for");
    match("(");
    tmp->children.append(assign_stmt());
    match(";");
    tmp->children.append(exp());
    match(";");
    SyntaxTree selfexpTree = selfexp();
    match(")");
//    qDebug() << TOKEN;
    tmp->children.append(stmt_sequence());
    tmp->children.append(selfexpTree);
    match("endfor");
    return tmp;
}

SyntaxTree MainWindow::while_stmt()
{
    match("while");
    SyntaxTree tmp = new SyntaxNode("while");
    match("(");
    tmp->children.append(exp());
    match(")");
    tmp->children.append(stmt_sequence());
    match("endwhile");
    return tmp;
}

SyntaxTree MainWindow::assign_stmt()
{
    QString tmpToken = TOKEN;
    match(3);   // 匹配标识符
    SyntaxTree equalTree = assign_sub_stmt();
    if (equalTree == nullptr) return nullptr;
    equalTree->children.push_front(new SyntaxNode("id(" + tmpToken + ")"));    // 赋值左子树为标识符，右子树为表达式
    return equalTree;
}

SyntaxTree MainWindow::read_stmt()
{
    match("read");
    SyntaxTree tmp = new SyntaxNode("read");
    QString tmpToken = TOKEN;
    match(3);   // 匹配标识符
    tmp->children.append(new SyntaxNode("id(" + tmpToken + ")"));
    return tmp;
}

SyntaxTree MainWindow::write_stmt()
{
    match("write");
    SyntaxTree tmp = new SyntaxNode("write");
    tmp->children.append(write_sub_stmt());
    return tmp;
}

SyntaxTree MainWindow::write_sub_stmt()
{
    if (curType == 1) { // 字符
        SyntaxTree tmp = new SyntaxNode("char(" + TOKEN + ")");
        match(1);
        return tmp;
    } else {
        return exp();
    }
}

SyntaxTree MainWindow::assign_sub_stmt()
{
    if (TOKEN == ":=") {
        SyntaxTree tmp = new SyntaxNode(":=");
        match(":=");
        tmp->children.append(exp());
        return tmp;
    } else if (TOKEN == "==") {
        SyntaxTree tmp = new SyntaxNode("==");
        match("==");
        tmp->children.append(re());
        return tmp;
    } else {
        ERROR(0);
        ERROR(4);
        return nullptr;
    }
}

SyntaxTree MainWindow::re()
{
    SyntaxTree tmp = orre();
    while (TOKEN == "|") {
        SyntaxTree newTmp = new SyntaxNode("|");
        match("|");
        newTmp->children.append(tmp);
        newTmp->children.append(orre());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::orre()
{
    SyntaxTree tmp = conre();
    while (TOKEN == "&") {
        SyntaxTree newTmp = new SyntaxNode("&");
        match("&");
        newTmp->children.append(tmp);
        newTmp->children.append(conre());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::conre()
{
    SyntaxTree tmp = repre();
    if (TOKEN == "#" || TOKEN == "?") {
        SyntaxTree newTmp = new SyntaxNode(TOKEN);
        match(TOKEN);       // 捕获闭包符号或可选符号
        newTmp->children.append(tmp);
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::repre()
{
    if (TOKEN == "(") {
        match("(");
        SyntaxTree tmp = re();
        match(")");
        return tmp;
    } else if (curType == 1) {      // 字符
        SyntaxTree tmp = new SyntaxNode("char(" + TOKEN + ")");
        match(1);
        return tmp;
    } else if (curType == 3) {      // 标识符
        SyntaxTree tmp = new SyntaxNode("id(" + TOKEN + ")");
        match(3);
        return tmp;
    } else {
        ERROR(0);
        ERROR(1);
        return nullptr;
    }
}

SyntaxTree MainWindow::exp()
{
    SyntaxTree tmp = simple_exp();
    if (TOKEN == "<" || TOKEN == ">" || TOKEN == "=" || TOKEN == ">=" || TOKEN == "<=" || TOKEN == "<>") {
        SyntaxTree newTmp = new SyntaxNode(TOKEN);
        match(TOKEN);
        newTmp->children.append(tmp);
        newTmp->children.append(simple_exp());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::simple_exp()
{
    SyntaxTree tmp = term();
    while (TOKEN == "+" || TOKEN == "-") {
        SyntaxTree newTmp = new SyntaxNode(TOKEN);
        match(TOKEN);
        newTmp->children.append(tmp);
        newTmp->children.append(term());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::term()
{
    SyntaxTree tmp = factor();
    while (TOKEN == "*" || TOKEN == "/" || TOKEN == "%") {
        SyntaxTree newTmp = new SyntaxNode(TOKEN);
        match(TOKEN);
        newTmp->children.append(tmp);
        newTmp->children.append(factor());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::factor()
{
    SyntaxTree tmp = resexp();
    while (TOKEN == "^") {
        SyntaxTree newTmp = new SyntaxNode(TOKEN);
        match(TOKEN);
        newTmp->children.append(tmp);
        newTmp->children.append(resexp());
        tmp = newTmp;
    }
    return tmp;
}

SyntaxTree MainWindow::selfexp()
{
    if (TOKEN == "++" || TOKEN == "--") {
        QString tmpToken = TOKEN;
        match(TOKEN);
        if (curType == 3) { // 标识符
            SyntaxTree tmp = new SyntaxNode(tmpToken);
            tmp->children.append(new SyntaxNode("id(" + TOKEN + ")"));
            match(3);
            return tmp;
        } else {
            ERROR(1);
            return nullptr;
        }
    } else {
        ERROR(0);
        return nullptr;
    }
}

SyntaxTree MainWindow::resexp()
{
    if (TOKEN == "(") {
        match("(");
        SyntaxTree tmp = exp();
        match(")");
        return tmp;
    } else if (curType == 2) {      // 数字
        SyntaxTree newTmp = new SyntaxNode("number(" + TOKEN + ")");
        match(2);
        return newTmp;
    } else if (curType == 3) {      // 标识符
        SyntaxTree newTmp = new SyntaxNode("id(" + TOKEN + ")");
        match(3);
        return newTmp;
    } else if (TOKEN == "++" || TOKEN == "--") {
        return selfexp();
    } else {
        ERROR(0);
        ERROR(1);
        return nullptr;
    }
}

void MainWindow::reset()
{

    ui->textEdit->clear();
    ui->errorWidget->clear();
    ui->treeWidget->clear();
    ui->wordWidget->clear();

    tokenList.clear();
    errorList.clear();

    tokenIndex = 0;
    TOKEN = "";
    curRow = 0;
    curCol = 0;
    curType = 6;

    // 清除语法树
    remove(root);

    // 设置表格列
    ui->wordWidget->setColumnCount(2);
    ui->wordWidget->setHorizontalHeaderLabels(QStringList() << "token" << "类型");
    ui->wordWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自适应宽度
    ui->errorWidget->setColumnCount(3);
    ui->errorWidget->setHorizontalHeaderLabels(QStringList() << "错误说明" << "行" << "列");
    ui->errorWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自适应宽度
    ui->treeWidget->setColumnCount(1);
}

void MainWindow::remove(SyntaxTree tr)
{
    if (tr == nullptr) return;
    for (SyntaxTree brotherTr: qAsConst(tr->brother)) {
        remove(brotherTr);
    }
    for (SyntaxTree childTr: qAsConst(tr->children)) {
        remove(childTr);
    }
    delete tr;
    tr = nullptr;
}

void MainWindow::showTree(QTreeWidgetItem *item, SyntaxTree tr)
{
    if (tr == nullptr) return;
    QTreeWidgetItem *newItem = new QTreeWidgetItem(QStringList() << tr->nodeStr);
    item->addChild(newItem);
    for (SyntaxTree childTr :qAsConst(tr->children)) {
        showTree(newItem, childTr);
    }
    for (SyntaxTree brotherTr: qAsConst(tr->brother)) {
        showTree(item, brotherTr);
    }
}
