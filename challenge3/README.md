# User Management and IAM Lab (Challenge 3)

Este proyecto consiste en un entorno Dockerizado basado en Ubuntu 22.04 que proporciona un script interactivo en Bash para la gestión de usuarios del sistema. Implementa funcionalidades de Identity and Access Management (IAM) básicas como la creación de usuarios con contraseñas seguras, políticas de expiración, bloqueo, eliminación y asignación de permisos de superusuario limitados.

## Características

El script `user_manager.sh` incluye un menú interactivo con las siguientes opciones:

1. **Crear usuario:** Crea un nuevo usuario en el sistema con una contraseña por defecto (`Admin1234%`). Configura la expiración de la contraseña en 30 días, advertencias 10 días antes, y fuerza el cambio de la misma en el primer inicio de sesión.
2. **Listar usuarios:** Muestra la lista de todos los usuarios registrados en el sistema.
3. **Bloquear usuario:** Deshabilita temporalmente el inicio de sesión de un usuario (bloquea la cuenta).
4. **Activar usuario:** Permite que un usuario previamente bloqueado pueda volver a iniciar sesión.
5. **Eliminar usuario:** Elimina completamente al usuario y su directorio personal (home).
6. **Agregar a sudo limitado:** Concede privilegios al usuario especificado para que pueda ejecutar únicamente el script `user_manager.sh` con `sudo`, sin necesidad de contraseña.
7. **Verificar expiración:** Recorre los usuarios del sistema para comprobar si su cuenta ha expirado. Si la fecha de expiración ha pasado, la cuenta se bloquea automáticamente.

Además, todas las acciones importantes quedan registradas en el archivo de logs `/var/log/user_manager.log`.

> **Nota de revisión del script:** Las implementaciones en el script `user_manager.sh` son correctas y cumplen buenas prácticas (como verificar ser superusuario, usar logs y variables). Se incluye la función `validar_password` que, si bien actualmente no se está utilizando directamente (ya que las cuentas se crean con la contraseña por defecto y su configuración se asume segura localmente o vía PAM más adelante), te servirá al extender las capacidades para capturar contraseñas desde el prompt.

## Requisitos previos

- Tener **Docker** instalado en el equipo.

## Instalación y uso

**Paso 1: Construir la imagen de Docker**

Dentro del directorio del proyecto (donde se encuentra el archivo `dockerfile`), ejecuta el siguiente comando para compilar la imagen:
```bash
docker build -t lab_iam_challenge3 .
```

**Paso 2: Ejecutar el contenedor interactivo**

Inicia un contenedor a partir de la imagen recién creada:
```bash
docker run -it --name iam_manager lab_iam_challenge3
```

**Paso 3: Utilizar el script**

Una vez dentro del contenedor (el prompt te aparecerá allí como usuario `root`), puedes ejecutar el script de gestión de usuarios en cualquier momento:
```bash
./user_manager.sh
```
*(Se abrirá el menú interactivo para realizar la gestión).*

## Detalles Técnicos
- **Sistema Local:** `Ubuntu 22.04`
- **Herramientas usadas:** `apt`, `bash`, `useradd`, `usermod`, `userdel`, `chpasswd`, `chage`, y archivos de configuración en `/etc/sudoers.d`.
- **Registro de Eventos (Logs):** Almacenado en `/var/log/user_manager.log`
