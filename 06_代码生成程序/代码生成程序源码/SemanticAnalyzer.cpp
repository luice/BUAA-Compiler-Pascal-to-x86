/*
    SemanticAnalyzer.cpp
    语义分析器的实现
*/
#include "SemanticAnalyzer.h"

SemanticAnalyzer &SemanticAnalyzer::initialSemanticAnalyzer(
    ErrorHandler &eh, SymbolTableManager &stm, Quadruples &q, LexicalAnalyzer &la) {
    static SemanticAnalyzer theSemanticAnalyzer(eh, stm, q, la);
    return theSemanticAnalyzer;
}

SemanticAnalyzer::SemanticAnalyzer(ErrorHandler &eh, SymbolTableManager &stm, 
                                   Quadruples &q, LexicalAnalyzer &la):
                    errorHandler(eh), table(stm), quadruples(q), lexicalAnalyzer(la) {
    temporaryCount = 0;
}

#define ADDMAP(st, ee)  errorHandler.addEndmap(SymbolType::st, ErrorEnd::ee)
#define DELMAP(st)      errorHandler.removeEndmap(SymbolType::st)

#define TOKEN_SYMBOL    (*lexicalAnalyzer.token.symbol)     // string 类型的 token
#define TOKEN_NUMBER    (lexicalAnalyzer.token.number)      // int 类型的 token
#define SYMBOL_TYPE     (lexicalAnalyzer.symbolType)        // 符号的 SymbotType 类型值
                                            // 定义宏，简化对符号和类型获取    

#define NEXTSYM lexicalAnalyzer.nextSymbol()

#define ERROUT  errorHandler.printError
#define WARNOUT errorHandler.printWarning

string SemanticAnalyzer::newTemporary() {
    temporaryCount++;
    return stringPlusInt("t_", temporaryCount);
}

void SemanticAnalyzer::programEndmapInit() {
    ADDMAP(FINISH, PROGRAM);
}

void SemanticAnalyzer::blockEndmapInit() {
    ADDMAP(SEMICOLON, BLOCK);
}

void SemanticAnalyzer::blockEndmapClear() {
    DELMAP(SEMICOLON);
}

void SemanticAnalyzer::constDeclareEndmapInit() {
    ADDMAP(VAR, CONST_DECLARE);
    ADDMAP(FUNCTION, CONST_DECLARE);
    ADDMAP(PROCEDURE, CONST_DECLARE);
    ADDMAP(BEGIN, CONST_DECLARE);
    ADDMAP(SEMICOLON, CONST_DECLARE);
}

void SemanticAnalyzer::constDeclareSkip() {
    if (SYMBOL_TYPE == SymbolType::SEMICOLON)
        NEXTSYM;
}

void SemanticAnalyzer::constDeclareEndmapClear() {
    DELMAP(VAR);
    DELMAP(FUNCTION);
    DELMAP(PROCEDURE);
    DELMAP(BEGIN);
    DELMAP(SEMICOLON);
}

void SemanticAnalyzer::constDefineEndmapInit() {
    ADDMAP(COMMA, CONST_DEFINE);
}

void SemanticAnalyzer::constDefineEndmapClear() {
    DELMAP(COMMA);
}

void SemanticAnalyzer::constDefine(const string &id, BasicType type, int value) {
    table.ste = new STE;
    table.ste->name = id;
    table.ste->type = IdentifierType::CONST;
    table.ste->valueType = type;
    table.ste->multiInt = value;
    table.insert();
}

void SemanticAnalyzer::charEndmapInit() {
    ADDMAP(SINGLE_QUOTE, CHAR);
}

void SemanticAnalyzer::charSkip() {
    if (SYMBOL_TYPE == SymbolType::SINGLE_QUOTE)
        NEXTSYM;
}

void SemanticAnalyzer::charEndmapClear() {
    DELMAP(SINGLE_QUOTE);
}

void SemanticAnalyzer::stringEndmapInit() {
    ADDMAP(DOUBLE_QUOTE, STRING);
}

void SemanticAnalyzer::stringEndmapClear() {
    DELMAP(DOUBLE_QUOTE);
}

void SemanticAnalyzer::stringSkip() {
    if (SYMBOL_TYPE == SymbolType::DOUBLE_QUOTE)
        NEXTSYM;
}

string &SemanticAnalyzer::stringDefine(const string &str) {
    static int count = 0;
    static string name;
    name = stringPlusInt("string_", ++count);
    table.addString(name, str);
    return name;
}

