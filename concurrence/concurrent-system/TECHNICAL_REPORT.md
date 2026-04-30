# Reporte Técnico: Sistema de Procesamiento Concurrente

## 1. Arquitectura del Sistema
El sistema está diseñado como un procesador de tareas distribuido utilizando un patrón productor-consumidor implementado a través de contenedores Docker.

### Componentes:
- **Base de Datos (PostgreSQL)**: El coordinador central y almacén de estado.
- **Servicio de Inicialización (`init`)**: Un contenedor de corta duración que llena la base de datos con las tareas iniciales.
- **Servicios Trabajadores (`worker1` a `worker5`)**: Múltiples instancias contenedorizadas que procesan tareas en paralelo.
- **Volumen Compartido**: Un sistema de archivos compartido para registrar los eventos de procesamiento.

### Organización de Directorios:
- `db/`: Contiene los scripts SQL necesarios para crear el esquema de la base de datos.
- `init/`: Contiene el código fuente Python para la semilla de datos inicial.
- `worker/`: Implementa la lógica de procesamiento concurrente y la interacción con la DB.
- `shared/`: Directorio destinado a ser montado como volumen compartido para los logs.


## 2. Distribución del Trabajo
El trabajo se distribuye utilizando un **Modelo de Pull**. En lugar de un orquestrador central que envía tareas a los trabajadores, cada trabajador solicita activamente trabajo a la base de datos.

- **Nivel de Contenedor**: Se inician 5 contenedores de trabajadores independientes.
- **Nivel de Hilo**: Cada contenedor de trabajador genera 5 hilos internos.
- **Ejecución**: Un hilo extrae una tarea, la procesa y confirma el resultado. Esto permite que el sistema escale horizontalmente (más contenedores) y verticalmente (más hilos por contenedor).

## 3. Prevención de Condiciones de Carrera
Para evitar que múltiples trabajadores o hilos procesen el mismo trabajo, el sistema emplea:
- **`FOR UPDATE SKIP LOCKED`**: Esta cláusula SQL bloquea la fila seleccionada e instruye a otras transacciones que ignoren las filas bloqueadas. Esto asegura que no haya dos hilos trabajando nunca en el mismo `input_id`.
- **`threading.Lock()`**: Se utiliza un mutex al escribir en la ruta compartida `/shared/logs.txt` para garantizar escrituras atómicas y evitar la corrupción de los logs.

## 4. Manejo de la Base de Datos
### Gestión Auto-incremental
La base de datos utiliza el tipo de datos `SERIAL` para las claves primarias en las tablas `input` y `result`. Esto proporciona una secuencia automatizada y segura para hilos al generar identificadores únicos sin requerir que los trabajadores rastreen el último ID utilizado.

### Concurrencia de la DB
La concurrencia se gestiona a través de:
- **Integridad Transaccional**: Cada trabajo se procesa dentro de una transacción. El estado cambia de `pending` $\rightarrow$ `in_process` $\rightarrow$ `processed`.
- **Gestión de Conexiones**: Cada hilo gestiona su propia conexión para evitar la fuga de estado entre tareas concurrentes.
- **Semáforo**: Un `threading.Semaphore(3)` limita el número de conexiones concurrentes a la base de datos por trabajador para evitar saturar el pool de conexiones de PostgreSQL.

## 5. Resultados Obtenidos
El sistema procesó exitosamente los datos iniciales (50 entradas). Los logs confirman que múltiples trabajadores (worker1-worker5) participaron en el procesamiento, y la tabla `result` contiene una relación 1:1 de las entradas procesadas.

## 6. Dificultades y Lecciones Aprendidas
- **Ciclo de Vida del Contenedor**: Las versiones iniciales tuvieron problemas donde los trabajadores iniciaban antes de que la base de datos estuviera totalmente inicializada o poblada. Esto se solucionó implementando una dependencia `service_completed_successfully` del contenedor `init`.
- **Distribución de Datos**: Con conjuntos de datos muy pequeños, se observó que no todos los trabajadores participaron. Dado que el trabajo es "extraído" (pull), los primeros trabajadores en iniciar podrían consumir todas las tareas disponibles antes de que otros puedan inicializarse, demostrando que la distribución de la carga depende del volumen de datos en relación con el número de trabajadores.
- **Gestión de Dependencias**: Asegurar que el servicio `init` terminara completamente antes de que los trabajadores comenzaran fue crítico para evitar que los trabajadores finalizaran inmediatamente debido a una tabla `input` vacía.
