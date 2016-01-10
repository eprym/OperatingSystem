#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define rowA 3
#define colA 2
#define colB 3

struct parameter
{
		int row;
		int col;
};

int matrixA[rowA][colA];
int matrixB[colA][colB];
int matrixC[rowA][colB];

void* thr_fn(void* p)
{
		struct parameter* fp = (struct parameter*)p;
		int row = fp->row;
		int col = fp->col;
		matrixC[row][col] = 0;
		for(int i=0; i<colA; i++){
				matrixC[row][col] += matrixA[row][i] * matrixB[i][col];
		}
		pthread_exit(0);
}

void printMatrix(int** matrix, int row, int col)
{
		for(int i=0; i<row; i++)
		{
				for(int j=0; j<col;j++)
				{
						printf("%d",*((int*)matrix + col*i + j));
						printf(" ");
				}
				printf("\n");
		}
}

int main(int argc, char** argv)
{
		//float matrixA[rowA][colA];
		//float matrixB[colA][colB];
		//float matrixC[rowA][colB];
		if (argc != 3)
		{
				printf("Not enough parameters\n");
				return -1;
		}
		FILE* fp;
		int input;
		if ((fp = fopen(argv[1],"r")) == NULL)
		{
				printf("cannot open the file A\n");
				return -1;
		}
		for (int i=0; i<rowA; i++)
		{
				for(int j=0; j<colA;j++)
				{
						fscanf(fp, "%d", &input);
						matrixA[i][j] = input;
						//printf("%d\n",matrixA[i][j]);
						
				}
		}
		fclose(fp);

		if ((fp = fopen(argv[2], "r")) == NULL)
		{
				printf("cannot open the file B\n");
				return -1;
		}
		for (int i=0; i<colA; i++)
		{
				for(int j=0; j<colB;j++)
				{	
						fscanf(fp, "%d", &input);
						matrixB[i][j] = input;		
				}
		}
		fclose(fp);
		
		printf("The matrix A is\n");
		printMatrix(matrixA, rowA, colA);
		printf("The matrix B is\n");
		printMatrix(matrixB, colA, colB);

		
		pthread_t tid[rowA*colB];
		for(int k=0; k<rowA*colB; k++)
		{
				struct parameter* p = (struct parameter*)malloc(sizeof(struct parameter)) ;
				p->row = k/colB;
				p->col = k%colB;
				int err = pthread_create(&tid[k], NULL, thr_fn, (void*)p);
				if(err != 0)
				{
						printf("cannot create the thread %d\n", k+1);
						exit(1);
				}
		}

		for(int k=0; k<rowA*colB; k++)
		{
				pthread_join(tid[k],NULL);
		}
		printf("The matrix C is\n");
		printMatrix(matrixC, rowA, colB);
		return 0;
}

		

				