void SemanticAnalyzer::varDeclareEndmapInit() {
    ADDMAP(FUNCTION, VAR_DECLARE);
    ADDMAP(PROCEDURE, VAR_DECLARE);
    ADDMAP(BEGIN, VAR_DECLARE);
}

void SemanticAnalyzer::varDeclareEndmapClear() {
    DELMAP(FUNCTION);
    DELMAP(PROCEDURE);
    DELMAP(BEGIN);
}

void SemanticAnalyzer::varDefineEndmapInit() {
    ADDMAP(SEMICOLON, VAR_DEFINE);
}

void SemanticAnalyzer::varDefineEndmapClear() {
    DELMAP(SEMICOLON);
}

void SemanticAnalyzer::varDefine(vector<string> &names, BasicType bType) {
    for (vector<string>::iterator it = names.begin(); it != names.end(); it++) {
        table.ste = new STE;
        table.ste->name = *it;
        table.ste->type = IdentifierType::VAR;
        table.ste->valueType = bType;
        table.insert();
    }
}

void SemanticAnalyzer::arrayDefine(vector<string> &names, BasicType bType, int lim) {
    for (vector<string>::iterator it = names.begin(); it != names.end(); it++) {
        table.ste = new STE;
        table.ste->name = *it;
        table.ste->type = IdentifierType::ARRAY;
        table.ste->valueType = bType;
        table.ste->multiInt = lim;
        table.insert();
    }
}

void SemanticAnalyzer::procedureDeclareEndmapInit() {
    ADDMAP(SEMICOLON, PROCEDURE_DECLARE);
    ADDMAP(FUNCTION, PROCEDURE_DECLARE);
    ADDMAP(PROCEDURE, PROCEDURE_DECLARE);
    ADDMAP(BEGIN, PROCEDURE_DECLARE);
}

void SemanticAnalyzer::procedureDeclareEndmapClear() {
    DELMAP(SEMICOLON);
    DELMAP(FUNCTION);
    DELMAP(PROCEDURE);
    DELMAP(BEGIN);
}

void SemanticAnalyzer::functionDeclareEndmapInit() {
    ADDMAP(SEMICOLON, FUNCTION_DECLARE);
    ADDMAP(FUNCTION, FUNCTION_DECLARE);
    ADDMAP(PROCEDURE, FUNCTION_DECLARE);
    ADDMAP(BEGIN, FUNCTION_DECLARE);
}

void SemanticAnalyzer::functionDeclareEndmapClear() {
    DELMAP(SEMICOLON);
    DELMAP(FUNCTION);
    DELMAP(PROCEDURE);
    DELMAP(BEGIN);
}

void SemanticAnalyzer::procedureHeadEndmapInit() {
    ADDMAP(CONST, PROCEDURE_HEAD);
    ADDMAP(VAR, PROCEDURE_HEAD);
    ADDMAP(PROCEDURE, PROCEDURE_HEAD);
    ADDMAP(FUNCTION, PROCEDURE_HEAD);
    ADDMAP(BEGIN, PROCEDURE_HEAD);
}

void SemanticAnalyzer::procedureHeadEndmapClear() {
    DELMAP(CONST);
    DELMAP(VAR);
    DELMAP(PROCEDURE);
    DELMAP(FUNCTION);
    DELMAP(BEGIN);
}

void SemanticAnalyzer::procedureDefine(const string &procname, vector<Argument> &args) {
    table.ste = new STE;
    table.ste->name = procname;
    table.ste->type = IdentifierType::PROCEDURE;
    if (!args.empty())
        table.defineArguments(table.ste, args);         // 处理参数表
    table.insert();
}

void SemanticAnalyzer::functionHeadEndmapInit() {
    ADDMAP(CONST, FUNCTION_HEAD);
    ADDMAP(VAR, FUNCTION_HEAD);
    ADDMAP(PROCEDURE, FUNCTION_HEAD);
    ADDMAP(FUNCTION, FUNCTION_HEAD);
    ADDMAP(BEGIN, FUNCTION_HEAD);
}

void SemanticAnalyzer::functionHeadEndmapClear() {
    DELMAP(CONST);
    DELMAP(VAR);
    DELMAP(PROCEDURE);
    DELMAP(FUNCTION);
    DELMAP(BEGIN);
}

void SemanticAnalyzer::functionDefine(const string &funcname, 
                                      vector<Argument> &args, BasicType type) {
    table.ste = new STE;
    table.ste->name = funcname;
    table.ste->type = IdentifierType::FUNCTION;
    table.ste->valueType = type;
    if (!args.empty())
        table.defineArguments(table.ste, args);         // 处理参数表
    table.insert();
}

