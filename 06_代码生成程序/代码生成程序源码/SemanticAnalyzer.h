/*
    SemanticAnalyzer.h
    语义分析器 声明。
*/
#ifndef SEMANTIC_ANALYZER__H
#define SEMANTIC_ANALYZER__H

#include "SymbolTableManager.h"
#include "ErrorHandler.h"
#include "Quadruples.h"
#include "LexicalAnalyzer.h"

class SemanticAnalyzer {
public:
    // 单例模式
    static SemanticAnalyzer &initialSemanticAnalyzer(
        ErrorHandler &, SymbolTableManager &, Quadruples &, LexicalAnalyzer &);

    // 以下，提供的语义动作函数，供语法分析器调用

    // 以下，分别在每个语法成分分析开头和结尾使用，供 skip 错误处理使用
    void programEndmapInit();
    void blockEndmapInit();
    void blockEndmapClear();
    void constDeclareEndmapInit();
    void constDeclareEndmapClear();
    void constDefineEndmapInit();
    void constDefineEndmapClear();
    void charEndmapInit();
    void charEndmapClear();
    void stringEndmapInit();
    void stringEndmapClear();
    void varDeclareEndmapInit();
    void varDeclareEndmapClear();
    void varDefineEndmapInit();
    void varDefineEndmapClear();
    void procedureDeclareEndmapInit();
    void procedureDeclareEndmapClear();
    void functionDeclareEndmapInit();
    void functionDeclareEndmapClear();
    void procedureHeadEndmapInit();
    void procedureHeadEndmapClear();
    void functionHeadEndmapInit();
    void functionHeadEndmapClear();
    void formalParameterEndmapInit();
    void formalParameterEndmapClear();
    void statementEndmapInit();
    void statementEndmapClear();
    void termEndmapInit();
    void termEndmapClear();
    void factorEndmapInit();
    void factorEndmapClear();

    // 以下，对 skip 之后得到的符号，有的可能要忽略这个符号，在这里处理
    void constDeclareSkip();
    void charSkip();
    void stringSkip();

    // 其他语义相关操作
    void constDefine(const string &, BasicType, int value);
    string &stringDefine(const string &);
    void varDefine(vector<string> &, BasicType);
    void arrayDefine(vector<string> &, BasicType, int limit);
    void procedureDefine(const string &procname, vector<Argument> &args);
    void functionDefine(const string &funcname, vector<Argument> &args, BasicType type);
    void formalParameterAdd(vector<string> &names, BasicType type, bool vary, vector<Argument> &);
    void assignToArray(const string &arrname, const string &offset, const string &value,
                        BasicType arraytype, BasicType valuetype);
    void assign(const string &name, const string &value,
                        BasicType nametype, BasicType valuetype);
    void negative(const string &oper1, string &dest);
    void addition(const string &oper1, const string &oper2, string &dest);
    void subtraction(const string &oper1, const string &oper2, string &dest);
    void multiplicatioin(const string &oper1, const string &oper2, string &dest);
    void division(const string &oper1, const string &oper2, string &dest);
    void intToString(int n, string &str);
    void assignFromArray(const string &arrname, const string &offset, string &value);
    void callFunction(const string &name, string &value);
    void callProcedure(const string &name);
    void functionAssign(const string &name, const string &value, 
                        BasicType functype, BasicType valuetype);
    void label(const string &lab);
    void funcprocStart();
    void funcprocReturn();
    void push(const string &value);
    void passParameter(const string &value, BasicType formaltype, BasicType realtype);
    void compare(const string &value1, const string &value2);
    void dissatisfyJump(SymbolType op, const string &lab);
    void satisfyJump(SymbolType op, const string &lab);
    void jumpAnyway(string lab);
    string &newLabel();
    string &newLabel(const string &forehead);
    void readChar(const string &name);
    void readInteger(const string &name);
    void writeString(const string &name);
    void writeInteger(const string &value);
    void writeChar(const string &ch);
    void newline();

private:
    ErrorHandler &errorHandler;
    SymbolTableManager &table;
    Quadruples &quadruples;
    LexicalAnalyzer &lexicalAnalyzer;
    // 临时变量计数
    int temporaryCount;
    string newTemporary();
    // label 计数
    int labelCount;
    // 检查类型，必要时截断
    void cut(BasicType desttype, BasicType valuetype, const string &value);
    // 单例模式
    SemanticAnalyzer(ErrorHandler &, SymbolTableManager &, Quadruples &, LexicalAnalyzer &);
    SemanticAnalyzer(const SemanticAnalyzer &);
    SemanticAnalyzer &operator= (const SemanticAnalyzer &);
};

#endif // !SEMANTIC_ANALYZER__H
