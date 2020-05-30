#include "utils.h"

int getline(char **lineptr, size_t *n, FILE *stream) {
    static char line[256];
    char *ptr;
    unsigned int len;

    if (lineptr == NULL || n == NULL) {
        return -1;
    }

    if (ferror(stream)) return -1;

    if (feof(stream)) return -1;

    fgets(line, 256, stream);

    ptr = strchr(line, '\n');
    if (ptr) *ptr = '\0';

    len = strlen(line);

    if ((len + 1) < 256) {
        ptr = realloc(*lineptr, 256);
        if (ptr == NULL) return (-1);
        *lineptr = ptr;
        *n = 256;
    }

    strcpy(*lineptr, line);
    return (len);
}

void tmem_reset() { TempMem.it = 0; }

void *talloc(size_t n) {
    uint8_t *start = TempMem.data + TempMem.it;
    TempMem.it += n;
    if (TempMem.it >= TMEM_SIZE) {
        printf(
            "ERROR:: Temporary allocator ran out of memory. Returning "
            "malloc.\n");
        return malloc(n);
    }
    return start;
}

char *tstrf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t len = snprintf(NULL, 0, fmt, args);
    tstr result = talloc(len);
    vsprintf(result, fmt, args);
    va_end(args);
    return result;
}

void Log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    printf(fmt, args);
    va_end(args);
}

const char *GetLog() { return buffer; }