#pragma once
#ifndef SKY_DICT_H
#define SKY_DICT_H

#include <stdint.h>
#include <stdlib.h>

#define DICT_SIZE 200

uint32_t hash(void* key);
uint32_t hash_str(const char* key);

typedef struct Entry {
    void* key;
    void* value;
    struct Entry* next;
} Entry;

typedef struct {
    Entry** data;
} Dict;

Dict* create_dict();

void dict_add(Dict* self, void* key, void* value);
void* dict_get(Dict* self, void* key);

void dict_addi(Dict* self, int key, void* value);
void* dict_geti(Dict* self, int key);

void dict_dump_table(Dict* self);

#endif  // HARP_DICT_H