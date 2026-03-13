# Simulador MLFQ (Multi-Level Feedback Queue)

Implementación de un planificador de procesos con cola multinivel con retroalimentación (MLFQ) desarrollado en C. Este simulador permite analizar el comportamiento de procesos bajo diferentes políticas de scheduling con múltiples niveles de prioridad.

## Descripción del Algoritmo

El MLFQ es un algoritmo de planificación que utiliza múltiples colas de prioridad para optimizar tanto el tiempo de respuesta como el tiempo de ejecución:

- **3 niveles de prioridad** (Q0, Q1, Q2)
- **Round-Robin** en cada cola con diferentes quantum:
  - Q0: 2 unidades de tiempo (alta prioridad)
  - Q1: 4 unidades de tiempo (prioridad media)
  - Q2: 8 unidades de tiempo (baja prioridad)
- **Priority Boost**: Periódicamente todos los procesos regresan a Q0 para evitar inanición

## Estructura del Proyecto

```
lab3/
├── Makefile                    # Sistema de compilación
├── README.md                   # Documentación del proyecto
├── mlfq_results.csv           # Archivo de resultados generado
├── assets/
│   └── main.c                  # Punto de entrada del programa
├── include/
│   ├── process/
│   │   └── process.c           # Implementación de procesos
│   ├── Sheduler/
│   │   ├── mlfq.c             # Implementación del algoritmo MLFQ
│   │   └── queue.c            # Implementación de colas circulares
│   └── utils/
│       └── file_manager.c     # Exportación de resultados a CSV
└── src/
    ├── process/
    │   └── process.h           # Definición de estructura Process
    ├── Sheduler/
    │   ├── mlfq.h             # Interfaz del planificador
    │   └── queue.h            # Interfaz de cola
    └── utils/
        └── file_manager.h     # Interfaz de exportación
```

## Compilación

El proyecto utiliza un Makefile con las siguientes opciones:

### Compilar el proyecto
```bash
make
```

### Compilar y ejecutar
```bash
make run
```

### Limpiar archivos generados
```bash
make clean
```

## Ejecución

Una vez compilado, ejecutar:

```bash
./mlfq
```

El programa simulará la ejecución de los procesos definidos en `main.c` y generará un archivo `mlfq_results.csv` con las métricas de rendimiento.

## Configuración de Procesos

Los procesos se configuran en [assets/main.c](assets/main.c):

```c
Process processes[4];

processes[0] = create_process(1, 0, 8);  // PID=1, Arrival=0, Burst=8
processes[1] = create_process(2, 1, 4);  // PID=2, Arrival=1, Burst=4
processes[2] = create_process(3, 2, 9);  // PID=3, Arrival=2, Burst=9
processes[3] = create_process(4, 3, 5);  // PID=4, Arrival=3, Burst=5

int priority_boost = 20;  // Intervalo de priority boost

run_mlfq(processes, 4, priority_boost);
```

## Archivo de Resultados

El simulador genera un archivo CSV (`mlfq_results.csv`) con las siguientes columnas:

| Columna | Descripción |
|---------|-------------|
| PID | Identificador del proceso |
| Arrival | Tiempo de llegada |
| Burst | Tiempo de CPU requerido |
| Start | Tiempo de inicio de ejecución |
| Finish | Tiempo de finalización |
| Response | Tiempo de respuesta (Start - Arrival) |
| Turnaround | Tiempo de retorno (Finish - Arrival) |
| Waiting | Tiempo de espera (Turnaround - Burst) |

## Explicaciones Técnicas

### 1. Estructura Process

```c
typedef struct {
    int PID;                    // Identificador del proceso
    int arrival_time;           // Tiempo de llegada
    int burst_time;             // Tiempo de CPU requerido
    int remaining_time;         // Tiempo restante de ejecución
    int start_time;             // Primer tiempo de ejecución
    int finish_time;            // Tiempo de finalización
    int first_response_time;    // Tiempo de primera respuesta
    int current_queue;          // Cola actual (0, 1, o 2)
} Process;
```

### 2. Estructura Queue (Cola Circular)

```c
typedef struct {
    Process* data[MAX_QUEUE];  // Array de punteros a procesos
    int front;                  // Índice del frente
    int rear;                   // Índice de la parte trasera
    int size;                   // Tamaño actual
} Queue;
```

