# Sistema de Procesamiento Concurrente

Un sistema distribuido implementado con Docker y PostgreSQL para demostrar el procesamiento de tareas de alta concurrencia.

## Estructura del Proyecto
- `db/`: Scripts SQL para la inicialización del esquema de la base de datos.
- `init/`: Script de semilla en Python para poblar la tabla de entrada.
- `worker/`: La lógica central para el procesamiento concurrente de tareas.
- `shared/`: Volumen compartido para el registro (logging) centralizado.

## Cómo funciona
El sistema implementa un flujo de procesamiento distribuido basado en un modelo de "pull":

1. **Inicialización**: Al levantar el stack, el servicio `init` puebla la tabla `input` de PostgreSQL con tareas pendientes.
2. **Competencia de Tareas**: Múltiples contenedores de trabajadores (`worker1` a `worker5`), cada uno con 5 hilos internos, consultan la base de datos buscando tareas con estado `pending`.
3. **Bloqueo Optimista**: Se utiliza la cláusula `FOR UPDATE SKIP LOCKED` en SQL. Esto permite que cada hilo reserve una tarea única sin bloquear a otros hilos; si una fila ya está bloqueada por otro trabajador, el hilo simplemente la ignora y pasa a la siguiente disponible.
4. **Procesamiento y Resultado**: Una vez obtenida la tarea, el trabajador la procesa, inserta el resultado en la tabla `result` y marca la tarea original como `processed`.
5. **Logging Centralizado**: Todas las actividades se registran en un archivo compartido (`/shared/logs.txt`), utilizando un bloqueo de hilos (`mutex`) para evitar que las escrituras solapadas corrompan el log.

## Despliegue
Ejecute todo el stack utilizando Docker Compose:
```bash
docker-compose up --build
```
