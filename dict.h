#pragma once
#ifndef SKY_DICT_H
#define SKY_DICT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // memset

#define DICT_SIZE 10

uint32_t hash(void* key);
uint32_t hash_str(const char* key);

typedef struct Entry {
    char* key;
    void* value;
    struct Entry* next;
} Entry;

typedef struct {
    Entry** data;
} Dict;

Dict* create_dict();

void dict_add(Dict* self, const char* key, void* value);
void* dict_get(Dict* self, const char* key);

bool dict_has(Dict* self, const char* key);

void* dict_get_or(Dict* self, const char* key, void* def);

void dict_addi(Dict* self, int key, void* value);
void* dict_geti(Dict* self, int key);

void dict_dump_table(Dict* self);

#endif  // HARP_DICT_H
