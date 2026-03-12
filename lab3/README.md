# Sistema de Gestión de Tickets

Sistema de registro y gestión de tickets de reclamación desarrollado en C, con enfoque en manejo seguro de memoria y validación de entrada de datos.

## Estructura del Proyecto

```
ticket_system/
├── Makefile                    # Sistema de compilación
├── README.md                   # Documentación del proyecto
├── assets/                     # Archivos principales y tickets generados
│   ├── main.c                  # Punto de entrada del programa
│   └── ticket_*.txt            # Tickets generados (formato: ticket_YYYYMMDDHHMMSS.txt)
├── include/                    # Archivos de cabecera
│   ├── ticket/
│   │   └── ticket.h            # Definición de estructura Ticket y funciones
│   └── utils/
│       ├── file_manager.h      # Manejo de archivos
│       ├── input.h             # Captura de entrada
│       └── validator.h         # Validación de datos
└── src/                        # Implementaciones
    ├── ticket/
    │   └── ticket.c            # Lógica de creación y guardado de tickets
    └── utils/
        ├── file_manager.c      # Implementación de guardado de archivos
        ├── input.c             # Implementación de lectura de entrada
        └── validator.c         # Implementación de validadores
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
./ticket_app
```

El programa solicitará:
1. **Identificación**: Solo dígitos numéricos
2. **Correo electrónico**: Debe contener '@'
3. **Tipo de reclamación**: Texto no vacío

Si todos los datos son válidos, se generará un ticket con un radicado único y se guardará en el directorio `assets/`.

## Explicaciones Técnicas

### 1. Uso de Punteros

#### Punteros a estructuras
```c
typedef struct {
    char *identificacion;  // Puntero a cadena dinámica
    char *correo;          // Puntero a cadena dinámica
    char *tipo;            // Puntero a cadena dinámica
    char *radicado;        // Puntero a cadena generada
} Ticket;
```

La estructura `Ticket` usa punteros a `char` para campos dinámicos, permitiendo:
- **Flexibilidad**: Almacenar cadenas de longitud variable sin límites fijos
- **Eficiencia**: Solo se reserva la memoria exacta necesaria para cada cadena
- **Gestión manual**: Control total sobre la vida útil de los datos mediante `malloc`/`free`

**Ejemplo de uso:**
```c
Ticket *t = malloc(sizeof(Ticket));           // Reserva espacio para la estructura
t->identificacion = malloc(strlen("123") + 1); // Reserva espacio para la cadena
strcpy(t->identificacion, "123");             // Copia el contenido
```

En lugar de arrays de tamaño fijo como `char identificacion[50]`, los punteros permiten manejar cualquier longitud y liberar la memoria cuando ya no se necesita.

#### Punteros a funciones
```c
char* input_read_validated(const char *mensaje,
                           int (*validator)(const char *));
```

Se pasan validadores como punteros a funciones, implementando un patrón de callback:

**Declaración del parámetro:**
- `int (*validator)(const char *)`: Puntero a función que recibe `const char*` y retorna `int`

**Uso en main.c:**
```c
char *id = input_read_validated("Ingrese ID: ", validate_numeric);
char *email = input_read_validated("Ingrese correo: ", validate_email);
```

**Ventajas:**
- **Modularidad**: Una función genérica que acepta diferentes validaciones
- **Extensibilidad**: Agregar nuevos validadores sin modificar `input_read_validated`
- **Reutilización**: Evita duplicar código para cada tipo de entrada
- **Desacoplamiento**: La función de entrada no conoce los detalles de validación

La función llama al validador mediante dereferencia: `if (validator(line))` ejecuta la función apuntada.

### 2. Manejo de Memoria

#### Estrategia de asignación
```c
Ticket *ticket = malloc(sizeof(Ticket));
ticket->identificacion = malloc(strlen(id) + 1);
```

**Principios aplicados:**
- **Reserva dinámica**: `malloc` solicita memoria del heap en tiempo de ejecución
- **Cálculo exacto**: `strlen(str) + 1` incluye el byte del terminador `\0`
- **Validación inmediata**: Verificación de `malloc != NULL` después de cada asignación
- **Evita desperdiciar memoria**: No se usan buffers de tamaño fijo innecesarios

**Flujo de asignación en ticket_create:**
1. Se reserva memoria para la estructura `Ticket` (4 punteros = ~32 bytes en 64-bit)
2. Se calculan los tamaños exactos de cada cadena: `strlen(str) + 1`
3. Se reserva memoria individual para cada campo `char*`
4. Se copian los datos a las ubicaciones reservadas con `strcpy`
5. Se genera el radicado dinámicamente (otra reserva de memoria)

