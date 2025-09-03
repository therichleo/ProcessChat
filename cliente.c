/*

IDEA PRIMERO CONECTAR A PIPE
LUEGO MANDAR MENSAJE A PIPE CON SU PID
Y DAR UN MENU DE OPCIONES DE REPORTES O DE MENSAJE

*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>

int main(){

    int fd;

    const char *fifo_path = "Users/leonardovaldeslizana/Proyects/ProcessChat";
    mode_t fifo_permissions = 0666;


    pid_t my_pid = getpid();
    printf("My PID: %d\n", my_pid);

    mkfifo(fifo_path,fifo_permissions);


}