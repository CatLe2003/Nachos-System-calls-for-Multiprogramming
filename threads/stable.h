#ifndef STABLE_H
#define STABLE_H
#include "bitmap.h"
#include "sem.h"
#define MAX_SEMAPHORE 10
class STable{
private:
    Bitmap* bm; // quản lý slot trống
    Sem* semTab[MAX_SEMAPHORE]; // quản lý tối đa 10 đối tượng Sem   
    
public:    
    STable();
    ~STable();
    int Create(char* name, int init);
    int Wait(char* name);
    int Signal(char* name);
    int FindFreeSlot();
};
#endif