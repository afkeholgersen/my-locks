/*
 * NAME, etc.
 *
 * sync.c
 *
 */
#define _REENTRANT
#include <stdio.h>
#include "sync.h"
#define MIN_DELAY 1000
#define MAX_DELAY 100000 //usec
/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
  lock->locked=0 ;
  lock->count=0 ;
  lock->threadID= 0;
  return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
  return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{

  if ( (lock->threadID ) == pthread_self() ){//only the original owner of the lock can unlock!
      if ( (lock-> count) == 1 || (lock-> count) == 0 ){
        lock->count=0;
        lock->threadID=0 ; //reset thread id to indicate anyone can now take the lock
        lock->locked= 0;
      //  printf("one ");
        return 0; // successfully achieved lock
      }
      else{ //has to be locked even amount of times that it was unlocked
        lock->count--;
      //  printf("two ");
        return 0;
      }
    }
    else {
      return-1;
    }


}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
    while ( tas(&(lock->locked)) &&  (lock->threadID) != pthread_self() ){  } //spin while "you" dont own the lock and it is unlocked
    if (lock->threadID == pthread_self() ){
      lock->count = (lock->count) +1  ;
      return 0;
    }
    lock->count=1 ;
    lock->threadID = pthread_self() ; //got the lock
    return 0;
 }


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{ //does not return anything until they get the lock
  while(1){
      while( (lock->locked) == 1  && (lock->threadID) != pthread_self() ){ } //see if lock available first - want value 1 if available
      if ( tas(&(lock->locked)) == 0  ) {//lock looks available-try to aquire
        //you get the lock!!
        lock->count =1;
        lock->threadID=pthread_self();
        return 0;
      }
      else if (lock->threadID == pthread_self() ){ //you already own the lock
        lock->count ++ ;
        return 0;
      }
      // else you dont own the lock- keep spinnin
    }
}
int my_spinlock_trylock(my_spinlock_t *lock)
{
  if ( ! tas( & lock->locked) ){
    lock->threadID ==pthread_self();
    return 0;
  }
  else return-1;
}


/*
 * Mutex routines (exponential backoff)
 */

int my_mutex_init(my_mutex_t *lock)
{
  lock->locked=0 ;
  lock->threadID=0;
  lock->count=0;
  return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
  return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
  if ( (lock->threadID ) == pthread_self() ){//only the original owner of the lock can unlock!
      if ( (lock-> count) == 1 || (lock-> count) == 0 ){
        lock->count=0;
        lock->threadID=0 ; //reset thread id to indicate anyone can now take the lock
        lock->locked= 0;
        return 0; // successfully achieved lock
      }
      else{ //has to be locked even amount of times that it was unlocked
        lock->count--;
        return 0;
      }
    }
  else {
    return-1;
  }
}

int my_mutex_lock(my_mutex_t *lock)
{ //this is ttas as well

  int delay=MIN_DELAY;
  while(1){
      while( (lock->locked) == 1 && (lock->threadID) != pthread_self() ){ } //see if lock available first - want value 1 if available
      if ( tas(&(lock->locked)) == 0 ) {//lock looks available-try to aquire
        lock->count =1;
        lock->threadID=pthread_self();
        return 0;
      }
      else if (lock->threadID == pthread_self() ){ //you already own the lock
        lock->count ++ ;
        return 0;
      }
      usleep( rand()% delay ) ;
      if( delay< MAX_DELAY){
        delay=2*delay;
      }
    }
}

int my_mutex_trylock(my_mutex_t *lock)
{
  if ( ! tas( & lock->locked) ){
    lock->threadID ==pthread_self();
    return 0;
  }
  else return-1;
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
  lock->next_ticket=0;
  lock->now_serving=0;
  lock->threadID=0;
  lock->count=0;
  return 0;

}

int my_queuelock_destroy(my_queuelock_t *lock)
{
  return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
    if ( (lock->threadID ) == pthread_self() ){//only the original owner of the lock can unlock!
      if ( (lock-> count) == 1 || (lock-> count) == 0 ){
      lock->count =0;
      lock->threadID=0;
      lock->now_serving++;
      //this is the unlock
      return 0;
      }
    else{
      //dont unlock- just decrease the count- has to be unlocked same amount of times its locked
      lock->count --;
      return 0;
    }
  }
  else{
    return-1;
  }
}

int my_queuelock_lock(my_queuelock_t *lock)
{
  if ( lock->threadID == pthread_self() ){
    lock->count ++ ;
    return 0; //you already own the lock
  }
  else{
    unsigned long my_ticket; //fetch and add- pseudo code on wikipedia
    my_ticket= __sync_fetch_and_add(&(lock->next_ticket), 1); // cas(& (lock->next_ticket), lock->next_ticket, lock->next_ticket+1  ); //
    while( lock->now_serving != my_ticket && (lock->threadID) != pthread_self() )  { }  //while not on your thread yet
    lock->count =1;
    lock->threadID=pthread_self();
    return 0;

  }

}

int my_queuelock_trylock(my_queuelock_t *lock)
{
  unsigned long my_ticket;
  my_ticket= __sync_fetch_and_add(&(lock->next_ticket), 1);
  if ( lock->now_serving != my_ticket && (lock->threadID) != pthread_self() ){
    //you already own the lock or its your turn now
    if (lock->threadID== pthread_self() ){
      lock->count ++ ;
      return 0; //you already own the lock
    }
    else{
      lock->count ++;
      lock->threadID== pthread_self();
      return 0;
    }
  }
  else return-1;
}
