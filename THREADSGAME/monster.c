#include "monster.h"
#include "grid.h"
#include <stdio.h>
#include <unistd.h>

void* monster_thread(void* arg){
    Monster* monster = (Monster*)arg;
    //casi el mismo accionamiento
    //buscar dentro de casillas al rededor para saber si por ahi esta el hero
}