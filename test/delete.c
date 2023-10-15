#include "syscall.h"
#define MAX_LENGTH 255
char fileName[256];
int main() 
{
    int result;
    PrintString("Input file to delete: ");
    ReadString(fileName, MAX_LENGTH);

    result = Remove(fileName);

    if (result == 0) 
    {
        PrintString("\nCannot delete file !");
    }
    if (result == 1) 
    {
        PrintString("\nSuccessfully delete file");
        PrintString(fileName);
    }
    Halt();
}