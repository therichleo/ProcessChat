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



int main(){

    int fd;
    const char *ServerListenFIFO = "/tmp/processchat_server_fifo";
    pid_t my_pid = getpid();
    int x = my_pid;

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


    
    close(fd);
    //NO eliminar el FIFO, es responsabilidad del servidor
    return 0;
}
