## Nachos System calls for Multiprogramming
# Multiprogramming
- Multiple programs can load into memory at once.
- Must handle releasing memory when the user program terminates.
- The important part is changing the instruction that loads the user programs into memory. Currently, address space allocation assumes that a program is loaded into consecutive segments of memory. Once we support multiprogramming, memory will no longer be contiguous. If we program incorrectly, loading a new program can damage the OS.
- Add synchronization to the routines that create and initialize address spaces so that they can be accessed concurrently by multiple programs. Note that scheduler.cc now saves and restores user machine state on context switches.
# System calls
- Implement the syscall SpaceID Exec(char* name). Exec return -1 if was error and Successful returns the Process SpaceID of the newly created user user program. This is the information that needs to be managed in the Ptable class.
- Implement the syscalls: int Join(SpaceID id) and void Exit(int exitCode). Join will wait and block on a “Process ProcessID ” as noted in its parameter. Join returns the exit code for the process it is blocking on, -1 if the join fails. The exit code parameter is set via the exitCode parameter. Exit returns an exit code to whoever is doing a join: 0 if a program successfully completes, another value if there is an error. A user program can only participate in processes that have been created with the Exec system call. You cannot join other processes or your main process. Use a semaphore to distribute activity between Join and Exit user programs.
- Implement the syscall int CreateSemaphore(char* name, int semval). Create a data structure to store 10 semaphores. System call CreateSemaphore returns 0 if successful, otherwise returns -1.
- Implement the syscall int Wait(char* name), and int Signal(char* name). Both system calls return 0 on success and -1 on failure. Errors can occur if the user uses the wrong semaphore name or the semaphore has not been created.
- The current version of the “Exec” system call does not provide any way for the user program to pass parameters or arguments to the newly created address space. UNIX does allow this, for instance, to pass in command line arguments to the new address space.
# Test program
- Install a simple shell program to test the system calls installed above, and at least two other utility programs such as UNIX cat and cp. The shell receives one command at a time from the user via the console and executes the program accordingly, then runs the command by invoking the kernel system call Exec. The UNIX program csh is an example of a shell.
- The shell should "join" each program and wait until the program terminates. When the Join function returns, show the exit code if it is not 0.
