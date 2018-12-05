#include "lib.h"
#include "stdio.h"
#include "pwd.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/wait.h"

NODE *head;
bool flag;

void setup(){
   signal(SIGINT, sigint_handler);
   signal(SIGQUIT, SIG_IGN);
}

void sigint_handler(int sig){
   printf("\n[command]$ ");
   fflush(stdout);
}

void init(){
  memset(cmd,0,sizeof(cmd));
  for(int i=0;i<PIPELINE;i++){
     cmd[i].infd=0;
     cmd[i].outfd=1;
  }
  for(int i=0;i<INFDLEN;i++)
     cmdinfd[i]=0;
  for(int j=0;j<OUTFDLEN;j++)
     cmdoutfd[j]=1;
  memset(input,0,sizeof(input));
  memset(indexline,0,sizeof(indexline));
  indexptr=indexline;
  lineptr=input;
  memset(infile,0,sizeof(infile));
  memset(outfile,0,sizeof(outfile));
  cmd_count=0;
  background=front;
  append=0;
  lastpid=0;
  flag=true;
}

int main(){  
  info_version();
  head=(NODE*)malloc(sizeof(NODE));
  head->next=NULL;
  setup();
  exec_loop();
  return 0;
}

void info_version(){
  printf(RED"Version 1.0! Good Luck!\n"NONE);
  printf(GREEN"The usage will be showed later!\n"NONE);
}

void Get_Name(){
  struct passwd* passcase;
  passcase = getpwuid(getuid());
  printf(BLUE"[%s-->"NONE,passcase->pw_name);
  char name [NameLen];
  gethostname(name,sizeof(name)-1);
  printf(DARY_GRAY"%s-->"NONE,name);
  char path[PathLen];
  if(!getcwd(path,sizeof(path)-1)){
       printf(RED"check getcwd!\n"NONE);   
       EXIT_ERR("getcwd!");	 
  }
  int len = strlen(path);
  char *p = path+len-1;
  while(*p!='/'&&len--)
	p--;
  p++;
  printf(CYAN"%s]$ "NONE,p);
}

void signal_handler_fun(int signal_num){
  printf(PURPLE"\nCatch signal %d\n"NONE,signal_num);
  printf(BROWN"Terminated\n"NONE);
  EXIT_ERR("Ctrl -c");
}

void exec_loop(){
  fputs("Welcome to my shell process!\n",export);
  signal_handler p_signal=signal_handler_fun;
  signal(SIGINT,p_signal);
  bool run=true;
  while(run){
     Get_Name();
     init();
     if(get_command()==-1)
       break;
     parse_command();
     /*printf(BLUE"cmd_count = %d\n"NONE,cmd_count);
     if(infile[0]!='\0')     
        printf(CYAN"infile=[%s]\n"NONE,infile);
     if(outfile[0]!='\0')
        printf(CYAN"outfile=[%s]\n"NONE,outfile);
     for(int i=0;i<cmd_count;i++){
        int j=0;
        while(cmd[i].args[j]!=NULL){
          printf("[%s] ",cmd[i].args[j]); 
          j++;
        }  
        printf("\n");
     }*/
     execute();
     /*state k=parse_command();
     switch(k){
       case correct_state:break;
       case exit_state:run=false;break;
       case cd_state:break;
       case history_state:break;
       default:break;
     }*/
  } 
}

