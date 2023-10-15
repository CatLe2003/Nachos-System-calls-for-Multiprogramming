#include "syscall.h"
#define MAX_LENGTH 255
char s[256];
int main() {
    int src, dst;
    int len;
    char srcFile[256];
    char destFile[256];

    PrintString("  Input source file: ");
    ReadString(srcFile, MAX_LENGTH);

    PrintString("Input destination file: ");
    ReadString(destFile, MAX_LENGTH);

    src = Open(srcFile, 1);
    if (src != -1) 
    {
        len = Seek(-1, src);
        Seek(0, src);
        len = Read(s, len, src);
        Close(src);
        PrintString("\nNumber of characters read: ");
        PrintNum(len);

        dst = Open(destFile, 0);
        if (dst == -1) {
            if (Create(destFile) == -1) 
            {
                PrintString("\nCannot create destination file !!");
                Halt();
            }
            else {
                PrintString("\nNew file created !");
                dst = Open(destFile, 0);
            }
        }
        Write(s, len, dst);
        PrintString("\nSuccessfully copy file !!");
        Close(dst);
    }
    else
        PrintString("\nNon-exist source file !!\n");
    //Halt();
}