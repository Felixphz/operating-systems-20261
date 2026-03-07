/*
 * validator.c - Funciones de validación de cadenas de texto
 *
 * Cada función recibe una cadena y retorna 1 si cumple el criterio
 * esperado, o 0 si no lo cumple o el puntero es NULL.
 */

#include <ctype.h>
#include <string.h>
#include "validator.h"

/*
 * validate_not_empty - Verifica que la cadena no esté vacía
 *
 * Retorna: 1 si la cadena tiene al menos un carácter, 0 en caso contrario.
 */
int validate_not_empty(const char *str) {
    if (!str) return 0;
    return strlen(str) > 0;
}

/*
 * validate_numeric - Verifica que la cadena sea completamente numérica
 *
 * Recorre cada carácter y comprueba que sea un dígito decimal (0-9).
 * Una cadena vacía se considera inválida.
 *
 * Retorna: 1 si todos los caracteres son dígitos, 0 en caso contrario.
 */
int validate_numeric(const char *str) {
    if (!str || strlen(str) == 0) return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        /* isdigit requiere cast a unsigned char para evitar UB con caracteres
         * extendidos (valores > 127) */
        if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }

    return 1;
}

/*
 * validate_email - Verificación básica de formato de correo electrónico
 *
 * Solo comprueba la presencia del carácter '@'; no valida dominio ni TLD.
 *
 * Retorna: 1 si la cadena contiene '@', 0 en caso contrario.
 */
int validate_email(const char *str) {
    if (!str) return 0;

    /* strchr retorna NULL si '@' no se encuentra en la cadena */
    const char *at = strchr(str, '@');
    return at != NULL;
}
