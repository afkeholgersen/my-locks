#include "main.h"


void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {
        c++;
    }
}


pthread_mutex_t count_mutex;

void *pthreadMutexTest()
{
  int i;
	int j;
	int k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {

  		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
  		{
  			localCount++;
  		}

  		pthread_mutex_lock(&count_mutex);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
  		{
  			c++;
  		}
  		pthread_mutex_unlock(&count_mutex);
    }
}

pthread_spinlock_t count_spin_pthread;

void *pthreadSpinTest()
{
  int i;
  int j;
  int k;
  int localCount = 0;
    for(i=0;i<numItterations;i++)
    {

      for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
      {
        localCount++;
      }

      pthread_spin_lock(&count_spin_pthread);
      for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
      {
        c++;
      }
      pthread_spin_unlock(&count_spin_pthread);
    }
}

my_spinlock_t count_spinlockTAS;

void *myspinlockTASTest()
{
  int i;
	int j;
	int k;
	int localCount = 0;
  //printf("pthread_self TAS : %i \n", pthread_self() ) ;
    for(i=0;i<numItterations;i++)
    {
  		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
  		{
  			localCount++;
  		}
  		my_spinlock_lockTAS(&count_spinlockTAS);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
  		{
  			c++;
  		}
  		my_spinlock_unlock(&count_spinlockTAS);

    }
}

my_spinlock_t count_spinlockTTAS;

void *myspinlockTTASTest()
{
  int i;
	int j;
	int k;
	int localCount = 0;
//  printf("pthread_self TTAS : %i \n", pthread_self() ) ;
    for(i=0;i<numItterations;i++)
    {
  		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
  		{
  			localCount++;
  		}
  		my_spinlock_lockTTAS(&count_spinlockTTAS);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
  		{
  			c++;
  		}
  		my_spinlock_unlock(&count_spinlockTTAS);
    }
}
my_mutex_t count_my_mutex;

void *myMutexTest() // exponential backoff
{
  int i;
	int j;
	int k;
	int localCount = 0;
    for(i=0;i<numItterations;i++)
    {

  		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
  		{
  			localCount++;
  		}

  		my_mutex_lock(&count_my_mutex);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
  		{
  			c++;
  		}
  		my_mutex_unlock(&count_my_mutex);
    }

}
my_queuelock_t count_my_queuelock;

void *myQueueTest()
{
  int i;
	int j;
	int k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {

  		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
  		{
  			localCount++;
  		}

  		my_queuelock_lock(&count_my_queuelock);
  		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
  		{
  			c++;
  		}
  		my_queuelock_unlock(&count_my_queuelock);
    }

}

