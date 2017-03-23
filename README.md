# Author
Afke Holgersen

## Locks Implementations
As an assignment for an Operating Systems class,  4 locks were implemented in C and compared with the pthread locks from the pthread library: 

- Spinlock test-and-set
- Spinlock test-and-test-and-set 
- Mutex (exponential backoff) lock
- Queue (Ticket) lock 

All locks are recursive safe, meaning the same thread can only release a lock if it is locked the same number of times. 


## Compile Instruction

after cloning, cd into project directory

make

./mylocks -t #threads -i #Iterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid

testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock


Number of threads, iterations, operations inside and outside locks and what lock(s) to test can be set by user.
