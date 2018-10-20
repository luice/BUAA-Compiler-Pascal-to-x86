/*
    StackManager.h
    运行栈管理器
*/

#include "x86_instruction.h"
#include "StackManager.h"

StackManager::StackManager(ofstream &file): targetFile(file) {}

void StackManager::initialStack() {
    pushCount = 0;
}

void StackManager::push(const string &value) {
    PUSH(targetFile, value);
    pushCount++;
}

void StackManager::pop(const string &registerName) {
    POP(targetFile, registerName);
    --pushCount;
}

void StackManager::allocN(int n) {
    SUB(targetFile, "esp", intToString(n << 2));
    pushCount += n;
}

void StackManager::freeN(int n) {
    ADD(targetFile, "esp", intToString(n << 2));
    pushCount -= n;
}

int StackManager::getOffset() {
    return pushCount;
}

string StackManager::getAddress(const string &dest, int offset, int baseoffset) {
    string addr;
    if (baseoffset == 0)
        if (offset >= 0)
            addr = "[ebp-" + intToString(offset << 2) + "]";
        else
            addr = "[ebp+" + intToString((1-offset) << 2) + "]";
    else {
        MOV(targetFile, dest, "[ebp+" + intToString((1-baseoffset) << 2) + "]");
        if (offset >= 0)
            addr = "["+dest+"-"+intToString(offset << 2)+"]";
        else
            addr = "["+dest+"+"+intToString((1-offset) << 2)+"]";
    }
    return addr;
}

void StackManager::getLocationOnStack(const string &dest, int offset, int baseoffset) {
    if (baseoffset == 0) {
        MOV(targetFile, dest, "ebp");
        if (offset > 0)
            SUB(targetFile, dest, intToString(offset << 2));
        else if (offset < 0)
            ADD(targetFile, dest, intToString((1-offset) << 2));
    } else {
        MOV(targetFile, dest, "[ebp+"+intToString((1-baseoffset) << 2)+"]");
        if (offset > 0)
            SUB(targetFile, dest, intToString(offset << 2));
        else if (offset < 0)
            ADD(targetFile, dest, intToString((1-offset) << 2));
    }
}