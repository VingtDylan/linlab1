#include "lib.h"
#include "stdio.h"
#include "pwd.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "stdlib.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "linux/limits.h"

int lastpid;

void  execute_command(int cnt){
     if(counter<1){
         printf(YELLOW"No exectuable command!\n"NONE);
         return ;
     }
     char *start=(char *)cmd_tables[cnt-1];
     int index=1;
     char *myargv[Argc]={};
     myargv[0]=start;
     while(*start){
       if(isspace(*start)){
           *start=0;
           start++;
           myargv[index++]=(char*)start;
       }else{
           ++start;
       }
     }  
     myargv[index-1]=NULL;
     if(outfile[0]!='\0'){
       cmdoutfd[cnt-1]=open(outfile,O_WRONLY | O_CREAT | O_APPEND,0666);
     }
     int pid=fork();
     if(pid==0){
         /*if(cmdinfd[cnt-1]==0&&background==back)
           cmdinfd[cnt-1]=open("/dev/null",O_RDONLY);
         if(cmdinfd[cnt-1]!=0){
           close(0);
           int son1=dup(cmdinfd[cnt-1]);
           printf("Debug : son1->%d\n",son1);
         }
         if(cmdoutfd[cnt-1]!=1){
           close(1);
           int son2=dup(cmdoutfd[cnt-1]);
           printf("Debug : son2->%d\n",son2);
         }*/
         execvp(myargv[0],myargv);
     }else{
        if(background==back){
           wait(NULL);
        }
     } 
     //if((fd=cmdoutfd[cnt-1])!=0)
       // close(fd);
}

void fork_execute(int i){
  pid_t pid=fork();
  if(pid==-1){
    EXIT_ERR("fork error");
  }  
  if(pid>0){
    if(background==back){
        printf("background!\n");
        NODE *new=(NODE*)malloc(sizeof(NODE));
        new->nodepid=pid;
        //printf(RED"Debug:command : %s\n",cmd[0].args[0]);
        NODE *tmp=head->next;
        head->next=new;
        new->next=tmp;
    }
    lastpid=pid;
    usleep(200);
  }else if(pid==0){
    if(cmd[i].infd==0&&background==back)
       cmd[i].infd=open("/dev/null",O_RDONLY);
    if(cmd[i].infd!=0){
       close(0);
       int son1=dup(cmd[i].infd);
       printf("Debug : fork son1->%d\n",son1);	
    }       
    if(cmd[i].outfd!=1){
       close(1);
       int son2=dup(cmd[i].outfd);
       printf("Debug : fork son2->%d\n",son2);
    }
    for(int k=3;k<OPEN_MAX;k++)
       close(k);
    if(background==front){
       signal(SIGINT, SIG_DFL);
       signal(SIGQUIT, SIG_DFL);
    }
    execvp(cmd[i].args[0],cmd[i].args);
    exit(EXIT_FAILURE);
  }
}

int execute(void){
    if(cmd_count==0){
       printf(RED"Debug:cmd_count=0!\n"NONE);
       return 0;
    }
    if(infile[0]!='\0')
       cmd[0].infd=open(infile,O_RDONLY);
    if(outfile[0]!='\0'){
       if(append)
          cmd[cmd_count-1].outfd=open(outfile,O_WRONLY | O_CREAT | O_APPEND,0666);
       else
          cmd[cmd_count-1].outfd=open(outfile,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if (background == 1)
       signal(SIGCHLD, SIG_IGN);
    else
       signal(SIGCHLD, SIG_DFL);
    int i;
    int fd;
    int fds[2];
    for(i=0;i<cmd_count;i++){
       if(i<cmd_count-1){
          int k=pipe(fds);
          printf("Debug: pipe success->%d\n",k);
          cmd[i].outfd=fds[1];
          cmd[i+1].infd=fds[0];
       }
       fork_execute(i);
       if((fd=cmd[i].infd)!=0)
          close(fd);
       if((fd=cmd[i].outfd)!=1)
          close(fd);
    }  
    if(background==front)
       while(wait(NULL)!=lastpid);
    return 0;
} 