#### Inicialización segura
```c
ticket->identificacion = NULL;
ticket->correo = NULL;
ticket->tipo = NULL;
ticket->radicado = NULL;
```

**Razón:** Inicializar a `NULL` antes de asignar memoria previene:
- **Punteros salvajes**: Sin inicializar, contienen direcciones arbitrarias (basura)
- **Doble free**: Intentar liberar memoria en direcciones inválidas causa crash
- **Undefined behavior**: `free()` en puntero no inicializado es comportamiento indefinido
- **Memory leaks en errores**: Si falla un `malloc`, `ticket_destroy` puede liberar con seguridad

**Ejemplo del problema que previene:**
```c
// SIN inicialización (MAL):
Ticket *t = malloc(sizeof(Ticket));
t->identificacion = malloc(10);  // OK
t->correo = malloc(20);          // Falla (retorna NULL)
ticket_destroy(t);               // ERROR: intenta free(tipo) y free(radicado) con basura

// CON inicialización (BIEN):
Ticket *t = malloc(sizeof(Ticket));
t->identificacion = NULL;
t->correo = NULL;
t->tipo = NULL;
t->radicado = NULL;
t->identificacion = malloc(10);  // OK
t->correo = malloc(20);          // Falla (retorna NULL)
ticket_destroy(t);               // OK: free(NULL) es seguro y no hace nada
```

#### Liberación de memoria
```c
void ticket_destroy(Ticket *ticket) {
    if (!ticket) return;
    free(ticket->identificacion);
    free(ticket->correo);
    free(ticket->tipo);
    free(ticket->radicado);
    free(ticket);
}
```

**Garantías:**
- Todo `malloc` tiene su correspondiente `free`
- **Orden correcto**: Primero liberar contenido (cadenas), luego contenedor (estructura)
- **Seguridad con NULL**: `free(NULL)` es válido según el estándar C y no hace nada
- Limpieza tanto en casos de éxito como en paths de error

**Por qué el orden importa:**
```c
// Orden correcto:
free(ticket->identificacion);  // Libera la cadena
free(ticket);                  // Libera la estructura

// Orden incorrecto (causaría problemas):
free(ticket);                  // Libera la estructura
free(ticket->identificacion);  // ERROR: accede a memoria ya liberada
```

#### Prevención de memory leaks
```c
if (!ticket->identificacion || !ticket->correo || 
    !ticket->tipo || !ticket->radicado) {
    ticket_destroy(ticket);  // Libera memoria parcialmente asignada
    return NULL;
}
```

**Escenario de memory leak prevenido:**
1. Se reserva memoria para `ticket` → éxito
2. Se reserva memoria para `identificacion` → éxito
3. Se reserva memoria para `correo` → éxito
4. Se reserva memoria para `tipo` → **falla** (retorna NULL)
5. Sin limpieza: Las asignaciones 1, 2 y 3 quedarían huérfanas (leak)
6. Con limpieza: `ticket_destroy` libera todo lo asignado hasta el momento

**Path de ejecución en main.c:**
```c
// Libera entrada previa si falla la siguiente
if (!correo) {
    free(id);    // Limpia asignación previa
    return 1;
}

// Al final siempre libera todo
free(id);
free(correo);
free(tipo);
ticket_destroy(ticket);  // Incluye el radicado generado
```

### 3. Generación del Radicado

```c
static char* generar_radicado(void) {
    time_t t = time(NULL);
    int aleatorio = rand() % 10000;
    
    char *radicado = malloc(32);
    if (!radicado) return NULL;
    
    snprintf(radicado, 32, "%ld_%04d", t, aleatorio);
    return radicado;
}
```

**Técnica empleada:**

1. **Timestamp UNIX**: 
   - `time(NULL)` retorna segundos desde el 1 de enero de 1970 (época UNIX)
   - Ejemplo: `1709064234` representa una fecha/hora específica
   - Garantiza unicidad temporal: no puede haber dos radicados iguales en diferentes momentos
   - Es monotónicamente creciente: radicados posteriores tienen valores mayores

2. **Componente aleatorio**:
   - `rand() % 10000` genera un número entre 0 y 9999
   - Previene colisiones si se crean múltiples tickets en el mismo segundo
   - Probabilidad de colisión en el mismo segundo: 1/10000 = 0.01%
   - `srand(time(NULL))` en main.c inicializa la semilla aleatoria

3. **Formato de cadena**:
   - Patrón: `%ld_%04d` donde:
     - `%ld`: long decimal (timestamp)
     - `_`: Separador visual
     - `%04d`: decimal con padding de 4 dígitos (0001, 0042, 9999)
   - Resultado ejemplo: `1709064234_0157`

