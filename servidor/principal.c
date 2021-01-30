#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "defs.h"
#include "tools.h"
#include "imagen.h"
#include "processing_image.h"
#include "threads.h"

#include "handler.h"

bmpInfoHeader info;
unsigned char *imgGray,*imgFiltrada;
int flag=1;

void *sobel_proc(void*);
int crea_servidor(int);
void atiende(conexion*);
void cierra_servidor(int);

int main(int argc,char **argv){
  if(argc<2){
    printf("Error\n\tUsage: %s <Port_server>\n",*argv),exit(EXIT_FAILURE);
  }
  int port=atoi(*(argv+1)),sock,client;
  if(port<=0){
    printf("Error please use an positive number\n"),exit(EXIT_FAILURE);
  }
  signal_handler();
  sock=crea_servidor(port);
  while(flag){
    printf("Waiting for clients...\n");
    if((client=accept(sock,NULL,NULL))<0){
      perror("Error when try to accept a client"),exit(EXIT_FAILURE);
    }
    if(!(fork())){
      conexion *c=(conexion*)malloc(sizeof(conexion));
      c->socket=client;
      atiende(c);
    }
  }
  cierra_servidor(sock);
  return 0;
}

void *sobel_proc(void *arg){
  informacion *i=(informacion*)arg;
  printf("Thread sobel %d init\n",i->id);
  if(i->width==0||i->height==0){
    printf("Error with the header data in %d\n",i->id);
    pthread_exit(arg);
  }
  sobel_bloque(*i->gray,*i->fil,i->width,i->height,i->id);
  printf("Thread sobel %d finish\n",i->id);
  pthread_exit(arg);
}


int crea_servidor(int port){
  int sock;
  struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion
  memset(&direccion_servidor, 0, sizeof (direccion_servidor));	//se limpia la estructura con ceros
  direccion_servidor.sin_family=AF_INET;
  direccion_servidor.sin_port=htons(port);
  direccion_servidor.sin_addr.s_addr=INADDR_ANY;
  printf("Socket create...\n");
  if((sock=socket(AF_INET, SOCK_STREAM,0))<0){
    perror("Error when try to create socket"),exit(EXIT_FAILURE);
  }
  printf("Config socket");
  if(bind(sock,(struct sockaddr*)&direccion_servidor,sizeof(direccion_servidor))<0){
    perror("Error when try to configure socket"),exit(EXIT_FAILURE);
  }
  printf("Listen socket for clients...\n");
  if(listen(sock,COLA_CLIENTES)<0){
    perror("Error listen another client..."),exit(EXIT_FAILURE);
  }
  return sock;
}

void atiende(conexion *arg){
  char namefile[MAX_MTU]="captura",bmp_file[MAX_MTU];
  conexion *c=arg;
  printf("Client:%d\nGenerate a bmp file with name %s.bmp\n",c->socket,namefile);
  call_for_photo(namefile);
  bmpInfoHeader info;
  unsigned char *imagenRGB;
  uint32_t tam_imagen;
  memset(bmp_file,0,MAX_MTU);
  memcpy(bmp_file,namefile,strlen(namefile));
  memcpy(bmp_file+strlen(namefile),".bmp",strlen(".bmp"));
  imagenRGB=abrirBMP(bmp_file,&info);
  tam_imagen=info.width*info.height;
  displayInfo(&info);
  /* Aca va todo lo de procesamiento del api previamente programada,,, */
  imgGray=reservar_memoria(info.width,info.height);
  imgFiltrada=reservar_memoria(info.width,info.height);

  memset(imgFiltrada,255,info.width*info.height);

  newRGBToGray(imagenRGB,imgGray,info.width,info.height);
  //GrayToRGB(imagenRGB,imgGray,info.width,info.height);
  //guardarBMP("gray.bmp",&info,imagenRGB);
  printf("Sent the header about the bmp information...\n");
  if(write(c->socket,&info,sizeof(info))<0){
    perror("Error when try to sending the struct information "),exit(EXIT_FAILURE);
  }
  printf("Processing the sobel filter init\n");
  //call_pthreads(1);

  register int i;
  pthread_t *id=(pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);
  informacion *val=(informacion*)malloc(sizeof(informacion)*NUM_THREADS);
  //int *id_thread=(int*)malloc(sizeof(int)*NUM_THREADS);
  if(!id){
    perror("Error allocating memory threads"),exit(EXIT_FAILURE);
  }
  if(!val){
    perror("Error allocating memory int"),exit(EXIT_FAILURE);
  }
  for(i=0;i<NUM_THREADS;i++){
    val[i].id=i;
    val[i].width=info.width;
    val[i].height=info.height;
    val[i].gray=&imgGray;
    val[i].fil=&imgFiltrada;
    pthread_create(&*(id+i),NULL,sobel_proc
      ,(void*)&val[i]);
  }
  for(i=0;i<NUM_THREADS;i++){
    pthread_join(*(id+i),NULL);
  }
  free(id);
  free(val);

  /*for(int i=0;i<NUM_THREADS;i++){
    sobel_bloque(imgGray,imgFiltrada,info.width,info.height,i);
  }*/
  printf("Sent processing image...\n");
  if(write(c->socket,imgFiltrada,tam_imagen)<0){
    perror("Error when try to sending the data about the image"),exit(EXIT_FAILURE);
  }
  /* Termina y manda sobel... */
  free(imagenRGB);
  free(imgGray);
  free(imgFiltrada);
  close(c->socket);
//  pthread_exit(arg);
  exit(0);
}

void cierra_servidor(int sock){
  close(sock);
}
