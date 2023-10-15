#include "syscall.h"
#define MAX_LENGTH 255

int main() 
{
    int fileID;
    int len;
    char fileName[] = "a.txt";
    char s[1024];

    // PrintString("Input file to cat: ");
    // ReadString(fileName, MAX_LENGTH);
    
    fileID = Open(fileName, 1);

    if (fileID != -1) 
    {
         len = Seek(-1, fileID);
  
        Close(fileID);
        fileID = Open(fileName, 1);
         Seek(0, fileID);
       
        Read(s, len + 1, fileID);

        
        Close(fileID);
        
        PrintString("   File content: \n");
        PrintString(s);
    }
    else
        PrintString("Cannot open file to read!");
    //Halt();
}