#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/*
 * @author Adrian Gonzalez Pardo
 **/

void call_repair(char *src){
  int status;
  char *file[3];
  file[0]="/usr/bin/convert";
  file[1]=src;
  file[2]=src;
  if(fork()==0){
    execv(file[0],file);
  }
  wait(&status);
  printf("Repair complete\n");
}

void call_for_photo(char *src){
  char *command[10],file[1000];
  int status;
  memset(file,0,1000);
  memcpy(file,src,strlen(src));
  memcpy(file+strlen(src),".bmp",strlen(".bmp"));
  command[0]="raspistill";
  command[1]="-e";
  command[2]="bmp";
  command[3]="-h";
  command[4]="480";
  command[5]="-w";
  command[6]="640";
  command[7]="-o";
  command[8]=file;
  command[9]=NULL;
  if(fork()==0){
    execv("/usr/bin/raspistill",command);
    exit(0);
  }
  wait(&status);
  command[0]="/usr/bin/convert";
  command[1]=file;
  command[2]=file;
  for(int i=3;i<10;i++){
    command[i]=NULL;
  }
  if(fork()==0){
    execv(command[0],command);
    exit(0);
  }
  wait(&status);
  printf("Captura completa del archivo %s.bmp\n",src);
}
