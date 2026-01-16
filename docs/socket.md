# socket 

Es el mecanismo que los sistemas operativos proporcionan para permitir que los programas accedan a la red, permitiendo el envío y recepción de mensajes entre aplicaciones (procesos) situadas en diferentes máquinas.

**Definición y Características**

• Interfaz de programación: Es la herramienta que permite a una aplicación "enchufarse" a la red. Aunque es independiente del tipo de red, el uso más común es a través de IP

• Descriptor de archivo: Cuando se crea un socket mediante la llamada al sistema socket(), el sistema operativo devuelve un descriptor de archivo (un número entero pequeño) que sirve para identificar ese punto de comunicación en el proceso

**Componentes de un Socket**

Para crear un socket funcional, se deben especificar tres parámetros fundamentales

1. Dominio (Familia de direcciones): Define el protocolo de red. Los más comunes son AF_INET para IPv4 y AF_INET6 para IPv6

2. Tipo: Define cómo se enviarán los datos. Por ejemplo, SOCK_STREAM se utiliza para conexiones fiables y bidireccionales (como TCP), mientras que SOCK_DGRAM se usa para mensajes sin conexión y no garantizados (como UDP)

3. Protocolo: Indica el protocolo específico a utilizar. Generalmente se deja en 0 para que el sistema operativo elija el protocolo predeterminado según el tipo seleccionado

**El Ciclo de Vida en un Servidor**

En el desarrollo de un servidor web (como en el componente Server Core), el uso de sockets sigue pasos específicos

• Creación (socket): Se establece el punto de comunicación

• Vinculación (bind): Se le asigna una "identidad" (una dirección IP y un número de puerto). Siguiendo la analogía anterior, esto sería asignar el número de teléfono a la línea

• Escucha (listen): El servidor marca el socket como "pasivo", indicando que está listo para aceptar conexiones entrantes

• Aceptación (accept): El servidor extrae una petición de la cola y crea un nuevo socket específico para esa conexión particular con el cliente

• Comunicación: Una vez conectados, los datos se intercambian usando funciones como read() y write() o send() y recv()

• Cierre (close): Se finaliza la sesión y se liberan los recursos
