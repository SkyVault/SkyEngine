#ifndef BENIS_UTILS_H
#define BENIS_UTILS_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "raymath.h"

#define ENGINE_NAME "Vault Engine"
#define VAULT_TITLE "The " ENGINE_NAME "!"
#define COMPANY_NAME "Joy Vault"

#define VAULT_VERSION_MAJOR "0"
#define VAULT_VERSION_MINOR "1"
#define VAULT_VERSION_PATCH "15"
#define VAULT_VERSION                                                          \
  VAULT_VERSION_MAJOR "." VAULT_VERSION_MINOR "." VAULT_VERSION_PATCH

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

static Matrix transform_to_matrix(Transform t) {
  Matrix m = MatrixIdentity();
  m = MatrixMultiply(m, MatrixScale(t.scale.x, t.scale.y, t.scale.z));
  m = MatrixMultiply(m, QuaternionToMatrix(t.rotation));
  m = MatrixMultiply(m, MatrixTranslate(t.translation.x, t.translation.y, t.translation.z));
  return m;
}

static BoundingBox transform_bounding_box(BoundingBox in, Transform t) {
    Matrix m = transform_to_matrix(t);
    BoundingBox out;
    out.min = Vector3Transform(in.min, m);
    out.max = Vector3Transform(in.max, m);
    return out;
}

#endif  // BENIS_UTILS_H
