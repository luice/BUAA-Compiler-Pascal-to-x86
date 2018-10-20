/*
    ErrorHandler.h
    定义 “错误处理器” 类及其相关的宏
*/

#ifndef ERROR_HANDLER__H
#define ERROR_HANDLER__H

#include "LexicalAnalyzer.h"
#include <map>
using std::map;

class ErrorHandler {
public:
    // 错误计数
    int errorCount;
    int warningCount;
    // skip 操作可结束时的 SymbolType 及返回值对照表
    map<SymbolType, ErrorEnd> endmap;
    void addEndmap(SymbolType, ErrorEnd);   // 往 endmap 里加一对，如果有重复，覆盖
    void removeEndmap(SymbolType);          // 从 endmap 里移除 SymbolType，如果没有，无操作
    // 单例模式
    static ErrorHandler &initialErrorHandler(LexicalAnalyzer &, bool warning);
    // 错误处理函数
    void printError(ErrorType);
    void printError(ErrorType, const string &);
    ErrorEnd skip();
    // 警告函数
    void ErrorHandler::printWarning(ErrorType err);
    void ErrorHandler::printWarning(ErrorType err, const string &msg);

private:
    // 词法分析器
    LexicalAnalyzer &lexicalAnalyzer;
    // 警告开关
    bool warning;
    // 单例模式
    ErrorHandler(LexicalAnalyzer &, bool warning);
    ErrorHandler(const ErrorHandler &);
    ErrorHandler &operator= (const ErrorHandler &);
};

#endif // !ERROR_HANDLER__H