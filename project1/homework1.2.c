#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



static void *thr_fn(int x)
{
	if (x < 4)
	{
		printf("Hello world from thread name%d\n", x+1);
		
	}
	pthread_exit(0);
}

int main()
{
	pthread_t tid[4];
	int i;
	for (i = 0; i<4; i++){
		int err = pthread_create(&tid[i], NULL, thr_fn, i);
		if (err != 0){
			printf("cannot create the thread %d\n", i+1);
			exit(1);
		}
		pthread_join(tid[i],NULL);
		//sleep(1);
	}
}
	
	
