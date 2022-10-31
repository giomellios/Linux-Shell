/*
 * GEORGIOS PANAGIOTIS MELLIOS 4416
 *
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<termios.h>
#define true 1
//int flag=0;
pid_t pid;

typedef struct Array{
	int size;
	char **data;
	int current;
}Array;


void print_prompt();

char **input_delimit(char *command,const char delimit[4]);

int pipe_count(char *command);

void execute_command(char *command);

void initArray(Array **a,int size);

void allocate(Array **a);

void process_piped(Array *a,char *command);

void execute_piped(Array *a,char *command);

void signal_handler();

void flow_control();
