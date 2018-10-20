/*
    GrammarAnalyzer.cpp
    语法分析器的实现
*/
#include "GrammarAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

                           
// 构造函数
#ifdef STAGE_LEXICAL
GrammarAnalyzer::GrammarAnalyzer(LexicalAnalyzer &aLexicalAnalyzer, ofstream &fout): 
    lexicalAnalyzer(aLexicalAnalyzer), resultFile(fout), symbolCount(0) {}
#elif defined (STAGE_GRAMMAR)
GrammarAnalyzer::GrammarAnalyzer(LexicalAnalyzer &la, ErrorHandler &eh, ofstream &ofs):
    lexicalAnalyzer(la), errorHandler(eh), resultFile(ofs), symbolCount(0) {}
#endif
  
#ifdef STAGE_LEXICAL
using namespace name_ST;

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
void GrammarAnalyzer::grammarAnalyze() {
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
    case(TO);
    case(DOWNTO);
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
                                           
#elif defined(STAGE_GRAMMAR)

// 向控制台和文件中输出语法成分信息
#define OUT(item) {                                     \
    cout << "This is a " << item << "!" << endl;        \
    resultFile << "This is a " << item << "!" << endl;  \
}

// 调试用
#define ERROR {                                         \
    cout << '\a' << "!!!!!!!" << endl;                  \
    resultFile << "!!!!!!!!!!!!!!!!!!!!!!" << endl;     \
}

// 语法分析函数。这是整个编译程序的核心函数！  
// 这里只分析语法，没有语义处理。
void GrammarAnalyzer::grammarAnalyze() {
    NEXTSYM;            // 读一个符号备用！！！！！！！
    // 分程序
    blockAnalyze();
    // 句号
    if (SYMBOL_TYPE != SymbolType::PERIOD) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    NEXTSYM;
    while (SYMBOL_TYPE != SymbolType::FINISH) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        NEXTSYM;
    }
    OUT("program");
}

#endif

// 用于检测里层语法成分的错误处理
static ErrorEnd err;

// 将 err 设为 NORMAL，即没有错误
#define CLEAR   err = ErrorEnd::NORMAL

// 测试 err 并做相应处理。定义这个宏为了代码复用
#define TEST(item) {                    \
    if (err != ErrorEnd::NORMAL) {      \
        if (err == item)                \
            CLEAR;                      \
        return;                         \
    }                                   \
}   

// <分程序> 分析
void GrammarAnalyzer::blockAnalyze() {
    // 常量说明部分
    if (SYMBOL_TYPE == SymbolType::CONST) {
        constDeclareAnalyze();
        TEST(ErrorEnd::BLOCK);
    }
    // 变量说明部分
    if (SYMBOL_TYPE == SymbolType::VAR) {
        varDeclareAnalyze();
        TEST(ErrorEnd::BLOCK);
    }
    // 过程、函数说明部分
    while (true) {
        switch (SYMBOL_TYPE) {
        case SymbolType::FUNCTION:
            functionDeclareAnalyze();
            TEST(ErrorEnd::BLOCK);
            break;
        case SymbolType::PROCEDURE:
            procedureDeclareAnalyze();
            TEST(ErrorEnd::BLOCK);
            break;
        default:
            goto END_DECLARE;
            break;
        }
    }
END_DECLARE:
    // 复合语句，先识别 BEGIN
    compoundStatementAnalyze();
    TEST(ErrorEnd::BLOCK);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("block");
#endif // STAGE_GRAMMAR      
}

// <常量说明部分> 分析
void GrammarAnalyzer::constDeclareAnalyze() {
    // const 关键字
    if (SYMBOL_TYPE != SymbolType::CONST) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 到少要有一个 <常量定义>
    constDefineAnalyze();
    TEST(ErrorEnd::CONST_DECLARE);
    // 对后续 <常量定义> 的处理
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        constDefineAnalyze();
        TEST(ErrorEnd::CONST_DECLARE);
    }
    // 识别结尾的分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("const declaration section");
#endif // STAGE_GRAMMAR      
}

// <常量定义> 分析
void GrammarAnalyzer::constDefineAnalyze() {
    // <标识符>
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
    // 等号
    if (SYMBOL_TYPE != SymbolType::EQUAL) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // <常量>
    constAnalyze();
    TEST(ErrorEnd::CONST_DEFINE);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("const defination");
#endif // STAGE_GRAMMAR
}