int runTest(int testID)
{ // ignore this:
	if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
	{
		c=0; //long long
		struct timespec start;
		struct timespec stop;
		unsigned long long result; //64 bit integer
    pthread_mutex_init(&count_mutex,NULL);
		pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
		int i;
		int rt;

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
  		if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
  		{
  			printf("Thread creation failed: %d\n", rt);
  			return -1;
  		}

		}

		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
    pthread_mutex_destroy(&count_mutex);
		printf("Threaded Run Pthread (Mutex) Total Count: %llu\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 2) /*Pthread Spinlock*/
	{
		/*Pthread Spinlock goes here*/
    c=0; //long long
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer
    pthread_spin_init(&count_spin_pthread,0);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int i;
    int rt;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &pthreadSpinTest, NULL)) )
      {
        printf("Thread creation failed: %d\n", rt);
        return -1;
      }

    }

    for(i=0;i<numThreads;i++) //Wait for all threads to finish
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pthread_spin_destroy(&count_spin_pthread);
    printf("Threaded Run Pthread (Spin) Total Count: %llu\n", c);
    result=timespecDiff(&stop,&start);
    printf("Pthread Spin time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 3) /*MySpinlockTAS*/
	{
    c=0; //long long
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer

    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int i;
    int rt;
    my_spinlock_init(&count_spinlockTAS);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &myspinlockTASTest, NULL)) )
      {
        printf("Thread creation failed: %d\n", rt);
        return -1;
      }
    }
    for(i=0;i<numThreads;i++) //Wait for all threads to finish
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    my_spinlock_destroy(&count_spinlockTAS);
    printf("Threaded Run My SpinlockTAS Total Count: %llu\n", c);
    result=timespecDiff(&stop,&start);
    printf("My SpinTAS time(ms): %llu\n",result/1000000);


	}


	if(testID == 0 || testID == 4) /*MySpinLock TTAS*/
	{
    c=0; //long long
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer

    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int i;
    int rt;
    my_spinlock_init(&count_spinlockTTAS);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &myspinlockTTASTest, NULL)) )
      {
        printf("Thread creation failed: %d\n", rt);
        return -1;
      }
    }
    for(i=0;i<numThreads;i++) //Wait for all threads to finish
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    my_spinlock_destroy(&count_spinlockTTAS);
    printf("Threaded Run My SpinlockTTAS Total Count: %llu\n", c);
    result=timespecDiff(&stop,&start);
    printf("My SpinTTAS time(ms): %llu\n",result/1000000);

	}


	if(testID == 0 || testID == 5) /*My Mutex TAS*/
	{
    c=0; //long long
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer
    my_mutex_init(&count_my_mutex);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int i;
    int rt;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &myMutexTest, NULL)) )
      {
        printf("Thread creation failed: %d\n", rt);
        return -1;
      }

    }

    for(i=0;i<numThreads;i++) //Wait for all threads to finish
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    my_mutex_destroy(&count_my_mutex);
    printf("Threaded Run My Mutex (exp. backoff) Lock Total Count: %llu\n", c);
    result=timespecDiff(&stop,&start);
    printf("My Mutex (exp. backoff) time(ms): %llu\n",result/1000000);
	}
	if(testID == 0 || testID == 6) /*My Queue Lock*/
	{
    c=0; //long long
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer
    my_queuelock_init(&count_my_queuelock);
    pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
    int i;
    int rt;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(i=0;i<numThreads;i++)
    {
      if( rt=(pthread_create( threads+i, NULL, &myQueueTest, NULL)) )
      {
        printf("Thread creation failed: %d\n", rt);
        return -1;
      }

    }

    for(i=0;i<numThreads;i++) //Wait for all threads to finish
    {
      pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    my_queuelock_destroy(&count_my_queuelock);
    printf("Threaded Run My Queue Lock Total Count: %llu\n", c);
    result=timespecDiff(&stop,&start);
    printf("My Queue Lock time(ms): %llu\n",result/1000000);
	}


		return 0;
}


int testAndSetExample()
{
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%ld\n",test);
	tas(&test);
	printf("Test after atomic OP:%ld\n",test);
}

int processInput(int argc, char *argv[])
{
  /*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/

	//Set default Values First:
	numThreads=4;
	numItterations=1000000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;

	int i=1; //ignore ./mylocks

	while (i<argc-1 ){
    if ( strcmp("-t", argv[i]) == 0 ){
        numThreads=atoi(argv[i+1]);
    }
		if ( strcmp("-i", argv[i]) == 0 ){
        numItterations=atoi(argv[i+1]);
		}
    if ( strcmp("-o", argv[i]) == 0 ){
        workOutsideCS=atoi(argv[i+1]);
		}
    if ( strcmp("-c", argv[i]) == 0 ){
        workInsideCS=atoi(argv[i+1]);
		}
    if ( strcmp("-d", argv[i]) == 0 ){
        testID=atoi(argv[i+1]);
    }
    i++;
	}

  //This function assumes no error- can be in any order, but cant enter: ./mylocks -c -t
	return 0;
}

int main(int argc, char *argv[])
{

	printf("Usage of: %s -t #threads -i #Iterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");

	//testAndSetExample(); //Uncomment this line to see how to use TAS

	processInput(argc,argv);
	runTest(testID);
	return 0;

}
