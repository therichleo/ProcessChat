#include <stdio.h>
#include <pthread.h>
#include "grid.h"
#include "hero.h"
#include "monster.h"

int main(){
    //inicializamos mapa
    init_grid(30,20);
    
    Hero hero = { .id=1, .hp=100, .x=1, .y=1};

    int monstruos;
    prinft("Ingrese cantidad de monstruos: ");
    scanf("%d",&monstruos);
    printf("\n");
    Monster monster[monstruos];
    //creacion de threads de cada uno
    pthread_t hero_tid;
    pthread_t monster_tids[monstruos];

    pthread_create(&hero_tid, NULL, hero_thread, &hero);
    for(int i=0;i<=monstruos;i++){
        pthread_create(&monster_tids[i], NULL, monster_thread, &monster);
    }
    //esperamos a que terminen todos
    pthread_join(hero_tid,NULL);
    for(int i=0;i<=monstruos;i++){
        pthrad_join(monster_tids[i],NULL);
    }
    //destruimos todo a la mierda
    destroy_grid();

    printf("\nSe termino esta wea");
    return 0;
}