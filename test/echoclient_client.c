#include "syscall.h"
#define MAX_LENGTH 1024
int main()
{
    char* recv;
    int client1, client2, client3, client4;
    int port = 1234;
    char *server_ip = "127.0.0.1";
    char *buffer;
    int res1, res2, res3, res4, result;
    int i;
    client1 = SocketTCP();
    client2 = SocketTCP();
    client3 = SocketTCP();
    client4 = SocketTCP();

    res1 = Connect(client1, server_ip, port);
    res2 = Connect(client2, server_ip, port);
    res3 = Connect(client3, server_ip, port);
    res4 = Connect(client4, server_ip, port);

//Client 1 
    if(res1<0)
    {
        PrintString("Client 1 cannot connect to server\n");
    }
    else
    {
        PrintString("----------------Client 1--------------------\n");

        PrintString("Input message to send: \n");
        ReadString(buffer,MAX_LENGTH);
        if(Send(client1,buffer, 1024)<0)
        {
            PrintString("Cannot write");
        }
          
        result = Receive(client1, recv, 1024);
        if (result< 0) 
        {
            PrintString("Cannot read");
        }

        PrintString("\nReceived from server: \n ");
        PrintString(recv);
        PrintString("\n");
        CloseSock(client1);
    }
// Client 2
    if(res2<0)
    {
        PrintString("Client 2 cannot connect to server\n");
    }
    else
    {
        PrintString("----------------Client 2--------------------\n");

        PrintString("Input message to send: \n");
        ReadString(buffer,MAX_LENGTH);
        if(Send(client2,buffer, 1024)<0)
        {
            PrintString("Cannot write");
        }
          
        result = Receive(client2, recv, 1024);
        if (result< 0) 
        {
            PrintString("Cannot read");
        }

        PrintString("\nReceived from server: \n ");
        PrintString(recv);
        PrintString("\n");  
        CloseSock(client2);
    }
//Client 3
    if(res3<0)
    {
        PrintString("Client 1 cannot connect to server\n");
    }
    else
    {
        PrintString("----------------Client 3--------------------\n");

        PrintString("Input message to send: \n");
        ReadString(buffer,MAX_LENGTH);
        if(Send(client3,buffer, 1024)<0)
        {
            PrintString("Cannot write");
        }
          
        result = Receive(client3, recv, 1024);
        if (result< 0) 
        {
            PrintString("Cannot read");
        }

        PrintString("\nReceived from server: \n ");
        PrintString(recv);
        PrintString("\n");  
        CloseSock(client3); 
    }
//Client 4
    if(res4<0)
    {
        PrintString("Client 1 cannot connect to server\n");
    }
    else
    {
        PrintString("----------------Client 4--------------------\n");

        PrintString("Input message to send: \n");
        ReadString(buffer,MAX_LENGTH);
        if(Send(client4,buffer, 1024)<0)
        {
            PrintString("Cannot write");
        }
          
        result = Receive(client4, recv, 1024);
        if (result< 0) 
        {
            PrintString("Cannot read");
        }

        PrintString("\nReceived from server: \n ");
        PrintString(recv);
        PrintString("\n");
        CloseSock(client4);
    }
    Halt();
}