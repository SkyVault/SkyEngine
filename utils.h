#ifndef BENIS_UTILS_H
#define BENIS_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define MACROSTR(k) #k
#define TO_STR(x) MACROSTR(x),
#define TO_ENUM(x) x,

int getline(char **lineptr, size_t *n, FILE *stream);

#endif  // BENIS_UTILS_H