4. **Seguridad con snprintf**:
   - Buffer de 32 bytes es suficiente:
     - Timestamp: ~10 dígitos (hasta el año 2286)
     - Separador: 1 carácter
     - Aleatorio: 4 dígitos
     - Terminador: 1 byte (`\0`)
     - Total: ~16 bytes usados de 32 disponibles
   - `snprintf(buffer, size, ...)` garantiza no exceder el tamaño
   - Previene buffer overflow automáticamente

**Ventajas del algoritmo:**
- **Unicidad**: Combinación de tiempo + aleatoriedad hace colisiones casi imposibles
- **Ordenabilidad**: Los tickets se pueden ordenar cronológicamente por radicado
- **Trazabilidad**: El timestamp permite determinar cuándo se creó el ticket
- **Compacto**: Formato legible sin caracteres especiales complejos
- **Eficiente**: Generación O(1) sin consultas a bases de datos

**Inicialización de la semilla aleatoria (main.c):**
```c
srand((unsigned int)time(NULL));  // Se ejecuta una vez al inicio
```
Sin esta línea, `rand()` siempre generaría la misma secuencia de números.

### 4. Manejo de Errores

El proyecto implementa una estrategia defensiva de manejo de errores en múltiples niveles:

#### Verificación de asignación de memoria
```c
Ticket *ticket = malloc(sizeof(Ticket));
if (!ticket) return NULL;
```

**Estrategia:**
- Verificar inmediatamente después de cada `malloc`
- Retornar `NULL` para indicar fallo (convención estándar en C)
- El llamador debe verificar el valor de retorno antes de usar el puntero

**Propagación del error:**
```c
Ticket *ticket = ticket_create(id, correo, tipo);
if (!ticket) {
    printf("Error creando ticket\n");
    return 1;  // Código de salida indicando error
}
```

#### Validación de entrada con reintentos
```c
char *line = input_read_validated("Mensaje: ", validate_function);
if (!line) {
    // Error de lectura (EOF o fallo de malloc)
    free(id);  // Liberar recursos previos
    return 1;
}
```

**Flujo de validación:**
1. `input_read_line` lee con `fgets` (seguro, límite de 256 bytes)
2. Si `fgets` falla (EOF/error): retorna `NULL` → error irrecuperable
3. Si `malloc` falla: retorna `NULL` → error irrecuperable
4. El validador verifica el formato: si falla → libera y reintenta
5. Bucle infinito hasta obtener entrada válida o error de sistema

**Ejemplo de flujo en main:**
```c
char *id = input_read_validated(...);      // Éxito
if (!id) return 1;

char *correo = input_read_validated(...);  // Falla
if (!correo) {
    free(id);      // CRÍTICO: liberar recursos previos
    return 1;
}

char *tipo = input_read_validated(...);    // Éxito
if (!tipo) {
    free(id);      // Liberar todos los recursos previos
    free(correo);
    return 1;
}
```

Cada nivel de fallo libera todos los recursos adquiridos hasta ese punto, previniendo memory leaks.

#### Verificación de operaciones de archivo
```c
FILE *fp = fopen(nombre, "w");
if (!fp) {
    perror("Error al crear archivo");  // Imprime errno del sistema
    return -1;
}
```

**Razones de fallo de fopen:**
- Permisos insuficientes (no puede escribir en el directorio)
- Directorio no existe (`assets/` no creado)
- Disco lleno o cuota excedida
- Nombre de archivo inválido
- Demasiados archivos abiertos

**`perror()` proporciona contexto:**
```
Error al crear archivo: Permission denied
Error al crear archivo: No such file or directory
```

#### Propagación y códigos de retorno
```c
if (ticket_save(ticket) == 0) {
    printf("\nRegistro exitoso.\n");
    printf("Radicado generado: %s\n", ticket->radicado);
} else {
    // ticket_save retornó -1, algo falló
    printf("Error al guardar ticket\n");
}
```

**Convenciones de retorno en el proyecto:**
- **Punteros**: `NULL` = error, puntero válido = éxito
- **ticket_save**: `0` = éxito, `-1` = error
- **Validadores**: `0` = inválido, `1` = válido
- **main**: `0` = éxito, `1` = error

#### Limpieza garantizada
```c
// Siempre se ejecuta, éxito o error
free(id);
free(correo);
free(tipo);
ticket_destroy(ticket);
return 0;
```

**Paths posibles en el flujo:**
1. Todo exitoso → libera todo → retorna 0
2. Falla entrada ID → retorna 1 (nada que liberar)
3. Falla entrada correo → libera ID → retorna 1
4. Falla entrada tipo → libera ID, correo → retorna 1
5. Falla creación ticket → libera ID, correo, tipo → retorna 1
6. Falla guardar → libera todo (ticket ya creado) → retorna 0

En todos los casos, no hay memory leaks.

