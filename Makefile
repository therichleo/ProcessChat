all: server2 cliente2 reportes

server2: Server/server2.c
	gcc Server/server2.c -o server2

reportes: Reportes/reportes.c
	gcc Reportes/reportes.c -o reportes

cliente2: Cliente/cliente2.c
	gcc Cliente/cliente2.c -o cliente2

run: all
	osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./server2"' &
	sleep 1
	osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./reportes"' &
	sleep 1
	osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./cliente2"' &
	osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./cliente2"' &

clean:
	rm -f server2 cliente2 reportes

.PHONY: all run clean
