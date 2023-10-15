#include "syscall.h"

int main()
{
    int newProc1;
    int newProc2;

    newProc1 = Exec("cat"); 
    newProc2 = Exec("copy"); 
    
    Join(newProc1);
    Join(newProc2);
    
    Halt();
}

