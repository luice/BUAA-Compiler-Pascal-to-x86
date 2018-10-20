/*
    LexicalAnalyzer.h
    提供词法分析程序的 class 声明。
*/
#ifndef LEXICAL_ANALYSIS__H
#define LEXICAL_ANALYSIS__H

#include <fstream>
#include <string>
#include "type.h"
using std::string;
using std::ifstream;

// 词法分析器
class LexicalAnalyzer {
public:
    static LexicalAnalyzer &initialLexicalAnalyzer(ifstream &file); 
                                    // 单例模式，原因是：
                                    //  token.symbol 依赖静态变量 strGot
                                    //  详见 LexicalAnalyzer.cpp
    bool overflow;          // 溢出标记
    void nextSymbol();      // 获取下一个符号，将符号类型存入 symbolType，
                            //  具体的符号存入 symbol
    void nextChar();        // 获取下一个 char 类型的字符，
                            //  如果是合法的 char，将 symbolType 
                            //  置为 CHARATER，并将其 ASCII 值存入 token.number
                            //  如果不是合法的 char，将 symbolType 置为 ILLEGAL
    void nextString();      // 获取下一个字符串，不含双引号，遇到非法字符或双引号截止，
                            //  将截止之前的部分存入 token.symbol，可能为空串
                            //  将symbolType 置为 STRING

private:
    ifstream &sourceFile;           // 源程序文件输入流
    int lineNumber;                 // 新符号在源代码中的行号
    SymbolType symbolType;          // 新符号的类型
    union Token {                   // 新符号的内容，使用联合类型是因为不同符号的类型不同
        int number;         // 整数型的整数值
        string *symbol;     // 字符串类型的字符串指针
    } token;                 
    friend class GrammarAnalyzer;   // 友元类：语法分析器
                                    //  语法分析器要直接获取新符号的类型和内容
    friend class ErrorHandler;      // 友元类：错误处理器
                                    //  错误处理时 skip 操作需要获取新符号的类型
    friend class SemanticAnalyzer;  // 友元类：语义分析器
                                    //  语义分析器需要知道新符号的类型和内容
                                        
    LexicalAnalyzer(ifstream &file);            // 私有化构造函数，单例模式
                                                //  参数为源文件的 ifstream 对象，
                                                //  这个对象必须是有效的
    LexicalAnalyzer(const LexicalAnalyzer &);   // 私有化拷贝构造，单例模式，禁止拷贝
    LexicalAnalyzer &operator= (const LexicalAnalyzer &);
                                                // 私有化重载的 = 运算符，禁止赋值
};

#endif  // !LEXICAL_NANLYSIS__H