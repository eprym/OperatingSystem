#include <stdio.h>

typedef int buffer_item;
#define BUFFER_SIZE 11

buffer_item buffer[BUFFER_SIZE];
int start = 0;
int end = 0;


int insertItem(buffer_item item){
		if((end+1)%BUFFER_SIZE == start){
				printf("Error:The buffer is full\n");
				return -1;
		}
		else{
				end = (end+1) % BUFFER_SIZE;
				buffer[end] = item;
				return 0;
		}
}

int removeItem(buffer_item* item){
		if (start == end){
				printf("Error:The buffer is empty\n");
				return -1;
		}
		else{
				start = (start + 1) % BUFFER_SIZE;
				*item = buffer[start];
				return 0;
		}
}


