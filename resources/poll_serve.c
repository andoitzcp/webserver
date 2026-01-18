/*
struct pollfd 
{
    int   fd;      // FD que quieres vigilar
    short events;  // qué te interesa (POLLIN, POLLOUT, ...)
    short revents; // qué ocurrió (lo rellena poll)
};

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

.fds: array de struct pollfd

.nfds: cuántos elementos tiene ese array

.timeout:
	-1 espera indefinidamente
	0 no espera (consulta “instantánea”)
	>0 milisegundos
-Retorna:
	>0 número de FDs con eventos
	0 timeout
	-1 error (mira errno)
Eventos típicos:
	POLLIN: hay datos para leer
	POLLOUT: puedes escribir sin bloquear (o el buffer tiene espacio)
	POLLERR, POLLHUP, POLLNVAL: condiciones de error/cierre/FD inválido
*/

#include <stdio.h>        // printf(), perror()
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), send(), recv()
#include <unistd.h>       // read(), write(), close()
#include <stdlib.h>       // exit(), EXIT_FAILURE
#include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <string.h>       // memset(), strlen()
#include <poll.h>         // poll(), struct pollfd, POLLIN, POLLERR, POLLHUP...

#define PORT 8080

int main(int argc, char const *argv[])
{
    int                 server_fd;
    int                 new_socket;
    long                valread;
    struct sockaddr_in  address;
    int                 addrlen;
    char                *hello;
    struct pollfd       pfds[2];
    int                 poll_ret;
    int                 timeout_ms;
    char                buffer[30000];

    (void)argc;
    (void)argv;

    server_fd = -1;
    new_socket = -1;
    valread = 0;
    addrlen = (int)sizeof(address);
    hello = "hola desde el servidor";
    timeout_ms = -1; /* -1 = poll espera indefinidamente */

    /* =========================
       1) CREACIÓN (socket)
       ========================= */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* =========================
       2) VINCULACIÓN (bind)
       ========================= */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; /* 0.0.0.0: escucha en todas las interfaces */
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* =========================
       3) ESCUCHA (listen)
       =========================
       listen() pone el socket en modo "servidor".
       A partir de aquí, server_fd NO es para read/write de datos del cliente,
       sino para aceptar conexiones con accept().

       Nota: el backlog (10) define cuántas conexiones pueden quedarse "en cola"
       esperando a que el servidor las acepte.
    */
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /*
     * =========================
     * 4) poll(): configuración general
     * =========================
     * Vamos a usar poll() para NO quedarnos bloqueados en accept() o read().
     *
     * IDEA:
     *  - Vigilar server_fd con POLLIN:
     *      si poll dice POLLIN, significa "hay al menos 1 conexión lista"
     *      => llamamos accept() con seguridad (sin bloquear).
     *
     *  - Tras aceptar un cliente, vigilar new_socket con POLLIN:
     *      si poll dice POLLIN, significa "hay datos listos para leer"
     *      => llamamos read() con seguridad (sin bloquear).
     *
     * Con esto, el servidor se vuelve más controlable: decide CUÁNTO esperar
     * y en qué momento hacer accept/read.
     */

    /* Inicialmente solo vigilamos el socket de escucha */
    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN; /* Objetivo: detectar nuevas conexiones entrantes */
    pfds[0].revents = 0;

    pfds[1].fd = -1;         /* -1 => entrada inactiva (no se vigila) */
    pfds[1].events = 0;
    pfds[1].revents = 0;

    while (1)
    {
        printf("\n+++++++ Waiting for activity (poll) ++++++++\n\n");

        /*
         * =========================
         * poll(): esperar eventos
         * =========================
         * Objetivo: dormir el proceso hasta que ocurra algo interesante:
         *  - nueva conexión (POLLIN en server_fd)
         *  - datos del cliente (POLLIN en new_socket)
         *  - o errores/cierre (POLLERR/POLLHUP)
         *
         * timeout_ms = -1 => espera infinita.
         * Podrías poner, por ejemplo, 2000 para 2s y hacer otras tareas.
         */
        poll_ret = poll(pfds, 2, timeout_ms);
        if (poll_ret < 0)
        {
            perror("poll");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        /*
         * =========================
         * 5) ¿Hay nueva conexión?
         * =========================
         * Si poll marcó POLLIN en server_fd:
         * Objetivo: aceptar conexiones SIN BLOQUEAR.
         */
        if (pfds[0].revents & POLLIN)
        {
            new_socket = accept(server_fd,
                                (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
            if (new_socket < 0)
            {
                perror("accept");
                /* seguimos vivos: simplemente continuamos */
            }
            else
            {
                printf("Nueva conexión aceptada (fd=%d)\n", new_socket);

                /*
                 * Ahora activamos el segundo pollfd para vigilar al cliente.
                 * Objetivo: esperar datos del cliente (POLLIN) o detectar cierre/error.
                 */
                pfds[1].fd = new_socket;
                pfds[1].events = POLLIN; /* Objetivo: leer cuando haya datos listos */
                pfds[1].revents = 0;
            }
        }

        /*
         * =========================
         * 6) ¿Hay datos del cliente?
         * =========================
         * Si tenemos cliente activo (pfds[1].fd != -1) y poll indicó evento:
         */
        if (pfds[1].fd != -1)
        {
            /* Si el cliente cerró o hay error, poll suele reflejarlo aquí */
            if (pfds[1].revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                printf("Cliente desconectado o error (revents=%d). Cerrando fd=%d\n",
                    pfds[1].revents, pfds[1].fd);
                close(pfds[1].fd);
                pfds[1].fd = -1;
                pfds[1].events = 0;
                pfds[1].revents = 0;
            }
            else if (pfds[1].revents & POLLIN)
            {
                /*
                 * poll indicó POLLIN:
                 * Objetivo: llamar read() sabiendo que hay datos disponibles
                 * (o que el peer cerró, en cuyo caso read devuelve 0).
                 */
                memset(buffer, 0, sizeof(buffer));
                valread = read(pfds[1].fd, buffer, sizeof(buffer) - 1);
                if (valread < 0)
                {
                    perror("read");
                    close(pfds[1].fd);
                    pfds[1].fd = -1;
                    pfds[1].events = 0;
                    pfds[1].revents = 0;
                }
                else if (valread == 0)
                {
                    /* El cliente cerró la conexión de forma limpia */
                    printf("El cliente cerró la conexión (fd=%d)\n", pfds[1].fd);
                    close(pfds[1].fd);
                    pfds[1].fd = -1;
                    pfds[1].events = 0;
                    pfds[1].revents = 0;
                }
                else
                {
                    /* Recibimos datos */
                    printf("Recibido del cliente:\n%s\n", buffer);

                    /* Respondemos al cliente */
                    if (write(pfds[1].fd, hello, strlen(hello)) < 0)
                    {
                        perror("write");
                        close(pfds[1].fd);
                        pfds[1].fd = -1;
                        pfds[1].events = 0;
                        pfds[1].revents = 0;
                    }
                    else
                        printf("------------------Hello message sent-------------------\n");

                    /*
                     * Este servidor sigue tu lógica original: atiende 1 mensaje
                     * y cierra el cliente.
                     */
                    close(pfds[1].fd);
                    pfds[1].fd = -1;
                    pfds[1].events = 0;
                    pfds[1].revents = 0;
                }
            }
        }

        /*
         * Importante:
         * poll() NO "resetea" revents automáticamente como tú quieres en todos los casos.
         * Es buena práctica limpiar revents a mano antes del siguiente poll.
         */
        pfds[0].revents = 0;
        if (pfds[1].fd != -1)
            pfds[1].revents = 0;
    }

    close(server_fd);
    return (0);
}
