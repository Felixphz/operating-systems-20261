#include <ctype.h>
#include <string.h>
#include "validator.h"

int validate_not_empty(const char *str) {
    if (!str) return 0;
    return strlen(str) > 0;
}

int validate_numeric(const char *str) {
    if (!str || strlen(str) == 0) return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }

    return 1;
}

int validate_email(const char *str) {
    if (!str) return 0;

    const char *at = strchr(str, '@');
    return at != NULL;
}