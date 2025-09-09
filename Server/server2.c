#include <sys/stat.h> //Para fifo y mode t
#include <fcntl.h> //PARA O_WRONLY (habilita que el archivo pipe pueda escribir pero no leer)
#include <unistd.h> //Para open() and close()
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

//estructura para envio y recibo de mensajes, esto hace que envien PID con mensaje
#define MSG_SIZE 256
typedef struct {
    int pid;
    char mensaje[MSG_SIZE];
} packet_t;

const char *ChatGeneralFIFO = "tmp/processchat_comunnity_chat"; //PIPE donde el server enviara mensajes generales
const char *ClientTalkFIFO = "tmp/processchat_client_talk"; //PIPE donde clientes envian mensaje al server
int fd;
int fd2;


void cleanup_handler(int sig){
    printf("\nCerrando servidor...\n");
    if(fd != -1) {
        close(fd); //Si no hubo error el cual cerrara la pipe, signfica que la pipe sigue, entonces la cierra
    }
    if(fd2 != -1){
        close(fd2); //Si no hubo error el cual cerrara la pipe, signfica que la pipe sigue, entonces la cierra
    }
    unlink(ChatGeneralFIFO);
    unlink(ClientTalkFIFO);
    exit(0);
}

int main(){

    //Configurar manejador de señales para limpieza para CTRL+C
    signal(SIGINT, cleanup_handler);

    //limpia los pipes
    unlink(ChatGeneralFIFO);
    unlink(ClientTalkFIFO);

    int x;
    ssize_t bytes_read;
    mode_t fifo_permissions = 0666; //permisos para lectura y escritura

    //Crear FIFO server talks, en donde el central enviara los mensajes de los logs
    if(mkfifo(ChatGeneralFIFO, fifo_permissions) == -1 ){
        
        printf("Error creando FIFO ChatGeneralFIFO");
        return(1);
        
    }
    printf("ChatGeneralFIFO creado exitosamente... \n");

    //Crear FIFO Client talks, en donde el cliente enviara mensajes, el server los escucha
    if(mkfifo(ClientTalkFIFO, fifo_permissions) == -1 ){
        printf("Error creando FIFO ClientTalkFIFO");
        return(1);
        
    }
    printf("ClientTalkFIFO creado exitosamente... \n");

    /*
    IDEA:
    El cliente envia mensajes por ClientTalksFIFO, el server los escucha
    El Server reenvia los mensajes por ChatGeneralFIFO, y los clientes lo escuchan
    */

    //Abrimos pipe en modo Lectura
    fd = open(ChatGeneralFIFO, O_WRONLY);
    if(fd == -1){
        printf("Error abriendo la PIPE ChatGeneralFIFO");
        return(1);
    }

    //Abrimos pipe en modo escritura
    fd2 = open(ClientTalkFIFO, O_RDONLY);
    if(fd2 == -1){
        printf("Error abriendo la PIPE ClientTalkFIFO");
    }


    int chats;
    printf("Cantidad de chats a crear: ");
    scanf("%d",&chats);

    while(chats){ 


        //crear el numero de chats, los cuales cada chat sea acompañado con su chat



        --chats;
    }
    
    packet_t pkt;
    ssize_t n;
    char temp[1200];

    while(1){
        while ((n = read(fd2, &pkt, sizeof(pkt))) > 0) { //Si lee algo entra
        if (n != sizeof(pkt)) {
            continue;
        }
        printf("PID=%d, Mensaje=\"%s\"\n", pkt.pid, pkt.mensaje);
        snprintf(temp, sizeof(temp), "%d: %s", pkt.pid, pkt.mensaje);  //Pasa los datos PID y Mensaje a un char "PID: Mensaje"
        write(fd, temp, strlen(temp) + 1); //Se pone +1 ya que en una cadena de char en C es 'H' 'O' 'L' 'A' '\0'
        }
    }
    
    return(0);
}