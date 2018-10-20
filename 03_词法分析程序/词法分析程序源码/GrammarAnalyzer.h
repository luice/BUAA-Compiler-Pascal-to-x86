/*
    GrammarAnalyzer.h
    提供语法分析程序的 calss 声明
*/
#ifndef GRAMMA_ANALYZER__H
#define GRAMMA_ANALYZER__H

#include <fstream>
#include "LexicalAnalyzer.h"
using std::ofstream;

// 语法分析器
class GrammarAnalyzer {

public:                   

#ifdef STAGE_LEXICAL  
    GrammarAnalyzer(LexicalAnalyzer &aLexicalAnalyzer, ofstream &fout);    
#endif // STAGE_LEXICAL

    void grammerAnalyze();

private:                                          
    int symbolCount;                    // 符号计数
    LexicalAnalyzer &lexicalAnalyzer;   // 词法分析器，语法分析使用      

#ifdef STAGE_LEXICAL
    ofstream &resultFile;
#endif // STAGE_LEXICAL

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