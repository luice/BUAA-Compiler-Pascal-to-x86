/*
    Quadruples.cpp
    四元式实现
*/
#include "Quadruples.h"
using namespace std;

Quadruples::Quadruples() {
    head = new Quadruple(Operator::LAB, "Hello", "", "");
    tail = head;
}

Quadruples::~Quadruples() {
    while (head != NULL) {
        tail = head->sucnode;
        delete head;
        head = tail;
    }
}

ostream &operator<< (ostream &os, Operator op) {
#define case(op) case Operator::op: \
    os << #op; break
    switch (op) {
    case(ADD);    // 加法
    case(SUB);    // 减法
    case(MUL);    // 乘法
    case(DIV);    // 除法
    case(NEG);    // 求负
    case(ASS);    // 赋值
    case(GA);     // 获取地址
    case(ASFA);   // 从地址中取东西，给别的东西赋值
    case(ASTA);   // 给地址中的东西赋值
    case(FAS);    // 给函数赋返回值
    case(CUT);    // 将整数截断成字符型
    case(LAB);    // 标签
    case(CMP);    // 比较
    case(JG);     // 大于跳转
    case(JGE);    // 大于或等于跳转
    case(JL);     // 小于跳转
    case(JLE);    // 小于或等于跳转
    case(JE);     // 等于跳转
    case(JNE);    // 不等于跳转
    case(JMP);    // 无条件跳转
    case(CALP);   // 过程调用，不带返回值
    case(CALF);   // 函数调用，带返回值
    case(RTN);    // 函数或过程返回
    case(PUSH);   // 压栈
    case(RI);     // 读整数
    case(RC);     // 读字符
    case(WS);     // 写字符串
    case(WI);     // 写整数
    case(WC);     // 写字符
    case(NL);     // 换行
    default:
        break;
    }
    return os;
#undef case
}

void Quadruples::addTail(Quadruple *quadruple) {
    quadruple->prenode = tail;
    quadruple->sucnode = tail->sucnode;
    tail->sucnode = quadruple;
    tail = quadruple;
/////////////////////////////////////
//    static ofstream ofs("fuck~~~~~~~~~~~~~~~~~~~~.txt");
//    Quadruple *pq = quadruple;
//    ofs << pq->op << " ";
//    bool out = false;
//    if (!pq->oper1.empty()) {
//        ofs << pq->oper1;
//        out = true;
//    }
//    if (!pq->oper2.empty()) {
//        ofs << (out ? ", " : "");
//        ofs << pq->oper2;
//        out = true;
//    }
//    if (!pq->dest.empty()) {
//        ofs << (out ? ", " : "");
//        ofs << pq->dest;
//    }
//    ofs << endl;
/////////////////////////////////////
}

void Quadruples::output(ofstream &ofs) {
    Quadruple *pq;
    for (pq = head->sucnode; pq != NULL; pq = pq->sucnode) {
        ofs << pq->op << " ";
        bool out = false;
        if (!pq->oper1.empty()) {
            ofs << pq->oper1;
            out = true;
        }
        if (!pq->oper2.empty()) {
            ofs << (out ? ", " : "");
            ofs << pq->oper2;
            out = true;
        }
        if (!pq->dest.empty()) {
            ofs << (out ? ", " : "");
            ofs << pq->dest;
        }
        ofs << endl;
    }
}