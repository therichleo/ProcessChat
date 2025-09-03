#include <sys/stat.h> //Para fifo y mode t
#include <fcntl.h>. //PARA O_WRONLY (habilita que el archivo pipe pueda escribir pero no leer)
#include <unistd.h> //Para open() and close()

/*
ICMP Echo Requests
utilizare este protocolo como seguimiento
al iniciar un cliente enviara su PID al server, este la recibe y les dice a todos, estos son los nodos disponibles para charlar
y si alguien quiere charlar con otro nodo, el nodo enviara el mensaje y dira a que pid quiere llegar, el server le mandara el mensaje hacia alla
*/
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