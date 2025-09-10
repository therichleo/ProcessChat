#include <unistd.h> //funciones basicas como write open, etc
#include <sys/types.h> //para varibales como ssize_t pid_t
#include <stdio.h> //para prinft snprintf
#include <fcntl.h> //constantes para abrir los PIPES (O_WRONLY etc)
#include <sys/stat.h> //MKFIFOS para permisos 0666
#include <errno.h> //para errores
#include <string.h> //para manejar cadenas de memoria
#include <stdlib.h> //exit cleanup_handler
#include <signal.h> //señales como control c

#define MSG_SIZE 256
typedef struct {
    int pid;
    char mensaje[MSG_SIZE];
} estructure;

const char *ClientTalkFIFO = "/tmp/processchat_client_talk"; //PIPE donde clientes envian mensaje al server
const char *ReportsFIFO = "/tmp/processchat_reports";
int fd2 = -1;
int fd3 = -1;
int contador = 0;

//FUNCION PARA CTRL+C
void cleanup_handler(int sig){
    printf("\nCerrando cliente...\n");
    if(fd2 != -1){
        close(fd2);
    }
    exit(0);
}

void usr1_handler(int sig){
    contador++;
    if(contador == 10){
        close(fd3);
        close(fd2);
        exit(0);
    }
}

int main(){

    signal(SIGUSR1, usr1_handler);

    signal(SIGINT, cleanup_handler); //interrupciones como control + c

    pid_t mypid = getpid(); //conseguimos nuestra PID
    printf("Hola mi PID es: %d \n",(int)mypid); 
    
    fd2 = open(ClientTalkFIFO,O_WRONLY); //escribe mensajes hacia el servidor
    if(fd2 == -1){
        printf("Error arbiendo PIPE ClientTalkFIFO en Cliente de pid: %d \n",(int)mypid);
    }
    fd3 = open(ReportsFIFO,O_WRONLY); //escribe mensajes hacia el reportador, enviara solo el PID a reportar y ya 
    if(fd3 == -1){
        printf("Error abriendo PIPE ReportsFIFO");
    }

    estructure pkt;

    snprintf(pkt.mensaje, MSG_SIZE, "%d: se conectó exitosamente", (int)mypid); //función que arma un texto formateado (como printf) pero lo escribe dentro de un buffer (un char[])
    if (write(fd2, &pkt, sizeof(pkt)) < 0) {
        perror("write (mensaje inicial)");
        cleanup_handler(0);
    }

    printf("\nPuedes escribir mensajes en este terminal, si pones de mensaje (-1) iras directamente al menu de reportes: \n");
    ssize_t bytes_read;
    char msg[256];

    int target_pid;

    while(1){
        printf("Escribe mensaje: ");
        fgets(pkt.mensaje, MSG_SIZE, stdin); //lee de teclado y lo guarda en variable pkt.mensaje y el tamaño de bytes de msg_size
        size_t L = strlen(pkt.mensaje);  //calcula longitud de la cadena leida
        if (L && pkt.mensaje[L-1] == '\n') pkt.mensaje[L-1] = '\0'; //limpia mensaje, quita el salto de linea por el 0
        if (strcmp(pkt.mensaje, "-1") == 0) { //compara con el -1
            printf("Menu de reportes: \n");
            printf("Digita el PID a reportar: ");
            scanf("%d",&target_pid);
            write(fd3,&target_pid,sizeof(target_pid));
        } else {
            pkt.pid = mypid;
            write(fd2,&pkt, sizeof(pkt));
            printf("\n");
        }
    }    
}
