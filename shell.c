/*
* GEORGIOS PANAGIOTIS MELLIOS 4416
*
*
*/
#include "functions.h"
int main(){
	char *input="";
	size_t sz=0; //for getline
	Array *a;
	int status;
	flow_control();
	while(true){
		print_prompt();
		getline(&input,&sz,stdin);
		input[strlen(input)-1]='\0';
		if(strcmp(input,"exit")==0)
				exit(EXIT_SUCCESS);

		if(strchr(input,'|')==NULL && strchr(input,'=')==NULL){//not piped command
			pid=fork();
			if(pid==0){//child process
				execute_command(input);
			}else if(pid>0){//parent process
				signal(SIGTSTP,signal_handler);
				waitpid(pid,NULL,WUNTRACED);
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