void SemanticAnalyzer::formalParameterEndmapInit() {
    ADDMAP(SEMICOLON, FORMAL_PARAMETER);
    ADDMAP(RIGHT_BRACKET, FORMAL_PARAMETER);
}

void SemanticAnalyzer::formalParameterEndmapClear() {
    DELMAP(SEMICOLON);
    DELMAP(RIGHT_BRACKET);
}

void SemanticAnalyzer::formalParameterAdd(vector<string> &names, BasicType type, 
                                          bool vary, vector<Argument> &args) {
    for (vector<string>::iterator it = names.begin(); it != names.end(); it++)
        args.push_back(Argument(*it, type, vary));
}

void SemanticAnalyzer::statementEndmapInit() {
    ADDMAP(SEMICOLON, STATEMENT);
    ADDMAP(BEGIN, STATEMENT);
    ADDMAP(FOR, STATEMENT);
    ADDMAP(DO, STATEMENT);
    ADDMAP(IF, STATEMENT);
    ADDMAP(READ, STATEMENT);
    ADDMAP(WRITE, STATEMENT);
}

void SemanticAnalyzer::statementEndmapClear() {
    DELMAP(SEMICOLON);
    DELMAP(BEGIN);
    DELMAP(FOR);
    DELMAP(DO);
    DELMAP(IF);
    DELMAP(READ);
    DELMAP(WRITE);
}

void SemanticAnalyzer::assignToArray(const string &arrname,
                                            const string &offset, const string &value,
                                            BasicType arraytype, BasicType valuetype) {
    quadruples.addTail(new Quadruple(Operator::ARAS, arrname, offset, value));
    cut(arraytype, valuetype, value);
}

void SemanticAnalyzer::assign(const string &name, const string &value,
                                     BasicType nametype, BasicType valuetype) {
    quadruples.addTail(new Quadruple(Operator::ASS, value, "", name));
    cut(nametype, valuetype, value);
}

void SemanticAnalyzer::negative(const string &oper1, string &dest) {
    dest = newTemporary();
    quadruples.addTail(new Quadruple(Operator::NEG, oper1, "", dest));
}

void SemanticAnalyzer::addition(const string &oper1, const string &oper2, string &dest) {
    dest = newTemporary();
    quadruples.addTail(new Quadruple(Operator::ADD, oper1, oper2, dest));
}

void SemanticAnalyzer::subtraction(const string &oper1, const string &oper2, string &dest) {
    dest = newTemporary();
    quadruples.addTail(new Quadruple(Operator::SUB, oper1, oper2, dest));
}

void SemanticAnalyzer::multiplicatioin(const string &oper1, const string &oper2, string &dest) {
    dest = newTemporary();
    quadruples.addTail(new Quadruple(Operator::MUL, oper1, oper2, dest));
}

void SemanticAnalyzer::division(const string &oper1, const string &oper2, string &dest) {
    dest = newTemporary();
    quadruples.addTail(new Quadruple(Operator::DIV, oper1, oper2, dest));
}

void SemanticAnalyzer::termEndmapInit() {
    ADDMAP(PLUS, TERM);
    ADDMAP(MINUS, TERM);
}

void SemanticAnalyzer::termEndmapClear() {
    DELMAP(PLUS);
    DELMAP(MINUS);
}

void SemanticAnalyzer::factorEndmapInit() {
    ADDMAP(ASTERISK, FACTOR);
    ADDMAP(SLASH, FACTOR);
}

void SemanticAnalyzer::factorEndmapClear() {
    DELMAP(ASTERISK);
    DELMAP(SLASH);
}

void SemanticAnalyzer::intToString(int n, string &str) {
    str = stringPlusInt("n_", n);
}

void SemanticAnalyzer::assignFromArray(const string &arrname,
                                              const string &offset, string &value) {
    value = newTemporary();
    quadruples.addTail(new Quadruple(Operator::ASAR, arrname, offset, value));
}

void SemanticAnalyzer::callFunction(const string &name, string &value) {
    value = newTemporary();
    quadruples.addTail(new Quadruple(Operator::CALF, name, "", value));
}

void SemanticAnalyzer::callProcedure(const string &name) {
    quadruples.addTail(new Quadruple(Operator::CALP, name, "", ""));
}

void SemanticAnalyzer::cut(BasicType desttype, BasicType valuetype, const string &value) {
    if (desttype == BasicType::CHAR && valuetype == BasicType::INTEGER) {
        WARNOUT(ErrorType::INT_TO_CHAR);
        quadruples.addTail(new Quadruple(Operator::CUT, "", "", value));
    }
}

