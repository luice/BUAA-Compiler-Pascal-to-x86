/*
    type.h
    类型定义。
*/               
#ifndef TYPE__H
#define TYPE__H

#include <string>
#include <vector>
using std::string;
using std::vector;

// 词法分析返回的新符号类型
namespace name_ST {
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
    TO,                 // to 关键字
    DOWNTO,             // downto 关键字
    
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
}

// 标识符的类型
namespace name_IT {
enum IdentifierType{
    VAR,
    CONST,
    ARRAY,
    PROCEDURE,
    FUNCTION
};
}

// 基本类型
namespace name_BT {
enum BasicType {
    INTEGER,
    CHAR
};
}

// 此处标记：错误处理时，跳过一些符号之后，到哪一个语法成分结束
// 例如：在处理表达式时，如果里面有错误，跳到 <表达式> 的结尾，则用这里的 EXPRESSION 
namespace name_EE{
enum ErrorEnd {
    NORMAL,                 // “正常”，表示没有错误，不需要处理
    BLOCK,                  // 分程序
    CONST_DECLARE,          // 常量说明
    CONST_DEFINE,           // 常量定义
    CONST,                  // 常量
    CHAR,                   // 字符
    STRING,                 // 字符串
    UNSIGNED,               // 无符号整数
    VAR_DECLARE,            // 变量说明
    VAR_DEFINE,             // 变量定义
    TYPE,                   // 类型
    BASIC_TYPE,             // 基本类型
    PROCEDURE_DECLARE,      // 过程说明
    FUNCTION_DECLARE,       // 函数说明
    PROCEDURE_HEAD,         // 过程首部
    FUNCTION_HEAD,          // 函数首部
    FORMAL_PARAMETER_LIST,  // 形式参数表
    FORMAL_PARAMETER,       // 形式参数
    STATEMENT,              // 语句
    ASSIGN_STATEMENT,       // 赋值语句
    EXPRESSION,             // 表达式
    TERM,                   // 项
    FACTOR,                 // 因子
    CALL_STATEMENT,         // 调用语句
    REAL_PARAMETER_LIST,    // 实在参数表
    CONDITION,              // 条件
    RELATION_OPERATOR,      // 关系运算符
    CONDITION_STATEMENT,    // 条件语句
    DO_WHILE_STATEMENT,     // do-while 语句
    FOR_STATEMENT,          // for 语句
    COMPOUND_STATEMENT,     // 复合语句
    READ_STATEMENT,         // read 语句
    WRITE_STATEMENT         // write 语句
};
}

using name_ST::SymbolType;
using name_IT::IdentifierType;
using name_BT::BasicType;
using name_EE::ErrorEnd;

// 每个参数的信息
struct Argument {
    string name;    // 参数名
    BasicType type; // 类型
    bool vary;      // true 表示传地址，false 表示传值
};

struct STE {
    string name;            // 标识符名称
    IdentifierType type;    // 标识符类型

};

#endif // !TYPE__H