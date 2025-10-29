#include "hero.h"
#include "grid.h"
#include <stdio.h>

void* hero_thread(void* arg){
    Hero* hero = (Hero*)arg;
    int opcion;
    //bool encontrado = false;
    while(1){
        printf("Ingrese opcion:\n");
        printf("1. Ver mapa\n");
        printf("2. Ver estadisticas\n");
        printf("3. Moverse una casilla\n");
        printf("4. Salirse del juego\n");
        printf("INGRESE OPCION: ");
        scanf("%d",&opcion);
        //accion de atacar
        //if(encontrado==true){
            /*
            while(MIENTRAS HAYAN MONSTRUOS CERCA){
            }
            */
        //}

        if(opcion==1){
            lock_grid();
                //accion de ver mapa
            unlock_grid();
        }
        if(opcion==2){
            printf("STATS:\n");
            printf("ID: %d\n", hero->id);
            printf("HP: %d\n", hero->hp);
            printf("COORDS: (%d,%d)\n", hero->x, hero->y);
            printf("==========================\n");
        }
        if(opcion==3){
            lock_grid();
                //accion de moverse dentro del mapa
            unlock_grid();
        }
        if(opcion==4){
            break;
        }

    }
}