// <常量> 分析
void GrammarAnalyzer::constAnalyze() {
    switch (SYMBOL_TYPE) {
    // 正负号
    case SymbolType::PLUS: case SymbolType::MINUS:
        NEXTSYM;
        unsignedAnalyze();
        TEST(ErrorEnd::CONST);
        break;
    // 无符号整数
    case SymbolType::NUMBER:
        unsignedAnalyze();
        TEST(ErrorEnd::CONST);
        break;
    // 字符，先识别单引号
    case SymbolType::SINGLE_QUOTE:
        charAnalyze();
        TEST(ErrorEnd::CONST);
        break;
    // 语法错误
    default:
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        break;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("constant");
#endif // STAGE_GRAMMAR
}

// <字符> 分析
void GrammarAnalyzer::charAnalyze() {
    // 左单引号
    if (SYMBOL_TYPE != SymbolType::SINGLE_QUOTE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    // 默认现在有单引号，继续
    NEXTCH;
    NEXTSYM;
    // 识别单引号
    if (SYMBOL_TYPE != SymbolType::SINGLE_QUOTE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("charactor");
#endif // STAGE_GRAMMAR
}

// <字符串> 分析
void GrammarAnalyzer::stringAnalyze() {
    // 左双引号
    if (SYMBOL_TYPE != SymbolType::DOUBLE_QUOTE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    // 默认有双引号，继续
    NEXTSTR;
    NEXTSYM;
    // 识别双引号
    if (SYMBOL_TYPE != SymbolType::DOUBLE_QUOTE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("string");
#endif // STAGE_GRAMMAR               
}

// <无符号整数> 分析
void GrammarAnalyzer::unsignedAnalyze() {
    // 先判断是不是整数！！！！！！！！！！！！！！！！！！！
    if (SYMBOL_TYPE != SymbolType::NUMBER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        // 截断等处理      
        NEXTSYM;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("unsigned integer");
#endif // STAGE_GRAMMAR     
}

// <变量说明部分> 分析
void GrammarAnalyzer::varDeclareAnalyze() {
    // var 关键字
    if (SYMBOL_TYPE != SymbolType::VAR) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 第一个 <变量说明（定义）> 的处理
    varDefineAnalyze();
    TEST(ErrorEnd::VAR_DECLARE);
    // 识别分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 后面所有的 <变量说明> 及分号的处理
    while (SYMBOL_TYPE == SymbolType::IDENTIFIER) {
        varDefineAnalyze();
        TEST(ErrorEnd::VAR_DECLARE);
        if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("variable declaration section");
#endif // STAGE_GRAMMAR      
}

// <变量说明（定义）> 分析
void GrammarAnalyzer::varDefineAnalyze() { 
    // 处理第一个标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 处理以逗号分隔的多个标识符
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        if (SYMBOL_TYPE == SymbolType::IDENTIFIER) {
            NEXTSYM;
        } else {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        }
    }
    // 识别冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 默认有冒号，继续
    typeAnalyze();
    TEST(ErrorEnd::VAR_DEFINE);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("variable defination");
#endif // STAGE_GRAMMAR     
}

// <类型> 分析
void GrammarAnalyzer::typeAnalyze() {
    if (SYMBOL_TYPE == SymbolType::ARRAY) {     // 数组
        NEXTSYM;
        // 识别左方括号
        if (SYMBOL_TYPE != SymbolType::LEFT_SQUARE) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
        // 默认有左方括号，继续
        unsignedAnalyze();   
        TEST(ErrorEnd::TYPE);
        // 识别右括号
        if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
        // 默认有右括号，继续
        // 识别 of
        if (SYMBOL_TYPE != SymbolType::OF) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
        // 默认有 of，继续
        basicTypeAnalyze();
        TEST(ErrorEnd::TYPE);
    } else {                                    // 基本类型
        basicTypeAnalyze();
        TEST(ErrorEnd::TYPE);
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("type");
#endif // STAGE_GRAMMAR
}

// <基础类型> 分析
void GrammarAnalyzer::basicTypeAnalyze() {
    switch (SYMBOL_TYPE) {
    case SymbolType::INTEGER:
        NEXTSYM;
        break;
    case SymbolType::CHAR:
        NEXTSYM;
        break;
    default:
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        break;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("basic type");
#endif // STAGE_GRAMMAR

}

// <过程说明部分>
void GrammarAnalyzer::procedureDeclareAnalyze() {
    // 过程首部
    procedureHeadAnalyze();
    TEST(ErrorEnd::PROCEDURE_DECLARE);
    // 分程序
    blockAnalyze();
    TEST(ErrorEnd::PROCEDURE_DECLARE);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("procedure declaration section");
#endif // STAGE_GRAMMAR          
}

// <函数说明部分> 分析
void GrammarAnalyzer::functionDeclareAnalyze() {
    // 函数首部
    functionHeadAnalyze();
    TEST(ErrorEnd::FUNCTION_DECLARE);
    // 分程序
    blockAnalyze();
    TEST(ErrorEnd::FUNCTION_DECLARE);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;

#ifdef STAGE_GRAMMAR
    OUT("function declaration section");
#endif // STAGE_GRAMMAR      
}

// <过程首部> 分析
void GrammarAnalyzer::procedureHeadAnalyze() {
    // function 关键字
    if (SYMBOL_TYPE != SymbolType::PROCEDURE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
    // 形式参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        formalParameterListAnalyze();
        TEST(ErrorEnd::PROCEDURE_HEAD);
    }
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("procedure head");
#endif // STAGE_GRAMMMAR
}

// <函数首部> 分析
void GrammarAnalyzer::functionHeadAnalyze() {
    // function 关键字
    if (SYMBOL_TYPE != SymbolType::FUNCTION) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
    // 形式参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        formalParameterListAnalyze();
        TEST(ErrorEnd::FUNCTION_HEAD);
    }
    // 冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 基本类型
    basicTypeAnalyze();
    TEST(ErrorEnd::FUNCTION_HEAD);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("function head");
#endif // STAGE_GRAMMAR
}

// 形式参数表
void GrammarAnalyzer::formalParameterListAnalyze() {
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 第一个形式参数段
    formalParameterAnalyze();
    TEST(ErrorEnd::FORMAL_PARAMETER_LIST);
    // 后续的形式参数段
    while (SYMBOL_TYPE == SymbolType::SEMICOLON) {
        NEXTSYM;
        formalParameterAnalyze();
        TEST(ErrorEnd::FORMAL_PARAMETER_LIST);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) { 
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("formal parameter list");
#endif // STAGE_GRAMMAR          
}

// 形式参数段
void GrammarAnalyzer::formalParameterAnalyze() {
    // var 关键字
    if (SYMBOL_TYPE == SymbolType::VAR) {
        NEXTSYM;
    }
    // 第一个标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
    // 后续标识符
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~
        } else {
            NEXTSYM;
        }
    }
    // 冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 基本类型
    basicTypeAnalyze();
    TEST(ErrorEnd::FORMAL_PARAMETER);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("formal parameter section");
#endif // STAGE_GRAMMAR          
}

// 语句
void GrammarAnalyzer::statementAnalyze() {
    switch (SYMBOL_TYPE) { 
    // 赋值语句 或 过程调用语句
    case SymbolType::IDENTIFIER:
#ifdef STAGE_GRAMMAR
        NEXTSYM;
        switch (SYMBOL_TYPE) {
        // 赋值语句
        case SymbolType::ASSIGN:
        case SymbolType::LEFT_SQUARE:
            assignStatementAnalyze();
            TEST(ErrorEnd::STATEMENT);
            break;
        // 调用语句
        default:
            callStatementAnalyze();
            TEST(ErrorEnd::STATEMENT);
            break;
        }
#else
#endif // STAGE_GRAMMAR
        break;
    // 条件语句
    case SymbolType::IF:
        conditionStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // do-while 语句
    case SymbolType::DO:
        doWhileStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // 复合语句
    case SymbolType::BEGIN:
        compoundStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // 读语句
    case SymbolType::READ:
        readStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // 写语句
    case SymbolType::WRITE:
        writeStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // for 语句
    case SymbolType::FOR:
        forStatementAnalyze();
        TEST(ErrorEnd::STATEMENT);
        break;
    // 空语句
    default:
        break;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("statement");
#endif // STAGE_GRAMMAR

}

// 赋值语句
void GrammarAnalyzer::assignStatementAnalyze() {
#ifndef STAGE_GRAMMAR
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
#endif // !STAGE_GRAMMAR
    // 左方括号，对数组赋值
    if (SYMBOL_TYPE == SymbolType::LEFT_SQUARE) {
        NEXTSYM;
        // 表达式
        expressionAnalyze();
        TEST(ErrorEnd::ASSIGN_STATEMENT);
        // 右方括号
        if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
    }
    // 赋值符
    if (SYMBOL_TYPE != SymbolType::ASSIGN) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 表达式
    expressionAnalyze();
    TEST(ErrorEnd::ASSIGN_STATEMENT);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("assign statement");
#endif // STAGE_GRAMMAR
}

// 表达式
void GrammarAnalyzer::expressionAnalyze() {
    // 正负号
    switch (SYMBOL_TYPE) {
    case SymbolType::PLUS:
        NEXTSYM;
        break;
    case SymbolType::MINUS:
        NEXTSYM;
        break;
    default:
        break;
    }
    // 项
    termAnalyze();
    TEST(ErrorEnd::EXPRESSION);
    while (true) {
        switch (SYMBOL_TYPE) {
        // 加号
        case SymbolType::PLUS:
            NEXTSYM;
            break;
        // 减号
        case SymbolType::MINUS:
            NEXTSYM;
            break;
        default:
            goto END_EXPRESSION;
            break;
        }
        // 项
        termAnalyze();
        TEST(ErrorEnd::EXPRESSION);
    }
END_EXPRESSION:
    CLEAR;
#ifdef STAGE_GRAMMAR
    OUT("expression");
#endif // STAGE_GRAMMAR
}

// 项
void GrammarAnalyzer::termAnalyze() {
    // 因子
    factorAnalyze();
    TEST(ErrorEnd::TERM);
    while (true) {
        switch (SYMBOL_TYPE) {
        // 乘号
        case SymbolType::ASTERISK:
            NEXTSYM;
            break;
        case SymbolType::SLASH:
            NEXTSYM;
            break;
        default:
            goto END_TERM;
        }
        // 因子
        factorAnalyze();
        TEST(ErrorEnd::TERM);
    }
END_TERM:
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("term");
#endif // STAGE_GRAMMAR
}

// 因子
void GrammarAnalyzer::factorAnalyze() {
    switch (SYMBOL_TYPE) {
    // 标识符、数组元素、函数调用语句
    case SymbolType::IDENTIFIER:
#ifdef STAGE_GRAMMAR
        NEXTSYM;
        switch (SYMBOL_TYPE) {
        // 数组元素
        case SymbolType::LEFT_SQUARE:
            NEXTSYM;
            expressionAnalyze();
            TEST(ErrorEnd::FACTOR);
            // 右方括号
            if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
                ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~·
            } else
                NEXTSYM;
            break;
        case SymbolType::LEFT_BRACKET:
            callStatementAnalyze();
            TEST(ErrorEnd::FACTOR);
            break;
        default:
            break;
        }
#endif // STAGE_GRAMMAR
        break;
    case SymbolType::LEFT_BRACKET:
        NEXTSYM;
        // 表达式
        expressionAnalyze();
        TEST(ErrorEnd::FACTOR);
        if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
        break;
    case SymbolType::NUMBER:
        // 无符号整数
        unsignedAnalyze();
        TEST(ErrorEnd::FACTOR);
        break;
    default:
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
        break;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("factor");
#endif // STAGE_GRAMMAR
}

// 调用语句
void GrammarAnalyzer::callStatementAnalyze() {
#ifndef STAGE_GRAMMAR
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
#endif // !STAGE_GRAMMER
    // 实在参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        realParameterListAnalyze();
        TEST(ErrorEnd::CALL_STATEMENT);
    }
    CLEAR;
    
#ifdef STAGE_GRAMMAR
    OUT("call statement");
#endif // STAGE_GRAMMAR
}

// 实在参数表
void GrammarAnalyzer::realParameterListAnalyze() {
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 第一个实在参数
    expressionAnalyze();
    TEST(ErrorEnd::REAL_PARAMETER_LIST);
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        // 实在参数
        expressionAnalyze();
        TEST(ErrorEnd::REAL_PARAMETER_LIST);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("real parameter list");
#endif // STAGE_GRAMMAR
}

// 条件
void GrammarAnalyzer::conditionAnalyze() {
    // 表达式
    expressionAnalyze();
    TEST(ErrorEnd::CONDITION);
    // 关系运算符
    relationOperatorAnalyze();
    TEST(ErrorEnd::CONDITION);
    // 表达式
    expressionAnalyze();
    TEST(ErrorEnd::CONDITION);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("condition");
#endif // STAGE_GRAMMAR
}

// 关系运算符
void GrammarAnalyzer::relationOperatorAnalyze() {
    switch (SYMBOL_TYPE) {
    case SymbolType::LESS:
        NEXTSYM;
        break;
    case SymbolType::LESS_EQUAL:
        NEXTSYM;
        break;
    case SymbolType::GREATER:
        NEXTSYM;
        break;
    case SymbolType::GREATER_EQUAL:
        NEXTSYM;
        break;
    case SymbolType::EQUAL:
        NEXTSYM;
        break;
    case SymbolType::INEQUAL:
        NEXTSYM;
        break;
    default:
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
        break;
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("relation operator");
#endif // STAGE_GRAMMAR
}

// 条件语句
void GrammarAnalyzer::conditionStatementAnalyze() {
    // if 关键字
    if (SYMBOL_TYPE != SymbolType::IF) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~
    } else {
        NEXTSYM;
    }
    // 条件
    conditionAnalyze();
    TEST(ErrorEnd::CONDITION);
    // then 关键字
    if (SYMBOL_TYPE != SymbolType::THEN) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::CONDITION_STATEMENT);
    // else 关键字
    if (SYMBOL_TYPE == SymbolType::ELSE) {
        NEXTSYM;
        statementAnalyze();
        TEST(ErrorEnd::CONDITION_STATEMENT);
    }
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("condition statement");
#endif // STAGE_GRAMMAR
}

// do-while 语句
void GrammarAnalyzer::doWhileStatementAnalyze() {
    // do 关键字
    if (SYMBOL_TYPE != SymbolType::DO) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::DO_WHILE_STATEMENT);
    // whlie 关键字
    if (SYMBOL_TYPE != SymbolType::WHILE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 条件
    conditionAnalyze();
    TEST(ErrorEnd::DO_WHILE_STATEMENT);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("do-while statement");
#endif // STAGE_GRAMMAR
}

// for 语句
void GrammarAnalyzer::forStatementAnalyze() {
    // for 关键字
    if (SYMBOL_TYPE != SymbolType::FOR) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 赋值符
    if (SYMBOL_TYPE != SymbolType::ASSIGN) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 表达式
    expressionAnalyze();
    TEST(ErrorEnd::FOR_STATEMENT);
    // to | downto 关键字
    switch (SYMBOL_TYPE) {
    case SymbolType::TO:
        NEXTSYM;
        break;
    case SymbolType::DOWNTO:
        NEXTSYM;
        break;
    default:
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
        break;
    }
    // 表达式
    expressionAnalyze();
    TEST(ErrorEnd::FOR_STATEMENT);
    // do 关键字
    if (SYMBOL_TYPE != SymbolType::DO) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::FOR_STATEMENT);
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("for statement");
#endif // STAGE_GRAMMAR
}

// 复合语句
void GrammarAnalyzer::compoundStatementAnalyze() {
    // begin 关键字
    if (SYMBOL_TYPE != SymbolType::BEGIN) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::COMPOUND_STATEMENT);
    // 逗号，多个语句
    while (SYMBOL_TYPE == SymbolType::SEMICOLON) {
        NEXTSYM;
        statementAnalyze();
        TEST(ErrorEnd::COMPOUND_STATEMENT);
    }
    // end 关键字
    if (SYMBOL_TYPE != SymbolType::END) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("compound statement");
#endif // STAGE_GRAMMAR
}

// 读语句
void GrammarAnalyzer::readStatementAnalyze() {
    // read 关键字
    if (SYMBOL_TYPE != SymbolType::READ) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
            ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~
        } else
            NEXTSYM;
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("read statement");
#endif // STAGE_GRAMMAR
}

// 写语句
void GrammarAnalyzer::writeStatementAnalyze() {
    // write 关键字
    if (SYMBOL_TYPE != SymbolType::WRITE) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    // 内容
    if (SYMBOL_TYPE == SymbolType::DOUBLE_QUOTE) {
        // 字符串
        stringAnalyze();
        TEST(ErrorEnd::WRITE_STATEMENT);
        if (SYMBOL_TYPE == SymbolType::COMMA) {
            NEXTSYM;
            // 表达式
            expressionAnalyze();
            TEST(ErrorEnd::WRITE_STATEMENT);
        }
    } else {                                      
        // 表达式
        expressionAnalyze();
        TEST(ErrorEnd::WRITE_STATEMENT);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROR;// 报错~~~~~~~~~~~~~~~~~~~~~~~~
    } else
        NEXTSYM;
    CLEAR;

#ifdef STAGE_GRAMMAR
    OUT("write statement");
#endif // STAGE_GRAMMAR

}