#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h> // Para INT_MAX

// --- ESTRUCTURAS DE DATOS ---

typedef struct {
    int x;
    int y;
} Coords;

typedef struct {
    int id;
    int hp;
    int attack_damage;
    int attack_range;
    Coords current_pos;
    Coords start_pos;
    Coords *path;
    int path_length;
    int path_index;
    int is_alive;
    int is_attacking;
} Hero;

typedef struct {
    int id;
    int hp;
    int attack_damage;
    int vision_range;
    int attack_range;
    Coords current_pos;
    int is_alive;
    int is_alerted;
} Monster;

// --- VARIABLES GLOBALES ---
// Son compartidas por todos los threads, por lo que requieren sincronización.

int GRID_WIDTH = 0;
int GRID_HEIGHT = 0;
Hero *GAME_HEROES = NULL; // Arreglo para N Héroes
int HERO_COUNT = 0;
Monster *GAME_MONSTERS = NULL; // Arreglo para M Monstruos
int MONSTER_COUNT = 0;

// Sincronización: Mutex para proteger todas las posiciones y HP de los personajes
pthread_mutex_t game_mutex;

// --- FUNCIONES DE UTILIDAD ---

/**
 * @brief Calcula la distancia de Manhattan entre dos puntos.
 */
int manhattan_distance(Coords p1, Coords p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

/**
 * @brief Busca el Monstruo vivo más cercano al Héroe.
 * Debe ser llamada DENTRO de un bloqueo de mutex.
 */
Monster* get_closest_monster(Hero *h, int *dist) {
    *dist = INT_MAX;
    Monster *closest = NULL;
    
    for (int i = 0; i < MONSTER_COUNT; i++) {
        if (GAME_MONSTERS[i].is_alive) {
            int current_dist = manhattan_distance(h->current_pos, GAME_MONSTERS[i].current_pos);
            if (current_dist < *dist) {
                *dist = current_dist;
                closest = &GAME_MONSTERS[i];
            }
        }
    }
    return closest;
}

/**
 * @brief Busca el Héroe vivo más cercano al Monstruo.
 * Debe ser llamada DENTRO de un bloqueo de mutex.
 */
Hero* get_closest_hero(Monster *m, int *dist) {
    *dist = INT_MAX;
    Hero *closest = NULL;
    
    for (int i = 0; i < HERO_COUNT; i++) {
        if (GAME_HEROES[i].is_alive) {
            int current_dist = manhattan_distance(m->current_pos, GAME_HEROES[i].current_pos);
            if (current_dist < *dist) {
                *dist = current_dist;
                closest = &GAME_HEROES[i];
            }
        }
    }
    return closest;
}

// --- THREADS Y LÓGICA DE JUEGO ---

/**
 * @brief Thread para el Monstruo (M threads en total).
 */
void* monster_thread(void* arg) {
    Monster *self = (Monster*)arg;

    printf("[MONSTER %d] Iniciado en (%d,%d). HP: %d\n", 
           self->id, self->current_pos.x, self->current_pos.y, self->hp);

    // Bucle principal del Monstruo
    while (self->is_alive) {
        pthread_mutex_lock(&game_mutex); // 🔒 Protege la lectura/escritura de posiciones y HP

        // Encontrar el héroe más cercano
        int dist_to_hero;
        Hero *target_hero = get_closest_hero(self, &dist_to_hero);

        if (target_hero == NULL) {
            // Todos los héroes están muertos. El monstruo se detiene.
            printf("[MONSTER %d] Misión cumplida. No quedan héroes.\n", self->id);
            pthread_mutex_unlock(&game_mutex);
            break;
        }

        // 1. Alerta / Detección
        if (self->is_alerted || dist_to_hero <= self->vision_range) {
            
            // Si ve al héroe por primera vez, se alerta
            if (!self->is_alerted) {
                self->is_alerted = 1;
                printf("[MONSTER %d] ¡Alerta! Héroe %d detectado.\n", self->id, target_hero->id);
            }
            
            // 2. Movimiento (si está fuera del rango de ataque)
            if (dist_to_hero > self->attack_range) {
                // Mover hacia el héroe objetivo (simplificación: 1 paso en la dirección más eficiente)
                if (self->current_pos.x != target_hero->current_pos.x) {
                    self->current_pos.x += (self->current_pos.x < target_hero->current_pos.x) ? 1 : -1;
                } else if (self->current_pos.y != target_hero->current_pos.y) {
                    self->current_pos.y += (self->current_pos.y < target_hero->current_pos.y) ? 1 : -1;
                }
                printf("[MONSTER %d] Se mueve a (%d,%d) hacia Héroe %d.\n", 
                       self->id, self->current_pos.x, self->current_pos.y, target_hero->id);
                
                // Recalcular distancia después del movimiento
                dist_to_hero = manhattan_distance(self->current_pos, target_hero->current_pos);
            }

            // 3. Ataque
            if (dist_to_hero <= self->attack_range) {
                target_hero->hp -= self->attack_damage;
                printf("[MONSTER %d] ATACA a Héroe %d. HP restante: %d\n", self->id, target_hero->id, target_hero->hp);
                
                if (target_hero->hp <= 0) {
                    target_hero->is_alive = 0;
                    printf("=================================\n");
                    printf("¡Héroe %d ha MUERTO!\n", target_hero->id);
                    printf("=================================\n");
                }
            }
        }

        pthread_mutex_unlock(&game_mutex); // 🔓 Libera el bloqueo
        usleep(500000); // Pausa (0.5 segundos) para simular el tiempo de turno
    }

    self->is_alive = 0; // Si sale del bucle, el monstruo ha muerto o el juego terminó.
    return NULL;
}

/**
 * @brief Bucle de ataque del héroe a todos los monstruos en su rango.
 */
void hero_attack_monsters(Hero *self) {
    int monsters_in_range_count = 0;
    //aaa
    // El héroe atacará continuamente hasta que no queden monstruos en rango
    while (self->is_alive) {
        monsters_in_range_count = 0;
        
        pthread_mutex_lock(&game_mutex); // 🔒 Protege el HP de los monstruos

        for (int i = 0; i < MONSTER_COUNT; i++) {
            Monster *m = &GAME_MONSTERS[i];
            if (m->is_alive && manhattan_distance(self->current_pos, m->current_pos) <= self->attack_range) {
                monsters_in_range_count = 1;
                
                // El héroe ataca
                m->hp -= self->attack_damage;
                printf("[HEROE %d] ATACA a MONSTER %d. HP restante: %d\n", self->id, m->id, m->hp);
                
                if (m->hp <= 0) {
                    m->is_alive = 0;
                    printf("[MONSTER %d] Ha sido derrotado por Héroe %d.\n", m->id, self->id);
                }
            }
        }
        
        pthread_mutex_unlock(&game_mutex); // 🔓

        if (monsters_in_range_count == 0) {
            break; // No hay más monstruos en rango, el héroe puede moverse
        }
        
        usleep(500000); // Pausa entre ataques para simular el turno
    }
}


/**
 * @brief Thread para el Héroe (N threads en total).
 */
void* hero_thread(void* arg) {
    Hero *self = (Hero*)arg;

    printf("[HEROE %d] Iniciado en (%d,%d). HP: %d\n", 
           self->id, self->current_pos.x, self->current_pos.y, self->hp);

    // Bucle principal del Héroe: se mueve mientras esté vivo y no haya llegado a la meta
    while (self->is_alive && self->path_index < self->path_length) {
        
        // 1. Verificar si hay monstruos en rango de ataque
        int dist_to_closest;
        
        // La búsqueda del monstruo debe hacerse bajo un bloqueo
        pthread_mutex_lock(&game_mutex); // 🔒
        Monster *closest_monster = get_closest_monster(self, &dist_to_closest);
        pthread_mutex_unlock(&game_mutex); // 🔓

        if (closest_monster != NULL && dist_to_closest <= self->attack_range) {
            // 2. Ataque
            printf("[HEROE %d] Monstruo %d detectado en rango %d. ¡Comienza el ataque!\n", 
                   self->id, closest_monster->id, self->attack_range);
            hero_attack_monsters(self); // Llama al bucle de ataque bloqueante
        } else {
            // 3. Movimiento: Sigue el PATH
            Coords next_pos = self->path[self->path_index];
            
            pthread_mutex_lock(&game_mutex); // 🔒 Escribir nueva posición
            self->current_pos = next_pos;
            self->path_index++;
            printf("[HEROE %d] Se mueve a (%d,%d). Paso %d/%d.\n", 
                   self->id, self->current_pos.x, self->current_pos.y, self->path_index, self->path_length);
            pthread_mutex_unlock(&game_mutex); // 🔓
            
            usleep(500000); // Pausa entre movimientos
        }
    }

    if (self->is_alive) {
        printf("=================================\n");
        printf("¡Héroe %d ha llegado a la meta!\n", self->id);
        printf("=================================\n");
    } else {
        printf("[HEROE %d] Su misión terminó en la muerte.\n", self->id);
    }
    
    return NULL;
}

// --- PARSEO DEL ARCHIVO DE CONFIGURACIÓN ---

/**
 * @brief Lee el archivo de configuración y prepara la simulación para N héroes y M monstruos.
 */
int parse_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir archivo de configuración");
        return -1;
    }

    char line[1024]; // Buffer más grande para líneas largas de PATH
    int max_hero_id = 0;
    
    // --- PRIMER PASO: Contar Héroes y Monstruos para asignar memoria ---
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char keyword[50];
        if (sscanf(line, "%49s", keyword) == 1) {
            if (strcmp(keyword, "MONSTER_COUNT") == 0) {
                sscanf(line, "MONSTER_COUNT %d", &MONSTER_COUNT);
            } else if (strstr(keyword, "HERO_")) {
                int id = 0;
                // Intentar leer el ID: "HERO_1_HP" -> id=1
                if (sscanf(keyword, "HERO_%d_", &id) == 1) {
                    if (id > max_hero_id) {
                        max_hero_id = id;
                    }
                }
            }
        }
    }
    HERO_COUNT = max_hero_id;
    rewind(file); // Volver al inicio del archivo

    if (HERO_COUNT <= 0 || MONSTER_COUNT <= 0) {
        fprintf(stderr, "Error: La configuración debe tener al menos 1 Héroe y 1 Monstruo.\n");
        fclose(file);
        return -1;
    }

    GAME_HEROES = (Hero*)calloc(HERO_COUNT, sizeof(Hero));
    GAME_MONSTERS = (Monster*)calloc(MONSTER_COUNT, sizeof(Monster));

    // --- SEGUNDO PASO: Leer valores y llenar estructuras ---
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue; 
        
        char keyword[50];
        if (sscanf(line, "%49s", keyword) == 1) {
            
            // GRID SIZE
            if (strcmp(keyword, "GRID_SIZE") == 0) {
                sscanf(line, "GRID_SIZE %d %d", &GRID_WIDTH, &GRID_HEIGHT);
            } 
            // HERO STATS
            else if (strstr(keyword, "HERO_")) {
                int id = 0;
                char type[50];
                // Separar ID y tipo: HERO_1_HP -> id=1, type="HP"
                if (sscanf(keyword, "HERO_%d_%49s", &id, type) == 2 && id > 0 && id <= HERO_COUNT) {
                    Hero *h = &GAME_HEROES[id - 1];
                    h->id = id;
                    h->is_alive = 1;

                    if (strcmp(type, "HP") == 0) {
                        sscanf(line, "HERO_%*d_HP %d", &h->hp);
                    } else if (strcmp(type, "ATTACK_DAMAGE") == 0) {
                        sscanf(line, "HERO_%*d_ATTACK_DAMAGE %d", &h->attack_damage);
                    } else if (strcmp(type, "ATTACK_RANGE") == 0) {
                        sscanf(line, "HERO_%*d_ATTACK_RANGE %d", &h->attack_range);
                    } else if (strcmp(type, "START") == 0) {
                        sscanf(line, "HERO_%*d_START %d %d", &h->start_pos.x, &h->start_pos.y);
                        h->current_pos = h->start_pos;
                    } else if (strcmp(type, "PATH") == 0) {
                        // Lógica de parseo del PATH: Asume que la línea contiene (x,y) pares
                        char *path_start = strstr(line, "PATH") + 4; 
                        
                        // Contar pares de coordenadas (aprox.)
                        int count = 0;
                        for (char *p = path_start; *p; p++) {
                            if (*p == '(') count++;
                        }
                        
                        // Asignar memoria para el PATH. Si se leyó en un paso previo, liberarla.
                        if (h->path) free(h->path);
                        h->path = (Coords*)calloc(count + 1, sizeof(Coords)); // +1 por seguridad
                        
                        // Parsear: buscar (x,y) con sscanf
                        char *ptr = path_start;
                        int i = 0;
                        while (i < count && sscanf(ptr, "%*[^(](%d,%d)", 
                                                    &h->path[i].x, &h->path[i].y) == 2) {
                            i++;
                            // Mover el puntero para buscar el siguiente par (x,y)
                            ptr = strchr(ptr + 1, '(');
                            if (!ptr) break;
                        }
                        h->path_length = i;
                    }
                }
            }
            // MONSTER STATS
            else if (strstr(keyword, "MONSTER_")) {
                int id = 0;
                char type[50];
                if (sscanf(keyword, "MONSTER_%d_%49s", &id, type) == 2 && id > 0 && id <= MONSTER_COUNT) {
                    Monster *m = &GAME_MONSTERS[id - 1];
                    m->id = id;
                    m->is_alive = 1;

                    if (strcmp(type, "HP") == 0) {
                        sscanf(line, "MONSTER_%*d_HP %d", &m->hp);
                    } else if (strcmp(type, "ATTACK_DAMAGE") == 0) {
                        sscanf(line, "MONSTER_%*d_ATTACK_DAMAGE %d", &m->attack_damage);
                    } else if (strcmp(type, "VISION_RANGE") == 0) {
                        sscanf(line, "MONSTER_%*d_VISION_RANGE %d", &m->vision_range);
                    } else if (strcmp(type, "ATTACK_RANGE") == 0) {
                        sscanf(line, "MONSTER_%*d_ATTACK_RANGE %d", &m->attack_range);
                    } else if (strcmp(type, "COORDS") == 0) {
                        sscanf(line, "MONSTER_%*d_COORDS %d %d", &m->current_pos.x, &m->current_pos.y);
                    }
                }
            }
        }
    }

    fclose(file);
    return 0;
}

