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

char input[INPUTLEN];
char indexline[INDEXLEN];
char *cmd_lines[CMD_LINES];
char *cmd_tables[CMD_TABLES];
int cmdinfd[INFDLEN];
int cmdoutfd[OUTFDLEN];
char infile[MAXNAME];
char outfile[MAXNAME];
char *lineptr;
char *indexptr;
int append;
int counter;
environment background;
int cmd_count;
command cmd[PIPELINE];

bool exist(char str){
   lineptr=cmd_tables[counter-1];
   int len=strlen(lineptr);
   for(int i=0;i<len;i++){
       if(lineptr[i]==str){
           lineptr+=i+1;
	   return true;
       }
   }
   return false;
}

void getname(char *name){
   while(*lineptr==' '||*lineptr=='\t')
	lineptr++;
   while(*lineptr!=' '
       &&*lineptr!='\0' 
       &&*lineptr!='\t'
       &&*lineptr!='\n'
       &&*lineptr!='<'
       &&*lineptr!='>'
       &&*lineptr!='|'
       &&*lineptr!='&')
   {
     *name++=*lineptr++;
   }
   *name='\0';
}

int checker(const char *str){
  char *p;
  while(*lineptr==' '|| *lineptr=='\t')
    lineptr++;
  p=lineptr;
  while(*str!='\0'&&*str==*p){
    str++;
    p++;
  }
  if(*str=='\0'){
    lineptr=p;
    return 1;
  }
  return 0;
}

bool isnumber(char *str){
   int len=strlen(str);
   for(int i=0;i<len;i++){
       if(!isdigit(str[i]))
	   return false;
   }
   return true;
}

void handle_command(int i){
   int index=0;
   int word;
   while(*lineptr!='\0'){
     while(*lineptr==' '||*lineptr=='\t')
       lineptr++;
     cmd[i].args[index]=indexptr;
     while(*lineptr!='\0'
         &&*lineptr!=' '
         &&*lineptr!='\t'
         &&*lineptr!='>'
         &&*lineptr!='<'
         &&*lineptr!='|'
         &&*lineptr!='&'
         &&*lineptr!='\n')
     {
         *indexptr++=*lineptr++;
         word=1;
     }
     *indexptr++='\0';
     switch(*lineptr){
       //case '\0':break;
       case ' ' :
       case '\t':word=0;index++;break;
       case '<' :
       case '>' :
       case '|' :
       case '&' :
       case '\n':
            if(word==0)
                cmd[i].args[index]=NULL;
            return ;
        default :return; 
     }  
   }
}

int get_command(){
   if(fgets(input,sizeof(input),import)==NULL){
        fputs("Input Error!\n",export);
        return -1;
   }
   counter++;
   return 0;
}

int parse_command(){ 
     printf("Debug: NO.%d command.\n",counter);
     cmd_tables[counter-1]=calloc(255,sizeof(char));
     strcpy(cmd_tables[counter-1],input);
     if(flag)
        for(int i=0;i<CMD_LINES;i++){
           free(cmd_lines[i]);
           //cmd_lines[i]='\0';
        }
     int index=0,indej=0;
     int handle=0;
     cmd_lines[indej]=calloc(255,sizeof(char));
     while(input[index]!='\n'&&input[index]!='\0'){
        if(input[index]!=' '){
           cmd_lines[indej][handle]=input[index];
           handle++;
        }else{
           cmd_lines[indej+1]=calloc(255,sizeof(char));
           handle=0;
	   indej++;
        }
        index++;
     }   
     cmd_lines[indej+1]='\0';
     flag=true;
     for(int k=0;k<=indej;k++){
        printf(BROWN"debug: cmd is %s\n"NONE,cmd_lines[k]);
     }
     if(checker("\n"))
        return 0;
     if(builtin())
        return 0;
     handle_command(0);
     if(checker("<")){
        getname(infile);
        printf(BLUE"Debug : %s\n"NONE,infile);
     }
     int i;
     for(i=1;i<PIPELINE;i++){
        if(checker("|"))
           handle_command(i);
        else
           break;
     }
     if(checker(">")){
        if(checker(">"))
          append=1;
        getname(outfile);
        printf(BLUE"Debug : %s \n"NONE,outfile);
     }
     if(checker("&"))
        background=back;
     if(checker("\n")){
        cmd_count=i;
        printf(BLUE"cmd_count:%d\n",cmd_count);
        return cmd_count;
     }else{
        return -1;
     }
     /*cmd_tables[counter-1]=calloc(255,sizeof(char));
     strcpy(cmd_tables[counter-1],input);
     int index=0,indej=0;
     int handle=0;
     cmd_lines[indej]=calloc(255,sizeof(char));
     while(input[index]!='\n'&&input[index]!='\0'){
        if(input[index]!=' '){
           cmd_lines[indej][handle]=input[index];
           handle++;
        }else{
           cmd_lines[indej+1]=calloc(255,sizeof(char));
           handle=0;
	   indej++;
        }
        index++;
     }   
     cmd_lines[indej+1]='\0';
     for(int k=0;k<=indej;k++){
        printf(BROWN"debug: cmd is %s\n"NONE,cmd_lines[k]);
     }
     if(strcmp(cmd_lines[0],"exit")==0){
        printf("Debug: the command is %s\n",cmd_lines[0]);
        printf(YELLOW"begin to stop the process!\n"NONE);
        fputs("Done\n",export);
        if(counter!=0){    
           for(int i=0;i<counter;i++)
             free(cmd_tables[i]);
        }
        return exit_state; 
     }else if(strcmp(cmd_lines[0],"cd")==0){
        printf("Debug: the command is %s\n",cmd_lines[0]);
        int fd=open(cmd_lines[1],O_RDONLY);
        if(fchdir(fd)==0)
            printf(BROWN"Change the directory!\n"NONE);
        close(fd);
        return cd_state;
     }else if(strcmp(cmd_lines[0],"history")==0){
        printf(CYAN"Debug: the command is %s\n"NONE,cmd_lines[0]);  
        if(cmd_lines[1]==NULL){
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
        return history_state;
     }else if(strcmp(cmd_lines[0],"ls")==0){
        execute_command(counter);
     }else{
        printf("Debug: unknown command %s!\n",cmd_lines[0]);
     }   
     for(int i=0;i<=indej;i++)
        free(cmd_lines[i]);   */
     return 0;
}















