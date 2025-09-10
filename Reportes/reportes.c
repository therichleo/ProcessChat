#include <unistd.h> //funciones basicas como write open, etc
#include <sys/types.h> //para varibales como ssize_t pid_t
#include <stdio.h> //para prinft snprintf
#include <fcntl.h> //constantes para abrir los PIPES (O_WRONLY etc)
#include <sys/stat.h> //MKFIFOS para permisos 0666
#include <errno.h> //para errores
#include <string.h> //para manejar cadenas de memoria
#include <stdlib.h> //exit cleanup_handler
#include <signal.h> //señales como control c

const char *ReportsFIFO = "/tmp/processchat_reports";
int fd3 = -1;

//FUNCION PARA CTRL+C
void cleanup_handler(int sig){
    printf("\nCerrando cliente...\n");
    if(fd3 != -1){
        close(fd3);
    }
    exit(0);
}

int main(){

    signal(SIGINT, cleanup_handler); //interrupciones como control + c
    
    mode_t fifo_permissios = 0666;

    if (mkfifo(ReportsFIFO, fifo_permissios) == -1) {
        fprintf(stderr, "Error creando FIFO %s: %s\n", ReportsFIFO, strerror(errno));
        return 1;
    }

    fd3 = open(fd3, O_RDONLY);
    if(fd3 == -1){
        printf("Error al abrir PIPE");
    }
    printf("PIPE ABIERTO CORRECTAMENTE... \n");

    ssize_t n;
    int target_pìd;

    while(1){
        n = read(fd3, target_pìd, sizeof(target_pìd));
        if(n>1){
            if(n!=sizeof(target_pìd)){
                continue;
            }
            printf("Reporte recibido hacia %d \n",target_pìd);
        } else if(n == 0){
            printf("No hay reportes aun. Esperando nuevos...\n");
            close(fd3);
            fd3 = open(ReportsFIFO,O_RDONLY);
            if(fd3 == -1){
                printf("Error reabriendo pipe");
                cleanup_handler(0);
            }
        } else {
            perror("read");
            break;
        }
    }

}