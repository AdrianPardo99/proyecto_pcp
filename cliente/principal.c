#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "imagen.h"
#include "processing_image.h"

void verifica_ip(char*,char*);
void recibe(int,unsigned char*,uint32_t);

int main(int argc,char **argv){
  if(argc<4){
    printf("Error\n\tUsage: %s <IP_or_HOST> <Port> <Name_of_file>",*argv),
    exit(EXIT_FAILURE);
  }
  int port=atoi(*(argv+2)),sockfd;
  char ip[100],*filename=*(argv+3);
  bmpInfoHeader info;
  struct sockaddr_in direccion_servidor;
  unsigned char *imagen,*rgb;
  verifica_ip(*(argv+1),&*ip);
  if(port<=0){
    printf("Error please use an positive number\n"),exit(EXIT_FAILURE);
  }
  printf("Try to connect for the server with ip: %s and port app: %d\n",
    ip,port);
  memset (&direccion_servidor, 0, sizeof (direccion_servidor));
  direccion_servidor.sin_family = AF_INET;
  direccion_servidor.sin_port = htons(port);
  if(inet_pton(AF_INET, ip, &direccion_servidor.sin_addr)<=0){
    perror("Error when the client try to convert the ip in bytes"),exit(EXIT_FAILURE);
  }
  printf("Creating socket...\n");
  if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){
    perror("Error when try to create socket"),exit(EXIT_FAILURE);
  }
  printf("Try to connect...\n");
  if(connect(sockfd,(struct sockaddr *)&direccion_servidor,sizeof(direccion_servidor))<0){
		perror("Ocurrio un problema al establecer la conexion");
		exit(EXIT_FAILURE);
	}
  printf("Read header\n");

  if(read(sockfd, &info, sizeof(bmpInfoHeader))<0){
		perror("Error when client try to read information about the bmp header"),exit(EXIT_FAILURE);
	}
  printf("End read header\n");

  imagen=reservar_memoria(info.width,info.height);
  rgb=reservar_memoria(info.width,info.height*3);

  printf("Client recibe the data...\n");
  recibe(sockfd,imagen,info.width*info.height);
  GrayToRGB(rgb,imagen,info.width,info.height);
  char last[1000];
  memset(last,0,1000);
  memcpy(last,filename,strlen(filename));
  memcpy(last+strlen(filename),".bmp",strlen(".bmp"));
  guardarBMP(last,&info,rgb);
  printf("Client recibe all the data\n");
  free(imagen);
  free(rgb);
  close(sockfd);
  return 0;
}

void verifica_ip(char *x,char *ip){
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  if((he=gethostbyname(x))==NULL){
    perror("gethostbyname"),exit(EXIT_FAILURE);
  }
  addr_list=(struct in_addr **)he->h_addr_list;
  for(i=0;addr_list[i]!=NULL;i++){
    strcpy(ip,inet_ntoa(*addr_list[i]));
    break;
  }
}

void recibe(int sockfd,unsigned char* imagen,uint32_t tam){
  int bytesRecibidos;
  while(tam>0){
    bytesRecibidos=read(sockfd,imagen,tam);
    if(bytesRecibidos<0){
      perror("Error when the client try to recibe information"),exit(EXIT_FAILURE);
    }
    if(bytesRecibidos>0){
      printf("Bytes recibidos: %d\n", bytesRecibidos);
    }
    tam-=bytesRecibidos;
    imagen+=bytesRecibidos;
  }
}