// --- FUNCIÓN PRINCIPAL ---

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_configuracion>\n", argv[0]);
        return 1;
    }

    // 1. Parsear configuración
    if (parse_config(argv[1]) != 0) {
        return 1;
    }
    
    // 2. Inicializar Mutex
    if (pthread_mutex_init(&game_mutex, NULL) != 0) {
        perror("Error al inicializar mutex");
        return 1;
    }

    printf("--- Iniciando Simulación Doom a escala ---\n");
    printf("Mapa: %dx%d. Héroes: %d. Monstruos: %d.\n", GRID_WIDTH, GRID_HEIGHT, HERO_COUNT, MONSTER_COUNT);

    // 3. Crear Threads
    pthread_t *hero_tids = (pthread_t*)calloc(HERO_COUNT, sizeof(pthread_t));
    pthread_t *monster_tids = (pthread_t*)calloc(MONSTER_COUNT, sizeof(pthread_t));

    // Threads de los Héroes (N threads)
    for (int i = 0; i < HERO_COUNT; i++) {
        pthread_create(&hero_tids[i], NULL, hero_thread, &GAME_HEROES[i]);
    }

    // Threads de los Monstruos (M threads)
    for (int i = 0; i < MONSTER_COUNT; i++) {
        pthread_create(&monster_tids[i], NULL, monster_thread, &GAME_MONSTERS[i]);
    }

    // 4. Esperar que terminen (Join)
    // Esperar a todos los Héroes
    for (int i = 0; i < HERO_COUNT; i++) {
        pthread_join(hero_tids[i], NULL);
    }

    // Esperar a todos los Monstruos
    for (int i = 0; i < MONSTER_COUNT; i++) {
        // En un juego real, se usaría una variable de estado global para parar.
        // Aquí, simplemente aseguramos que el thread termine.
        pthread_join(monster_tids[i], NULL); 
    }
    
    printf("--- Simulación Finalizada ---\n");

    // 5. Limpieza (Importante para evitar fugas de memoria)
    pthread_mutex_destroy(&game_mutex);
    for(int i = 0; i < HERO_COUNT; i++) {
        if (GAME_HEROES[i].path) free(GAME_HEROES[i].path);
    }
    if (GAME_HEROES) free(GAME_HEROES);
    if (GAME_MONSTERS) free(GAME_MONSTERS);
    if (hero_tids) free(hero_tids);
    if (monster_tids) free(monster_tids);

    return 0;
}
