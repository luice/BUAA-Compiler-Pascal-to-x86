/*
    SymbolTableManager.h
    符号表管理器的接口和变量声明
*/
#ifndef SYMBOL_TABLE_MANAGER__H
#define SYMBOL_TABLE_MANAGER__H

#include "type.h"

class SymbolTableManager {
public:
    static bool found;
    static string id;
    static IdentifierType idtype;      
    static BasicType vtype;

    static SymbolTableManager  &initialSymbolTableManager();

};

#endif // !SYMBOL_TABLE_MANAGER__H
