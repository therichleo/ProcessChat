#include "grid.h"
#include <stdio.h>

//define el mutex global
pthread_mutex_t grid_lock;

//INICIAMOS GRID Y MUTEX GLOBAL
void init_grid(int alto, int ancho){
    printf("Inicio de grid %dx%d \n", alto, ancho);
    pthread_mutex_init(&grid_lock,NULL);
}

//Destruimos el mutex al finalizarlo
void destroy_grid(){
    pthread_mutex_destroy(&grid_lock);
    printf("Grid destruido correctamente.\n");
}

//helpers para bloquear/desbloquear el grid
void lock_grid(){
    pthread_mutex_lock(&grid_lock);
}

void unlock_grid(){
    pthread_mutex_unlock(&grid_lock);
}