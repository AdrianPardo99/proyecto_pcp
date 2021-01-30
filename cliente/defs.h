#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#ifndef DEFS_H

/*
 * @author Adrian Gonzalez Pardo
 **/

#define DEFS_H

#define NUM_THREADS 4
#define DIMASK 5
#define SOBELMASK 3
#define FACTOR 330
#define COLA_CLIENTES 5
#define MAX_MTU 1000

typedef struct conexiones{
  int socket;
  pthread_t id;
}conexion;

#endif
