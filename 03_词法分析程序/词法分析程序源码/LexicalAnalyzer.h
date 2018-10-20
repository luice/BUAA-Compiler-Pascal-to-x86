/*
    LexicalAnalyzer.h
    提供词法分析程序的 class 声明。
*/
#ifndef LEXICAL_ANALYSIS__H
#define LEXICAL_ANALYSIS__H

#include <fstream>
using std::string;
using std::ifstream;

// 新符号类型
enum SymbolType { 
    ILLEGAL,            // 非法字符
    FINISH,             // 文件结尾

    PROCEDURE,          // procedure 关键字
    FUNCTION,           // function 关键字
    CONST,              // const 关键字   
    VAR,                // var 关键字
    ARRAY,              // array 关键字
    OF,                 // of 关键字
    INTEGER,            // integer 关键字
    CHAR,               // char 关键字
    BEGIN,              // begin 关键字
    END,                // end 关键字
    FOR,                // for 关键字
    DO,                 // do 关键字
    WHILE,              // while 关键字
    IF,                 // if 关键字
    THEN,               // then 关键字
    ELSE,               // else 关键字
    READ,               // read 关键字
    WRITE,              // write 关键字
    
    IDENTIFIER,         // 标识符
    NUMBER,             // 整数   
    STRING,             // 字符串
    CHARATER,           // char 类型的字符

    LEFT_BRACKET,       // 左括号
    RIGHT_BRACKET,      // 右括号
    LEFT_SQUARE,        // 左方括号
    RIGHT_SQUARE,       // 右方括号
    SINGLE_QUOTE,       // 单引号                      
    DOUBLE_QUOTE,       // 双引号
    COMMA,              // 逗号
    SEMICOLON,          // 分号
    PERIOD,             // 句点
    PLUS,               // 加号
    MINUS,              // 减号
    ASTERISK,           // 星号，乘号
    SLASH,              // 斜杠，除号
    COLON,              // 冒号
    EQUAL,              // 等号
    ASSIGN,             // 赋值符
    LESS,               // 小于号
    LESS_EQUAL,         // 小于等于号
    GREATER,            // 大于号
    GREATER_EQUAL,      // 大于等于号
    INEQUAL             // 不等号
};

// 词法分析器
class LexicalAnalyzer {
public:
    static LexicalAnalyzer &initialLexicalAnalyzer(ifstream &file); 
                                    // 单例模式，原因是：
                                    //  token.symbol 依赖静态变量 strGot
                                    //  详见 LexicalAnalyzer.cpp
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
    SymbolType symbolType;          // 新符号的类型
    union Token {                   // 新符号的内容，使用联合类型是因为不同符号的类型不同
        int number;         // 整数型的整数值
        string *symbol;     // 字符串类型的字符串指针
    } token;                 
    friend class GrammarAnalyzer;   // 友元类：语法分析器
                                    //  语法分析器要直接获取新符号的类型和内容
                                        
    LexicalAnalyzer(ifstream &file);            // 私有化构造函数，单例模式
                                                //  参数为源文件的 ifstream 对象，
                                                //  这个对象必须是有效的
    LexicalAnalyzer(const LexicalAnalyzer &);   // 私有化拷贝构造，单例模式，禁止拷贝
    LexicalAnalyzer & operator = (const LexicalAnalyzer &);
                                                // 私有化重载的 = 运算符，禁止赋值
};

// 以下，为了【词法分析】阶段作业的输出更好看，加了一个编译开关 STAGE_LEXICAL
//  最终的编译器中，会取消对 STAGE_LEXICAL 的定义
#define STAGE_LEXICAL

#ifdef STAGE_LEXICAL
using std::ostream;
ostream &operator<< (ostream &ofs, SymbolType st);      // 重载 SymbolType 的 << 运算符
                                                        //  使得能输出枚举名而不是整数
#endif // STAGE_LEXICAL


#endif  // !LEXICAL_NANLYSIS__H