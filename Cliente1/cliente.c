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


int main(){

    int fd;

    const char *fifo_path = "Users/leonardovaldeslizana/Proyects/ProcessChat/Cliente1";
    mode_t fifo_permissions = 0666;

    mkfifo(fifo_path,fifo_permissions);
    fd = open(fifo_path,O_WRONLY);
    pid_t my_pid = getpid();

    write(fd, &my_pid, sizeof(my_pid));
    printf("My PID: %d\n", my_pid);

    

    close(fd);
    unlink(fifo_path);
    return 0;
}