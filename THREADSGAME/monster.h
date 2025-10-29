#ifndef MONSTER_H
#define MONSTER_H

typedef struct{
    int id;
    int hp;
    int x;
    int y;
} Monster;

void* monster_thread(void* arg);

#endif  