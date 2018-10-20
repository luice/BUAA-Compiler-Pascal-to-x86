/*
    GrammarAnalyzer.cpp
    语法分析器的实现
*/
#include "GrammarAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;


#ifdef STAGE_LEXICAL
// 构造函数
GrammarAnalyzer::GrammarAnalyzer(LexicalAnalyzer &aLexicalAnalyzer, ofstream &fout): 
    lexicalAnalyzer(aLexicalAnalyzer), resultFile(fout), symbolCount(0) {}
#endif // STAGE_LEXICAL
  
#ifdef STAGE_LEXICAL

#define OUT_BASE(width, content, tail) {    \
    cout << setw(width) << content << tail;         \
    resultFile << setw(width) << content << tail;   \
} 
#define OUT(content) {                      \
    OUT_BASE(3, symbolCount, " ");          \
    OUT_BASE(15, SYMBOL_TYPE, "    ");      \
    OUT_BASE(0, content, endl);             \
}                                           // 定义宏，简化此处往两个流中输出的操作

// 执行这个宏时，要保证当前符号已经处理完毕，还没读新的
// 当这个宏执行完毕后，目标符号已经处理完毕，还没读新的
#define SCAN_TO(type) {                     \
    do {           /* 先至少无条件读一个字符 */ \
        NEXTSYM;                            \
        if (SYMBOL_TYPE == NUMBER) {        \
            OUT(TOKEN_NUMBER);              \
        } else {                            \
            OUT(TOKEN_SYMBOL);              \
        }                                   \
    } while (SYMBOL_TYPE != type);          \
    /* 此时目标符号已经处理完毕 */              \
}
        

// 语法分析函数。这是整个编译程序的核心函数！  
// 这里，将词法分析的结果输出两份，屏幕上一份，文件中一份
void GrammarAnalyzer::grammerAnalyze() {
    NEXTSYM;                                        // 先读入一个备用
    while (SYMBOL_TYPE != FINISH) {  // 以文件结束符为程序结束标志
        switch (SYMBOL_TYPE) {                      // 判断符号的类型，决定输出什么
        case NUMBER:
            OUT(TOKEN_NUMBER);                      // 以整数形式解释 token
            NEXTSYM;                                // 不要忘了再读一个新的备用！
            break;                                              
        default:            
            OUT(TOKEN_SYMBOL);                      // 以字符串形式解释 token     
                                                    // 先把 token 输出，再判断它是什么
            // 此处不着急再读一个新字符，要根据读到的符号决定要读什么
            switch (SYMBOL_TYPE) {
            case SINGLE_QUOTE:
                NEXTCH;                             // 遇单引号，解释成字符，要再读一个字符
                OUT(TOKEN_SYMBOL);                  // 把读到的这个字符输出
                SCAN_TO(SINGLE_QUOTE);              // 扫描，直到遇到下一个单引号
                NEXTSYM;                            // 再读一个新符号备用
                break;
            case DOUBLE_QUOTE:
                NEXTSTR;                            // 遇双引号，要把下面的部分当成字符串来读
                OUT(TOKEN_SYMBOL);                  // 把读到的这个字符串输出
                SCAN_TO(DOUBLE_QUOTE);              // 扫描，直到遇到下一个双引号
                NEXTSYM;                            // 再读一个新符号备用
                break;
            default:                                      
                NEXTSYM;                            // 再读一个新的备用！千万不能忘！！！
                break;
            }
        }
    }  
}   

// 重载 SymbolType 的 << 运算符
ostream &operator<< (ostream &ofs, SymbolType st) {
    switch (st) {   

#define case(type) case type:       \
    ofs << #type;                   \
    break                           // 定义宏，简化 case 语句的输入
                   
    case(ILLEGAL);
    case(FINISH);
    case(PROCEDURE);
    case(FUNCTION);
    case(CONST);
    case(VAR);
    case(ARRAY);
    case(OF);
    case(INTEGER);
    case(CHAR);
    case(BEGIN);
    case(END);
    case(FOR);
    case(DO);
    case(WHILE);
    case(IF);
    case(THEN);
    case(ELSE);
    case(READ);
    case(WRITE);
    case(IDENTIFIER);
    case(NUMBER);
    case(STRING);
    case(CHARATER);
    case(LEFT_BRACKET);
    case(RIGHT_BRACKET);
    case(LEFT_SQUARE);
    case(RIGHT_SQUARE);
    case(SINGLE_QUOTE);
    case(DOUBLE_QUOTE);
    case(COMMA);
    case(SEMICOLON);
    case(PERIOD);
    case(PLUS);
    case(MINUS);
    case(ASTERISK);
    case(SLASH);
    case(COLON);
    case(EQUAL);
    case(ASSIGN);
    case(LESS);
    case(LESS_EQUAL);
    case(GREATER);
    case(GREATER_EQUAL);
    case(INEQUAL);                
#undef case                       // 取消定义 case，让出这个名字
    default:
        break;
    }
    return ofs;
}
                                           
#else  // !STAGE_LEXICAL
                                                 
// 语法分析函数。这是整个编译程序的核心函数！  
void GrammarAnalyzer::grammarAnalyze() {}

#endif // STAGE_LEXICAL