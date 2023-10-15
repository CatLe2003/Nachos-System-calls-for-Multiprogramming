#include "syscall.h"
#define MAX_LENGTH 1024
char file1[256];
char file2[256];

char s1[1024];
char s2[1024];

int main() {
    int result, fileID, pos;
    PrintString("Input file 1: ");
    ReadString(file1,MAX_LENGTH);

    PrintString("Input file 2: ");
    ReadString(file2,MAX_LENGTH);
  
    fileID = Open(file1, 1);
    if (fileID == -1)
    {
        PrintString("\nCannot open file 1 !");
        Halt();
        return 0;
    }

    //Read content of file 2
    Read(s1,MAX_LENGTH,fileID);
    Close(fileID);

    fileID = Open(file2, 1);
    if (fileID == -1)
    {
        PrintString("\nCannot open file 1 !");
        Halt();
        return 0;
    }

    Read(s2,MAX_LENGTH,fileID);
    Close(fileID);
    
    // resConcatenate.txt contain the result
    fileID = Open("resConcatenate.txt", 0);
    if (fileID != -1)
    {
        Close(fileID);
        Remove("resConcatenate.txt");
    }
    fileID = Create("resConcatenate.txt");
    Close(fileID);
    
    if (fileID == -1) 
    {
        PrintString("\nCannot create result file\n");
        Halt();
    }
    PrintString(s1);
    PrintString(s2);
    fileID = Open("resConcatenate.txt", 0);

    pos = 0;
    //Count the size of file 1 content
    while(s1[pos] != '\0') 
    {
        pos++;
    }
    //Write file 1 content into result file
    Write(s1, pos, fileID);  

    pos = 0;
    //Count the size of file 2 content
    while(s2[pos] != '\0') 
    {
        pos++;
    }
    //Write file 2 content into result file
    Write(s2, pos, fileID);

    if (Close(fileID) == 0)
    {
        PrintString("\nConcatenate file successfully !\n");
    }
    else
    {
        PrintString("\nCannot concatenate file !");
    }
    
    Halt();
}