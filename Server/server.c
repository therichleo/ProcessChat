#include <sys/stat.h> //Para fifo y mode t
#include <fcntl.h> //PARA O_WRONLY (habilita que el archivo pipe pueda escribir pero no leer)
#include <unistd.h> //Para open() and close()
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

/*
ICMP Echo Requests
utilizare este protocolo como seguimiento
al iniciar un cliente enviara su PID al server, este la recibe y les dice a todos, estos son los nodos disponibles para charlar
y si alguien quiere charlar con otro nodo, el nodo enviara el mensaje y dira a que pid quiere llegar, el server le mandara el mensaje hacia alla



-> Lo que necesitamos es, un fifo servidor el cual todos los clientes enviaran su PID
-> un fifo para enviar info a cada cliente con los clientes disponibles para chatear
-> un fifo para cada cliente que se utilizara para recibir 

*/

const char *ServerListenFIFO = "/tmp/processchat_server_fifo";
const char *ServerTalksFIFO = "tmp/processchat_server_talks";
const char *ServerTalkToClient1 = "tmp/processchat_server_talk_to_client";
int fd = -1;

void cleanup_handler(int sig){
    printf("\nCerrando servidor...\n");
    if(fd != -1) {
        close(fd);
    }
    unlink(ServerListenFIFO);
    unlink(ServerTalksFIFO);
    unlink(ServerTalkToClient1);
    exit(0);
}

int main(){
    int x;
    ssize_t bytes_read;
    mode_t fifo_permissions = 0666; //permisos para lectura y escritura

    //Configurar manejador de señales para limpieza
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    //Limpiar FIFO previo si existe
    unlink(ServerListenFIFO);
    
    //Crear FIFO
    if(mkfifo(ServerListenFIFO, fifo_permissions) == -1) {
        if(errno != EEXIST) {
            printf("Error creando FIFO: %s\n", strerror(errno));
            return 1;
        }
    }
    
    printf("Servidor iniciado. Esperando clientes en %s\n", ServerListenFIFO);
    printf("Presiona Ctrl+C para salir.\n");
    
    fd = open(ServerListenFIFO, O_RDWR); //O_RDWR evita bloqueo cuando no hay escritores

    if(fd == -1){
        printf("Error abriendo FIFO: %s\n", strerror(errno));
        unlink(ServerListenFIFO);
        return 1;
    }

    while(1){
        bytes_read = read(fd, &x, sizeof(x));
        if(x == -1){
            printf("Cliente cerro conexion");
        } else {
            if(bytes_read > 0) {
            printf("Cliente conectado - PID: %d\n", x);
            } else if(bytes_read == 0) {
                //EOF, el escritor se desconectó
                printf("Cliente desconectado\n");
            } else if(bytes_read == -1 && errno != EAGAIN) {
                printf("Error leyendo FIFO: %s\n", strerror(errno));
                break;
            }
        }
        
    }
    
    cleanup_handler(0);
    return 0;
}
