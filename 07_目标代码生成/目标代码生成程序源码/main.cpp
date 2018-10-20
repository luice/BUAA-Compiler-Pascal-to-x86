#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "GrammarAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "TargetCodeGenerator.h"
using namespace std;      

#define WARNING     0X1
#define OPTIMIZE_1  0X2
#define OPTIMIZE_2  0x4

// 与命令行参数有关的，包括一些编译选项，输入输出文件名等
struct Options {
    int flags;
    ifstream sourceCode;
    ofstream intermediateCode;
    ofstream assemblyCode;
};

// 处理命令行参数
bool handleOptions(int argc, char *argv[], Options &opt) {
    int p = 1;      // 命令行参数指针
    opt.flags = 0x7;
    if (argc < 4)
        return false;
    else if (argc >= 5 && argv[1][0] == '-') {
        p = 2;
        char *options = argv[1] + 1;
        while (*options != '\0') {
            switch (*options) {
            case 'd':
                options++;
                switch (*options) {
                case 'w':
                    opt.flags &= ~WARNING;
                    break;
                case 'o':
                    opt.flags &= ~OPTIMIZE_1;
                    break;
                case 'O':
                    opt.flags &= ~OPTIMIZE_2;
                    break;
                default:
                    return false;
                    break;
                }
                break;
            case 'w':
                opt.flags |= WARNING;
                break;
            case 'o':
                opt.flags |= OPTIMIZE_1;
                break;
            case 'O':
                opt.flags |= OPTIMIZE_2;
                break;
            default:
                return false;
                break;
            }
            options++;
        }
    }
    opt.sourceCode.open(argv[p++]);
    opt.intermediateCode.open(argv[p++]);
    opt.assemblyCode.open(argv[p]);
    return true;
}

// 关闭文件
void closeFiles(Options &opt) {
    opt.sourceCode.close();
    opt.intermediateCode.close();
    opt.assemblyCode.close();
}

// 用法
void usage(char *order) {
    cerr << "usage: \n\t" << order << " ";
    cerr << "[- [[d]w] [[d]o] [[d]O] ] ";
    cerr << "<sourceFile> <intermediateFile> <assemblyFile>" << endl;
}

int main(int argc, char *argv[]) {
    // 对命令行参数的处理过程
    Options opt;
    if (!handleOptions(argc, argv, opt)) {
        usage(argv[0]);
        return 0;
    }

    // 各大组件的初始化
    // 词法分析器
    LexicalAnalyzer &lexicalAnalyzer = LexicalAnalyzer::initialLexicalAnalyzer(opt.sourceCode);
    // 错误处理器
    ErrorHandler &errorHandler = ErrorHandler::initialErrorHandler(
        lexicalAnalyzer, (opt.flags & WARNING)!=0);
    // 符号表管理器
    SymbolTableManager &symbolTableManager = SymbolTableManager::initialSymbolTableManager();
    // 结构化的四元式
    Quadruples quadruples;
    // 语义分析器
    SemanticAnalyzer &semanticAnalyzer = SemanticAnalyzer::initialSemanticAnalyzer(
        errorHandler, symbolTableManager, quadruples, lexicalAnalyzer);
    // 语法分析器
    GrammarAnalyzer &grammarAnalyzer = GrammarAnalyzer::initialGrammarAnalyzer(
        lexicalAnalyzer, errorHandler, symbolTableManager, semanticAnalyzer);
    // 目标代码生成器
    TargetCodeGenerator targetCodeGenerator(opt.assemblyCode, quadruples, symbolTableManager);

    // 分析
    grammarAnalyzer.grammarAnalyze();

    // 检查出错、警告次数，如果出错，不生成代码
    if (errorHandler.warningCount != 0)
        cerr << endl << errorHandler.warningCount << " WARNING in total" << endl;
    cerr << endl << errorHandler.errorCount << " ERROR in total" << endl;
    if (errorHandler.errorCount != 0)
        goto END;

    // 输出四元式
    quadruples.output(opt.intermediateCode);

    // 生成目标代码
    targetCodeGenerator.generateCode();

END:
    closeFiles(opt);
    return 0;
}