#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <errno.h>


//Reading the working directory
char* getCurrentDir()
{
	/*int size = 64;
	char* currentdir = (char*)malloc(size);
	while (1){
		if (getcwd(currentdir, size) == currentdir)
			return currentdir;
		else size *=2;
	}*/
	
	return getcwd(NULL,0);
}

void showHostname_And_Dir()
{
	char *hostname = (char*)malloc(32);
	if (gethostname(hostname, 32))
	{
		printf("Get hostname error\n");
		exit(0);
	}
	struct passwd* mypasswd = getpwuid(getuid());
	char* dir = getCurrentDir();
	printf("%s@%s:%s$\n", mypasswd->pw_name, hostname, dir);
	free(hostname);
	free(dir);
}

void extractOrder(int begin, int end, char* src, char* dst)
{
	for (int i = 0; i<end-begin+1; i++)
	{
		dst[i] = src[begin+i];
	}
}

void resizeOrder(char** order, int resize)
{
	char** new_order = (char**)malloc(sizeof(char*) * resize);
	for (int i=0; i<resize/2; i++){
		new_order[i] = order[i];
		free(order[i]);
	}
	free(order);
	order = new_order;
}
	
char** readOrder(char* buffer, int* order_number)
{
	int size = 32;
	char input = getchar();
	//vector<char**> order(size);
	char** order = (char**)malloc(sizeof(char*) * size);
	int i = 0;
	int order_num = 0;
	for (i=0; input != '\n'; i++)
	{
		buffer[i] = input;
		input = getchar();
	}
	buffer[i] = '\0';
	int begin = 0;
	int end = 0;
	for (int j=0; j < strlen(buffer)+1; j++)
	{
		if (buffer[j] == ' ' || buffer[j] == '\0'){	
			end = (j-1 > 0)? j-1:0;
			if (end >= begin && buffer[begin] != " "){
				order[order_num] = (char*)malloc((end-begin+1)*sizeof(char));
				extractOrder(begin, end, buffer, order[order_num]);
				order_num++;
				if (order_num >= size){
					size *= 2;
					resizeOrder(order, size);
				}
			}
			begin = j+1;

		}
		else continue;
	}
	*order_number = order_num;
	order[order_num] = NULL;
	return order;

}

int isPipe(char** order, int* order_number)
{
	for (int i=0; i<*order_number; i++){
		if (strcmp(order[i], "|") == 0){
			return 1;
		}
	}
	return 0;
}

void pipeCommand(char** order, int* order_number)
{
	int fd[2], status, pid;
	pipe(fd);
	pid = fork();
	if (pid == 0){
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		//printf("this is from pipe\n");
		execv(order[0], NULL);
		//exit(0);
	}
	else{
		waitpid(-1, NULL, 0);
		close(fd[1]);
		pid = fork();
		if (pid == 0){
			dup2(fd[0], STDIN_FILENO);
			execv(order[2], NULL);
			//exit(0);
		}
		else waitpid(-1, NULL, 0);
	}
}

void judgeOrder(char** order, int* order_number)
{
	if (strcmp(order[0], "exit") == 0){
		for (int i=0; i<*order_number; i++){
			free(order[i]);
		}
		free(order);
		printf("Good bye from yuliqiang shell\n");
		exit(0);
	}
	else if(strcmp(order[0], "cd") == 0){
		if (chdir(order[1]) < 0){
			printf("cd : No such file directory\n");
			return;
		}
		else{
			printf("current working directory:%s\n", getcwd(NULL, NULL));
			return;
		}
	}
	else{
		if (access(order[0], F_OK) == 0){
			if (fork() == 0) execv(order[0], order);
			else waitpid(-1, NULL, 0);
		}
		else printf("command not found\n");
	}


}




int main()
{
	int* order_number;	
	char** order;
	printf("yuliqiang shell is initialized\n");
	while(1){
		char* buffer = (char*)malloc(128 * sizeof(char));
		showHostname_And_Dir();
		order = readOrder(buffer, order_number);	
		if (order == NULL) continue;	
		if (isPipe(order, order_number) == 1)   pipeCommand(order, order_number);
		else                               judgeOrder(order, order_number);
		free(buffer);
		free(order);
		*order_number = 0;
	}

	return 0;
}
