# 📨 ProcessChat – Chat Multi-Proceso con FIFOs y Señales

Este proyecto implementa un **chat multi-proceso** en **C** usando **Pipes con nombre (FIFOs)** y **señales** en sistemas tipo Unix (probado en macOS).  

Permite comunicación entre **clientes**, un **servidor** central y un **módulo de reportes** que actúa como moderador.  

---

## 🚀 Funcionamiento

El sistema está dividido en **tres programas principales**:

1. **Servidor** 🖥️  
   - Recibe mensajes de los clientes.  
   - Los muestra en pantalla junto al **PID** de quien los envía.  
   - Responde con una confirmación de éxito.  
   - Si un cliente se desconecta, reabre la FIFO para aceptar nuevos.  

2. **Clientes** 👤  
   - Obtienen su **PID** al conectarse.  
   - Envían mensajes al servidor.  
   - Pueden abrir un menú de opciones:  
     1. **Reportar** → envía un PID al proceso de reportes.  
     2. **Copiar** → abre una nueva terminal con otra instancia del cliente (usando `fork` y `osascript`).  
   - Si reciben la señal `SIGUSR1` (por ser reportados), incrementan un contador.  
   - Al llegar a 10 reportes, el cliente se cierra automáticamente.  

3. **Reportes** 🛡️  
   - Actúa como un **moderador**.  
   - Recibe reportes mediante un FIFO especial.  
   - Envía una señal `SIGUSR1` al cliente reportado.  

---

### 📡 Comunicación Principal

Se usan **FIFOs** para interconectar procesos:

- `processchat_client_talk` → los **clientes escriben** mensajes → el **servidor los lee**.  
- `processchat_server_talk` → el **servidor responde** → los **clientes leen**.  
- `processchat_reports` → los **clientes envían reportes** → el **módulo de reportes los procesa**.

📊 **Flujo de información**:  
Cliente ───► Servidor ───► Confirmación Cliente ───► Reportes ───► Señal SIGUSR1 ───► Cliente reportado

---

## ⚙️ Ejecución

Este proyecto se desarrolló en una **MacBook Pro 13"** (macOS), usando:

- **Terminal.app** (para correr múltiples instancias de clientes).
- **osascript** (nativo de macOS, para abrir nuevas terminales automáticamente).
- **Xcode Command Line Tools** para compilar (`gcc`).

👉 Si no tienes instaladas las herramientas de compilación:  
```bash
xcode-select --install
````

- Aunque por defecto es todo nativo de la macbook

---

### ▶️ Compilación y ejecución

El proyecto incluye un **Makefile** con los comandos principales:

```bash
make all       # Compila todos los programas
make run       # Ejecuta el servidor y abre clientes
make clean     # Limpia binarios
make cleanup   # Borra FIFOs generados
make fullclean # Limpia todo completamente
```

Si se desea abrir más clientes de manera automática, editar el **Makefile** agregando más líneas como:

```makefile
osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./cliente2"' &
osascript -e 'tell application "Terminal" to do script "cd $(PWD) && ./cliente2"' &
```

---

## 🏗️ Estructura del Proyecto

```
ProcessChat/
├── Makefile
├── Server/
│   └── server2.c
├── Cliente/
│   └── cliente2.c
└── Reportes/
    └── reportes.c
```

---

## 📖 Conceptos Clave

* **FIFOs (Pipes con nombre)** → permiten comunicación entre procesos no relacionados.
* **Señales (SIGUSR1, SIGINT)** → notifican eventos a procesos específicos.
* **Fork + osascript** → los clientes pueden replicarse en nuevas terminales.
* **Sincronización** → el servidor espera bloqueado hasta que un cliente escriba.

---

## 🎯 Ejemplo de uso

1. Inicia el servidor con `make run`.
2. Abre clientes (manual o automático desde el Makefile).
3. Escribe mensajes:

   * Mensajes normales → enviados al servidor y confirmados.
   * Escribe `-1` para abrir el menú de opciones:

     * **Reportar** → ingresa el PID de otro cliente para reportarlo.
     * **Copiar** → genera una nueva instancia del cliente en otra terminal.
4. El programa de reportes recibe las quejas y envía señales.
5. Cuando un cliente acumula 10 reportes, se cierra.

---

## 📌 Resumen

**ProcessChat** es un ejemplo práctico de:

* Comunicación entre procesos (**IPC**) con **pipes con nombre**.
* Manejo de procesos y señales en C.
* Integración con herramientas de macOS para replicar clientes.

Un sistema de chat sencillo pero completo, que muestra **cómo coordinar múltiples procesos concurrentes** en un entorno Unix.

```





