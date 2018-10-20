/*
    Quadruples.h
    结构化的四元式
    以链表方式组织
*/
#ifndef QUADRUPLES__H
#define QUADRUPLES__H

#include "type.h"
#include <fstream>
using std::ofstream;

struct Quadruple {
    Operator op;
    string oper1;
    string oper2;
    string dest;
    Quadruple(Operator ope, const string &o1, const string &o2, const string &d): 
        op(ope), oper1(o1), oper2(o2), dest(d), prenode(NULL), sucnode(NULL) {}
private:
    // 与链表结构相关的指针域
    Quadruple *prenode; // 前驱节点
    Quadruple *sucnode; // 后继节点
    // 友元类，可以操作链表结构
    friend class Quadruples;
};

class Quadruples {
public:
    void addTail(Quadruple *);      // 往结尾加一个四元式
    void output(ofstream &);        // 输出四元式到文件
    Quadruples();                   // 构造函数
    ~Quadruples();                  // 析构函数
    class iterator {
    private:
        Quadruple *point;
        iterator(Quadruple *p): point(p) {}
        friend class Quadruples;
    public:
        bool operator== (const iterator &b) const {
            return point == b.point;
        }
        bool operator!= (const iterator &b) const {
            return point != b.point;
        }
        iterator &operator++ (int i) {
            return operator++();
        }
        iterator &operator++ () {
            point = point->sucnode;
            return *this;
        }
        iterator &operator-- (int i) {
            return operator--();
        }
        iterator &operator-- () {
            point = point->prenode;
            return *this;
        }
        Quadruple &operator* () {
            return *point;
        }
        Quadruple *operator-> () {
            return point;
        }
    };
    iterator &begin() { 
        static iterator it(head);
        it.point = head->sucnode;
        return it;
    }
    iterator &end() {
        static iterator it(NULL);
        return it;
    }

private:
    Quadruple *head;    // 四元式链表的头节点
    Quadruple *tail;    // 四元式链表的尾节点
    friend class TargetCodeGenerator;
    // 禁用拷贝构造
    Quadruples(const Quadruples &);
};

#endif // !QUADRUPLES__H
