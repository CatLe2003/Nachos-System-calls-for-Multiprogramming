#ifndef SEM_H
#define SEM_H
#include "synch.h"
class Sem{
public:
    char name[50];
    Semaphore *sem; // Tạo Semaphore để quản lý
public:
    // khởi tạo đối tượng Sem. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm sử dụng
    Sem(char* na, int i);
    ~Sem();
    void wait();
    void signal();
    char* GetName();
};
#endif