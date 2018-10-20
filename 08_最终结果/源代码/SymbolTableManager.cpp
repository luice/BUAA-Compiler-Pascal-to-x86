/*
    SymbolTableManager.cpp
    实现符号表的组织和管理
*/
#include "SymbolTableManager.h"
using namespace std;

static STE root;                        // 树形符号表的根，代码最外层代码（整个程序），无名
static map<string, string> stringMap;   // 程序中所有用到的字符串

// 单例模式，假的构造器
SymbolTableManager &SymbolTableManager::initialSymbolTableManager() {
    static SymbolTableManager theSymbolTableManager;
    return theSymbolTableManager;
}

// 真正的构造函数
SymbolTableManager::SymbolTableManager(): ste(NULL), position(&root) {
    root.level = 0;
    root.type = IdentifierType::PROCEDURE;
    root.name = "_main";
    offset = 0;
}

// 将 position 置为 root。
// 当编译工作进行到目标代码生成时，要从头开始查符号表
void SymbolTableManager::home() {
    position = &root;
}

// 将 position 往里推进一层
// 必须保证 block 已经正确插入到符号表中
// 逻辑上，block 必须为函数名或过程名
void SymbolTableManager::goIn(const string &block) {
    map<string, STE*>::iterator it = position->childs->find(block);
    if (it != position->childs->end()) {
        position = it->second;
        offset = position->type == IdentifierType::FUNCTION ? 1 : 0;
    }
}

// 将 position 的 multiInt 更新为 指定值，
// 作为临时变量的个数
void SymbolTableManager::setTempcount(int count) {
    position->multiInt = count;
}

int SymbolTableManager::getTempcount() {
    return position->multiInt;
}

// 将 position 往外退回一层
void SymbolTableManager::goOut() {
    if (position->father != NULL)
        position = position->father;
}

// 将 position 指向 ste
void SymbolTableManager::comeHere() {
    position = ste;
}

// 判断表中当前层是否有这个符号
bool SymbolTableManager::isHere(const string &id) {
    if (position->childs != NULL)
        return position->childs->count(id) > 0 || id == position->name;
    return id == position->name;
}

// 查表，查到返回 true
// 把查到的表项的指针赋给 ste
// 若没有查到，则将 ste 置为 NULL 并返回 false
bool SymbolTableManager::find(const string &id) {
    STE *ps = position->childs != NULL ? position : position->father;
    while (ps != NULL) {
        map<string, STE *>::iterator it = ps->childs->find(id);
        if (it != ps->childs->end()) {
            ste = it->second;
            return true;
        }
        ps = ps->father;
    }
    ste = NULL;
    return false;
}

// 填表, 将 ste 插入到表中当前层（position层）
// 在此之前必须确保 isHere(ste->name) == false
void SymbolTableManager::insert() {
    // 计算 level，函数和过程名比其父节点多一层 level，变量常量等与其父节点同层
    if (ste->type == IdentifierType::FUNCTION || ste->type == IdentifierType::PROCEDURE)
        ste->level = position->level + 1;
    else {
        ste->level = position->level;
        if (ste->type == IdentifierType::VAR && ste->location == 0)
            ste->location = ++offset;
        else if (ste->type == IdentifierType::ARRAY) {
            ste->location = offset + 1;
            offset += ste->multiInt;
        }
    }
    ste->father = position;
    if (position->childs == NULL)
        position->childs = new map<string, STE *>;
    position->childs->insert(pair<string, STE *>(ste->name, ste));
}

static char *pointToStr(const void *point) {
    static char str[9];
    std::sprintf(str, "%08X", point);
    return str;
}

// 把 ste 转化为唯一标识
string &SymbolTableManager::encodeKey() {
    static string idkey;
    idkey.clear();
    switch (ste->type) {
    case IdentifierType::ARRAY:
        idkey.append("a_");
        break;
    case IdentifierType::FUNCTION:
        idkey.append("f_");
        break;
    case IdentifierType::PROCEDURE:
        idkey.append("p_");
        break;
    case IdentifierType::VAR:
    case IdentifierType::ADDR:
        idkey.append("v_");
        break;
    default:
        idkey.append("fuck_");
        break;
    }
    idkey.append(ste->name);
    idkey.append("_");
    idkey.append(pointToStr(ste));
    return idkey;
}

// 把 position 转化为唯一标识
string &SymbolTableManager::encodePosition() {
    static string key;
    if (position == &root) {
        key = "_main";
        return key;
    } else {
        ste = position;
        return encodeKey();
    }        
}

// 把唯一标识转化为 ste
void SymbolTableManager::decodeKey(const string &idkey) {
    if (idkey == "_main")
        ste = &root;
    else {
        string s = idkey.substr(idkey.size() - 8);
        unsigned int res = 0;
        for (int i = 0; i < 8; i++) {
            char ch = s[i];
            res <<= 4;
            if (isdigit(ch))
                ch -= '0';
            else if (isupper(ch))
                ch = ch - 'A' + 10;
            else
                ch = ch - 'a' + 10;
            res |= ch;
        }
        ste = (STE *)res;
        //! std::sscanf(idkey.substr(idkey.size() - 8).c_str(), "%08X", &ste);
    }
}

void SymbolTableManager::addString(const string &name, const string &str) {
    stringMap.insert(pair<string, string>(name, str));
}

map<string, string> &SymbolTableManager::getStringMap() {
    return stringMap;
}

void SymbolTableManager::defineArguments(STE *te, vector<Argument> &args) {
    // 清空原参数表
    if (te->arguments != NULL)
        delete te->arguments;
    te->arguments = new vector<Argument>(args);
    // 确保子节点 map 有效
    if (te->childs == NULL)
        te->childs = new map<string, STE *>;
    // 逐个把参数作为 te 的局部变量
    int loc = -1;
    for (vector<Argument>::iterator it = args.begin(); it != args.end(); it++, --loc) {
        STE *nte = new STE;
        nte->name = it->name;
        if (it->vary)
            nte->type = IdentifierType::ADDR;
        else
            nte->type = IdentifierType::VAR;
        nte->valueType = it->type;
        nte->level = te->level;
        nte->location = loc;
        nte->father = te;
        te->childs->insert(pair<string, STE *>(it->name, nte));
    }
}