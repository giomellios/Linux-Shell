/*
 * GEORGIOS PANAGIOTIS MELLIOS 4416
 *
 *
 */
#include "functions.h"

void print_prompt(){
	char cwd[100];

	char *user=getlogin();
	getcwd(cwd,sizeof(cwd));
	printf("%s@csd345sh%s$ ",user,cwd);
}
/* function to delimit the input into strings */
char ** input_delimit(char *command,const char delimit[4]){
	char **retval;
	int i=0;
	retval=(char**)malloc(sizeof(char*));//
	retval[i]=strtok(command,delimit);
	while(retval[i]!=NULL){
		i++;
		retval=(char**)realloc(retval,sizeof(char*)*(i+1));
		retval[i]=strtok(NULL,delimit);
	}	
	return retval;	
}
int pipe_count(char *command){
	int i=0;
	int cnt=0;

	while(command[i]!='\0'){
		if(command[i]=='|')
			cnt++;
		i++;
	}
	return cnt;
}

void execute_command(char *command){
	char **args=input_delimit(command," \"");
	if(strcmp(args[0],"cd")==0){
		char * home=getenv("HOME");
		if(args[1]!=NULL){
			if(chdir(args[1])!=0){
				perror("Could not change directory");
			}
		}else{
			if(chdir(home)!=0)
				perror("Could not change directory");
		}
	}else{
		if(strcmp(args[0],"echo")==0){
			char *env;
			for(int i=1;args[i]!=NULL;i++){
				if(strchr(args[i],'$')!=NULL){
					args[i]=strtok(args[i],"$");
					if(getenv(args[i])!=NULL)
						env=strdup(getenv(args[i]));	
					else
						env=NULL;
					if(env!=NULL){
						args[i]=malloc(strlen(env)+1);
						strcpy(args[i],env);	
					}else{
						strcpy(args[i],"");
					}

				}
			}

		}
		if(execvp(args[0],args)<0){
			perror("not executed");
		}
	}
}
void initArray(Array **a,int size){
	
	*a=(Array*)malloc(size*sizeof(Array));
	//(*a)->data=(char**)malloc(sizeof(char*));
	(*a)->size=size;
	(*a)->current=0;

}
void allocate(Array **a){
	if((*a)->current==(*a)->size){
		(*a)->size*=2;
		(*a)->data=(char**)realloc((*a)->data,(*a)->size*sizeof(char*));
	}

}

void process_piped(Array *a,char *command){
	char *temp=strdup(command);
	int len=pipe_count(command);
	char **pipes=input_delimit(temp,"|");
	for(int i=0;i<len+1;i++){ 
		//a[a->current].data=(char**)malloc(strlen(pipes[i]+1));
		a[a->current].data=input_delimit(pipes[i]," ");
		a->current++;
	}
}



void execute_piped(Array *a,char *command){
	int len=a->current;
	int pipes[2*len];//we need 2 pipes for each command, one for input and one for output
	for(int i=0;i<len;i++){
		if(pipe(pipes+i*2)<0)
			perror("Error on pipe");

	}
	int i=0;
	int j=0;
	int pid;
	while(a[i].data!=NULL){
		pid=fork();
		if(pid==0){
			if(a[i+1].data!=NULL){
				if(dup2(pipes[j+1],1)<0){
					perror("1Error on dup2");
				}

			}
			if(j!=0){
				if(dup2(pipes[j-2],0)<0){
					printf("%d\n",j);
					perror("Error on dup2");
				}
			}
			for(int k=0;k<2*a->current;k++){
				close(pipes[k]);
			}
	
			if(execvp(a[i].data[0],a[i].data)<0){
				perror("Could not execute");
			}
		}else if(pid<0)
			perror("Error on fork");
		i++;
		j+=2;
	}
	for(i=0;i<2*a->current;i++)
		close(pipes[i]);
	for(i=0;i<a->current;i++)
		wait(NULL);

}

void signal_handler(){
	printf("\nProcess stopped %d\n",pid);
	kill(pid,SIGTSTP);
	char *input="";
	size_t sz=0; //for getline
	Array *a;
	int spid;
	
	flow_control();
	while(true){
		print_prompt();
		getline(&input,&sz,stdin);
		input[strlen(input)-1]='\0';
		if(strcmp(input,"exit")==0)
				exit(EXIT_SUCCESS);

		if(strchr(input,'|')==NULL && strchr(input,'=')==NULL){//not piped command
			
			spid=fork();
			if(spid==0){//child process
				execute_command(input);
			}else if(spid>0){//parent process
				if(strcmp(input,"fg")==0){
					if(kill(pid,SIGCONT)<0)
						perror("SIGONCT");
					kill(spid,SIGKILL);
					break;
				}
				waitpid(spid,NULL,WUNTRACED);
			}else{
				printf("Fork not executed succesfully\n");
			}		

		}else if(strchr(input,'|')!=NULL){

			initArray(&a,20);
			process_piped(a,input);
			execute_piped(a,input);
			free(a);
		}else if(strchr(input,'=')!=NULL){
			char *temp=strdup(input);
			char *env=strtok(temp,"=");
			char *val=strtok(NULL,"$=\"");
			if(getenv(val)!=NULL)
				val=getenv(val);
			if(getenv(env)!=NULL){
				setenv(env,val,1);
			}else{
				setenv(env,val,0);
			}
		}

	}


		
}


void flow_control(){
	struct termios t1;
	
	if(tcgetattr(STDIN_FILENO,&t1)<0)
		perror("at get");
	t1.c_iflag|=IXON|IXOFF;
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&t1);
}




