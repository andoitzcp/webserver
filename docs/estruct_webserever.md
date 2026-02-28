# Esquema para desarrollar un Webserver en C++ (Estándar C++98)

. 	**Configuración e Inicialización:**
        Desarrollar un analizador del archivo de configuración para definir puertos, nombres de servidor, rutas raíz (root), tamaños máximos de cuerpo y páginas de error.
        Configurar los sockets para que sean no bloqueantes utilizando fcntl() con O_NONBLOCK.

. 	**Red y Multiplexación (Server Core):**
        Crear sockets de escucha para cada puerto especificado (socket, bind, listen).
        Implementar un bucle principal gobernado por una única llamada a poll() (o equivalente como select o epoll) para supervisar eventos de lectura y escritura en todos los descriptores de archivo simultáneamente.

. 	**Gestión de Solicitudes (Request Parser):**
        Implementar una clase HttpRequest que utilice un método como feed() para procesar datos de forma parcial a medida que llegan del socket.
        Validar los métodos aceptados (mínimo GET, POST y DELETE) y gestionar el almacenamiento de archivos subidos por los clientes.

.   **Lógica de Negocio y Recursos:**

        Archivos estáticos: Servir archivos desde el directorio raíz especificado.
        Listado de directorios: Implementar un autoindex si el recurso solicitado es un directorio.
        CGI (Common Gateway Interface): Ejecutar programas externos (Python, PHP, etc.) para contenido dinámico usando fork y execve, pasando las variables de entorno necesarias.

.	**Generación de Salida (Response Builder):**
		Crear una clase HttpResponse para ensamblar la respuesta completa.
        Asegurar que los códigos de estado sean precisos y que se envíen las páginas de error correspondientes si algo falla.
        Enviar la respuesta de forma no bloqueante a través de send() o write() una vez que poll() indique disponibilidad.

.   **Limpieza y Mantenimiento:**
        Gestionar adecuadamente las desconexiones de los clientes y cerrar los descriptores de archivo con close() para evitar fugas de recursos.

# Funcionamiento del Serve-core

	El núcleo del servidor se encarga de la gestión del ciclo de vida de las conexiones mediante el uso de sockets. Sus tareas principales incluyen:

    Creación y Gestión de Sockets: Utiliza llamadas al sistema como socket() para crear puntos finales de comunicación y bind() para asignarles una dirección y puerto específicos.
    Escucha de Conexiones: Marca un socket como "pasivo" mediante listen(), indicando que está listo para aceptar peticiones entrantes.
    Multiplexación de E/S (I/O Multiplexing): Este es uno de sus objetivos más críticos. Permite supervisar múltiples descriptores de archivos (sockets) simultáneamente para determinar si es posible realizar operaciones de lectura o escritura sin bloquear el servidor. Para esto, se emplean funciones como select(), poll() o epoll().
    Gestión de Sockets no bloqueantes: Para evitar que el servidor se detenga mientras espera datos de un cliente lento, los sockets suelen configurarse en modo no bloqueante (usando fcntl con O_NONBLOCK), lo que permite manejar miles de conexiones de forma eficiente.

**Input del Server Core**
	El Server Core recibe principalmente información bruta y señales de la red:

    .Flujos de datos TCP: Recibe bytes crudos provenientes de los clientes a través de la conexión establecida.

    .Peticiones de conexión: Señales de red de clientes que intentan iniciar un saludo de tres vías (three-way handshake) para establecer una sesión.

    .Parámetros de configuración: Información del archivo de configuración, como los números de puerto en los que debe escuchar y las interfaces de red (direcciones IP) a las que debe vincularse.

    .Eventos de red: Notificaciones del sistema operativo indicando que hay datos listos para ser leídos o que un socket está listo para enviar datos.

**Output del Server Core**
	Una vez procesada la entrada de red, el núcleo genera los siguientes resultados:

    .Nuevos descriptores de socket: Cuando se acepta una conexión mediante accept(), el Server Core genera un nuevo descriptor de archivo específico para la comunicación con ese cliente particular.

    .Buffers de datos para el Parser: Entrega los bytes recibidos de la red al componente Request Parser para que este pueda interpretar la semántica del protocolo HTTP (métodos, cabeceras, etc.).

    .Respuestas enviadas al cliente: Transmite los flujos de datos formateados (provenientes del Response Builder) de vuelta al cliente a través de la red utilizando funciones como send() o write().

    .Gestión de errores de red: Reporta fallos en la conexión o agotamiento de recursos al sistema (por ejemplo, mediante códigos de error de sistema como ECONNREFUSED si la cola de conexiones está llena).

En el contexto de un desarrollo en C++, el Server Core es responsable de mantener la resiliencia del sistema, asegurando que el servidor permanezca operativo incluso bajo pruebas de estrés y gestionando adecuadamente las desconexiones de los clientes
