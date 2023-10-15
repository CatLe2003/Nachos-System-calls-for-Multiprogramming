
#include "syscall.h"


int
main()
{
  char fileName[256];
  int openId;
  char tmp;
  PrintString("Input file name to create\n");
  ReadString(fileName, 255);
  PrintString(fileName);
  PrintString("\n");

  if (Create(fileName) == 0) {
    PrintString("Created file, shutting down...\n");
  }
  else {
    PrintString("Error have occurred, shutting down...\n");
  }
  Halt();
}