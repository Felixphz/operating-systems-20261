#!/bin/bash

LOG_FILE="/var/log/user_manager.log"
DEFAULT_PASSWORD="Admin1234%"

# ==============================
# VALIDACIÓN ROOT
# ==============================
if [ "$EUID" -ne 0 ]; then
  echo "Solo el superusuario puede ejecutar este script"
  exit 1
fi

# ==============================
# FUNCIONES AUXILIARES
# ==============================

log_event() {
  echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> $LOG_FILE
}

usuario_existe() {
  id "$1" &>/dev/null
}

validar_password() {
  local pass="$1"

  if [[ "$pass" =~ [[:space:]] ]]; then
    echo " No debe contener espacios"
    return 1
  fi

  if [[ ! "$pass" =~ [A-Z] ]]; then
    echo " Debe tener al menos una mayúscula"
    return 1
  fi

  if [[ ! "$pass" =~ [^a-zA-Z0-9] ]]; then
    echo " Debe tener al menos un carácter especial"
    return 1
  fi

  return 0
}

# ==============================
# FUNCIONES PRINCIPALES
# ==============================

crear_usuario() {
  read -p " Nombre de usuario: " user

  if usuario_existe "$user"; then
    echo " El usuario ya existe"
    return
  fi

  useradd -m "$user"

  # contraseña por defecto
  echo "$user:$DEFAULT_PASSWORD" | chpasswd

  # forzar cambio en primer login
  chage -d 0 "$user"

  # expiración: 30 días, advertencia 10 días antes
  chage -M 30 -W 10 "$user"

  echo " Usuario creado con contraseña: $DEFAULT_PASSWORD"
  log_event "Usuario creado: $user"
}

listar_usuarios() {
  echo " Usuarios del sistema:"
  cut -d: -f1 /etc/passwd
}

bloquear_usuario() {
  read -p " Usuario a bloquear: " user

  if ! usuario_existe "$user"; then
    echo " Usuario no existe"
    return
  fi

  usermod -L "$user"
  echo "  Usuario bloqueado"
  log_event "Usuario bloqueado: $user"
}

activar_usuario() {
  read -p " Usuario a activar: " user

  if ! usuario_existe "$user"; then
    echo " Usuario no existe"
    return
  fi

  usermod -U "$user"
  echo " Usuario activado"
  log_event "Usuario activado: $user"
}

eliminar_usuario() {
  read -p " Usuario a eliminar: " user

  if ! usuario_existe "$user"; then
    echo " Usuario no existe"
    return
  fi

  userdel -r "$user"
  echo " Usuario eliminado"
  log_event "Usuario eliminado: $user"
}

agregar_sudo_limitado() {
  read -p " Usuario a agregar a sudo limitado: " user

  if ! usuario_existe "$user"; then
    echo " Usuario no existe"
    return
  fi

  echo "$user ALL=(ALL) NOPASSWD: /app/user_manager.sh" >> /etc/sudoers.d/lab_users

  chmod 440 /etc/sudoers.d/lab_users

  echo "  Usuario agregado a sudo limitado"
  log_event "Usuario agregado a sudo: $user"
}

verificar_expiracion() {
  echo " Verificando usuarios expirados..."

  for user in $(cut -d: -f1 /etc/passwd); do
    exp=$(chage -l "$user" | grep "Account expires" | cut -d: -f2)

    if [[ "$exp" != " never" ]]; then
      exp_date=$(date -d "$exp" +%s 2>/dev/null)
      now=$(date +%s)

      if [[ $exp_date -lt $now ]]; then
        usermod -L "$user"
        echo " Usuario $user bloqueado por expiración"
        log_event "Usuario bloqueado por expiración: $user"
      fi
    fi
  done
}

# ==============================
# MENÚ
# ==============================

menu() {
  echo ""
  echo "=====  GESTIÓN DE USUARIOS ====="
  echo "1. Crear usuario"
  echo "2. Listar usuarios"
  echo "3. Bloquear usuario"
  echo "4. Activar usuario"
  echo "5. Eliminar usuario"
  echo "6. Agregar a sudo limitado"
  echo "7. Verificar expiración"
  echo "8. Salir"
  echo "=================================="
  read -p "Seleccione opción: " opcion
}

# ==============================
# LOOP PRINCIPAL
# ==============================

while true; do
  menu

  case $opcion in
    1) crear_usuario ;;
    2) listar_usuarios ;;
    3) bloquear_usuario ;;
    4) activar_usuario ;;
    5) eliminar_usuario ;;
    6) agregar_sudo_limitado ;;
    7) verificar_expiracion ;;
    8) echo " Saliendo..."; exit 0 ;;
    *) echo " Opción inválida" ;;
  esac
done