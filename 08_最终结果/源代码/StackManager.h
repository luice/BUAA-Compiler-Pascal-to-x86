/*
    StackManager.h
    运行栈管理器
    管理 esp 指针，提供一些小函数。最后的功能是：
        使用者需要对 esp 进行的操作，以及与 esp 相关的寻址工作，都给它管。
        使用者只需要在使用前初始化它一，然后只需要给它提供相当对栈底的偏移，
            就能获取相对 esp 的地址
    要想让 StackManager 正常工作，所有压栈、出栈、以及对 esp 指针的加减操作
        都必须使用这里提供的接口。
*/

#ifndef STACK_MANAGER__H
#define STACK_MANAGER__H

#include <string>
#include <fstream>
using std::string;
using std::ofstream;

/////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!
// 这个类是经过重构的！！！
// 之前运行栈只用 esp 一个指针维护，但是这样行不通！！！
// 重构之后，改为用 esp 和 ebp 两个指针维护
// 为了与之前的版本兼容，这里如果 baseoffset 或者 offset 为负，则会自动再减一个
// 原因如下：
// 在返回地址的下面，会多存一个 ebp，这时把 offset 记为 0，为了寻址方便
// 但是如果偏移为负，则因为多了一个 ebp，所以应该多加一个 4.
/////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!
class StackManager {
public:
    StackManager(ofstream &file);
    // 执行完这条指令后，相当于 esp 指向这个栈的栈底，
    // 相当于进入了一个新的栈帧，以后的操作都在这个新的栈上，
    // 此后，给的偏移也都是相对这个栈的栈底而言的。
    void initialStack();    
    // 生成 push 指令, push value
    void push(const string &value);         
    // 生成 pop 指令, pop regist
    void pop(const string &registerName);   
    // 生成以下指令： sub esp, n; 其中 n = count * 4
    void allocN(int count);         
    // 生成以下指令： add esp, n; 其中 n = count * 4
    void freeN(int count);          
    // 返回当前 esp 相对于栈底的偏移，注意以 4 个字节为一个单位，
    //  例如返回 n，则相当于 esp == ebp + n * 4
    int getOffset();                
    // 返回一个形如 [ebp-n] 的字符串，
    //  其中 n == offset * 4
    string getAddress(const string &dest, int offset, int baseoffset = 0);
    // 获取变量在运行栈上的地址，将地址存入 dest 寄存器
    void getLocationOnStack(const string &dest, int offset, int baseoffset = 0);

private:
    ofstream &targetFile;
    int pushCount;
};

#endif // !STACK_MANAGER__H
