/*
    SymbolTableManager.h
    符号表管理器的接口和变量声明
*/
#ifndef SYMBOL_TABLE_MANAGER__H
#define SYMBOL_TABLE_MANAGER__H

#include "type.h"

class SymbolTableManager {
public:
    STE *ste;           // 表项指针，用于符号表管理器与外界交换信息。
                        // 查表时，将查到的表项放在这里；
                        // 填表时，将待填的表项放在这里。

    static SymbolTableManager &initialSymbolTableManager();     // 单例模式
    void home();
    void goIn(const string &);                                  // 将 position 往里推进一层
    void setTempcount(int count);                               // 将 position 的 multicount
                                                                // 设为 count
    int getTempcount();                                         // 得到 position 的 multicount
    void goOut();                                               // 将 position 往外退回一层
    void comeHere();                                            // 将 position 指向 ste
    bool isHere(const string &);                                // 判断表中当前层是否有这个符号
    bool find(const string &);                                  // 查表，查到返回 true，
                                                                // 若查到，将 ste 指向它
    void insert();                                              // 填表
    void defineArguments(STE *te, vector<Argument> &args);// 定义参数表，将参数再复制一份作为局部变量
    string &encodeKey();                                        // 把 ste 转化为唯一标识
    string &encodePosition();                                   // 把 position 转化为唯一标识
    void decodeKey(const string &);                             // 把唯一标识转化为 ste
    void addString(const string &name, const string &str);      // 把 str 命名为 name 并存入表中
    map<string, string> &getStringMap();                        // 返回 stringMap 给代码生成使用
    map<string, STE*> *getChilds() { return position->childs; } // 返回 childs 指针

private:
    STE *position;      // 当前编译位置
    int offset;         // 给局部变量分配空间
    SymbolTableManager();                                       // 构造函数
    SymbolTableManager(const SymbolTableManager &);             // 拷贝构造
    SymbolTableManager &operator= (const SymbolTableManager &); // = 运算符
};

#endif // !SYMBOL_TABLE_MANAGER__H
