#include "utils.h"

int getPriority(const char &op) // 返回运算符优先级
{
    switch (op) {
    case '(':
    case ')':
        return 0;
    case '*':
        return 4;
    case '|':
        return 1;
    case '.':
        return 2;
    default:
        return -1;
    }
}

bool isOperator(const char &ch)
{
    switch (ch) {
    case '(':
    case ')':
    case '*':
    case '|':
    case '.':
        return true;        // 运算符返回true
    default:
        return false;       // 非运算符返回false
    }
}
