/*
    RegisterPool.cpp
    临时寄存器池的实现
*/

#include "RegisterPool.h"
#include "x86_instruction.h"
using namespace std;

RegisterPool::RegisterPool(const vector<string> &registers, StackManager &stack, 
                           ofstream &out): stack(stack), targetFile(out) {
    vector<string>::const_iterator vit = registers.begin();
    for ( ; vit != registers.end(); vit++)
        this->registers.insert(pair<string, string>(*vit, ""));
}

void RegisterPool::initialRegisterPool() {
    map<string, string>::iterator rit = registers.begin();
    while (rit != registers.end()) {
        rit->second = "";
        rit++;
    }
    temporaries.clear();
    registerit = registers.begin();
    lockedRegisters.clear();
}

void RegisterPool::home(const string &registerName) {
    map<string, string>::iterator rit = registers.find(registerName);
    if (rit == registers.end() || rit->second.empty())
        return;
    map<string, Temporary>::iterator tit = temporaries.find(rit->second);
    rit->second.clear();
    if (tit != temporaries.end()) {
        tit->second.registerName.clear();
        string reg = getRegister();
        string addr = stack.getAddress(reg, tit->second.offset, tit->second.baseoffset);
        MOV(targetFile, addr, registerName);
    }
}

void RegisterPool::home() {
    for (map<string, string>::iterator rit = registers.begin();
                                rit != registers.end(); rit++) {
        home(rit->first);
    }
}

string RegisterPool::getDestRegister(const string &temporary, bool wantValue) {
    string reg;
    map<string, Temporary>::const_iterator it = temporaries.find(temporary);
    if (it == temporaries.end())
        reg = getDestRegister(temporary, ++lastOffset, 0, wantValue);
    else
        reg = getDestRegister(temporary, it->second.offset, 0, wantValue);
    return reg;
}

string RegisterPool::getDestRegister(const string &variable, int offset,
                                     int baseoffset, bool wantValue) {
    string name;
    // 如果之前没有在这里做过记录，要做一个记录，与临时变量不同的是，
    // 这里不用在运行栈上分配新的空间。
    if (temporaries.count(variable) < 1) {
        Temporary temp = { offset, baseoffset, "" };
        temporaries.insert(pair<string, Temporary>(variable, temp));
    }
    map<string, Temporary>::iterator tit = temporaries.find(variable);    
    // 变量没有寄存器，要分配寄存器
    if (tit->second.registerName == "") {
        name = getRegister(variable);
        tit->second.registerName = name;
        if (wantValue)
            MOV(targetFile, name,
            stack.getAddress(name, tit->second.offset, tit->second.baseoffset));
    // 变量有寄存器，直接返回其寄存器名
    } else
        name = tit->second.registerName;
    return name;
}

string RegisterPool::getDest(const string &temporary) {
    string dest;
    map<string, Temporary>::iterator it = temporaries.find(temporary);
    if (it == temporaries.end())
        dest = getDest(temporary, ++lastOffset);
    else
        dest = getDest(temporary, it->second.offset);
    return dest;
}

string RegisterPool::getDest(const string &variable, int offset, int baseoffset) {
    string name;
    map<string, Temporary>::iterator tit = temporaries.find(variable);
    if (tit == temporaries.end() || tit->second.registerName.empty()) {
        Temporary temp = { offset, baseoffset, "" };
        temporaries.insert(pair<string, Temporary>(variable, temp));
        if (baseoffset != 0) {
            string reg = getRegister();
            name = stack.getAddress(reg, offset, baseoffset);
        } else
            name = stack.getAddress("", offset, baseoffset);
    } else
        name = tit->second.registerName;
    return name;
}

string RegisterPool::getRegister(const string &variable) {
    static set<string> tempregs;
    if (lockedRegisters.empty())
        tempregs.clear();
    string name;
    map<string, string>::iterator it = registers.begin();
    for ( ; it != registers.end(); it++)
        if (it->second.empty() && lockedRegisters.count(it->first) == 0
                                && tempregs.count(it->first) == 0)
            break;
    if (it != registers.end())
        registerit = it;
    while (true) {
        if (registerit == registers.end())
            registerit = registers.begin();
        else if (lockedRegisters.count(registerit->first) || 
                    tempregs.count(registerit->first))
            registerit++;
        else
            break;
    }
    name = registerit->first;
    if (!registerit->second.empty()) {
        home(registerit->first);
        registerit = registers.find(name);
    }
    registerit->second = variable;
    registerit++;
    if (registerit == registers.end())
        registerit = registers.begin();
    tempregs.insert(name);
    return name;
}

string RegisterPool::getRegister() {
    return getRegister("");
}

void RegisterPool::lock(const string &registerName) {
    lockedRegisters.insert(registerName);
}

void RegisterPool::unlock(const string &registerName) {
    lockedRegisters.erase(registerName);
}

void RegisterPool::setLastOffset(int offset) {
    lastOffset = offset;
}