#include "dict.h"

uint32_t hash(void* key) {
    uint32_t a = (uint32_t)key;
    a = (a + 0x7ed55d16) + (a << 12);
    a = (a ^ 0xc761c23c) ^ (a >> 19);
    a = (a + 0x165667b1) + (a << 5);
    a = (a + 0xd3a2646c) ^ (a << 9);
    a = (a + 0xfd7046c5) + (a << 3);
    a = (a ^ 0xb55a4f09) ^ (a >> 16);
    return a % DICT_SIZE;
}

Dict* create_dict() {
    Dict* result = malloc(sizeof(Dict));
    result->data = malloc(sizeof(Entry) * DICT_SIZE);
    memset(result->data, NULL, sizeof(Entry) * DICT_SIZE);
    return result;
}

void dict_addi(Dict* self, int key, void* value) {
    dict_add(self, (uint32_t)key, value);
}

void* dict_geti(Dict* self, int key) { return dict_get(self, (uint32_t)key); }

void dict_add(Dict* self, void* key, void* value) {
    uint32_t index = hash(key);

    if (self->data[index] == NULL) {
        self->data[index] = malloc(sizeof(Entry));
        self->data[index]->key = key;
        self->data[index]->next = NULL;
        self->data[index]->value = value;
    } else {
        Entry* it = self->data[index];

        while (it->next != NULL) it = it->next;

        it->next = malloc(sizeof(Entry));
        it->next->key = key;
        it->next->next = NULL;
        it->next->value = value;
    }
}

void* dict_get(Dict* self, void* key) {
    uint32_t index = hash(key);
    Entry* entry = self->data[index];
    if (entry == NULL) return NULL;
    if (entry->next != NULL) {
        if (entry->key != key) {
            Entry* it = entry->next;
            while (it->next != NULL) {
                if (it->key == key) return it->value;
                it = it->next;
            }
            if (it->key != key) return NULL;
        } else {
            return entry->value;
        }
    } else {
        return entry->value;
    }
}

void dict_dump_table(Dict* self) {
    for (int i = 0; i < DICT_SIZE; i++) {
        if (self->data[i] == NULL) continue;

        Entry* it = self->data[i];

        while (it) {
            printf("(%p, %p)", it->key, it->value);
            if (it->next != NULL)
                printf(" ");
            else
                printf("\n");

            it = it->next;
        }
    }
}