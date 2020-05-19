#ifndef BENIS_UTILS_H
#define BENIS_UTILS_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MACROSTR(k) #k
#define TO_STR(x) MACROSTR(x),
#define TO_ENUM(x) x,

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

#endif  // BENIS_UTILS_H
