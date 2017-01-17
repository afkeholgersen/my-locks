For an operating systems class, I implemented the following locks and compared them with the pthread locks: 
- Spinlock test-and-set
- Spinlock test-and-test-and-set 
- Mutex lock (exponential backoff)
- Queue (Ticket) lock 

All of these locks are also recursive safe, meaning the same thread can only release a lock if it was locked the same number of times. 

Make to make into the executable "mylocks", make clean to clear this. Can set the number of threads, and work done inside and outside the lock. 


 
