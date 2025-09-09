#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MSG_SIZE 256
typedef struct {
    int pid;
    char mensaje[MSG_SIZE];
} estructure;

const char *ChatGeneralFIFO = "tmp/processchat_comunnity_chat"; //PIPE donde el server enviara mensajes generales
const char *ClientTalkFIFO = "tmp/processchat_client_talk"; //PIPE donde clientes envian mensaje al server
int fd;
int fd2;

//FUNCION PARA CTRL+C
void cleanup_handler(int sig){
    printf("\nCerrando cliente...\n");
    if (fd != -1) {
        close(fd);
    }
    if(fd2 != -1){
        close(fd2);
    }
    exit(0);
}

int main(){

    signal(SIGINT, cleanup_handler); //interrupciones como control + c

    pid_t mypid = getpid(); //conseguimos nuestra PID
    printf("Hola mi PID es: %d",&mypid); 

    fd = open(ChatGeneralFIFO, O_RDONLY); //Lee mensajes del servidor
    if(fd == -1){
        printf("Error abriendo PIPE ChatGeneralFIFO en Cliente de pid: %d", &mypid);
    }
    
    fd2 = open(ClientTalkFIFO,O_WRONLY); //escribe mensajes hacia el servidor
    if(fd2 == -1){
        printf("Error arbiendo PIPE ClientTalkFIFO en Cliente de pid: $d", &mypid);
    }


    estructure pkt;
    ssize_t n;
    char temp[1200];

    while(1){}    








}
