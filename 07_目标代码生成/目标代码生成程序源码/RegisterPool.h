/*
    RegisterPool.h
    临时寄存器池 声明
*/

#ifndef REGISTER_POOL__H
#define REGISTER_POOL__H

#include "StackManager.h"
#include <vector>
#include <map>
#include <fstream>
#include <set>
using std::set;
using std::vector;
using std::map;
using std::ofstream;

// 变量的信息
struct Temporary {
    int offset;             // 在其所在运行栈上的基址。
    int baseoffset;         // 所在运行栈的基址。
                            // 如果是当前层，为 0；如果是外层，为一个负数
    string registerName;    // 分配到的寄存器名，没有则为空
};

class RegisterPool {
public:

    // 构造函数。第一个参数为临时寄存器的个数，后面为临时寄存器的名称
    RegisterPool(const vector<string> &registers, StackManager &stack, ofstream &out);
    // 每进入一个新的过程或函数，都要调用这个函数！
    void initialRegisterPool();
    // 让 regist 寄存器回家（放回栈上）
    void home(const string &registerName);
    // 让所有寄存器回家（放回栈上）
    void home();
    /* 参数为一个临时变量，返回它所在的寄存器名。
        如果这个临时变量在栈上，则给它分配一个寄存器
        如果所有寄存器中都有有效值，则让某个临时变量回家，然后把腾出来的寄存器给要的临时变量
        如果已给临时变量分配栈空间，没有问题
        如果还没有给这个临时变量分配栈空间，则会给它分配空间，这时会改变 pushCount 的值 */
    // 要求要的值在寄存器里
    string getDestRegister(const string &temporary, bool wantValue = false);
    string getDestRegister(const string &variable, int offset,
                            int baseoffset = 0, bool wantValue = false);
    // 不要求变量的值在寄存器里，可以在栈上
    string getDest(const string &temporary);
    string getDest(const string &variable, int offset, int baseoffset = 0);
    // 获取一个空的寄存器，不与变量相关联
    string getRegister();
    // 锁定，锁定后的寄存器不参与寄存器池的调度
    void lock(const string &registerName);
    void unlock(const string &registerName);
    // 设置 lastOffset 的值
    void setLastOffset(int);

private:
    // 记录临时变量分配空间之前的偏移，由于是先真正分配空间，
    // 到用时再“假”分配，所以要记录它们的偏移
    int lastOffset;
    map<string, string>::iterator registerit;
    StackManager &stack;    // 运行栈管理器
    ofstream &targetFile;   // 目标代码文件
    map<string, Temporary> temporaries; // 临时变量信息表，变量名称对应 Temporary 结构
    map<string, string> registers;      // 临时寄存器表，寄存器名对应变量名
    set<string> lockedRegisters;        // 锁定的寄存器，这些寄存器不参与寄存器池的调度
    // 取一个空的寄存器，与变量（已能够确定在表中）关联
    // 只关联一半，未改 temporaries 表
    string getRegister(const string &variable);
};

#endif // !REGISTER_POOL__H
