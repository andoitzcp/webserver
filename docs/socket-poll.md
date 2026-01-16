La relación entre un socket y la función poll() es fundamental para la eficiencia de un servidor web, ya que permite la multiplexación de E/S (entrada/salida). En términos sencillos, mientras que el socket es el canal de comunicación, poll() es el vigilante que supervisa muchos de estos canales al mismo tiempo

**Relacionan técnica**

1. El Socket como Descriptor de Archivo

Un socket, una vez creado con la función socket(), es tratado por el sistema operativo como un descriptor de archivo (un número entero) Dado que las funciones de red pueden ser lentas o quedarse esperando datos, un servidor que maneje cada socket de forma individual y bloqueante solo podría atender a un cliente a la vez

2. Supervisión mediante poll()

La función poll() tiene la tarea de monitorear un conjunto de estos descriptores de archivo (sockets) para ver si es posible realizar una operación de E/S sin que el programa se detenga (bloquee)

• El mecanismo: El programador crea un array de estructuras llamado pollfd. Cada estructura contiene el descriptor de un socket y los eventos que se desean supervisar (como POLLIN para lectura o POLLOUT para escritura)

• La espera: El servidor llama a poll() pasando este array. La función "bloquea" el proceso del servidor hasta que al menos uno de los sockets esté listo o se cumpla un tiempo de espera (timeout)

3. Implementación en el Server Core

En el desarrollo de un servidor como webserv, la relación se vuelve estricta por requisitos de rendimiento y diseño:

• E/S no bloqueante: Todos los sockets (tanto los de escucha como los de conexión con clientes) deben configurarse en modo no bloqueante (usando fcntl con O_NONBLOCK)

• Un solo ciclo principal: Se debe utilizar un único poll() (o equivalente como select o epoll) para controlar todas las operaciones de lectura y escritura simultáneamente

**Flujo de trabajo:**
`
1. Se agregan los sockets al array de poll().
2. poll() notifica qué sockets tienen eventos pendientes
3. El servidor recorre el array y, solo para los sockets marcados como listos, ejecuta funciones como accept(), recv() o send()

**Nota**
El servidor entrega una lista de sockets a poll(), y esta le devuelve cuáles están listos para trabajar, permitiendo gestionar miles de clientes con un solo hilo de ejecución
