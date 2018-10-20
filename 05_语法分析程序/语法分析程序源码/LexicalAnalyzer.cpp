/*
    LexicalAnalyzer.cpp
    词法分析器的实现。
*/                    
#include "LexicalAnalyzer.h"
#include <cctype>
#include <map>
#include <string>
using namespace std;
using namespace name_ST;

static string strGot;   // 读到的字符串
static char ch;         // 下一个即将读出来的（还没读到它）的字符
                        //  设这个变量的目的是提高效率

static map<string, SymbolType> reservedWords;   // 保留字表，建立每个保留字与其类型之前的关系

// 单例模式，对外提供的“构造函数”
LexicalAnalyzer &LexicalAnalyzer::initialLexicalAnalyzer(ifstream &file) {      
    static LexicalAnalyzer theLexicalAnalyzer(file);
    return theLexicalAnalyzer;
}
                
// 真正的构造函数
LexicalAnalyzer::LexicalAnalyzer(ifstream &file): sourceFile(file) {
                                                // 将文件输入流置为参数
    // 初始化行号为 1
    lineNumber = 1;
    // 读进来文件中的第一个字符备用
    ch = sourceFile.get();
    //初始化保留字表                          
#define INSERT_RES(str, type)    \
        reservedWords.insert(pair<string, SymbolType>(str, type))   // 定义宏，简化填表代码
    INSERT_RES("procedure", PROCEDURE);
    INSERT_RES("function", FUNCTION);
    INSERT_RES("const", CONST);
    INSERT_RES("var",  VAR);
    INSERT_RES("array", ARRAY);
    INSERT_RES("of", OF);
    INSERT_RES("integer", INTEGER);
    INSERT_RES("char", CHAR);
    INSERT_RES("begin", BEGIN);
    INSERT_RES("end", END);
    INSERT_RES("for", FOR);
    INSERT_RES("do", DO);
    INSERT_RES("while", WHILE);
    INSERT_RES("if", IF);
    INSERT_RES("then", THEN);
    INSERT_RES("else", ELSE);
    INSERT_RES("read", READ);
    INSERT_RES("write", WRITE);
    INSERT_RES("to", TO);
    INSERT_RES("downto", DOWNTO);
#undef  INSERT_RES                                                  // 取消定义 INSERT_RES 宏
}

#define GET     ch = sourceFile.get()   // 定义宏，简化从文件中读取一个字符的操作
#define PEEK    ch = sourceFile.peek()  // 定义宏，简化查看文件流第一个字符的操作
#define ADD(c)  strGot.append(1, c)     // 定义宏，简化往字符串结尾添加字符的操作

