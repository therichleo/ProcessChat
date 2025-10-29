#ifndef HERO_H
#define HERO_H

#include <pthread.h>

typedef struct{
    int id;
    int hp;
    int x;
    int y;
} Hero;

void* hero_thread(void* arg);

#endif
