#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define MSG_SIZE 256
typedef struct {
    int pid;
    char mensaje[MSG_SIZE];
} estructure;

const char *ClientTalkFIFO = "/tmp/processchat_client_talk"; //PIPE donde clientes envian mensaje al server
int fd2 = -1;

//FUNCION PARA CTRL+C
void cleanup_handler(int sig){
    printf("\nCerrando cliente...\n");
    if(fd2 != -1){
        close(fd2);
    }
    exit(0);
}

int main(){

    signal(SIGINT, cleanup_handler); //interrupciones como control + c

    pid_t mypid = getpid(); //conseguimos nuestra PID
    printf("Hola mi PID es: %d \n",(int)mypid); 
    
    fd2 = open(ClientTalkFIFO,O_WRONLY); //escribe mensajes hacia el servidor
    if(fd2 == -1){
        printf("Error arbiendo PIPE ClientTalkFIFO en Cliente de pid: %d \n",(int)mypid);
    }


    estructure pkt;
    printf("Escribe mensaje:");
    ssize_t bytes_read;
    char msg[256];


    while(1){
        
        fgets(pkt.mensaje, MSG_SIZE, stdin); //lee de teclado y lo guarda en variable pkt.mensaje y el tamaño de bytes de msg_size
        size_t L = strlen(pkt.mensaje);  //calcula longitud de la cadena leida
        if (L && pkt.mensaje[L-1] == '\n') pkt.mensaje[L-1] = '\0'; //limpia mensaje, quita el salto de linea por el 0
        if (strcmp(pkt.mensaje, "-1") == 0) { //compara con el -1
            printf("Cliente finalizado por el usuario.\n");
            cleanup_handler(0);
        }
        pkt.pid = mypid;
        write(fd2,&pkt, sizeof(pkt));
    }    


}
