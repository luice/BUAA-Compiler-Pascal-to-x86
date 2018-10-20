/*
    GrammarAnalyzer.cpp
    语法分析器的实现
*/
#include "GrammarAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#define TOKEN_SYMBOL    (*lexicalAnalyzer.token.symbol)     // string 类型的 token
#define TOKEN_NUMBER    (lexicalAnalyzer.token.number)      // int 类型的 token
#define SYMBOL_TYPE     (lexicalAnalyzer.symbolType)        // 符号的 SymbotType 类型值
                                            // 定义宏，简化对符号和类型获取    

#define NEXTSYM lexicalAnalyzer.nextSymbol()

#define NEXTSTR lexicalAnalyzer.nextString()

#define NEXTCH lexicalAnalyzer.nextChar()
                                            // 定义宏，简化获取下一个符号的操作

// 用于检测里层语法成分的错误处理
static ErrorEnd err;

// 错误信息输出
#define ERROUT  errorHandler.printError
#define WARNOUT errorHandler.printWarning

// 错误处理，跳过一些字符
#define SKIP    err = errorHandler.skip()

// 将 err 设为 NORMAL，即没有错误
#define CLEAR   err = ErrorEnd::NORMAL

// 测试 err 并做相应处理。定义这个宏为了代码复用
#define TEST(item) {                    \
    if (err != ErrorEnd::NORMAL) {      \
        if (err == item)                \
            CLEAR;                      \
        goto END;                         \
    }                                   \
}
                           
// 构造函数
// 单例模式
GrammarAnalyzer::GrammarAnalyzer(LexicalAnalyzer &la, ErrorHandler &eh, 
                                 SymbolTableManager &stm, SemanticAnalyzer &sa):
    lexicalAnalyzer(la), errorHandler(eh), table(stm), semanticAnalyzer(sa) {}
GrammarAnalyzer &GrammarAnalyzer::initialGrammarAnalyzer(LexicalAnalyzer &la,
                                                         ErrorHandler &eh,
                                                         SymbolTableManager &stm,
                                                         SemanticAnalyzer &sa) {
    static GrammarAnalyzer theGrammarAnalyzer(la, eh, stm, sa);
    return theGrammarAnalyzer;
}

// 程序 分析
// 这是整个分析过程的入口
void GrammarAnalyzer::grammarAnalyze() {
    NEXTSYM;            // 读一个符号备用！！！！！！！
    semanticAnalyzer.programEndmapInit();
    // 分程序
    blockAnalyze();
    TEST(ErrorEnd::PROGRAM);
    // 句号
    if (SYMBOL_TYPE != SymbolType::PERIOD) {
        ERROUT(ErrorType::NO_PERIOD);
    }
    NEXTSYM;
    if (SYMBOL_TYPE != SymbolType::FINISH)
        ERROUT(ErrorType::NO_FINISH);
END:
    ;
}

// <分程序> 分析
void GrammarAnalyzer::blockAnalyze() {
    semanticAnalyzer.blockEndmapInit();
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
    // 复合语句
    semanticAnalyzer.funcprocStart();   // 函数/过程开始
    compoundStatementAnalyze();
    semanticAnalyzer.funcprocReturn();  // 函数/过程返回
    TEST(ErrorEnd::BLOCK);
    CLEAR;
END:
    semanticAnalyzer.blockEndmapClear();
}

// <常量说明部分> 分析
void GrammarAnalyzer::constDeclareAnalyze() {
    semanticAnalyzer.constDeclareEndmapInit();
    // const 关键字
    if (SYMBOL_TYPE != SymbolType::CONST) {
        ERROUT(ErrorType::NO_CONST);
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
        ERROUT(ErrorType::NO_SEMICOLON, "The former statement missed a semicolon");
        SKIP;
        semanticAnalyzer.constDeclareSkip();
        TEST(ErrorEnd::CONST_DECLARE);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.constDeclareEndmapClear();
}

// <常量定义> 分析
void GrammarAnalyzer::constDefineAnalyze() {
    semanticAnalyzer.constDeclareEndmapInit();
    // <标识符>
    string id;      // 标识符
    BasicType type; // 类型，整数 or 字符
    int value = 0;  // 常量的值，默认为 0
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::CONST_DEFINE);
    } else if (table.isHere(TOKEN_SYMBOL)) {
        ERROUT(ErrorType::REDEFINE, TOKEN_SYMBOL);
        SKIP;
        TEST(ErrorEnd::CONST_DEFINE);
    } else {
        id = TOKEN_SYMBOL;
        NEXTSYM;
    }
    // 等号
    if (SYMBOL_TYPE != SymbolType::EQUAL) {
        ERROUT(ErrorType::NO_EQUAL);
    } else
        NEXTSYM;
    // 默认此处有等号，继续
    // <常量>
    constAnalyze(value, type);
    TEST(ErrorEnd::CONST_DEFINE);
    // 没有错误，填表
    semanticAnalyzer.constDefine(id, type, value);
    CLEAR;
END:
    semanticAnalyzer.constDeclareEndmapClear();
}

// <常量> 分析
void GrammarAnalyzer::constAnalyze(int &value, BasicType &type) {
    switch (SYMBOL_TYPE) {
    // 正负号
    case SymbolType::PLUS: case SymbolType::MINUS:
        type = BasicType::INTEGER;
        NEXTSYM;
        unsignedAnalyze(value);
        TEST(ErrorEnd::CONST);
        break;
    // 无符号整数
    case SymbolType::NUMBER:
        type = BasicType::INTEGER;
        unsignedAnalyze(value);
        TEST(ErrorEnd::CONST);
        break;
    // 字符，先识别单引号
    case SymbolType::SINGLE_QUOTE:
        type = BasicType::CHAR;
        charAnalyze(value);
        TEST(ErrorEnd::CONST);
        break;
    // 语法错误
    default:
        ERROUT(ErrorType::NO_CHNUM);
        break;
    }
    CLEAR;
END:
    ;
}

