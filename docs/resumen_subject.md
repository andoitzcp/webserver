
# Resumen Subject - [subject completo](/hocs/en.subject.pdf)

## Objetivo

Codificar un Servidor HTTP en C++98.

```bash
# Ejecucion
./webserv [config_file]
```

## Entrega

- Nombre del programa: `webserv`
- Archivos a entregar: Makefile, .hpp, .cpp, .tpp, .ipp y archivos de configuracion
- Argumentos: archivo de configuracion

## Funciones permitidas

- execve  
- pipe
- strerror
- gai_strerror
- errno
- dup
- dup2
- fork
- socketpair
- htons
- htonl
- ntohs
- ntohl,
- select
- poll
- epoll (epoll_create, epoll_ctl, epoll_wait)
- kqueue (kqueue, kevent)
- socket
- accept
- listen
- send
- recv
- chdir
- bind
- connect
- getaddrinfo
- freeaddrinfo
- setsockopt
- getsockname
- getprotobyname
- fcntl
- close
- read
- write
- waitpid
- kill
- signal
- access
- stat
- open
- opendir
- readdir
- closedir

## Requisitos

### Condicionantes

- [] No se puede hacer un `execve()` de otro webserv.
- [] Nunca se debe escribir o leer sin pasar por `poll()`
- [] No se puede usar `errno` como condicion para el funcionamiento del servidor 
despues de operaciones de escritura o lectura
- [] No es necesario usar `poll()` para la lectura o escritura en disco 
- [] Se puede usar cualquier macro asociada a `poll()` o equivalente (p.e. FD_SET)
- [] Solo se puede usar `fork()` para CGI (php, python, etc)

### Features

- [] Archivo de configuracion con una ruta por defecto o como argumento.
- [] El servidor `non-block` en todo momento 
- [] Gestion de desconexiones de los clientes
- [] `poll()` debe gestionar la escritura y lectura de forma simultanea
- [] Solo 1 `poll()` o similar para todas las operaciones de E/S
- [] Ninguna request deberia congelar el servicio
- [] Comparar comportamiento del servidor con `NGINX`
- [] Devolver `status-code` correctos
- [] Paginas de error por defecto
- [] Compatibilidad con exploradores estandar a nuestra eleccion
- [] Capacidad para mostrar un sitio web completamente estatico
- [] Poder subir archivos como cliente
- [] Metodos minimos: GET, POST y DELETE
- [] Disponibilidad bajo estres
- [] Capacidad para escuchar por varios puertos (mirar archivo de configuracion)

## Archivo de configuraion

### Parametros

- [] Puertos de escucha
- [] Definir paginas de error por defecto
- [] Especificar configuraciones para una URL concreta
    - [] Lista de metodos HTTP aceptados
    - [] Redireccion HTTP
    - [] Ruta del directorio donde encontrar el archivo especificado
    - [] Habilitar / Deshabilitar listado de directorios
    - [] Archivo por defecto cuando se pide un directorio
    - [] Autorizacion para subir archivos y directorio correspondiente
    - [] Configuracion de CGI en base a la extension del archivo
