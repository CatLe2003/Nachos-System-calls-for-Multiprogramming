

// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "ksyscall.h"
#include "main.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case NoException:
        return;

    case PageFaultException:
        /* A page fault occurs when a program attempts to access
        data or code that is in its address space,
        but is not currently located in the system RAM
        */
        DEBUG(dbgSys, "Invalid page found.\n");
        printf("\n\n Invalid page found");
        SysHalt();
        break;

    case ReadOnlyException:
        /* Read-only is a file attribute which only allows a user to view a file,
        restricting any writing to the file. */
        DEBUG(dbgSys, "Restrict any writing to the read-only page.\n");
        printf("\n\n Restrict any writing to the read-only page.");
        SysHalt();
        break;

    case BusErrorException:
        /*  bus error is a fault raised by hardware, notifying an operating system (OS)
        that a process is trying to access memory that the CPU cannot physically address:
        an invalid address for the address bus, hence the name.
        */
        DEBUG(dbgSys, "Translation resulted invalid physical address.\n");
        printf("\n\n Translation resulted invalid physical address");
        SysHalt();
        break;

    case AddressErrorException:
        /* An addressing error is detected by determining whether
        the target address output on the address path is detected at the memory.
        */
        DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space.\n");
        printf("\n\n Unaligned reference or one that was beyond the end of the address space");
        SysHalt();
        break;

    case OverflowException:
        DEBUG(dbgSys, "Overflow in add or sub type.\n");
        printf("\n\n Overflow in add or sub type.");
        SysHalt();
        break;

    case IllegalInstrException:
        // They arise because not every possible memory value corresponds to a valid instruction.
        DEBUG(dbgSys, "Unimplemented or reserved instruction.\n");
        printf("\n\n Unimplemented or reserved instruction.");
        SysHalt();
        break;

    case NumExceptionTypes:
        DEBUG(dbgSys, "Number exception types.\n");
        printf("\n\n Number exception types");
        SysHalt();
        break;

    case SyscallException: 
    {
        switch (type) 
        {
            case SC_Halt:
            {    
                DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
                printf("\n\n Shutdown, initiated by user program.");
                SysHalt();
                ASSERTNOTREACHED();
                break;
            }
            case SC_Add:
                break;
            case SC_ReadString: 
            {
                int length = kernel->machine->ReadRegister(5);
                if (length > MAXLENGTH || length < 1) 
                {
                    cerr << "\nInvalid string length\n";
                }
                else 
                {
                    SysReadString(length);
                }
                break;
            }
            case SC_PrintString: 
            {
                SysPrintString();
                break;
            }
            case SC_PrintNum: 
            {
                SysPrintNum();
                break;
            }
            case SC_Create: 
            {
                SysCreate();
                break;
            }
            case SC_Remove: 
            {
                SysRemove();
                break;
            }
            case SC_Open: 
            {
                SysOpen();
                break;
            }
            case SC_Close: 
            {
                SysClose();
                break;
            }
            case SC_Read: 
            {
                int virtAddr = kernel->machine->ReadRegister(4);
                int length = kernel->machine->ReadRegister(5);
                OpenFileId fileID = kernel->machine->ReadRegister(6);
                if (length < 0 || fileID < 0 || fileID >= MAX_FILES) 
                {
                    cerr << "Invalid arguments" << type << "\n";
                    kernel->machine->WriteRegister(2,-1);
                }
                else 
                {
                    int n = SysRead(virtAddr, length, fileID);
                    kernel->machine->WriteRegister(2, n);
                }
                break;
            }
            case SC_Write: 
            {
                int virtAddr = kernel->machine->ReadRegister(4);
                int length = kernel->machine->ReadRegister(5);
                OpenFileId fileID = kernel->machine->ReadRegister(6);

                if (length < 1 || fileID < 0 || fileID >= MAX_FILES) 
                {
                    cerr << "Invalid arguments" << type << "\n";
                    kernel->machine->WriteRegister(2, -1);
                }
                else 
                {
                    kernel->machine->WriteRegister(2, SysWrite(virtAddr, length, fileID));
                }
                break;
            }
            case SC_Seek: 
            {
                int pos = kernel->machine->ReadRegister(4);
                int fileID = kernel->machine->ReadRegister(5);
                 if (fileID < 0 || fileID >= MAX_FILES) 
                {
                    cerr << "Invalid inputed arguments" << type << "\n";
                    kernel->machine->WriteRegister(2, -1);
                }
                else if (fileID == 0 || fileID == 1) 
                {
                    kernel->machine->WriteRegister(2, -1);
                }
                else 
                {
                    kernel->machine->WriteRegister(2, SysSeek(fileID, pos));
                }
                break;
            }
            case SC_SocketTCP:
            {
                int socketID = SysSocketTCP();
                if(socketID == -1)
                {
                    //cerr<< "Cannot create socket! \n";
                    kernel->machine->WriteRegister(2,-1);
                    break;
                }
                else
                {
                    //cerr<< "Socket created! \n";
                    kernel->machine->WriteRegister(2, socketID);
                    break;
                }
                ASSERTNOTREACHED();
            }
            case SC_CloseSock:
            {
                SysCloseSocket();
                break;
            }
            case SC_Connect:
            {
                int socketID = kernel->machine->ReadRegister(4);
                int virtAddr = kernel->machine->ReadRegister(5);
                int port = kernel->machine->ReadRegister(6);

                char* IP = User2System(virtAddr,1024);

                int result = SysConnect(socketID,IP,port);
                //cerr <<"Result of connection "<< socketID <<" : "<<result<<"\n";
                kernel->machine->WriteRegister(2,result);
              
                delete[] IP;
                break;
            }
            case SC_Send:
            {
                int socketID = kernel->machine->ReadRegister(4);
                int virtAddr = kernel->machine->ReadRegister(5);
                int length = kernel->machine->ReadRegister(6);

                char* buffer = User2System(virtAddr,1024);
                //cerr << buffer <<"\n";
                int result = SysSend(socketID, buffer, length);
                kernel->machine->WriteRegister(2,result);
                break;
            }
            case SC_Receive:
            {
                int socketID = kernel->machine->ReadRegister(4);
                int virtAddr = kernel->machine->ReadRegister(5);
                int length = kernel->machine->ReadRegister(6);

                char* buffer = User2System(virtAddr,1024);

                int result = SysRecieve(socketID, buffer, length, virtAddr);
                kernel->machine->WriteRegister(2,result);
                break;
            }
            case SC_Exec:
            {
                int virtAddr = kernel->machine->ReadRegister(4);
                char* fileName = User2System(virtAddr, 1024);
                int result = SysExec(fileName);
                kernel->machine->WriteRegister(2,result);
                break;
            }
            case SC_Join:
            {
                int id = kernel->machine->ReadRegister(4);
                int result = SysJoin(id);
                kernel->machine->WriteRegister(2,result);
                break;
            }
            case SC_Exit:
            {
                int exitCode = kernel->machine->ReadRegister(4);
                SysExit(exitCode);
                break;
            }
            case SC_CreateSemaphore:
            {
                int virtAdrr = kernel->machine->ReadRegister(4);
                int semVal = kernel->machine->ReadRegister(5);
                char* name = User2System(virtAdrr,1024);

                int result = SysCreateSemaphore(name,semVal);
                kernel->machine->WriteRegister(2, result);
                break;
            }
            case SC_Wait:
            {
                int virtAddr = kernel->machine->ReadRegister(4);
                char* name = User2System(virtAddr,1024);

                int result = SysWait(name);
                kernel->machine->WriteRegister(2, result);
                break;
            }
            case SC_Signal:
            {
                int virtAddr = kernel->machine->ReadRegister(4);
                char* name = User2System(virtAddr,1024);

                int result = SysSignal(name);
                kernel->machine->WriteRegister(2, result);
                break;
            }
            default:
            cerr
                << "Unexpected system call "
                << type
                << "\n";
            break;
        }
        IncreasePC();
        break;
    }

    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }
}