// <字符> 分析
void GrammarAnalyzer::charAnalyze(int &value) {
    semanticAnalyzer.charEndmapInit();
    // 左单引号
    if (SYMBOL_TYPE != SymbolType::SINGLE_QUOTE) {
        ERROUT(ErrorType::NO_SINGLEQUOTE);
    }
    // 默认现在有单引号，继续
    NEXTCH;
    if (SYMBOL_TYPE != SymbolType::CHARATER) {
        ERROUT(ErrorType::ILLEGAL_CHAR, TOKEN_SYMBOL);
        SKIP;
        semanticAnalyzer.charSkip();
        TEST(ErrorEnd::CHAR);
    } else {
        value = TOKEN_NUMBER;
        NEXTSYM;
    }
    // 识别单引号
    if (SYMBOL_TYPE != SymbolType::SINGLE_QUOTE) {
        ERROUT(ErrorType::NO_SINGLEQUOTE);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.charEndmapClear();
}

// <字符串> 分析
void GrammarAnalyzer::stringAnalyze(string &strname) {
    semanticAnalyzer.stringEndmapInit();
    // 左双引号
    if (SYMBOL_TYPE != SymbolType::DOUBLE_QUOTE) {
        ERROUT(ErrorType::NO_DOUBLEQUOTE);
    }
    // 默认有双引号，继续
    NEXTSTR;
    if (SYMBOL_TYPE == SymbolType::STRING) {
        strname = semanticAnalyzer.stringDefine(TOKEN_SYMBOL);
        NEXTSYM;
    } else {
        ERROUT(ErrorType::ILLEGAL_STRING, TOKEN_SYMBOL);
        SKIP;
        semanticAnalyzer.stringSkip();
        TEST(ErrorEnd::STRING);
    }
    // 识别双引号
    if (SYMBOL_TYPE != SymbolType::DOUBLE_QUOTE) {
        ERROUT(ErrorType::NO_DOUBLEQUOTE);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.stringEndmapClear();
}

// <无符号整数> 分析
void GrammarAnalyzer::unsignedAnalyze(int &value) {
    // 先判断是不是整数！！！！！！！！！！！！！！！！！！！
    if (SYMBOL_TYPE != SymbolType::NUMBER) {
        ERROUT(ErrorType::NO_NUMBER);
    } else {
        value = TOKEN_NUMBER;
        if (lexicalAnalyzer.overflow) {
            WARNOUT(ErrorType::INT_OVERFLOW, stringPlusInt("New value is ", value));
        }
        NEXTSYM;
    }
    CLEAR;    
}

// <变量说明部分> 分析
void GrammarAnalyzer::varDeclareAnalyze() {
    semanticAnalyzer.varDeclareEndmapInit();
    // var 关键字
    if (SYMBOL_TYPE != SymbolType::VAR) {
        ERROUT(ErrorType::NO_VAR);
    } else
        NEXTSYM;
    // 第一个 <变量说明（定义）> 的处理
    varDefineAnalyze();
    TEST(ErrorEnd::VAR_DECLARE);
    // 识别分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROUT(ErrorType::NO_SEMICOLON, "former var declaration missed a semicolon");
    } else
        NEXTSYM;
    // 后面所有的 <变量说明> 及分号的处理
    while (SYMBOL_TYPE == SymbolType::IDENTIFIER) {
        varDefineAnalyze();
        TEST(ErrorEnd::VAR_DECLARE);
        if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
            ERROUT(ErrorType::NO_SEMICOLON, "former var declaration missed a semicolon");
        } else
            NEXTSYM;
    }
    CLEAR;
END:
    semanticAnalyzer.varDeclareEndmapClear();
}

// <变量说明（定义）> 分析
void GrammarAnalyzer::varDefineAnalyze() { 
    semanticAnalyzer.varDefineEndmapInit();
    vector<string> names;                           // 标识符名字，说明中可能有多个标识符，
                                                    //  要先存起来，一起填表
    IdentifierType idType = IdentifierType::VAR;    // 标识符类型：变量/数组
    BasicType bType;                                // 基本类型：整型/字符型
    int limit;                                      //  数组容量限
    // 处理第一个标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::VAR_DEFINE);
    } else {
        // 检查是否重定义
        if (table.isHere(TOKEN_SYMBOL)) {
            ERROUT(ErrorType::REDEFINE, TOKEN_SYMBOL);
        } else
            names.push_back(TOKEN_SYMBOL);
        NEXTSYM;
    }
    // 处理以逗号分隔的多个标识符
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        if (SYMBOL_TYPE == SymbolType::IDENTIFIER) {
            // 检查是否重定义
            if (table.isHere(TOKEN_SYMBOL))
                ERROUT(ErrorType::REDEFINE, TOKEN_SYMBOL);
            else
                names.push_back(TOKEN_SYMBOL);
            NEXTSYM;
        } else {
            ERROUT(ErrorType::NO_IDENTIFIER);
            SKIP;
            TEST(ErrorEnd::VAR_DEFINE);
        }
    }
    // 识别冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROUT(ErrorType::NO_COLON);
    } else
        NEXTSYM;
    // 默认有冒号，继续
    typeAnalyze(idType, bType, limit);
    TEST(ErrorEnd::VAR_DEFINE);
    // 填符号表
    if (idType == IdentifierType::VAR)
        semanticAnalyzer.varDefine(names, bType);
    else
        semanticAnalyzer.arrayDefine(names, bType, limit);
    CLEAR;
