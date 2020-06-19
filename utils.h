#ifndef BENIS_UTILS_H
#define BENIS_UTILS_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MACROSTR(k) #k
#define TO_STR(x) MACROSTR(x),
#define TO_ENUM(x) x,

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

int getline(char **lineptr, size_t *n, FILE *stream);

#define TMEM_SIZE (0x800000)

static struct {
    uint8_t data[TMEM_SIZE];
    size_t it;
} TempMem = {
    .data = {0},
    .it = 0,
};

typedef char *tstr;

void tmem_reset();
void *talloc(size_t n);

tstr tstrf(const char *fmt, ...);

#define BUFFER_SIZE (1024)
static char buffer[BUFFER_SIZE] = {'\0'};

const char *GetLog();
void Log(const char *fmt, ...);

#endif  // BENIS_UTILS_H
