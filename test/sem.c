
#include "syscall.h"
int main() 
{
    int res = CreateSemaphore("sem", 1);
    if (res == -1) {
        PrintString("CreateSemaphore failed\n");
        return 1;
    }
    Wait("sem");
    Signal("sem");
    PrintString("Semaphore test successfully\n");
    Halt();
}