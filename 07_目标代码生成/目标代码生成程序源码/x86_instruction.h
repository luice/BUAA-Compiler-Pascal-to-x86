/*
    x86_instruction.h
    生成格式化的指令。
*/

#ifndef X86_INSTRUCTION__H
#define X86_INSTRUCTION__H

#include <fstream>
#include <string>
using std::ofstream;
using std::string;
using std::endl;

inline string intToString(int n) {
    char num[20];
    sprintf(num, "%d", n);
    return num;
}

inline string registerToAddress(const string &registerName) {
    return "[" + registerName + "]";
}

inline void MOV(ofstream &file, const string &oper1, const string &oper2) {
    file << "\tmov\t" << oper1 << ", " << oper2 << endl;
}

inline void PUSH(ofstream &file, const string &oper) {
    file << "\tpush\t" << oper << endl;
}

inline void POP(ofstream &file, const string &oper) {
    file << "\tpop\t" << oper << endl;
}

inline void ADD(ofstream &file, const string &oper1, const string &oper2) {
    file << "\tadd\t" << oper1 << ", " << oper2 << endl;
}

inline void SUB(ofstream &file, const string &oper1, const string &oper2) {
    file << "\tsub\t" << oper1 << ", " << oper2 << endl;
}

inline void IMUL(ofstream &file, const string &oper1, const string &oper2) {
    file << "\timul\t" << oper1 << ", " << oper2 << endl;
}

inline void CDQ(ofstream &file) {
    file << "\tcdq" << endl;
}

inline void IDIV(ofstream &file, const string &oper) {
    file << "\tidiv\t" << oper << endl;
}

inline void NEG(ofstream &file, const string &dest) {
    file << "\tneg\t" << dest << endl;
}

inline void AND(ofstream &file, const string &dest, const string &oper) {
    file << "\tand\t" << dest << ", " << oper << endl;
}

inline void LAB(ofstream &file, const string &label) {
    file << label << ": " << endl;
}

inline void PROC(ofstream &file, const string &procname) {
    file << procname << "\tproc" << endl;
}

inline void ENDP(ofstream &file, const string &procname) {
    file << procname << "\tendp" << endl << endl;
}

inline void END(ofstream &file, const string &main) {
    file << "end\t" << main << endl;
}

inline void CMP(ofstream &file, const string &oper1, const string &oper2) {
    file << "\tcmp\t" << oper1 << ", " << oper2 << endl;
}

inline void JG(ofstream &file, const string &label) {
    file << "\tjg\t" << label << endl;
}

inline void JGE(ofstream &file, const string &label) {
    file << "\tjge\t" << label << endl;
}

inline void JL(ofstream &file, const string &label) {
    file << "\tjl\t" << label << endl;
}

inline void JLE(ofstream &file, const string &label) {
    file << "\tjle\t" << label << endl;
}

inline void JE(ofstream &file, const string &label) {
    file << "\tje\t" << label << endl;
}

inline void JNE(ofstream &file, const string &label) {
    file << "\tjne\t" << label << endl;
}

inline void JMP(ofstream &file, const string &label) {
    file << "\tjmp\t" << label << endl;
}

inline void CALL(ofstream &file, const string &label) {
    file << "\tcall\t" << label << endl;
}

inline void RET(ofstream &file) {
    file << "\tret" << endl;
}

inline void LEAVE(ofstream &file) {
    file << "\tleave" << endl;
}

inline void SHL(ofstream &file, const string &oper1, const string &oper2) {
    file << "\tshl\t" << oper1 << ", " << oper2 << endl;
}

#endif // !X86_INSTRUCTION__H
