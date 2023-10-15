
/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "syscall.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "main.h"
#define MAXLENGTH 1024
#define FILENAME_MAXLEN 32


char *User2System(int virtAddr, int limit) 
{
    int i; 
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1];

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++) {
        kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, char *buffer, int length) 
{
    if (length <= 0)
        return length;
    int i = 0;
    int oneChar = 0;
    do 
    {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < length && oneChar != 0);

    return i;
}

void IncreasePC()
/* Modify return point */
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}
// -------------------------------------------------------------------

void SysHalt() 
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
    return op1 + op2;
}
void SysReadString(int length) 
{
    char *buffer = new char[length + 1];
    if (buffer == NULL) 
    {
        return;
    }

    int i = 0;
    char tmp;

    while (i < length) 
    {
        tmp = kernel->synchConsoleIn->GetChar();
        if (tmp == '\0' || tmp == '\n') 
        {
            buffer[i] = '\0';
            break;
        }
        buffer[i++] = tmp;
    }

    // Overflow control
    if (tmp != '\0' && tmp != '\n') 
    {
        while (true) 
        {
            tmp = kernel->synchConsoleIn->GetChar();
            if (tmp == '\0' || tmp == '\n')
                break;
        }
    }

    int virtAddr = kernel->machine->ReadRegister(4);
    System2User(virtAddr, buffer, length);
    delete[] buffer;
}

void SysPrintString() 
{
    int i = 0;
    int virtAddr = kernel->machine->ReadRegister(4);

    //Read string input from User
    char *buffer = User2System(virtAddr, MAXLENGTH);

    //Write to console word by word until the end of string
    while (buffer[i] != '\0') 
    {
        kernel->synchConsoleOut->PutChar(buffer[i++]);
    }
    delete[] buffer;
}
void SysPrintNum() 
{
    bool isNegative = false;
    int number = kernel->machine->ReadRegister(4);
    int i = 0;
    char *buffer = new char[12 + 1];

    if (number == -2147483648 - 1) 
    {
        strcpy(buffer, "-2147483648");
        buffer[strlen(buffer)] = '\0';
        i = 0;
        while (buffer[i] != '\0') 
        {
            kernel->synchConsoleOut->PutChar(buffer[i++]);
        }
    }
    else 
    {
        if (number != 0) 
        {
            if (number < 0) 
            {
                isNegative = true;
                number = -number;
            }
            int num = 0;
            while (number != 0) 
            {
                num = number % 10;
                buffer[i] = num + '0';
                number = number / 10;
                i++;
            }
            if (isNegative) 
            {
                buffer[i] = '-';
                i++;
            }
        }
        else 
        {
            buffer[i] = '0';
            i++;
        }
        buffer[i] = '\0';
        while (i >= 0) {
            kernel->synchConsoleOut->PutChar(buffer[i--]);
        }
    }
    delete[] buffer;
}

void SysCreate() 
{
    int result;
    char *fileName;
    int virtAddr = kernel->machine->ReadRegister(4);

    fileName = User2System(virtAddr, FILENAME_MAXLEN + 1);
    
    //Check whether there is error on reading file name
    if (strlen(fileName) == 0 || fileName == NULL) 
    {
        result = -1;
    }
    else if (kernel->fileSystem->Create(fileName) == true) 
    {
        result = 0;
    }
    else 
    {
        result = -1;
    }
    kernel->machine->WriteRegister(2, result);
    delete[] fileName;
}

void SysRemove() 
{
    int result;
    int virtAddr = kernel->machine->ReadRegister(4);
    char *fileName = User2System(virtAddr, MAXLENGTH);

    //Check whether there is error on reading file name
    if (fileName == NULL || strlen(fileName) == 0) 
    {
        kernel->machine->WriteRegister(2, -1);
        if (fileName)
            delete[] fileName;
        return;
    }
    //Check whether file is opening
    OpenFile *opening = kernel->fileSystem->GetFileDescriptor(fileName);
    if (opening != NULL) 
    {
        // If file is opening then cannot remove file
        printf("File is opening !! Close file to remove");
        kernel->machine->WriteRegister(2, -1);
    }
    else 
    {
        result = kernel->fileSystem->Remove(fileName);
        kernel->machine->WriteRegister(2, result);
    }
    delete[] fileName;
}

