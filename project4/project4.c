#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

sem_t mutex, full, empty;
int* state;
int numProducer, numConsumer, timeSleep;
FILE* fp;
char* Descrip[7] = {"Producing items", "Consuming items", "Placing items", "Fetching items",
                    "Waiting for positions", "Waiting for products", "Waiting for entering buffer"};


typedef struct _Parameter
{
		int threadNum;
}Parameter;

void* producer(void* param){
		int err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != 0) printf("Cannot set the cancel state of the thread\n");
		err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		if (err != 0) printf("Cannot set the cancel type of the thread\n");

		Parameter* fparam = (Parameter*)param;
		int threadNum = fparam->threadNum;
		buffer_item item;
		while(1){
			state[threadNum] = 1;
		//	pthread_testcancel();
			sleep(rand()%2);
		//	pthread_testcancel();
			item = (buffer_item)rand()%1000;
			state[threadNum] = 5;
			sem_wait(&empty);
			state[threadNum] = 7;
			sem_wait(&mutex);
			state[threadNum] = 3;
			insertItem(item);
			printf("The producer inserted the item %d\n", item);
			sleep(1);
			sem_post(&mutex);
			sem_post(&full);
		}
}

void* consumer(void* param){
		int err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != 0)	printf("Cannot set the cancel state of the thread\n");
		err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		if (err != 0)	printf("Cannot set the cancel type of the thread\n");

		Parameter* fparam = (Parameter*)param;
		int threadNum = fparam->threadNum;
		buffer_item* item = (buffer_item*)malloc(sizeof(buffer_item));
		while(1){
				state[threadNum] = 2;
			//	pthread_testcancel();
				sleep(rand()%2);
			//	pthread_testcancel();
				state[threadNum] = 6;
				sem_wait(&full);
				state[threadNum] = 7;
				sem_wait(&mutex);
				state[threadNum] = 4;
				removeItem(item);
				printf("The consumer consumed the item %d\n", *item);
				sleep(1);
				sem_post(&mutex);
				sem_post(&empty);
		}
		free(item);
}


void printState(){
		printf("\n");
		fprintf(fp, "\n");
		printf("The number of producer		The state of producer\n");
		fprintf(fp, "The number of producer		The state of producer\n");

		for (int i=0; i<numProducer; i++){
				printf("	%d				%d %s\n",i,state[i], Descrip[state[i]-1]);
				fprintf(fp, "	%d				%d %s\n",i,state[i], Descrip[state[i]-1]);
		}
		printf("The number of consumer		The state of consumer\n");
		fprintf(fp, "The number of consumer		The state of consumer\n");
		for(int j=numProducer; j<numProducer+numConsumer; j++){
				printf("	%d				%d %s\n",j,state[j], Descrip[state[j]-1]);
				fprintf(fp, "	%d				%d %s\n",j,state[j], Descrip[state[j]-1]);
		}
		printf("The state of buffer is			%d (<=0 Occupied; >0 Not Occupied)\n",mutex);
		fprintf(fp, "The state of buffer is			%d\n",mutex);

}



void SignalHandler(int sigNum){
		printState();
		signal(SIGINT, SignalHandler);
}



int main(int argc, char** argv){
		signal(SIGINT, SignalHandler);
		if (argc != 5){
				printf("The number of parameters is not correct\n");
				return -1;
		}

		numProducer = atoi(argv[1]);
		numConsumer = atoi(argv[2]);
		timeSleep = atoi(argv[3]);
		state = (int*)malloc((numProducer + numConsumer)*sizeof(int));
		//File* fp;
		if ((fp = fopen(argv[4],"w")) == NULL){
				printf("cannot open the file\n");
				return -1;
		}

		
		sem_init(&mutex, 0, 1);
		sem_init(&empty, 0, BUFFER_SIZE-1);
		sem_init(&full, 0, 0);

		pthread_t tid[numProducer+numConsumer];
		int err;
		for (int i=0; i<numProducer; i++){
				Parameter* args = (Parameter*)malloc(sizeof(Parameter));
				args->threadNum = i;
				err = pthread_create(&tid[i], NULL, producer, (void*)args);
				if(err != 0){
						printf("cannot create the thread %d\n",i+1);
				}
		}
		
		for (int j=numProducer; j<numProducer+numConsumer; j++){
				Parameter* args = (Parameter*)malloc(sizeof(Parameter));
				args->threadNum = j;
				err = pthread_create(&tid[j], NULL, consumer, (void*)args);
				if(err != 0){
						printf("cannot create the thread %d\n",j+1);
				}
				
				
		}


		int timeleft = timeSleep;
		while(timeleft > 0){
				timeleft = sleep(timeleft);
		}
		printf("Time is over, ending the simulation\n");
		for (int i=0; i<numProducer+numConsumer; i++){
				err = pthread_cancel(tid[i]);
				//printf("error is %d\n",err);
				if(err != 0){
						printf("cannot cancel thread %d\n", i+1);
				}

				//pthread_join(tid[i], NULL);

		}
		for (int j=0; j<numProducer+numConsumer;j++){
			pthread_join(tid[j], NULL);
		}
		fclose(fp);

				

		
}
		