END:
    semanticAnalyzer.varDefineEndmapClear();
}

// <类型> 分析
void GrammarAnalyzer::typeAnalyze(IdentifierType &idType, BasicType &bType, int &lim) {
    if (SYMBOL_TYPE == SymbolType::ARRAY) {     // 数组
        idType = IdentifierType::ARRAY;
        NEXTSYM;
        // 识别左方括号
        if (SYMBOL_TYPE != SymbolType::LEFT_SQUARE) {
            ERROUT(ErrorType::NO_LEFTSQUARE);
        } else
            NEXTSYM;
        // 默认有左方括号，继续
        unsignedAnalyze(lim);   
        TEST(ErrorEnd::TYPE);
        // 识别右括号
        if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
            ERROUT(ErrorType::NO_RIGHTSQUARE);
        } else
            NEXTSYM;
        // 默认有右括号，继续
        // 识别 of
        if (SYMBOL_TYPE != SymbolType::OF) {
            ERROUT(ErrorType::NO_OF);
        } else
            NEXTSYM;
        // 默认有 of，继续
        basicTypeAnalyze(bType);
        TEST(ErrorEnd::TYPE);
    } else {                                    // 基本类型
        basicTypeAnalyze(bType);
        TEST(ErrorEnd::TYPE);
    }
    CLEAR;
END:
    ;
}

// <基础类型> 分析
void GrammarAnalyzer::basicTypeAnalyze(BasicType &bType) {
    switch (SYMBOL_TYPE) {
    case SymbolType::INTEGER:
        NEXTSYM;
        break;
    case SymbolType::CHAR:
        NEXTSYM;
        break;
    default:
        ERROUT(ErrorType::NO_BASICTYPE);
        SKIP;
        return;
        break;
    }
    CLEAR;
}

// <过程说明部分>
void GrammarAnalyzer::procedureDeclareAnalyze() {
    semanticAnalyzer.procedureDeclareEndmapInit();
    string procname;
    // 过程首部
    procedureHeadAnalyze(procname);
    TEST(ErrorEnd::PROCEDURE_DECLARE);
    table.goIn(procname);   // 符号表联动，往里深入一层
    // 分程序
    blockAnalyze();
    table.goOut();          // 符号表联动，出来一层
    TEST(ErrorEnd::PROCEDURE_DECLARE);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROUT(ErrorType::NO_SEMICOLON);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.procedureDeclareEndmapClear();
}

// <函数说明部分> 分析
void GrammarAnalyzer::functionDeclareAnalyze() {
    semanticAnalyzer.functionDeclareEndmapInit();
    string funcname;
    // 函数首部
    functionHeadAnalyze(funcname);
    TEST(ErrorEnd::FUNCTION_DECLARE);
    // 分程序
    table.goIn(funcname);           // 往里深入一层
    blockAnalyze();
    table.goOut();                  // 往外出来一层
    TEST(ErrorEnd::FUNCTION_DECLARE);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROUT(ErrorType::NO_SEMICOLON);
    } else
        NEXTSYM;
END:
    semanticAnalyzer.functionDeclareEndmapClear();
}

// <过程首部> 分析
void GrammarAnalyzer::procedureHeadAnalyze(string &name) {
    semanticAnalyzer.procedureHeadEndmapInit();
    vector<Argument> args;
    // procedure 关键字
    if (SYMBOL_TYPE != SymbolType::PROCEDURE) {
        ERROUT(ErrorType::NO_PROCEDURE);
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER, "The procedure should have a name");
        SKIP;
        TEST(ErrorEnd::PROCEDURE_HEAD);
    } else {
        if (table.isHere(TOKEN_SYMBOL)) {
            ERROUT(ErrorType::REDEFINE, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::PROCEDURE_HEAD);
        } else {
            name = TOKEN_SYMBOL;
            NEXTSYM;
        }
    }
    // 形式参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        formalParameterListAnalyze(args);
        TEST(ErrorEnd::PROCEDURE_HEAD);
    }
    // 填表
    semanticAnalyzer.procedureDefine(name, args);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROUT(ErrorType::NO_SEMICOLON);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.procedureHeadEndmapClear();
}

// <函数首部> 分析
void GrammarAnalyzer::functionHeadAnalyze(string &name) {
    semanticAnalyzer.functionHeadEndmapInit();
    vector<Argument> args;
    BasicType returnType;
    // function 关键字
    if (SYMBOL_TYPE != SymbolType::FUNCTION) {
        ERROUT(ErrorType::NO_FUNCTION);
    } else
        NEXTSYM;
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER, "There should be a name of function");
        SKIP;
        TEST(ErrorEnd::FUNCTION_HEAD);
    } else {
        // 检查重定义
        if (table.isHere(TOKEN_SYMBOL)) {
            ERROUT(ErrorType::REDEFINE, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::FUNCTION_HEAD);
        } else {
            name = TOKEN_SYMBOL;
            NEXTSYM;
        }
    }
    // 形式参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        formalParameterListAnalyze(args);
        TEST(ErrorEnd::FUNCTION_HEAD);
    }
    // 冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROUT(ErrorType::NO_COLON);
    } else
        NEXTSYM;
    // 默认有冒号，继续
    // 基本类型
    basicTypeAnalyze(returnType);
    TEST(ErrorEnd::FUNCTION_HEAD);
    // 填表
    semanticAnalyzer.functionDefine(name, args, returnType);
    // 分号
    if (SYMBOL_TYPE != SymbolType::SEMICOLON) {
        ERROUT(ErrorType::NO_SEMICOLON);
    } else
        NEXTSYM;
    CLEAR;