void SysOpen() 
{
    int virtAddr = kernel->machine->ReadRegister(4);
    int fileType = kernel->machine->ReadRegister(5);

    char *fileName;
    fileName = User2System(virtAddr, FILENAME_MAXLEN + 1);
    //Check whether there is error on reading file name
    if (fileName == NULL || strlen(fileName) == 0) 
    {
        kernel->machine->WriteRegister(2, -1);
        if (fileName)
            delete[] fileName;
        return;
    }

    //Get the file ID if file have been opening
    int isOpening = kernel->fileSystem->GetFileDescriptorID(fileName);
    if (isOpening != -1) 
    {
        kernel->machine->WriteRegister(2, isOpening);
        delete[] fileName;
        return;
    }

    //If file have not opened yet, find the new FileID 
    int freeIndex = kernel->fileSystem->FileDescriptorFree();
    if (freeIndex == -1) 
    {
        kernel->machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    switch (fileType) 
    {
    // 0: read-write, 1: read-only
        case 0: 
        {
            if (kernel->fileSystem->AssignFileDescriptor(freeIndex, fileName, fileType) != NULL) 
            {
                //cerr << freeIndex << "\n";
                kernel->machine->WriteRegister(2, freeIndex);
            }
            else 
            {
                kernel->machine->WriteRegister(2, -1);
            }
        break;
        }
        case 1: 
        {
            if (kernel->fileSystem->AssignFileDescriptor(freeIndex, fileName, fileType) != NULL) 
            {
            kernel->machine->WriteRegister(2, freeIndex);
            }
            else 
            {
                kernel->machine->WriteRegister(2, -1);
            }
            break;
        }
        default: 
        {
            //Invalid file type
            kernel->machine->WriteRegister(2, -1);
            break;
        }
    }
    delete[] fileName;
}
void SysClose() 
{
    OpenFileId fileId = kernel->machine->ReadRegister(4);
    if (fileId >= 0 && fileId < MAX_FILES) 
    {
        if (kernel->fileSystem->RemoveFileDescriptor(fileId)) 
        {
            kernel->machine->WriteRegister(2, 0);
        }
        else 
        {
            kernel->machine->WriteRegister(2, -1);
        }
    }
    else 
    {
        kernel->machine->WriteRegister(2, -1);
    }
}

int SysRead(int virtAddr, int length, OpenFileId fileID) 
{
    //Number of bytes read
    int cnt = -1;
    //cerr << fileID << "\n";
    switch (fileID) 
    {
        // Console Input
        case 0: 
        {
            // Similar to ReadString
            char *buffer = new char[length + 1];
            char ch;
            int i;
            if (buffer == NULL)
                return -1;

            for (i = 0; i < length; ++i) 
            {
                ch = kernel->synchConsoleIn->GetChar();
                if (ch == '\0' || ch == '\n') 
                {
                    buffer[i] = '\0';
                    break;
                }
                buffer[i] = ch;
            }
            System2User(virtAddr, buffer, i);
            
            //Overflow Control
            if (ch != '\0' && ch != '\n') 
            {
                while (true) 
                {
                ch = kernel->synchConsoleIn->GetChar();
                if (ch == '\0' || ch == '\n')
                    break;
                }
            }
            cnt = i;
            delete[] buffer;
            break;
        }
        // Console Output
        case 1: 
        {
            break;
        }
        default: 
        {
            char *buffer = new char[length + 1];
            
            //Read the content into buffer and get the bytes into cnt 
            cnt = kernel->fileSystem->GetFileDescriptor(fileID)->Read(buffer, length);
            //cerr << "cnt = "<< cnt <<"\n";
            if (cnt > 0) 
            {
                buffer[cnt] = 0;
                System2User(virtAddr, buffer, cnt);
            }
            else 
            {
                cerr<<"File is empty!\n";
            }
            delete[] buffer;
            break;
        }
    }
    return cnt;
}

int SysWrite(int virtAddr, int length, OpenFileId fileID) 
{
    // Number of bytes read
    int cnt = -1;

    switch (fileID) 
    {
        // Console Input 
        case 0: 
        {
            break;
        }
        // Console Output 
        case 1: 
        {
            // Similar to PrintString
            char *buffer = User2System(virtAddr, length);
            int i = 0;

            if (buffer == NULL) 
            {
                return -1;
            }

            // Respectively write word to console until the end of string
            while (buffer[i] != '\0') 
            {
                kernel->synchConsoleOut->PutChar(buffer[i++]);
            }
            cnt = i;
            delete[] buffer;
            break;
        }
        default: 
        {
            char *buffer = User2System(virtAddr, length);
            OpenFile *filePtr = kernel->fileSystem->GetFileDescriptor(fileID);
            if (buffer == NULL) 
            {
                return -1;
            }
            if (filePtr->GetFileType() != 0) 
            {
               cerr << "Cannot written to read-only file mode!\n";
            }
            else 
            {
            }
            //Write to file 
            cnt = filePtr->Write(buffer, length);
            if (cnt > 0)
                buffer[cnt] = 0;
            delete[] buffer;
        }
    }
    return cnt;
}

int SysSeek(int fileID, int pos) 
{
    OpenFile *filePtr = kernel->fileSystem->GetFileDescriptor(fileID);

    //Check whether file exist
    if (filePtr == NULL) 
    {
        kernel->machine->WriteRegister(2, -1);
        return -1;
    }
    //Get file length
    if (pos == -1)
        pos = filePtr->Length();

    if (pos > filePtr->Length() || pos < 0) 
    {
        cerr <<"Cannot seek !\n";
        pos = -1;
    }
    else 
    {
        filePtr->Seek(pos);
    }
    return pos;
}
int SysSocketTCP()
{
    return kernel->socketTable->SocketTCP();
}
 void SysCloseSocket()
{
     int socketID = kernel->machine->ReadRegister(4);

     if(socketID < 0)
     {
        kernel->machine->WriteRegister(2,-1);
        return;
     }
    
    int res = kernel->socketTable->Close(socketID);
    if(res == 0)
    {
        kernel->machine->WriteRegister(2,-1);
        return;
    }
    kernel->machine->WriteRegister(2,1);
    return;
 }
 int SysConnect(int socketID, char* ip,int port)
 {
    if(socketID <0 || socketID >=MAX_SOCKETS || kernel->socketTable->getInUse(socketID) == 0)
    {
        return -1;
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);

    int result = connect(socketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    return result;
 }
 int SysSend(int socketID, char* buffer, int len)
 {
  int result = -1;

  int n = send(socketID, buffer, len +1, 0);
  if (n > 0) result = n;  

  return result; 
}
int SysRecieve(int socketID, char* buffer, int len, int virtAddr)
{
    int result = -1;
    int n = read(socketID, buffer, len);
    if (n >= 0) 
    {   
        result = n;
    }    
    //cerr << "Buffer read from socket: "<< buffer <<"\n";
    System2User(virtAddr,buffer,len);
    return result;
}
SpaceId SysExec(char* fileName)
{
    if(fileName == NULL)
    {
        delete[] fileName;
        return -1;
    }

    OpenFile* f = kernel->fileSystem->Open(fileName);
    if(f == NULL)
    {
        cerr <<"Cannot open file!\n";
        return -1;
    }

    delete f;

    //Process ID
    SpaceId pID = kernel->pTab->ExecUpdate(fileName);
    delete[] fileName;
    return pID;
}
int SysJoin(int ID)
{
    int res = kernel->pTab->JoinUpdate(ID);
    if(res == -1)
    {
        cerr <<"Error on SysJoin\n";
    }
    else
    {
        //cerr <<"Successfully\n";
    }
    return res;
}
void SysExit(int exitCode)
{
    if(exitCode == 0)
    {
        int res = kernel->pTab->ExitUpdate(exitCode);
        if(kernel->currentThread->space != 0)
        {
            delete kernel->currentThread->space;
        }
        kernel->currentThread->Finish();
        //cerr <<"Successfully\n";
    }
    else
    {
        cerr <<"Error on ec = " << exitCode << " \n";
    }
}
int SysCreateSemaphore(char* name, int semval)
{
    if(name == NULL)
    {
        cerr <<"Name cannot be empty\n";
        delete[] name;
        return -1;
    }
    
    int res = kernel->sTab->Create(name,semval);
    
    if(res == -1)
    {
        cerr <<"Cannot create semaphore\n";
        delete[] name;
        return -1;
    }

    delete[] name;
    return res;
}
int SysWait(char* name)
{
    if(name == NULL)
    {
        cerr <<"Name cannot be empty\n";
        return -1;
    }

    int res = kernel->sTab->Wait(name);

    if(res == -1)
    {
        cerr <<"Semaphore non-existed!\n";
        return -1;
    }
    delete[] name;
    return res;
}
int SysSignal(char* name)
{
    if(name == NULL)
    {
        cerr <<"Name cannot be empty\n";
        return -1;
    }

    int res = kernel->sTab->Signal(name);
    if(res == -1)
    {
        cerr <<"Semaphore non-existed!\n";
        return -1;
    }
    delete[] name;
    return res;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */