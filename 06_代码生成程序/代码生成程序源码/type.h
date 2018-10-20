/*
    type.h
    类型定义。
*/               
#ifndef TYPE__H
#define TYPE__H

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;

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

// 此处标记：错误处理时，跳过一些符号之后，到哪一个语法成分结束
// 例如：在处理表达式时，如果里面有错误，跳到 <表达式> 的结尾，则用这里的 EXPRESSION 
namespace name_EE{
enum ErrorEnd {
    NORMAL,                 // “正常”，表示没有错误，不需要处理
    PROGRAM,                // 整个程序
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
    FUNCTION_ASSIGN,        // 函数返回值赋值
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

// 错误类型
namespace name_ET {
enum ErrorType {
    NO_PERIOD,      // 缺少句点
    ILLEGAL_CHAR,   // 非法字符
    NO_FINISH,      // 希望结束而没有结束
    FINISH,         // 程序尚不完整而文件结束
    NO_CONST,       // 缺少 const 关键字
    NO_SEMICOLON,   // 缺少分号
    NO_IDENTIFIER,  // 缺少标识符
    REDEFINE,       // 标识符重定义
    NO_EQUAL,       // 缺少等号
    NO_CHNUM,       // 缺少字符或整数（<常量>）
    NO_SINGLEQUOTE, // 缺少单引号
    NO_DOUBLEQUOTE, // 缺少双引号
    ILLEGAL_STRING, // 非法字符串
    NO_NUMBER,      // 希望是数字而不是
    INT_OVERFLOW,   // 整数溢出
    NO_VAR,         // 缺少 var 关键字
    NO_COLON,       // 缺少冒号
    NO_LEFTSQUARE,  // 缺少左方括号
    NO_RIGHTSQUARE, // 缺少右方括号
    NO_OF,          // 缺少 of 关键字
    NO_BASICTYPE,   // 缺少基本类型
    NO_PROCEDURE,   // 缺少 procedure 关键字
    NO_FUNCTION,    // 缺少 function 关键字
    NO_LEFTBRACKET, // 缺少左括号
    NO_RIGHTBRACKET,// 缺少右括号
    CONSTANT,       // 意外的常量标识符
    NO_ASSIGN,      // 缺少赋值符，即希望是赋值语句，实际不是
    NO_VALUE,       // 缺少值
    UNDEFINED,      // 标识符未定义
    CANNOT_CALL,    // 标识符不能用于函数或过程调用
    INT_TO_CHAR,    // 将整型数转化为字符型
    TOO_LESS_ARG,   // 参数太少
    TOO_MANY_ARG,   // 参数太多
    UNVARIABLE,     // 需要变量表达式
    NO_RELATION,    // 需要关系运算符而没有
    NO_IF,          // 缺少 if 关键字
    NO_THEN,        // 缺少 then 关键字
    NO_ELSE,        // 缺少 else 关键字
    NO_DO,          // 缺少 do 关键字
    NO_WHILE,       // 缺少 while 关键字
    NO_FOR,         // 缺少 for 关键字
    NO_DOWN_TO,     // 缺少 to 或 downto 关键字
    NO_BEGIN,       // 缺少 begin 关键字
    NO_END,         // 缺少 end 关键字
    NO_READ,        // 缺少 read 关键字
    NO_WRITE        // 缺少 write 关键字
};
}

// 四元式中的操作符
namespace name_OP {
enum Operator {
    ADD,    // 加法
    SUB,    // 减法
    MUL,    // 乘法
    DIV,    // 除法
    NEG,    // 求负
    ASS,    // 赋值
    ARAS,   // 给数组元素赋值
    ASAR,   // 将数组元素赋值给变量
    FAS,    // 给函数赋返回值
    CUT,    // 将整数截断成字符型
    LAB,    // 标签
    CMP,    // 比较
    JG,     // 大于跳转
    JGE,    // 大于或等于跳转
    JL,     // 小于跳转
    JLE,    // 小于或等于跳转
    JE,     // 等于跳转
    JNE,    // 不等于跳转
    JMP,    // 无条件跳转
    CALP,   // 过程调用，不带返回值
    CALF,   // 函数调用，带返回值
    RTN,    // 函数或过程返回
    PUSH,   // 压栈
    RI,     // 读整数
    RC,     // 读字符
    WS,     // 写字符串
    WI,     // 写整数
    WC,     // 写字符
    NL      // 换行
};
}

using name_ST::SymbolType;
using name_IT::IdentifierType;
using name_BT::BasicType;
using name_EE::ErrorEnd;
using name_ET::ErrorType;
using name_OP::Operator;
                        
// 每个参数的信息
struct Argument {
    string name;    // 参数名
    BasicType type; // 类型
    bool vary;      // true 表示传地址，false 表示传值

    // 构造函数
    Argument(const string &n, BasicType t, bool v): name(n), type(t), vary(v) {}
};

// 符号表结构
struct STE {
    string name;                    // 标识符名称
    IdentifierType type;            // 标识符类型
    BasicType valueType;            // 常量、变量：值的类型；
                                    // 数组：元素的类型；
                                    // 函数：返回值类型
    int multiInt;                   // 常量：值；
                                    // 数组：容量限
    int location;                   // 变量/数组在运行栈上的相对地址
    vector<Argument> *arguments;    // 函数/过程的参数列表                
    ////////////////////////////////////////////////////////////////
private:    // 以下，为构建树形符号表而必须的指针
    STE *father;                    // 当前符号的父节点（所属函数/过程）
    map<string, STE *> *childs;     // 当前符号(函数/过程)下的符号
    friend class SymbolTableManager;// 把数据结构相关的指针暴露给符号表管理器
    ////////////////////////////////////////////////////////////////
public:
    STE(): arguments(NULL), 
        childs(NULL), 
        father(NULL) {}     // 构造函数：给参数列表置为 NULL
    ~STE() {
        delete childs;      // 先删除子树，再删除当前节点
        delete arguments;
    }                       // 析构函数：先删除参数列表
                            // 慎用！析构时会删除以当前节点为根的子树！！
    STE(const STE &ste) {
        name = ste.name;
        type = ste.type;
        valueType = ste.valueType;
        multiInt = ste.multiInt;
        location = ste.location;
        // 对这个指针要格外小心！！！
        arguments = ste.arguments == NULL ? NULL :
                    new vector<Argument>(*ste.arguments);
        // 数据结构相关的指针留着以后处理。
        father = NULL;
        childs = NULL;
    }                       // 拷贝构造
};

static string stringPlusInt(const string &str, int n) {
    string newstr;
    char num[20];
    newstr = str;
    sprintf(num, "%d", n);
    newstr.append(num);
    return newstr;
}

#endif // !TYPE__H