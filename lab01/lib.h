#ifndef __LIB_H__
#define __LIB_H__

#include <stdlib.h>

#define NONE                   "\033[m"   
#define RED                    "\033[0;32;31m"   
#define GREEN                  "\033[0;32;32m"   
#define BLUE                   "\033[0;32;34m"    
#define DARY_GRAY              "\033[1;30m"   
#define CYAN                   "\033[0;36m"    
#define PURPLE                 "\033[0;35m"   
#define BROWN                  "\033[0;33m"   
#define YELLOW                 "\033[1;33m"    
#define WHITE                  "\033[1;37m"

#define EXIT_ERR(m)\
do\
{\
   perror(m);\
   exit(EXIT_FAILURE);\
}\
while(0);

#define export stdout
#define import stdin

#define NameLen    128
#define PathLen    128
#define INPUTLEN   256
#define INDEXLEN   256
#define CMD_LINES  256
#define CMD_TABLES 256
#define PATTERN    256
#define INFDLEN    256
#define OUTFDLEN   256
#define OPEN_MAX   1024
#define PIPELINE   4
#define MAXNAME    4
#define TestLen    4
#define Argc       32
 
typedef void (*signal_handler)(int);
typedef void (*CMD_HANDLER)(void);
typedef enum {false,true}bool;
/*typedef enum {correct_state,cd_state,history_state,exit_state,default_state}state;*/
typedef enum {front,back}environment;

typedef struct COMMAND{
  char *args[CMD_LINES];
  int infd;
  int outfd;
}command;

typedef struct node{
  pid_t nodepid;
  char *later[32];
  struct node *next;
}NODE;

typedef struct builtin_cmd{
  char *name;
  CMD_HANDLER handler;
}BUILTIN_CMD;

extern command cmd[PIPELINE];
extern bool flag;
extern char input[INPUTLEN];
extern char indexline[INDEXLEN];
extern char *cmd_lines[CMD_LINES];
extern char *cmd_tables[CMD_TABLES];
extern int  cmdinfd[INFDLEN];
extern int  cmdoutfd[OUTFDLEN];
extern char infile[MAXNAME];
extern char outfile[MAXNAME];
extern char *lineptr;
extern char *indexptr;
extern int  append;
extern int counter;
extern environment background;
extern int lastpid;
extern int cmd_count;
extern NODE *head;

void setup();
void info_version();
void Get_Name();
void init();
void signal_handler_fun(int signal_num);
void sigint_handler(int sig);
void exec_loop();

int builtin(void);
void do_exit(void);
void do_cd(void);
void do_history(void);

void check();
int  checker(const char *str);
bool exist(char str);
void getname(char *name);
bool isnumber(char *str);
void handle_command(int i);
int  get_command();
int  parse_command();

void execute_command(int cnt);
void fork_execute(int i);
int execute(void);

#endif
