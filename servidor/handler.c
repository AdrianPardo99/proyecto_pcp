#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

extern int flag;

void catch_handler(int signal_enter){
  int estado;
  pid_t pid;
  if(signal_enter==SIGCHLD){
    printf("The server recibe SIGCHLD\n");
    pid=wait(&estado);
    printf("The process %d for service the client ends with %d\n",pid,estado>>8);
  }else if(signal_enter==SIGINT){
    printf("The server recibe SIGINT\n"
          "The server is close...\n");
    flag=0;
  }
}

void signal_handler(){
  if(signal(SIGCHLD,catch_handler)==SIG_ERR){
    perror("Error creating signal handler SIGCHLD"),exit(EXIT_FAILURE);
  }
  if(signal(SIGINT,catch_handler)==SIG_ERR){
    perror("Error creating signal handler SIGINT"),exit(EXIT_FAILURE);
  }
}