**Estrategias aplicadas:**
- **Detección temprana**: Validar inmediatamente después de operaciones críticas
- **Limpieza progresiva**: Cada nivel libera lo que sus predecesores asignaron
- **Códigos consistentes**: Convenciones claras para éxito/error
- **Mensajes descriptivos**: `perror()` para errores del sistema, `printf()` para lógica
- **RAII manual**: Simula Resource Acquisition Is Initialization mediante disciplina estricta

### 5. Funcionamiento del Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Iinclude/utils -Iinclude/ticket
```

#### Variables de configuración
- **CC**: Compilador a utilizar (`gcc`)
- **CFLAGS**: Flags de compilación
  - `-Wall`: Habilita todos los warnings comunes
  - `-Wextra`: Habilita warnings adicionales (conversiones, comparaciones)
  - `-std=c11`: Usa el estándar C11 (no extensiones del compilador)
  - `-Iinclude`: Agrega `include/` a las rutas de búsqueda de headers
  - `-Iinclude/utils`: Permite `#include "input.h"` sin necesidad de `utils/`
  - `-Iinclude/ticket`: Permite `#include "ticket.h"` sin necesidad de `ticket/`

**Por qué múltiples `-I`:** 
Permite incluir headers directamente: `#include "ticket.h"` en lugar de `#include "ticket/ticket.h"`

#### Archivos fuente y objetos
```makefile
SRC = assets/main.c \
      src/utils/input.c \
      src/utils/validator.c \
      src/utils/file_manager.c \
      src/ticket/ticket.c

OBJ = $(SRC:.c=.o)
```

**Transformación automática**: 
- `$(SRC:.c=.o)` es una sustitución de patrón
- Convierte cada `.c` en `.o`: `assets/main.c` → `assets/main.o`
- Resultado: `OBJ = assets/main.o src/utils/input.o src/utils/validator.o ...`

#### Reglas de compilación

**Target principal:**
```makefile
all: $(TARGET)
```
- `all` es el target por defecto (se ejecuta con `make`)
- Dependencia: `ticket_app` (el ejecutable)
- Si `ticket_app` existe y está actualizado → no hace nada
- Si no existe o está desactualizado → ejecuta la regla de `$(TARGET)`

**Linkeo (vinculación):**
```makefile
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
```

**Flujo:**
1. Verifica si `ticket_app` necesita actualizarse
2. Necesita actualizarse si:
   - No existe
   - Algún archivo `.o` es más nuevo que `ticket_app`
3. Si necesita actualizarse:
   - Primero construye todos los `$(OBJ)` (ver siguiente regla)
   - Luego ejecuta: `gcc -Wall ... -o ticket_app assets/main.o src/utils/input.o ...`
   - Esta fase se llama **linkeo**: une todos los `.o` en un ejecutable

**Compilación de objetos (pattern rule):**
```makefile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```

**Expansión del patrón:**
- `%` coincide con cualquier cadena
- Para `assets/main.o`:
  - `%.o` = `assets/main.o` → `%` = `assets/main`
  - `%.c` = `assets/main.c`
  - Dependencia: `assets/main.o` depende de `assets/main.c`
  
**Variables automáticas:**
- `$<`: Primer prerequisito (el archivo `.c`)
- `$@`: Target actual (el archivo `.o`)
- `-c`: Compilar sin linkear (genera archivo objeto)

**Ejemplo de expansión:**
```makefile
assets/main.o: assets/main.c
	gcc -Wall -Wextra -std=c11 -I... -c assets/main.c -o assets/main.o
```

**Compilación incremental:**
- Make compara timestamps de archivos
- Si `main.c` es más nuevo que `main.o` → recompila `main.o`
- Si `main.o` ya existe y es más nuevo que `main.c` → no hace nada
- Solo recompila lo modificado (ahorra tiempo)

**Target de ejecución:**
```makefile
run: $(TARGET)
	./$(TARGET)
```

**Funcionamiento:**
1. `run` depende de `$(TARGET)` (ticket_app)
2. Si `ticket_app` no existe o está desactualizado → lo compila primero
3. Luego ejecuta: `./ticket_app`
4. Equivalente a: `make && ./ticket_app`

**Limpieza:**
```makefile
clean:
	rm -f assets/*.o src/utils/*.o src/ticket/*.o $(TARGET)
```

**Funcionamiento:**
- No tiene dependencias → siempre se ejecuta
- `rm -f`: Elimina archivos sin pedir confirmación, no falla si no existen
- Elimina todos los `.o` y el ejecutable
- Siguiente `make` recompilará todo desde cero

**Archivos eliminados:**
```
assets/main.o
src/utils/input.o
src/utils/validator.o
src/utils/file_manager.o
src/ticket/ticket.o
ticket_app
```