END:
    semanticAnalyzer.functionHeadEndmapClear();
}

// 形式参数表
void GrammarAnalyzer::formalParameterListAnalyze(vector<Argument> &args) {
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROUT(ErrorType::NO_LEFTBRACKET);
    } else
        NEXTSYM;
    // 默认有左括号，继续
    // 第一个形式参数段
    formalParameterAnalyze(args);
    TEST(ErrorEnd::FORMAL_PARAMETER_LIST);
    // 后续的形式参数段
    while (SYMBOL_TYPE == SymbolType::SEMICOLON) {
        NEXTSYM;
        formalParameterAnalyze(args);
        TEST(ErrorEnd::FORMAL_PARAMETER_LIST);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROUT(ErrorType::NO_RIGHTBRACKET);
    } else
        NEXTSYM;
    CLEAR;
END:
    ;
}

// 形式参数段
void GrammarAnalyzer::formalParameterAnalyze(vector<Argument> &args) {
    semanticAnalyzer.formalParameterEndmapInit();
    bool vary = false;
    BasicType type;
    vector<string> names;
    // var 关键字
    if (SYMBOL_TYPE == SymbolType::VAR) {
        vary = true;
        NEXTSYM;
    }
    // 第一个标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::FORMAL_PARAMETER);
    } else {
        names.push_back(TOKEN_SYMBOL);
        NEXTSYM;
    }
    // 后续标识符
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
            ERROUT(ErrorType::NO_IDENTIFIER);
            SKIP;
            TEST(ErrorEnd::FORMAL_PARAMETER);
        } else {
            names.push_back(TOKEN_SYMBOL);
            NEXTSYM;
        }
    }
    // 冒号
    if (SYMBOL_TYPE != SymbolType::COLON) {
        ERROUT(ErrorType::NO_COLON);
    } else
        NEXTSYM;
    // 默认有冒号，继续
    // 基本类型
    basicTypeAnalyze(type);
    TEST(ErrorEnd::FORMAL_PARAMETER);
    // 把读到的结果填到参数表中
    semanticAnalyzer.formalParameterAdd(names, type, vary, args);
    CLEAR;
END:
    semanticAnalyzer.formalParameterEndmapClear();
}

// 语句
void GrammarAnalyzer::statementAnalyze() {
    semanticAnalyzer.statementEndmapInit();
    string value;
    BasicType type;
    switch (SYMBOL_TYPE) { 
    // 赋值语句 或 过程调用语句
    case SymbolType::IDENTIFIER:
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
            ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::STATEMENT);
        }
        switch (table.ste->type) {
        case IdentifierType::ARRAY:
        case IdentifierType::VAR:
            assignStatementAnalyze();
            TEST(ErrorEnd::STATEMENT);
            break;
        case IdentifierType::FUNCTION:  // 给函数返回值赋值的语句
        case IdentifierType::PROCEDURE:
            callStatementAnalyze(value, type);
            TEST(ErrorEnd::STATEMENT);
            break;
        default:
            ERROUT(ErrorType::CONSTANT, TOKEN_SYMBOL);
            break;
        }
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
END:
    semanticAnalyzer.statementEndmapClear();
}

// 赋值语句
void GrammarAnalyzer::assignStatementAnalyze() {
    // 标识符
    string name;        // 标识符
    string offset;      // 如果是数组，记录下标
    string value;       // 要赋的值
    BasicType nameType; // 被赋值的变量的类型
    BasicType valueType;// 要赋的值的类型
    bool variable;      // 表达式是否为变量表达式
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
    } else {
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
            ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::ASSIGN_STATEMENT);
        }
        name = table.encodeKey();   // 唯一标识
        nameType = table.ste->valueType;
        switch (table.ste->type) {
        // 给 数组元素 赋值
        case IdentifierType::ARRAY:
            NEXTSYM;
            // 左方括号
            if (SYMBOL_TYPE != SymbolType::LEFT_SQUARE) {
                ERROUT(ErrorType::NO_LEFTSQUARE);
            } else
                NEXTSYM;
            // 默认有左方括号，继续
            // 表达式，下标
            expressionAnalyze(offset, valueType, variable);
            TEST(ErrorEnd::ASSIGN_STATEMENT);
            // 右方括号
            if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
                ERROUT(ErrorType::NO_RIGHTSQUARE);
            } else
                NEXTSYM;
            // 默认有右方括号，继续
            // 赋值符
            if (SYMBOL_TYPE != SymbolType::ASSIGN) {
                ERROUT(ErrorType::NO_ASSIGN);
                SKIP;
                TEST(ErrorEnd::ASSIGN_STATEMENT);
            } else
                NEXTSYM;
            // 表达式，要赋的值
            expressionAnalyze(value, valueType, variable);
            TEST(ErrorEnd::ASSIGN_STATEMENT);
            // 生成指令
            semanticAnalyzer.assignToArray(name, offset, value, nameType, valueType);
            break;
        // 给普通变量赋值
        default:
            NEXTSYM;
            // 赋值符
            if (SYMBOL_TYPE != SymbolType::ASSIGN) {
                ERROUT(ErrorType::NO_ASSIGN);
                SKIP;
                TEST(ErrorEnd::ASSIGN_STATEMENT);
            } else
                NEXTSYM;
            // 表达式，要赋的值
            expressionAnalyze(value, valueType, variable);
            TEST(ErrorEnd::ASSIGN_STATEMENT);
            // 生成指令
            semanticAnalyzer.assign(name, value, nameType, valueType);
            break;
        }
    }
    CLEAR;
END:
    ;
}

// 给函数赋返回值
void GrammarAnalyzer::functionAssignStatementAnalyze(const string &name) {
    table.decodeKey(name);
    BasicType functype = table.ste->valueType;
    BasicType valuetype;
    bool variable;
    string value;
    // 赋值符
    if (SYMBOL_TYPE != SymbolType::ASSIGN) {
        ERROUT(ErrorType::NO_ASSIGN);
        SKIP;
        TEST(ErrorEnd::FUNCTION_ASSIGN);
    } else
        NEXTSYM;
    // 表达式
    expressionAnalyze(value, valuetype, variable);
    TEST(ErrorEnd::FUNCTION_ASSIGN);
    semanticAnalyzer.functionAssign(name, value, functype, valuetype);
    CLEAR;
END:
    ;
}

// 表达式
void GrammarAnalyzer::expressionAnalyze(string &value, BasicType &type, bool &variable) {
    // 正负号
    bool neg = false;           // 首项的符号
    string oper1, oper2, dest;  // 参与运算的操作数及结果
    char op;                    // 运算符
    BasicType oper2type;
    bool oper2variable;
    switch (SYMBOL_TYPE) {
    case SymbolType::PLUS:
        NEXTSYM;
        break;
    case SymbolType::MINUS:
        neg = true;
        NEXTSYM;
        break;
    default:
        break;
    }
    // 项
    termAnalyze(dest, type, variable);
    TEST(ErrorEnd::EXPRESSION);
    if (neg) {
        oper1 = dest;
        semanticAnalyzer.negative(oper1, dest);
    }
    while (true) {
        switch (SYMBOL_TYPE) {
        // 加号
        case SymbolType::PLUS:
            op = '+';
            NEXTSYM;
            break;
        // 减号
        case SymbolType::MINUS:
            op = '-';
            NEXTSYM;
            break;
        default:
            goto END_EXPRESSION;
            break;
        }
        // 项
        variable = false;
        oper1 = dest;
        termAnalyze(oper2, oper2type, oper2variable);
        TEST(ErrorEnd::EXPRESSION);
        type = BasicType::INTEGER;
        switch (op) {
        case '+':
            semanticAnalyzer.addition(oper1, oper2, dest);
            break;
        case '-':
            semanticAnalyzer.subtraction(oper1, oper2, dest);
            break;
        default:
            goto END_EXPRESSION;
            break;
        }
    }
END_EXPRESSION:
    CLEAR;
END:
    value = dest;
}

// 项
void GrammarAnalyzer::termAnalyze(string &value, BasicType &type, bool &variable) {
    semanticAnalyzer.termEndmapInit();
    string oper1, oper2, dest;
    char op;
    BasicType oper2type;
    bool oper2variable;
    // 因子
    factorAnalyze(dest, type, variable);
    TEST(ErrorEnd::TERM);
    while (true) {
        switch (SYMBOL_TYPE) {
        // 乘号
        case SymbolType::ASTERISK:
            op = '*';
            NEXTSYM;
            break;
        case SymbolType::SLASH:
            op = '/';
            NEXTSYM;
            break;
        default:
            goto END_TERM;
        }
        // 因子
        variable = false;
        oper1 = dest;
        factorAnalyze(oper2, oper2type, oper2variable);
        TEST(ErrorEnd::TERM);
        type = BasicType::INTEGER;
        switch (op) {
        case '*':
            semanticAnalyzer.multiplicatioin(oper1, oper2, dest);
            break;
        case '/':
            semanticAnalyzer.division(oper1, oper2, dest);
            break;
        default:
            goto END_TERM;
            break;
        }
    }
END_TERM:
    CLEAR;
END:
    value = dest;
    semanticAnalyzer.termEndmapClear();
}

// 因子
void GrammarAnalyzer::factorAnalyze(string &value, BasicType &type, bool &variable) {
    semanticAnalyzer.factorEndmapInit();
    string name;
    string offset;
    int n;
    BasicType t;
    bool v;
    switch (SYMBOL_TYPE) {
    // 标识符、数组元素、函数调用语句
    case SymbolType::IDENTIFIER:
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
            ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::FACTOR);
        }
        switch (table.ste->type) {
        // 变量
        case IdentifierType::VAR:
            value = table.encodeKey();
            type = table.ste->valueType;
            variable = true;
            NEXTSYM;
            break;
        // 常量
        case IdentifierType::CONST:
            semanticAnalyzer.intToString(table.ste->multiInt, value);
            type = table.ste->valueType;
            variable = false;
            NEXTSYM;
            break;
        // 数组元素
        case IdentifierType::ARRAY:
            name = table.encodeKey();
            type = table.ste->valueType;
            variable = true;
            NEXTSYM;
            // 左方括号
            if (SYMBOL_TYPE != SymbolType::LEFT_SQUARE) {
                ERROUT(ErrorType::NO_LEFTSQUARE);
            } else
                NEXTSYM;
            // 默认有左方括号，开始计算表达式
            expressionAnalyze(offset, t, v);
            TEST(ErrorEnd::FACTOR);
            // 右方括号
            if (SYMBOL_TYPE != SymbolType::RIGHT_SQUARE) {
                ERROUT(ErrorType::NO_RIGHTSQUARE);
            } else
                NEXTSYM;
            semanticAnalyzer.assignFromArray(name, offset, value);
            break;
        // 函数调用语句
        case IdentifierType::FUNCTION:
            variable = false;
            callStatementAnalyze(value, type);
            TEST(ErrorEnd::FACTOR);
            break;
        default:
            ERROUT(ErrorType::NO_VALUE, TOKEN_SYMBOL + " is not a valid value");
            SKIP;
            break;
        }
        break;
    case SymbolType::LEFT_BRACKET:
        NEXTSYM;
        // 表达式
        expressionAnalyze(value, type, variable);
        TEST(ErrorEnd::FACTOR);
        if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
            ERROUT(ErrorType::NO_RIGHTBRACKET);
        } else
            NEXTSYM;
        break;
    case SymbolType::NUMBER:
        // 无符号整数
        unsignedAnalyze(n);
        TEST(ErrorEnd::FACTOR);
        semanticAnalyzer.intToString(n, value);
        type = BasicType::INTEGER;
        variable = false;
        break;
    default:
        ERROUT(ErrorType::NO_VALUE);
        break;
    }
    CLEAR;