// 读取下一个符号，将符号类型存入 symbolType，具体的符号存入 symbol
void LexicalAnalyzer::nextSymbol() {
    while (isspace(ch))
        GET;
    if (isdigit(ch)) {          // 遇数字，这里读到的一定是整数     
        symbolType = NUMBER;                // 置符号类型为“数字”        
        token.number = ch - '0';            // 将数字字符转化为整数
        for (GET; isdigit(ch); GET) 
            token.number = token.number * 10 + ch - '0';  
                                            // 将数字字符转化为整数，更新 number 的值
        // 这里，循环跳出时，ch 是非数字字符
        //  由于 ch 是还未处理的字符，所以逻辑正确
    } else if (isalpha(ch)) {   // 遇字母，这里读到的一定是标识符或保留字
        token.symbol = &strGot;             // 这时读到的符号是一定要存入 token.symbol 的
        strGot.clear();                     // 先清空字符串
        ADD(ch);                            // 先将第一个字符存入字符串，以备后面再读
        for (GET; isalnum(ch); GET)
            ADD(ch);                                                           
        // 这里，循环跳出时，ch 是一个新的未处理的字符，逻辑正确

        // 以下，判断读到的符号是标识符还是保留字，将其具体类型存入 symbolType
        map<string, SymbolType>::iterator it = reservedWords.find(strGot);
        if (it == reservedWords.end())      // 不在保留字表中，说明是标识符
            symbolType = IDENTIFIER;
        else                                // 在保留字表中
            symbolType = it->second;        // 要把保留字的具体类型存入 symbol
    } else {                    // 遇其他符号，根据符号本身判断符号的类型
                                //  每种符号唯一对应一种类型，所以不必存入 token
#ifdef STAGE_LEXICAL
        token.symbol = &strGot;             // 把 token.symbol 指向字符串
        strGot.clear();
        ADD(ch);                            // 先把待判断的字符加入字符串
#endif // STAGE_LEXICAL
        switch (ch) {
        // 以下，在这个 switch 块中，每一种情况执行完之后，都使得 ch 是处理完的字符
        // 在这个 switch 块外面，统一再读一个新字符备用
        case '(':
            symbolType = LEFT_BRACKET;
            break;
        case ')':
            symbolType = RIGHT_BRACKET;
            break;
        case '[':
            symbolType = LEFT_SQUARE;
            break;
        case ']':
            symbolType = RIGHT_SQUARE;
            break;
        case '\'':
            symbolType = SINGLE_QUOTE;
            break;
        case '\"':
            symbolType = DOUBLE_QUOTE;
            break;
        case ',':
            symbolType = COMMA;
            break;
        case ';':
            symbolType = SEMICOLON;
            break;
        case '.':
            symbolType = PERIOD;
            break;
        case '+':
            symbolType = PLUS;
            break;
        case '-':
            symbolType = MINUS;
            break;
        case '*':
            symbolType = ASTERISK;
            break;
        case '/':
            symbolType = SLASH;
            break;
        case ':':
            PEEK;                           // 这里不能用 get！
                                            //  如果是普通的冒号，用 get 还必须 putback！
                                            // 我们设 ch 这个变量的目的就是不必读完了再退回去，
                                            //  如果不用 ch，全文使用 peek，效率不如 ch 高
                                            // 所以这里采用 ch 变量，而且避免用 putback!
            if (ch == '=') {
#ifdef STAGE_LEXICAL
                ADD(ch);
#endif // STAGE_LEXICAL
                GET;                        // 如果是赋值符，再把等号取出来
                symbolType = ASSIGN;
            } else
                symbolType = COLON;         // 对普通的冒号，无须再读下一个字符
            break;
        case '=':
            symbolType = EQUAL;
            break;
        case '<':
            PEEK;                           // 同上，这里必须用 peek 而不用 get！
            switch (ch) {
            case '=':  
#ifdef STAGE_LEXICAL
                ADD(ch);
#endif // STAGE_LEXICAL
                GET;                        // 注意这个 get
                symbolType = LESS_EQUAL;
                break;
            case '>':  
#ifdef STAGE_LEXICAL
                ADD(ch);
#endif // STAGE_LEXICAL
                GET;                        // 注意这个 get
                symbolType = INEQUAL;
                break;
            default:
                symbolType = LESS;          // 注意这里没有 get
                break;
            }
            break;
        case '>':
            PEEK;                           // 同上，这里不能用 get!
            if (ch == '=') {
#ifdef STAGE_LEXICAL
                ADD(ch);
#endif // STAGE_LEXICAL
                GET;                        // get!
                symbolType = GREATER_EQUAL;
            } else
                symbolType = GREATER;       // 没有 get!
            break;
        case EOF:                           // 读至文件结尾
            symbolType = FINISH;
            break;
        default:                            // 本函数不处理单独字符和字符串
                                            //  如果读到的是其他字符，一定是非法字符
            symbolType = ILLEGAL;
            break;
        }
        // 以上，switch 块出来之后，ch 为处理过的字符
        GET;  // 此处需要再 get 一个新的字符备用
    }                                
}

// 读取一个字符串，不含双引号，遇到非法字符或双引号截止，
//  将截止之前的部分存入 token.symbol，可能为空串
//  将symbolType 置为 STRING
void LexicalAnalyzer::nextString() {
    token.symbol = &strGot; // 令 token.symbol 指向新读的字符串
    strGot.clear();         // 清空字符串以备用       
    while (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126)) {
        ADD(ch);
        GET;
    }
    // 这里，循环跳出时，ch 是一个新的未处理的字符，逻辑正确
#ifdef STAGE_LEXICAL
    symbolType = STRING;
#else // !STAGE_LEXCIAL
    if (ch != '\"') {           // 读到非法字符
        symbolType = ILLEGAL;   // 返回 ILLEGAL 类型
        strGot.clear();
        ADD(ch);                // 将非法的字符存入 token 中，
                                //  此时 token 中只有这一个非法字符
        GET;                    // 不要忘了再读一个备用！！！
    } else
        symbolType = STRING;    // 正常，返回 STRING
#endif // STAGE_LEXICAL
}

// 获取下一个 char 类型的字符，
// 如果是合法的 char，将 symbolType 置为 CHARATER，
//  并将其 ASCII 值存入 token.number
// 如果不是合法的 char，将 symbolType 置为 ILLEGAL
void LexicalAnalyzer::nextChar() {
    while (isspace(ch))
        GET;                    // 跳过空白符
#ifdef STAGE_LEXICAL
    token.symbol = &strGot;
    strGot.clear();
    ADD(ch);
#else // !STAGE_LEXICAL
    token.number = ch;          // 将字符的 ASCII 值存入 token.number
#endif // STAGE_LEXCIAL
    if (isalnum(ch))            // 是合法字符，即字母或数字字符
        symbolType = CHARATER;
    else                        // 非法字符
        symbolType = ILLEGAL;
    GET;                        // get 一个新的字符备用！！
}