/*
    TargetCodeGenerator.h
    目标代码生成器
*/
#ifndef TARGET_CODE_GENERATOR__H
#define TARGET_CODE_GENERATOR__H

#include "SymbolTableManager.h"
#include "Quadruples.h"
#include "RegisterPool.h"
#include <fstream>
#include <string>
using std::ofstream;
using std::string;

class TargetCodeGenerator {
public:
    TargetCodeGenerator(ofstream &, Quadruples &, SymbolTableManager &);
    void generateCode();

private:
    ofstream &targetFile;
    StackManager stack;
    RegisterPool *registerPool;
    Quadruples &quadruples;
    SymbolTableManager &table;
    Quadruples::iterator qit;
    int level;
    int displayOffset;
    vector<Argument> *args;
    string procname;
    vector<string> parameters;

    /* 一些内部用的小函数 */
    // 生成每个文件的开头部分
    void printAssembleHead();
    // 处理四元式中的一个操作数（仅 变量、常量、临时变量），不要求是寄存器
    void getOper(const string &oldformat, string &newformat);
    // 处理四元式中的目的操作数，要求是寄存器
    void getDest(const string &oldformat, string &newformat, bool wantValue = false);
    // 除法指令
    void divInstruction();
    // 三指令四元式处理
    void threeOperatorInstruction(void (*func)(ofstream &, 
                                            const string &, const string &));
    // NEG 指令
    void negInstruction();
    // ASS 指令
    void assInstruction();
    // GA 指令
    void gaInstructioin();
    // ASFA 指令
    void asfaInstruction();
    // ASTA 指令
    void astaInstruction();
    // FAS 指令
    void fasInstruction();
    // CUT 指令
    void cutInstruction();
    // LAB 指令
    void labInstruction();
    // 处理过程/函数头： 输出标签，重置寄存器池和堆栈管理器，
    // 符号表联动！！！
    // 重置 level, args, displayOffset, procname.
    void handleHead();
    // 初始化 局部变量 和 临时变量
    void initialVariables();
    // CMP 指令
    void cmpInstruction();
    // 跳转指令
    void jumpInstruction(void (*func)(ofstream &, const string &));
    // 建立 display 区
    void buildDisplay(int destLevel);
    // 传递参数
    void passParameters();
    // CALP 指令
    void calpInstruction();
    // CALF 指令
    void calfInstruction();
    // RTN 指令，生成清理堆栈的代码
    void rtnInstruction();
    // 读写指令
    void readOrWrite(const string &functionname,
        const string &formatString, const string &content);
    void readOrWriteInstruction(const string &function, const string &formatString);
    // NL 指令
    void nlInstruction();
};

#endif // !TARGET_CODE_GENERATOR__H