END:
    semanticAnalyzer.factorEndmapClear();
}

// 调用语句
void GrammarAnalyzer::callStatementAnalyze(string &value, BasicType &type) {
    // 标识符
    bool hasValue;
    string name;
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::CALL_STATEMENT);
    } else {
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
            ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::CALL_STATEMENT);
        }
        name = table.encodeKey();
        switch (table.ste->type) {
        case IdentifierType::FUNCTION:
            NEXTSYM;
            if (SYMBOL_TYPE == SymbolType::ASSIGN) {
            // 函数返回值赋值
                functionAssignStatementAnalyze(name);
                TEST(ErrorEnd::CALL_STATEMENT);
                goto END;
            } else {
                hasValue = true;
                type = table.ste->valueType;
            }
            break;
        case IdentifierType::PROCEDURE:
            hasValue = false;
            NEXTSYM;
            break;
        default:
            ERROUT(ErrorType::CANNOT_CALL, TOKEN_SYMBOL);
            SKIP;
            TEST(ErrorEnd::CALL_STATEMENT);
            break;
        }
    }
    // 实在参数表
    if (SYMBOL_TYPE == SymbolType::LEFT_BRACKET) {
        realParameterListAnalyze(name);
        TEST(ErrorEnd::CALL_STATEMENT);
    }
    if (hasValue)
        semanticAnalyzer.callFunction(name, value);
    else
        semanticAnalyzer.callProcedure(name);
    CLEAR;
END:
    ;
}

// 实在参数表
void GrammarAnalyzer::realParameterListAnalyze(const string &name) {
    // 查参数表
    table.decodeKey(name);
    vector<Argument> *args = table.ste->arguments;      // 参数表
    vector<Argument>::iterator argit;                   // 参数表的迭代器
    string value;                                       // 实参的值
    BasicType type;                                     // 实参的类型
    bool variable;                                      // 实参是否是变量表达式
    // 确认参数表非空
    if (args == NULL) {
        ERROUT(ErrorType::TOO_MANY_ARG, "there should not be real parameters");
        SKIP;
        TEST(ErrorEnd::REAL_PARAMETER_LIST);
    }
    argit = args->begin();
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROUT(ErrorType::NO_LEFTBRACKET);
    } else
        NEXTSYM;
    // 默认有左括号
    // 第一个实在参数
    expressionAnalyze(value, type, variable);
    TEST(ErrorEnd::REAL_PARAMETER_LIST);
    // 检查参数个数是否过多
    if (argit == args->end()) {
        ERROUT(ErrorType::TOO_MANY_ARG);
        SKIP;
        TEST(ErrorEnd::REAL_PARAMETER_LIST);
    // 检查实参与形参声明中的 var 是否可对应
    } else if (argit->vary == true && variable == false) {
        ERROUT(ErrorType::UNVARIABLE);
    }
    // 传参
    semanticAnalyzer.passParameter(value, argit->type, type);
    argit++;
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        // 实在参数
        expressionAnalyze(value, type, variable);
        TEST(ErrorEnd::REAL_PARAMETER_LIST);
        // 检查参数个数是否过多
        if (argit == args->end()) {
            ERROUT(ErrorType::TOO_MANY_ARG);
            SKIP;
            TEST(ErrorEnd::REAL_PARAMETER_LIST);
        // 检查实参与形参声明中的 var 是否可对应
        } else if (argit->vary == true && variable == false) {
            ERROUT(ErrorType::UNVARIABLE);
        }
        // 传参
        semanticAnalyzer.passParameter(value, argit->type, type);
        argit++;
    }
    if (argit != args->end()) {
        ERROUT(ErrorType::TOO_LESS_ARG);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROUT(ErrorType::NO_RIGHTBRACKET);
    } else
        NEXTSYM;
    CLEAR;
END:
    ;
}

// 条件
void GrammarAnalyzer::conditionAnalyze(SymbolType &relationOp) {
    // 表达式
    string value1, value2;
    BasicType t;
    bool v;
    expressionAnalyze(value1, t, v);
    TEST(ErrorEnd::CONDITION);
    // 关系运算符
    relationOperatorAnalyze(relationOp);
    TEST(ErrorEnd::CONDITION);
    // 表达式
    expressionAnalyze(value2, t, v);
    TEST(ErrorEnd::CONDITION);
    semanticAnalyzer.compare(value1, value2);
    CLEAR;
END:
    ;
}

