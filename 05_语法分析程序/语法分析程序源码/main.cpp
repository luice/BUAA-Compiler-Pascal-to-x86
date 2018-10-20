#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "GrammarAnalyzer.h"
using namespace std;      

// 与命令行参数有关的，包括一些编译选项，输入输出文件名等
struct Options {
    ifstream sourceCode;
    ofstream intermediateCode;
    ofstream assemblyCode;
};

// 处理命令行参数
bool handleOptions(int argc, char *argv[], Options &opt) {
    if (argc <= 1)
        return false;
    opt.sourceCode.open(argv[1]);
    opt.intermediateCode.open("result.txt");
    return true;
}

// 关闭文件
void closeFiles(Options &opt) {
    opt.sourceCode.close();
    opt.intermediateCode.close();
    opt.assemblyCode.close();
}

int main(int argc, char *argv[]) {
    // 对命令行参数的处理过程
    Options opt;
    handleOptions(argc, argv, opt);

    LexicalAnalyzer &lexicalAnalyzer = LexicalAnalyzer::initialLexicalAnalyzer(opt.sourceCode);
    ErrorHandler &errorHandler = ErrorHandler();
#ifdef STAGE_LEXICAL
    GrammarAnalyzer grammarAnalyzer(lexicalAnalyzer, opt.intermediateCode);
#elif defined(STAGE_GRAMMAR) 
    GrammarAnalyzer grammarAnalyzer(lexicalAnalyzer, errorHandler, opt.intermediateCode);
#endif 
    grammarAnalyzer.grammarAnalyze();

    closeFiles(opt);
    return 0;
}