**Ventajas de la cola circular:**
- Uso eficiente del espacio (no hay desplazamientos)
- Operaciones O(1) para enqueue/dequeue
- Manejo de punteros a procesos sin copiar estructuras completas

### 3. Manejo de Punteros

#### Punteros a estructuras
```c
void enqueue(Queue *q, Process *p) {
    if(q->size == MAX_QUEUE) return;
    q->data[q->rear] = p;  // Almacena puntero, no copia la estructura
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->size++;
}
```

**Ventajas:**
- **Eficiencia**: Solo se almacenan direcciones (8 bytes) en lugar de copiar toda la estructura
- **Modificación directa**: Los cambios en el proceso se reflejan en todas las colas
- **Sin duplicación**: Todos los punteros apuntan al mismo proceso en el array original

#### Verificación de punteros nulos
```c
Process* dequeue(Queue *q) {
    if(is_empty(q)) return NULL;  // Manejo seguro
    
    Process *p = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    q->size--;
    
    return p;
}

// En mlfq.c:
Process *p = dequeue(current_queue);
if(p == NULL) continue;  // Verificación de seguridad
```

### 4. Algoritmo MLFQ

El algoritmo principal sigue estos pasos:

1. **Agregar procesos que llegan** al tiempo actual a Q0
2. **Priority Boost**: Si es tiempo de boost, mover todos los procesos de Q1 y Q2 a Q0
3. **Seleccionar cola**: Elegir la cola de mayor prioridad que no esté vacía
4. **Ejecutar proceso**: Extraer proceso y ejecutarlo por su quantum
5. **Reasignar o completar**:
   - Si el proceso terminó: marcarlo como completado
   - Si consumió todo el quantum: degradarlo a la siguiente cola
   - Si no consumió el quantum: mantenerlo en la misma cola

### 5. Gestión de Archivos

```c
void export_results(Process processes[], int n, const char *filename) {
    FILE *f = fopen(filename, "w");
    
    if(f == NULL) {
        fprintf(stderr, "Error: No se pudo crear el archivo %s\n", filename);
        return;  // Manejo de error seguro
    }
    
    // Escritura de datos...
    
    fclose(f);  // Liberación de recursos
}
```

**Buenas prácticas:**
- Verificación de que `fopen()` no retorne `NULL`
- Cierre del archivo con `fclose()`
- Mensajes de error descriptivos a `stderr`

## Métricas de Rendimiento

El simulador calcula automáticamente:

- **Tiempo de Respuesta**: Tiempo desde que llega hasta que comienza a ejecutarse
- **Tiempo de Retorno**: Tiempo total desde que llega hasta que termina
- **Tiempo de Espera**: Tiempo total esperando en las colas

Estas métricas permiten evaluar la eficiencia del planificador MLFQ.

## Analisis de comportamiento 

**¿Qué ocurre si el boost es muy frecuente?**

Si el priority boost ocurre con demasiada frecuencia, los procesos son promovidos constantemente a la cola de mayor prioridad. Como consecuencia, los procesos no permanecen el tiempo suficiente en las colas inferiores para que el algoritmo pueda diferenciar entre procesos interactivos y procesos intensivos en CPU.

En este escenario, la mayoría de los procesos tienden a acumularse en la cola de mayor prioridad y el comportamiento del scheduler se vuelve similar al de un Round Robin, reduciendo la efectividad del mecanismo de retroalimentación del algoritmo MLFQ.

**¿Qué ocurre si no existe boost?**
 
Es muy probable que los procesos que desciendan a las colas de menor prioridad no vuelvan a ejecutarse, especialmente si continúan llegando nuevos procesos al sistema que ocupan las colas de mayor prioridad. Como consecuencia, estos procesos pueden permanecer indefinidamente esperando por CPU, lo que provoca inanición (starvation) en el sistema.

**¿Cómo afecta un quantum pequeño en la cola de mayor prioridad?**

Un quantum de tiempo pequeño en la cola de mayor prioridad permite obtener un tiempo de respuesta muy bajo, ya que los procesos, a medida que llegan al sistema, tienen una alta probabilidad de ser atendidos rápidamente por el procesador. Esto favorece especialmente a los procesos interactivos o de corta duración.

Sin embargo, esta configuración también tiene un inconveniente: los procesos que utilizan completamente su quantum son demovidos rápidamente a colas de menor prioridad, lo que puede provocar que los procesos intensivos en CPU desciendan con rapidez en la jerarquía de colas y deban esperar más tiempo para volver a ejecutarse.  