void SemanticAnalyzer::functionAssign(const string &name, const string &value,
                                             BasicType functype, BasicType valuetype) {
    if (functype == BasicType::CHAR && valuetype == BasicType::INTEGER) {
        string t = newTemporary();
        assign(t, value, functype, valuetype);
        quadruples.addTail(new Quadruple(Operator::FAS, t, "", name));
    } else
        quadruples.addTail(new Quadruple(Operator::FAS, value, "", name));
}

void SemanticAnalyzer::label(const string &lab) {
    quadruples.addTail(new Quadruple(Operator::LAB, lab, "", ""));
}

void SemanticAnalyzer::funcprocStart() {
    label(table.encodePosition());
}

void SemanticAnalyzer::funcprocReturn() {
    quadruples.addTail(new Quadruple(Operator::RTN, "", "", ""));
}

void SemanticAnalyzer::push(const string &value) {
    quadruples.addTail(new Quadruple(Operator::PUSH, value, "", ""));
}

void SemanticAnalyzer::passParameter(const string &value, 
                                            BasicType formaltype, BasicType realtype) {
    if (formaltype == BasicType::CHAR && realtype == BasicType::INTEGER) {
        string t = newTemporary();
        assign(t, value, formaltype, realtype);
        push(t);
    } else
        push(value);
}

void SemanticAnalyzer::compare(const string &value1, const string &value2) {
    quadruples.addTail(new Quadruple(Operator::CMP, value1, value2, ""));
}


void SemanticAnalyzer::dissatisfyJump(SymbolType op, const string &lab) {
    Operator jmpop;
    switch (op) {
    case SymbolType::LESS:
        jmpop = Operator::JGE;
        break;
    case SymbolType::LESS_EQUAL:
        jmpop = Operator::JG;
        break;
    case SymbolType::GREATER:
        jmpop = Operator::JLE;
        break;
    case SymbolType::GREATER_EQUAL:
        jmpop = Operator::JL;
        break;
    case SymbolType::EQUAL:
        jmpop = Operator::JNE;
        break;
    case SymbolType::INEQUAL:
        jmpop = Operator::JE;
        break;
    default:
        ERROUT(ErrorType::NO_RELATION, "\a");
        exit(-2441);
        break;
    }
    quadruples.addTail(new Quadruple(jmpop, "", "", lab));
}

void SemanticAnalyzer::jumpAnyway(string lab) {
    quadruples.addTail(new Quadruple(Operator::JMP, "", "", lab));
}

string &SemanticAnalyzer::newLabel() {
    return newLabel("");
}

string &SemanticAnalyzer::newLabel(const string &forehead) {
    static string newlab;
    labelCount++;
    newlab = forehead;
    newlab += stringPlusInt("_label_", labelCount);
    return newlab;
}

void SemanticAnalyzer::satisfyJump(SymbolType op, const string &lab) {
    Operator jmpop;
    switch (op) {
    case SymbolType::LESS:
        jmpop = Operator::JL;
        break;
    case SymbolType::LESS_EQUAL:
        jmpop = Operator::JLE;
        break;
    case SymbolType::GREATER:
        jmpop = Operator::JG;
        break;
    case SymbolType::GREATER_EQUAL:
        jmpop = Operator::JGE;
        break;
    case SymbolType::EQUAL:
        jmpop = Operator::JE;
        break;
    case SymbolType::INEQUAL:
        jmpop = Operator::JNE;
        break;
    default:
        ERROUT(ErrorType::NO_RELATION, "\a");
        exit(-2441);
        break;
    }
    quadruples.addTail(new Quadruple(jmpop, "", "", lab));
}

void SemanticAnalyzer::readChar(const string &name) {
    quadruples.addTail(new Quadruple(Operator::RC, "", "", name));
}

void SemanticAnalyzer::readInteger(const string &name) {
    quadruples.addTail(new Quadruple(Operator::RI, "", "", name));
}

void SemanticAnalyzer::writeString(const string &name) {
    quadruples.addTail(new Quadruple(Operator::WS, name, "", ""));
}

void SemanticAnalyzer::writeInteger(const string &value) {
    quadruples.addTail(new Quadruple(Operator::WI, value, "", ""));
}

void SemanticAnalyzer::writeChar(const string &ch) {
    quadruples.addTail(new Quadruple(Operator::WC, ch, "", ""));
}

void SemanticAnalyzer::newline() {
    quadruples.addTail(new Quadruple(Operator::NL, "", "", ""));
}