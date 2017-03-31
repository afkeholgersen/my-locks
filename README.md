# Author
Afke Holgersen

## Locks Implementations
As an assignment for an Operating Systems class,  4 locks were implemented in C and compared with the pthread locks from the pthread library: 

- Spinlock test-and-set
- Spinlock test-and-test-and-set 
- Mutex (exponential backoff) lock
- Queue (Ticket) lock 

All locks are recursive safe, meaning the same thread can only release a lock if it is locked the same number of times. 


## Compile and Execution Instruction

To execute this program, the user must clone this project to his own repository and access the project directory through his own terminal. 
This can be done by using the command "Cd".

Before execution, compilation can be done by using the "make"command. 

To execute, the user must use the following command to start the program : 
      ./mylocks -t #threads -i #Iterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid
      
The user is responsible for directing operations, numbers of threads and locks intended to test. This can be done through the terminal, and by defining testid. The test id is a code that the user can define by using the following glossary : 
     testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock
    
