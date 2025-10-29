#ifndef GRID_H 
#define GRID_H 

/*
#ifndef, if is not defined, si no esta definido, el codigo sigue
Si esta definido se salta todo hasta #endif, es para compartir headers, y no duplicacion
*/

#include <pthread.h>

//estructura de mapa
typedef struct{
    int ancho;
    int alto;
};

//funciones del mapa

void init_grid(int ancho, int alto);
void destroy_grid();
void lock_grid();
void unlock_grid();

#endif