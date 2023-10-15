#include "syscall.h"


int main() 
{
    int pid;
    pid = Exec("cat");
    if (pid < 0) 
    {
       PrintString("Failed!\n");
    } else
        Join(pid);
}