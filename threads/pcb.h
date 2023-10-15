#ifndef PCB_H
#define PCB_H
#include "synch.h"
#include "thread.h"

extern void StartProcess_2(int id);

class PCB
{
private:
    Semaphore* joinsem;
    Semaphore* exitsem;
    Semaphore* multex;
    int exitcode;
    int numwait;
    Thread* thread;
    char FileName[32];

public:
    int parentID;
    char boolBG;
    PCB();
    PCB(int id = 0);
    ~PCB();
    
   
    int Exec(char *filename, int pid); // Tạo 1 thread mới có tên là filename và process là pid 
    
    int GetID();// Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();// Trả về số lượng tiến trình chờ

    void JoinWait();// 1. Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait();// 4. Tiến trình con kết thúc

    void JoinRelease();// 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();// 3. Cho phép tiến trình con kết thúc

    void IncNumWait();// Tăng số tiến trình chờ
    void DecNumWait();// Giảm số tiến trình chờ

    void SetExitCode(int ec);// Đặt exitcode của tiến trình
    int GetExitCode();// Trả về exitcode

    void SetFileName(char* fn);// Đặt tên của tiến trình
    char* GetFileName();// Trả về tên của tiến trình 
};

#endif