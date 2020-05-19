#ifndef VAULT_ECS_H
#define VAULT_ECS_H

// TODO(Dustin):
// make add_comp return the added component

#include "comps.h"

#ifndef X_COMPONENT_TYPES
#define X_COMPONENT_TYPES X(CT_NumComponentTypes)
#endif  // X_COMPONENT_TYPES

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ENT_CHUNK (32)

#ifdef X_COMPONENT_TYPES
#ifndef MACROSTR
#define MACROSTR(k) #k
#endif

enum ComponentTypes {
#define X(Enum) Enum,
    X_COMPONENT_TYPES
#undef X
};

static const char* ComponentTypesNames[] = {
#define X(String) MACROSTR(String),
    X_COMPONENT_TYPES
#undef X
};
#endif  // X_COMPONENT_TYPES

typedef int EntId;

enum EntStatus {
    ES_ALIVE,
    ES_DYING,
    ES_DEAD,
};

typedef struct {
    int component_indexes[CT_NumComponentTypes];
    int status;
    int uuid;
    int id;
} EntStruct;

struct CompList {
    void** list;
    int number;
};

typedef struct EntIt {
    EntId id;
    struct EntIt* next;
} EntIt;

typedef struct {
    int max_num_entities;
    EntStruct** entities;

    int num_entities;
    int free_spot;

    struct CompList components[CT_NumComponentTypes];
} EcsWorld;

int find_free_comp(struct CompList* list);

#define CAT(a, b) a##b

#ifndef NEW_COMP
#define NEW_COMP(C, ...) \
    memcpy(malloc(sizeof(C)), &(C const){__VA_ARGS__}, sizeof(C));
#endif  // NEW_COMP

// Lets make a safer function for getting entities.
#define add_comp(world, ent, comp, ...)                                 \
    {                                                                   \
        int place = find_free_comp(&world->components[CAT(CT_, comp)]); \
        ent->component_indexes[CAT(CT_, comp)] = place;                 \
        world->components[CAT(CT_, comp)].list[place] =                 \
            (void*)NEW_COMP(comp, __VA_ARGS__);                         \
    }

#define add_comp_obj(world, ent, comp, init)                            \
    {                                                                   \
        int place = find_free_comp(&world->components[CAT(CT_, comp)]); \
        ent->component_indexes[CAT(CT_, comp)] = place;                 \
        world->components[CAT(CT_, comp)].list[place] =                 \
            (void*)malloc(sizeof(comp));                                \
        comp* _c = get_comp(world, ent, comp);                          \
        *_c = (init);                                                   \
    }

#define get_comp(world, ent, comp)            \
    ((comp*)world->components[CAT(CT_, comp)] \
         .list[ent->component_indexes[CAT(CT_, comp)]])

#define get_comp_name(comp) (ComponentTypesNames[CAT(CT, comp)])

EntId get_first_with_(EcsWorld* world, int comp);
#define get_first_with(world, comp) (get_first_with_(world, CAT(CT_, comp)))

#define has_comp(world, e, comp) ((e)->component_indexes[CAT(CT_, comp)] >= 0)
#define get_ent(world, id) ((world)->entities[(id)])

int get_uuid();
int is_ent_alive(EcsWorld* world, EntId ent);

EntStruct* make_ent();

EntId add_ent(EcsWorld* world, EntStruct* ent);
EntId create_ent(EcsWorld* world);

#define create_and_get_ent(world) get_ent(world, create_ent(world));

void add_ent_at(EcsWorld* world, EntStruct* ent, EntId where);

int kill_ent(EntStruct* ent);                      // Sets the status to dying
int destroy_ent(EcsWorld* world, EntStruct* ent);  // Cleans up the memory

EcsWorld* create_ecs_world();

void update_and_cleanup_ecs_world(EcsWorld* world);

#endif  // VAULT_ECS_H
