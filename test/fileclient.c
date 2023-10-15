#include "syscall.h"

int main()
{
    int src, dst;
    int length;
    int socketID;
    char srcFile[32];
    char destFile[32];
    char content[1024];
    char* serverIP = "127.0.0.1";
    int port = 1234;
    int res;
    PrintString("Input source file : ");
    ReadString(srcFile,32);
    PrintString("Input destination file : ");
    ReadString(destFile,32);

    //Open source file in read-only mode
    src = Open(srcFile,1);

    if(src == -1)
    {
        PrintString("Source file do not exist!");
        Halt();
    }
    else
    {
        //Get the length of source file
        length = Seek(-1, src);
        //Back to the start of source file
        Seek(0,src);

        //Read the source file content and get number of byte read 
        length = Read(content,length,src);

        //Close source file
        Close(src);

        //Create socket 
        socketID = SocketTCP();

        if(socketID == -1)
        {
            PrintString("Cannot create socket !\n");
            Halt();
        }
        else
        {
            //Connect to server
            if(Connect(socketID, serverIP, port) < 0)
            {
                PrintString("Cannot connect to server\n");
                Halt();
            }
            else
            {
                //Send file content to server
                if(Send(socketID, content, 1024) < 0)
                {
                    PrintString("Cannot write to socket\n");
                }
                else
                {
                    //Read file content after uppercase from server
                    if(Receive(socketID, content, 1024) < 0)
                    {
                        PrintString("Cannot read from socket\n");
                    }
                    else
                    {
                        //Open destination file in read-write mode
                        dst = Open(destFile,0);
                        //If destination file did not exist then create new file
                        if(dst == -1)
                        {
                            dst = Create(destFile);
                            dst = Open(destFile,0);
                        }
                        //Write source file content to destination file
                        Write(content, length, dst);
                        PrintString("Write successfully.");
                        //Close destination file
                        Close(dst);
                        Halt();
                    }
                }
            }
        }

    }
}