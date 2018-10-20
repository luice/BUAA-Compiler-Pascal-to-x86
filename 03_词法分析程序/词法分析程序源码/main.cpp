#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "LexicalAnalyzer.h"
#include "GrammarAnalyzer.h"
using namespace std;      

int main(int argc, char *argv[]) {
    // 以下，对源程序文件的打开过程
    string fname;           // 源程序文件的名称
    if (argc > 1)           // 命令行参数方式输入源程序文件名
        fname = argv[1];
    else                    // 控制台输方式输入源程序文件名
        cin >> fname;
    ifstream fin(fname);    // 打开源程序文件
    if (!fin.is_open()) {   // 检查文件是否成功打开，如否，报错并退出程序
        cerr << "err: cannot open file: " << fname << endl;
        cerr << "Please check the path and file name." << endl;
        return 0;
    }
    // 以下，对结果输出文件的打开过程
    ofstream fout;
    if (argc > 2)           // 命令行参数输入结果文件名
        fout.open(argv[2]);
    else                    // 默认结果文件名
        fout.open(fname + "_result.txt");

    LexicalAnalyzer &lexicalAnalyzer = LexicalAnalyzer::initialLexicalAnalyzer(fin);
#ifdef STAGE_LEXICAL
    GrammarAnalyzer grammaAnalyzer(lexicalAnalyzer, fout);
#endif // STAGE_LEXICAL
    grammaAnalyzer.grammerAnalyze();
    fout.close();           // 关闭结果文件
    fin.close();            // 关闭源程序文件
    return 0;
}