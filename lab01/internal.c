#include "lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

BUILTIN_CMD builtins[]={
  {"exit", do_exit},
  {"cd"  , do_cd},
  {"history",do_history},
  {NULL, NULL}
};

int builtin(void)
{
    int i = 0;
    int found = 0;
    while (builtins[i].name != NULL){
	if(checker(builtins[i].name)){
	     builtins[i].handler();
             found = 1;
 	     break;
	}
	i++;
    }
   return found;
}

void do_exit(void){
  printf("Debug: the command is %s\n",cmd_lines[0]);
  int index=0;
  NODE* cur=head->next;
  while(cur!=NULL){
    index++;
    cur=cur->next;
  }
  if(index!=0){
    printf(RED"Warning : Some programs are still in run.Think twice!\n"NONE);
    printf(GREEN"input y to exit,n to continue the shell program"NONE);
    char option=getchar();
    if(option=='n'){
        if(counter!=0){    
           for(int i=0;i<counter;i++)
             free(cmd_tables[i]);
        }
	return;
    }
    else
        goto loop;
  }
  loop:
    printf(BLUE"Begin to exit!\n"NONE);
    printf(BLUE"Done!\n"NONE);
    exit(EXIT_SUCCESS);
}

void do_cd(void){
    printf("Debug: the command is %s\n",cmd_lines[0]);
    handle_command(0);
    int fd;
    fd=open(*(cmd[0].args),O_RDONLY);
    if(fchdir(fd)==0)
       printf(BROWN"Change the directory!\n"NONE);
    close(fd);
}

void do_history(void){
   handle_command(0);
   printf(CYAN"Debug: the command is %s\n"NONE,cmd_lines[0]);  
   if(cmd_lines[1]=='\0'){
      printf(CYAN"command history:No parameter!\n"NONE);
      if(counter<100){
          printf(RED"Only %d messages of commands!\n"NONE,counter);
          for(int i=0;i<counter;i++)
              printf(YELLOW"command %d:%s\n"NONE,i+1,cmd_tables[i]);   
       }else{
          printf(RED"The 100 messages of commands are as follows\n"NONE);
          for(int i=counter-100;i<counter;i++)
              printf(YELLOW"command %d:%s\n"NONE,i-counter+101,cmd_tables[i]);
       } 
   }else if(strcmp(cmd_lines[1],"-c")==0){
      printf(CYAN"command history:parameter is -c!\n"NONE);
      for(int i=0;i<counter;i++)
          free(cmd_tables[i]);
      counter=0;
      printf(PURPLE"all cleared!\n"NONE); 
   }else if(strcmp(cmd_lines[1],"!!")==0){
      printf(CYAN"exectute the latest command!\n"NONE);
      execute_command(counter-1); 
   }else{
      if(cmd_lines[1][0]=='!'){
         printf(GREEN"The messages found are as follows\n"NONE);
         char *start=cmd_lines[1];
         start++;
         printf(RED"Debug: pattern is %s\n"NONE,start);
	 printf(BLUE"The messages are as follow(if it exists)\n"NONE);
	 for(int i=0;i<counter;i++){
             if(strncmp(cmd_tables[i],start,strlen(start))==0)
	         printf(YELLOW"history : %s\n"NONE,cmd_tables[i]);
         }
      }else if(isnumber(cmd_lines[1])){
         int number=atoi(cmd_lines[1]);
         for(int i=0;i<number;i++){
	     printf(BLUE"executed command : %s\n"NONE,cmd_tables[i]);	
	     }
      }else{
  	 printf(BLUE"Uncompleted command!\n"NONE);
      }
  }
}

