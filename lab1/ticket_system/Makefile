# ===============================
# Configuración del compilador
# ===============================
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Iinclude/utils -Iinclude/ticket

# ===============================
# Archivos fuente y objetos
# ===============================
SRC = assets/main.c \
      src/utils/input.c \
      src/utils/validator.c \
      src/utils/file_manager.c \
      src/ticket/ticket.c

OBJ = $(SRC:.c=.o)

# ===============================
# Nombre del ejecutable
# ===============================
TARGET = ticket_app

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
	rm -f assets/*.o src/utils/*.o src/ticket/*.o $(TARGET)