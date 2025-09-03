#include <sys/stat.h> //Para fifo y mode t
#include <fcntl.h>. //PARA O_WRONLY (habilita que el archivo pipe pueda escribir pero no leer)
#include <unistd.h> //Para open() and close()

int main(){

    int fd;

    const char *fifo_path = "Users/leonardovaldeslizana/Proyects/ProcessChat";
    mode_t fifo_permissions = 0666; // permisos para lectura y escritura

    mkfifo(fifo_path,fifo_permissions);



    //SI RECIBO MENSAJE
    if(1){
        fd = open(fifo_path, O_WRONLY);
    }

}