// 关系运算符
void GrammarAnalyzer::relationOperatorAnalyze(SymbolType &relationOp) {
    switch (SYMBOL_TYPE) {
    case SymbolType::LESS:
    case SymbolType::LESS_EQUAL:
    case SymbolType::GREATER:
    case SymbolType::GREATER_EQUAL:
    case SymbolType::EQUAL:
    case SymbolType::INEQUAL:
        relationOp = SYMBOL_TYPE;
        NEXTSYM;
        break;
    default:
        ERROUT(ErrorType::NO_RELATION);
        relationOp = SymbolType::ILLEGAL;
        SKIP;
        TEST(ErrorEnd::RELATION_OPERATOR);
        break;
    }
    CLEAR;
END:
    ;
}

// 条件语句
void GrammarAnalyzer::conditionStatementAnalyze() {
    // if 关键字
    SymbolType condition;   // 判断条件
    string elseLabel = semanticAnalyzer.newLabel("condition_else");
    string endLabel = semanticAnalyzer.newLabel("condition_end");
    if (SYMBOL_TYPE != SymbolType::IF) {
        ERROUT(ErrorType::NO_IF);
    } else {
        NEXTSYM;
    }
    // 条件
    conditionAnalyze(condition);
    TEST(ErrorEnd::CONDITION);
    semanticAnalyzer.dissatisfyJump(condition, elseLabel);
    // then 关键字
    if (SYMBOL_TYPE != SymbolType::THEN) {
        ERROUT(ErrorType::NO_THEN);
    } else
        NEXTSYM;
    // 默认有 then，继续
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::CONDITION_STATEMENT);
    semanticAnalyzer.jumpAnyway(endLabel);
    // 不管有没有 else 关键字，else-labe 必须有
    semanticAnalyzer.label(elseLabel);
    // else 关键字
    if (SYMBOL_TYPE == SymbolType::ELSE) {
        NEXTSYM;
        statementAnalyze();
        TEST(ErrorEnd::CONDITION_STATEMENT);
    }
    semanticAnalyzer.label(endLabel);
    CLEAR;
END:
    ;
}

// do-while 语句
void GrammarAnalyzer::doWhileStatementAnalyze() {
    SymbolType condition;
    string lab = semanticAnalyzer.newLabel("loop_do_while");
    // do 关键字
    if (SYMBOL_TYPE != SymbolType::DO) {
        ERROUT(ErrorType::NO_DO);
    } else
        NEXTSYM;
    // label
    semanticAnalyzer.label(lab);
    // 语句
    statementAnalyze();
    TEST(ErrorEnd::DO_WHILE_STATEMENT);
    // whlie 关键字
    if (SYMBOL_TYPE != SymbolType::WHILE) {
        ERROUT(ErrorType::NO_WHILE);
        SKIP;
        TEST(ErrorEnd::DO_WHILE_STATEMENT);
    } else
        NEXTSYM;
    // 条件
    conditionAnalyze(condition);
    TEST(ErrorEnd::DO_WHILE_STATEMENT);
    // 测试循环条件，跳转指令
    semanticAnalyzer.satisfyJump(condition, lab);
    CLEAR;
END:
    ;
}

// for 语句
void GrammarAnalyzer::forStatementAnalyze() {
    string step;            // 每一步要加的数值。如果是 TO，则 step 应为 1，DOWNTO 应为 -1
    SymbolType condition;   // 循环判断条件，如果是 TO，则应为 <=，DOWNTO应为 >=
    string forvar;          // 循环变量名
    string start;           // 循环变量初值
    string end;             // 循环变量终值
    string startlabel = semanticAnalyzer.newLabel("loop_for_start");
    string endlabel = semanticAnalyzer.newLabel("loop_for_end");
    BasicType forvartype;
    bool v;
    BasicType t;
    string tmp;
    // for 关键字
    if (SYMBOL_TYPE != SymbolType::FOR) {
        ERROUT(ErrorType::NO_FOR);
    } else
        NEXTSYM;
    // 标识符，循环变量
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::FOR_STATEMENT);
    } else {
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
        // 检查是否有定义
            ERROUT(ErrorType::UNDEFINED);
            SKIP;
            TEST(ErrorEnd::FOR_STATEMENT);
        } else if (table.ste->type != IdentifierType::VAR) {
        // 检查是不是变量
            ERROUT(ErrorType::UNVARIABLE, "assign to function is forbid");
            SKIP;
            TEST(ErrorEnd::FOR_STATEMENT);
        } else {
            forvar = table.encodeKey();
            forvartype = table.ste->valueType;
            NEXTSYM;
        }
    }
    // 赋值符
    if (SYMBOL_TYPE != SymbolType::ASSIGN) {
        ERROUT(ErrorType::NO_ASSIGN);
    } else
        NEXTSYM;
    // 默认有赋值符，继续
    // 表达式，循环变量的初值
    expressionAnalyze(start, t, v);
    TEST(ErrorEnd::FOR_STATEMENT);
    // 给循环变量赋初值
    semanticAnalyzer.assign(forvar, start, forvartype, t);
    // to | downto 关键字
    switch (SYMBOL_TYPE) {
    case SymbolType::TO:
        semanticAnalyzer.intToString(1, step);
        condition = SymbolType::LESS_EQUAL;
        NEXTSYM;
        break;
    case SymbolType::DOWNTO:
        semanticAnalyzer.intToString(-1, step);
        condition = SymbolType::GREATER_EQUAL;
        NEXTSYM;
        break;
    default:
        ERROUT(ErrorType::NO_DOWN_TO);
        SKIP;
        TEST(ErrorEnd::FOR_STATEMENT);
        break;
    }
    // 表达式，循环终止条件
    expressionAnalyze(end, t, v);
    TEST(ErrorEnd::FOR_STATEMENT);
    // do 关键字
    if (SYMBOL_TYPE != SymbolType::DO) {
        ERROUT(ErrorType::NO_DO);
    } else
        NEXTSYM;
    // 默认有 do，继续
    semanticAnalyzer.label(startlabel);                     // 循环开始标识
    semanticAnalyzer.compare(forvar, end);
    semanticAnalyzer.dissatisfyJump(condition, endlabel);   // 测试循环条件
    // 语句
    statementAnalyze();                                     // 循环体
    TEST(ErrorEnd::FOR_STATEMENT);
    semanticAnalyzer.addition(forvar, step, tmp);           // 循环变量自增或自减
    semanticAnalyzer.assign(forvar, tmp, forvartype, BasicType::INTEGER);
    semanticAnalyzer.jumpAnyway(startlabel);                // 无条件跳转到循环开始
    semanticAnalyzer.label(endlabel);                       // 循环结束标识！！！
    CLEAR;
