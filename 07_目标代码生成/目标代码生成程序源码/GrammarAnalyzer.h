/*
    GrammarAnalyzer.h
    提供语法分析程序的 calss 声明
*/
#ifndef GRAMMA_ANALYZER__H
#define GRAMMA_ANALYZER__H

#include <fstream>
#include "LexicalAnalyzer.h"
#include "SemanticAnalyzer.h"
using std::ofstream;

// 语法分析器
class GrammarAnalyzer {
private:
    // 单例模式
    GrammarAnalyzer(LexicalAnalyzer &, ErrorHandler &, 
                    SymbolTableManager &, SemanticAnalyzer &);
    GrammarAnalyzer(const GrammarAnalyzer &);
    GrammarAnalyzer &operator= (const GrammarAnalyzer &);
public:
    static GrammarAnalyzer &initialGrammarAnalyzer(LexicalAnalyzer &, ErrorHandler &, 
                                                SymbolTableManager &, SemanticAnalyzer &);

    void grammarAnalyze();

private:
    LexicalAnalyzer &lexicalAnalyzer;   // 词法分析器，语法分析使用
    SemanticAnalyzer &semanticAnalyzer; // 语义分析器
    ErrorHandler &errorHandler;         // 错误处理器
    SymbolTableManager &table;          // 符号表管理器

    // 以下，各语法成分的分析
    void blockAnalyze();
    void constDeclareAnalyze();
    void constDefineAnalyze();
    void constAnalyze(int &, BasicType &);
    void charAnalyze(int &);
    void stringAnalyze(string &);
    void unsignedAnalyze(int &);
    void varDeclareAnalyze();
    void varDefineAnalyze();
    void typeAnalyze(IdentifierType &, BasicType &, int &);
    void basicTypeAnalyze(BasicType &);
    void procedureDeclareAnalyze();
    void functionDeclareAnalyze();
    void procedureHeadAnalyze(string &);
    void functionHeadAnalyze(string &);
    void formalParameterListAnalyze(vector<Argument> &);
    void formalParameterAnalyze(vector<Argument> &);
    void statementAnalyze();
    void assignStatementAnalyze();
    void functionAssignStatementAnalyze(const string &name);
    void expressionAnalyze(string &value, BasicType &type, bool &variable, bool addr);
    void termAnalyze(string &value, BasicType &type, bool &variable, bool addr);
    void factorAnalyze(string &value, BasicType &type, bool &variable, bool addr);
    void callStatementAnalyze(string &value, BasicType &type);
    void realParameterListAnalyze(const string &name);
    void conditionAnalyze(SymbolType &relationOp);
    void relationOperatorAnalyze(SymbolType &relationOp);
    void conditionStatementAnalyze();
    void doWhileStatementAnalyze();
    void forStatementAnalyze();
    void compoundStatementAnalyze();
    void readStatementAnalyze();
    void writeStatementAnalyze();

};                

#endif // !GRAMMA_ANALYZER__H