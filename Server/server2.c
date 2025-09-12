#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define MSG_SIZE 256
typedef struct {
    int pid;
    char mensaje[MSG_SIZE];
} estructure;

const char *ClientTalkFIFO = "/tmp/processchat_client_talk"; // clientes -> servidor
const char *ServerTalkFIFO = "/tmp/processchat_server_talk"; //PIPE donde server envia mensaje al cliente
int fd = -1;
int fd2 = -1;

void cleanup_handler(int sig){
    (void)sig;
    printf("\nCerrando servidor...\n");
    if (fd2 != -1){
        close(fd2); //cierra PIPE si esque se hizo un control c y sigue activa
    }  
    if (fd != -1){
        close(fd);
    }
    unlink(ClientTalkFIFO);
    exit(0);
}

int main(void){
    signal(SIGINT, cleanup_handler);

    unlink(ClientTalkFIFO); //limpia la PIPE

    mode_t fifo_permissions = 0666; //permisos para lectura y escritura
    if (mkfifo(ClientTalkFIFO, fifo_permissions) == -1) {
        fprintf(stderr, "Error creando FIFO %s: %s\n", ClientTalkFIFO, strerror(errno));
        return 1;
    }
    printf("FIFO creada: %s\n", ClientTalkFIFO);

    fd = open(ServerTalkFIFO, O_WRONLY);
    if(fd == -1){
        printf("Error abriendo FIFO ServerTalksFIFO");
        cleanup_handler(0);
    }

    // Abre para lectura (bloquea hasta que un cliente abra en escritura)
    fd2 = open(ClientTalkFIFO, O_RDONLY);
    if (fd2 == -1){
        fprintf(stderr, "Error abriendo FIFO %s en lectura: %s\n", ClientTalkFIFO, strerror(errno));
        cleanup_handler(0);
    }

    printf("Servidor listo. Esperando mensajes...\n");

    estructure pkt;
    ssize_t n;
    int exito = 1;

    while (1) {
        n = read(fd2, &pkt, sizeof(pkt));
        if (n > 0) {
            if (n != sizeof(pkt)) {
                continue;
            }
            pkt.mensaje[MSG_SIZE - 1] = '\0';
            printf("PID=%d, Mensaje=\"%s\"\n", pkt.pid, pkt.mensaje);
            write(fd, exito,sizeof(exito));
                } else if (n == 0) { //si nadie escribe, la cierra y la reabre para nuevas personas entrando
            printf("No hay clientes escribiendo. Esperando nuevos...\n");
            close(fd2);
            fd2 = open(ClientTalkFIFO, O_RDONLY);
            if (fd2 == -1){
                fprintf(stderr, "Error reabriendo FIFO %s: %s\n", ClientTalkFIFO, strerror(errno));
                cleanup_handler(0);
            }
        } else {
            perror("read");
            break;
        }
    }

    cleanup_handler(0);
    return 0;
}