END:
    ;
}

// 复合语句
void GrammarAnalyzer::compoundStatementAnalyze() {
    // begin 关键字
    if (SYMBOL_TYPE != SymbolType::BEGIN) {
        ERROUT(ErrorType::NO_BEGIN);
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
        ERROUT(ErrorType::NO_END);
    } else
        NEXTSYM;
    CLEAR;
END:
    ;
}

// 读语句
void GrammarAnalyzer::readStatementAnalyze() {
    // read 关键字
    string var;
    if (SYMBOL_TYPE != SymbolType::READ) {
        ERROUT(ErrorType::NO_READ);
        SKIP;
        TEST(ErrorEnd::READ_STATEMENT);
    } else
        NEXTSYM;
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROUT(ErrorType::NO_LEFTBRACKET);
    } else
        NEXTSYM;
    // 默认有左括号，继续
    // 标识符
    if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
        ERROUT(ErrorType::NO_IDENTIFIER);
        SKIP;
        TEST(ErrorEnd::READ_STATEMENT);
    } else {
        table.find(TOKEN_SYMBOL);
        if (table.ste == NULL) {
        // 检查标识符是否有定义
            ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
        } else if (table.ste->type != IdentifierType::VAR) {
        // 检查是不是变量
            ERROUT(ErrorType::UNVARIABLE, TOKEN_SYMBOL);
        } else {
        // 检查变量类型，生成读指令
            var = table.encodeKey();
            if (table.ste->valueType == BasicType::CHAR)
                semanticAnalyzer.readChar(var);
            else
                semanticAnalyzer.readInteger(var);
        }
    }
    NEXTSYM;
    while (SYMBOL_TYPE == SymbolType::COMMA) {
        NEXTSYM;
        // 标识符
        if (SYMBOL_TYPE != SymbolType::IDENTIFIER) {
            ERROUT(ErrorType::NO_IDENTIFIER);
            SKIP;
            TEST(ErrorEnd::READ_STATEMENT);
        } else {
            table.find(TOKEN_SYMBOL);
            if (table.ste == NULL) {
            // 检查标识符是否有定义
                ERROUT(ErrorType::UNDEFINED, TOKEN_SYMBOL);
            } else if (table.ste->type != IdentifierType::VAR) {
            // 检查是不是变量
                ERROUT(ErrorType::UNVARIABLE, TOKEN_SYMBOL);
            } else {
            // 检查变量类型，生成读指令
                var = table.encodeKey();
                if (table.ste->valueType == BasicType::CHAR)
                    semanticAnalyzer.readChar(var);
                else
                    semanticAnalyzer.readInteger(var);
            }
        }
        NEXTSYM;
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROUT(ErrorType::NO_RIGHTBRACKET);
    } else
        NEXTSYM;
    CLEAR;
END:
    ;
}

// 写语句
void GrammarAnalyzer::writeStatementAnalyze() {
    string name;
    BasicType type;
    bool v;
    // write 关键字
    if (SYMBOL_TYPE != SymbolType::WRITE) {
        ERROUT(ErrorType::NO_WRITE);
    } else
        NEXTSYM;
    // 左括号
    if (SYMBOL_TYPE != SymbolType::LEFT_BRACKET) {
        ERROUT(ErrorType::NO_LEFTBRACKET);
    } else
        NEXTSYM;
    // 默认有左括号
    // 内容
    if (SYMBOL_TYPE == SymbolType::DOUBLE_QUOTE) {
        // 字符串
        stringAnalyze(name);
        TEST(ErrorEnd::WRITE_STATEMENT);
        semanticAnalyzer.writeString(name);
        if (SYMBOL_TYPE == SymbolType::COMMA) {
            NEXTSYM;
            // 表达式
            expressionAnalyze(name, type, v);
            TEST(ErrorEnd::WRITE_STATEMENT);
            if (type == BasicType::CHAR)
                semanticAnalyzer.writeChar(name);
            else
                semanticAnalyzer.writeInteger(name);
        }
    } else {                                      
        // 表达式
        expressionAnalyze(name, type, v);
        TEST(ErrorEnd::WRITE_STATEMENT);
        if (type == BasicType::CHAR)
            semanticAnalyzer.writeChar(name);
        else
            semanticAnalyzer.writeInteger(name);
    }
    // 右括号
    if (SYMBOL_TYPE != SymbolType::RIGHT_BRACKET) {
        ERROUT(ErrorType::NO_RIGHTBRACKET);
    } else
        NEXTSYM;
    semanticAnalyzer.newline();
    CLEAR;
END:
    ;
}