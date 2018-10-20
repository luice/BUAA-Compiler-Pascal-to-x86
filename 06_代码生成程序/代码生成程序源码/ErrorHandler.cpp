/*
    ErrorHandler.cpp
    错误处理器的实现
*/                        
#include "ErrorHandler.h"
#include <iostream>
#include <string>
using namespace std;

#define TOKEN_SYMBOL    (*lexicalAnalyzer.token.symbol)     // string 类型的 token
#define TOKEN_NUMBER    (lexicalAnalyzer.token.number)      // int 类型的 token
#define SYMBOL_TYPE     (lexicalAnalyzer.symbolType)        // 符号的 SymbotType 类型值
                                            // 定义宏，简化对符号和类型获取    

#define NEXTSYM lexicalAnalyzer.nextSymbol()                // 定义宏，简化获取下一个符号的操作

// 单例模式
ErrorHandler &ErrorHandler::initialErrorHandler(LexicalAnalyzer &la, bool warning) {
    static ErrorHandler theErrorHandler(la, warning);
    return theErrorHandler;
}

static map<ErrorType, string> errout;       // 错误类型与错误输出对应表

// 初始化 errout 表
static void initialErrout() {
#define INSERT(type, msg)   \
    errout.insert(pair<ErrorType, string>(ErrorType::type, msg))
    // 初始化错误类型表
    INSERT(NO_PERIOD, "period expected");
    INSERT(ILLEGAL_CHAR, "illegal symbol");
    INSERT(NO_FINISH, "file should be end");
    INSERT(FINISH, "unexpected end of file");
    INSERT(NO_CONST, "const keyword expected");
    INSERT(NO_SEMICOLON, "semilcolon expected");
    INSERT(NO_IDENTIFIER, "identifier expected");
    INSERT(NO_EQUAL, "equality expected");
    INSERT(NO_CHNUM, "number or charactor expected");
    INSERT(NO_SINGLEQUOTE, "single quote expected");
    INSERT(NO_DOUBLEQUOTE, "double quote expected");
    INSERT(ILLEGAL_STRING, "illegal symbol in string");
    INSERT(NO_NUMBER, "number expected");
    INSERT(INT_OVERFLOW, "integer input is too long, it will be cut off");
    INSERT(NO_VAR, "var keyword expected");
    INSERT(NO_COLON, "colon expected");
    INSERT(NO_LEFTSQUARE, "symbol [ expected");
    INSERT(NO_RIGHTSQUARE, "symbol ] expected");
    INSERT(NO_OF, "of keyword expected");
    INSERT(NO_BASICTYPE, "basic type (integer or char) expected");
    INSERT(NO_PROCEDURE, "procedure keyword expected");
    INSERT(NO_FUNCTION, "function keyword expected");
    INSERT(NO_LEFTBRACKET, "symbol ( expected");
    INSERT(NO_RIGHTBRACKET, "symbol ) expected");
    INSERT(CONSTANT, "unexpected constant identifier");
    INSERT(NO_ASSIGN, "symbol = expected. Here is ecpected to be an assigning statement");
    INSERT(NO_VALUE, "value expected");
    INSERT(UNDEFINED, "undefined identifier");
    INSERT(CANNOT_CALL, "identifier is not a name of function or procedure");
    INSERT(INT_TO_CHAR, "assign a integer to a char. Maybe cut off");
    INSERT(TOO_LESS_ARG, "too less real parameter");
    INSERT(TOO_MANY_ARG, "too many real parameter");
    INSERT(UNVARIABLE, "real parameter is not variable");
    INSERT(NO_RELATION, "relation operator expected");
    INSERT(NO_IF, "if keyword expected");
    INSERT(NO_THEN, "then keyword expected");
    INSERT(NO_ELSE, "else keyword expected");
    INSERT(NO_DO, "do keyword expected");
    INSERT(NO_WHILE, "while keyword expected");
    INSERT(NO_FOR, "for keyword expected");
    INSERT(NO_DOWN_TO, "downto or to keyword expected");
    INSERT(NO_BEGIN, "begin keyword expected");
    INSERT(NO_END, "end keyword expected");
    INSERT(NO_READ, "read keyword expected");
    INSERT(NO_WRITE, "write keyword expected");
#undef INSERT
}

// 警告
void ErrorHandler::printWarning(ErrorType err) {
    if (warning) {
        warningCount++;
        cerr << "Warn " << warningCount << ": ";
        cerr << "line " << lexicalAnalyzer.lineNumber << ": warning W" << err << ": ";
        cerr << errout.find(err)->second << endl;
    }
}

// 警告附加信息
void ErrorHandler::printWarning(ErrorType err, const string &msg) {
    if (warning) {
        warningCount++;
        cerr << "Warn " << warningCount << ": ";
        cerr << "line " << lexicalAnalyzer.lineNumber << ": warning W" << err << ": ";
        cerr << errout.find(err)->second << ". (" << msg << ")" << endl;
    }
}

// 构造函数
ErrorHandler::ErrorHandler(LexicalAnalyzer &la, bool warning): 
                            lexicalAnalyzer(la), warning(warning) {
    errorCount = 0;
    warningCount = 0;
    initialErrout();
}

// 输出错误信息
void ErrorHandler::printError(ErrorType err) {
    errorCount++;
    cerr << "Err " << errorCount << ": ";
    cerr << "line " << lexicalAnalyzer.lineNumber << ": error E" << err << ": ";
    cerr << errout.find(err)->second << endl;
}

// 输出错误信息（带附加信息）
void ErrorHandler::printError(ErrorType err, const string &msg) {
    errorCount++;
    cerr << "Err " << errorCount << ": ";
    cerr << "line " << lexicalAnalyzer.lineNumber << ": error E" << err << ": ";
    cerr << errout.find(err)->second << ". (" << msg << ")" << endl;
}

// 跳到给定的符号之一，并返回相应类型
ErrorEnd ErrorHandler::skip() {
    map<SymbolType, ErrorEnd>::iterator it = endmap.find(SYMBOL_TYPE);
    while (it == endmap.end()) {
        NEXTSYM;
        it = endmap.find(SYMBOL_TYPE);
    }
    return it->second;
}

// 往 endmap 里加一对，如果有重复，覆盖
void ErrorHandler::addEndmap(SymbolType st, ErrorEnd ee) {
    map<SymbolType, ErrorEnd>::iterator it = endmap.find(st);
    if (it == endmap.end())
        endmap.insert(pair<SymbolType, ErrorEnd>(st, ee));
    else
        it->second = ee;
}

// 从 endmap 里移除 SymbolType，如果没有，无操作
void ErrorHandler::removeEndmap(SymbolType st) {
    endmap.erase(st);
}