# ===============================
# Configuración del compilador
# ===============================
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. -Isrc -Iinclude -Iinclude/utils -Iinclude/process -Iinclude/Sheduler

# ===============================
# Archivos fuente y objetos
# ===============================
SRC = assets/main.c \
      include/process/process.c \
      include/Sheduler/mlfq.c \
      include/Sheduler/queue.c \
	  include/utils/file_manager.c
OBJ = $(SRC:.c=.o)

# ===============================
# Nombre del ejecutable
# ===============================
TARGET = mlfq

# ===============================
# Target principal
# ===============================
all: $(TARGET)

# ===============================
# Regla para generar el ejecutable
# ===============================
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# ===============================
# Regla para compilar .c a .o
# ===============================
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ===============================
# Ejecutar el programa
# ===============================
run: $(TARGET)
	./$(TARGET)

# ===============================
# Limpiar archivos compilados
# ===============================
clean:
	rm -f assets/*.o include/process/*.o include/Sheduler/*.o include/utils/*.o src/utils/*.o  $(TARGET)