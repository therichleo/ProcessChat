/*

IDEA PRIMERO CONECTAR A PIPE
LUEGO MANDAR MENSAJE A PIPE CON SU PID
Y DAR UN MENU DE OPCIONES DE REPORTES O DE MENSAJE

*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

const char *ServerListenFIFO = "/tmp/processchat_server_fifo";
const char *ServerTalksFIFO = "/tmp/processchat_server_talks";
const char *ServerTalkToClient1 = "/tmp/processchat_server_talk_to_client";
int fd = -1;
int fd2 = -1;
mode_t fifo_permissions = 0666;


//FUNCION PARA CTRL+C
void cleanup_handler(int sig){
    printf("\nCerrando cliente...\n");
    int x = -1;
    if (fd != -1) {
        write(fd, &x, sizeof(x));
        close(fd);
    }
    exit(0);
}

int main(){
    //le aplica señal sigint por interrumpcion (control + c )
    const char stringX = "A";

    signal(SIGINT, cleanup_handler);

    //Estructura creada de mensaje
    /*
    IDEA: enviar id procces
    */
    struct Message{
        int ProcessID;
        char Palabra[100];
    };
   

    ssize_t bytes_writed;

    pid_t my_pid = getpid();
    int x = my_pid;
    int z = -1;

    char word[100];
    struct Message mensaje_enviado = {my_pid,scanf("%c",&word)};



    printf("Cliente iniciando con PID: %d\n", x);
    
    //Intentar abrir el FIFO (el servidor debe estar ejecutándose)
    fd = open(ServerListenFIFO, O_WRONLY);
    if(fd == -1) {
        printf("Error: No se puede conectar al servidor.\n");
        printf("Asegúrate de que el servidor esté ejecutándose.\n");
        return 1;
    }

    //Enviar PID al servidor
    if(write(fd, &x, sizeof(x)) == -1) {
        printf("Error enviando PID al servidor: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    
    printf("PID %d enviado al servidor exitosamente\n", x);

    //limpiamos server talks
    unlink(ServerTalksFIFO);

    //creamos server talks
    if(mkfifo(ServerTalksFIFO,fifo_permissions) == -1){
        if(errno != EEXIST){
            printf("Error creand FIFO ServerTalksFIFO: %s\n", strerror(errno));
            return 1;
        }
    }
    printf("\nServidor Server Talks FIFO iniciado.\n");

    fd2 = open(ServerTalksFIFO, O_RDWR); 
    if(fd2 == -1){
        printf("Error abriendo SeverTalksFIFO: %s\n", strerror(errno));
        unlink(ServerTalksFIFO);
        return 1;
    }

    int opciones;
    //Definimos estructura de mensaje a enviar

    while(1){
        printf("Elije numero:\nOpcion 1: Chatear\nOpcion 2: Reportar\nOpcion 3: Desconectar\nOpcion: ");
        scanf("%d", &opciones);
        if(opciones == 1){
            ssize_t bytes_read = read(fd2, &z, sizeof(z));
            bytes_writed = write(fd,&mensaje_enviado,sizeof(struct Message));
            printf("SI QUIERES SALIR DEL CHAT PON (-1)");
            //aqui se envia estructura de mensaje
            const char chat_reader = read();
            printf("IMPRIMIMOS VARIABLE QUE NOS DEVUELVE EL CENTRAL");
            while(1){
                if(word == -1){
                    break;
                }
                bytes_writed = write(fd,&mensaje_enviado,sizeof(struct Message));

            }

            
        }

    }
    


    
 

    cleanup_handler(0);
    return 0;
}
