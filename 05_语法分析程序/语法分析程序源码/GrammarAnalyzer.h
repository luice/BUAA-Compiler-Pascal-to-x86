/*
    GrammarAnalyzer.h
    提供语法分析程序的 calss 声明
*/
#ifndef GRAMMA_ANALYZER__H
#define GRAMMA_ANALYZER__H

// 截止到 “语法分析” 作业，要实现的东西在这里。以后还可能会改。
#define STAGE_GRAMMAR
////////////////////////////////////////////////////

#include <fstream>
#include "LexicalAnalyzer.h"
#include "ErrorHandler.h"
using std::ofstream;

// 语法分析器
class GrammarAnalyzer {

public:                   

#ifdef STAGE_LEXICAL
    GrammarAnalyzer(LexicalAnalyzer &aLexicalAnalyzer, ofstream &fout);    
#elif defined(STAGE_GRAMMAR)
    GrammarAnalyzer(LexicalAnalyzer &, ErrorHandler &, ofstream &);
#endif // STAGE_LEXICAL

    void grammarAnalyze();

private:                                          
    int symbolCount;                    // 符号计数
    LexicalAnalyzer &lexicalAnalyzer;   // 词法分析器，语法分析使用
#ifndef STAGE_LEXICAL
    ErrorHandler &errorHandler;         // 错误处理器
#endif // !STAGE_LEXICAL

#if defined(STAGE_LEXICAL) || defined(STAGE_GRAMMAR)
    ofstream &resultFile;                               
#endif // STAGE_LEXICAL || STAGE_GRAMMAR

    // 以下，各语法成分的分析
    void blockAnalyze();
    void constDeclareAnalyze();
    void constDefineAnalyze();
    void constAnalyze();
    void charAnalyze();
    void stringAnalyze();
    void unsignedAnalyze();
    void varDeclareAnalyze();
    void varDefineAnalyze();
    void typeAnalyze();
    void basicTypeAnalyze();
    void procedureDeclareAnalyze();
    void functionDeclareAnalyze();
    void procedureHeadAnalyze();
    void functionHeadAnalyze();
    void formalParameterListAnalyze();
    void formalParameterAnalyze();
    void statementAnalyze();
    void assignStatementAnalyze();
    void expressionAnalyze();
    void termAnalyze();
    void factorAnalyze();
    void callStatementAnalyze();
    void realParameterListAnalyze();
    void conditionAnalyze();
    void relationOperatorAnalyze();
    void conditionStatementAnalyze();
    void doWhileStatementAnalyze();
    void forStatementAnalyze();
    void compoundStatementAnalyze();
    void readStatementAnalyze();
    void writeStatementAnalyze();

};                
                                                    
#define TOKEN_SYMBOL    (*lexicalAnalyzer.token.symbol)     // string 类型的 token
#define TOKEN_NUMBER    (lexicalAnalyzer.token.number)      // int 类型的 token
#define SYMBOL_TYPE     (lexicalAnalyzer.symbolType)        // 符号的 SymbotType 类型值
                                            // 定义宏，简化对符号和类型获取    

#define NEXTSYM {                           \
    lexicalAnalyzer.nextSymbol();           \
    symbolCount++;                          \
}
#define NEXTSTR {                           \
    lexicalAnalyzer.nextString();           \
    symbolCount++;                          \
}
#define NEXTCH {                            \
    lexicalAnalyzer.nextChar();             \
    symbolCount++;                          \
}                                           // 定义宏，简化获取下一个符号的操作

#endif // !GRAMMA_ANALYZER__H