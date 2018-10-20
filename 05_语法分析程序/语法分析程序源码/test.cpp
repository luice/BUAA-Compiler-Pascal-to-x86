/*
    test.cpp
    开发过程中有很多关于 C++ 语言的细节需要在尝试中学习，
    这些尝试在本文件中做。
    在正式版本中，不包含这个文件。
*/
#include "type.h"
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    vector<Argument> args;
    Argument a = {"12345t", BasicType::INTEGER, false};
    ProcedureProperty pp = {args};
    args.push_back(a);
    cout << pp.arguments.size() << endl;
    a.name = "str";
    a.type = BasicType::CHAR;
    a.vary = false;
    cout << args[0].name << endl;

    return 0;
}