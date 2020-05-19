#include "ecs.h"

int get_uuid() {
    static int uuid = 0;
    return uuid++;
}

EntStruct* make_ent() {
    EntStruct* ent = malloc(sizeof(EntStruct));
    *ent = (EntStruct){
        .component_indexes = {0},
        .status = ES_ALIVE,
        .uuid = get_uuid(),
        .id = -1,
    };
    memset(ent->component_indexes, -1, sizeof(int) * CT_NumComponentTypes);
    return ent;
}

EntId get_free_space(EcsWorld* world) {
    for (int i = 0; i < world->max_num_entities; i++) {
        if (world->entities[i] == NULL ||
            world->entities[i]->status == ES_DEAD) {
            return i;
        }
    }

    int n = world->max_num_entities;
    world->max_num_entities *= 2;
    world->entities =
        realloc(world->entities, sizeof(EntStruct*) * world->max_num_entities);
    memset(world->entities + n, 0, sizeof(EntStruct*) * n);
    world->free_spot = n + 1;

    return n;
}

EntId add_ent(EcsWorld* world, EntStruct* ent) {
    world->num_entities++;
    int n = get_free_space(world);
    world->entities[n] = ent;
    ent->id = n;
    return n;
}

void add_ent_at(EcsWorld* world, EntStruct* ent, EntId where) {
    world->num_entities++;
    world->entities[where] = ent;
    ent->id = where;
}

EntId create_ent(EcsWorld* world) {
    int n = get_free_space(world);
    if (world->entities[n] == NULL) {
        EntStruct* ent = make_ent();
        return add_ent(world, ent);
    } else {
        memset(world->entities[n]->component_indexes, -1,
               CT_NumComponentTypes * sizeof(int));
        world->entities[n]->status = ES_ALIVE;
        world->entities[n]->uuid = get_uuid();
        world->entities[n]->id = n;
        world->num_entities++;
        printf("Reusing memory id: %d uuid: id: %d \n", n,
               world->entities[n]->uuid);
        return n;
    }
}

EcsWorld* create_ecs_world() {
    EcsWorld* world = malloc(sizeof(EcsWorld));
    *world = (EcsWorld){
        .max_num_entities = NUM_ENT_CHUNK,
        .entities = NULL,
        .free_spot = 0,
    };

    world->entities = malloc(sizeof(EntStruct*) * NUM_ENT_CHUNK);
    memset(world->entities, 0, sizeof(EntStruct*) * NUM_ENT_CHUNK);

    for (int i = 0; i < CT_NumComponentTypes; i++) {
        world->components[i].number = NUM_ENT_CHUNK;
        world->components[i].list = malloc(sizeof(void*) * NUM_ENT_CHUNK);
        memset(world->components[i].list, 0, sizeof(void*) * NUM_ENT_CHUNK);
    }

    return world;
}

int find_free_comp(struct CompList* list) {
    // TODO(Dustin): Use binary search, not linear
    for (int i = 0; i < list->number; i++) {
        if (list->list[i] == NULL) {
            return i;
        }
    }

    int prev = list->number;

    void* new = realloc(list->list, sizeof(void*) * (prev * 2));

    if (list->list == NULL) {
        printf("Failed to reallocate memory for the component list\n");
        return prev;
    }

    list->list = new;
    list->number *= 2;

    printf("Allocated more memory for component (%d)\n", list->number);

    for (int i = prev; i < list->number; i++) {
        list->list[i] = NULL;
    }

    return prev;
}

int has_comp_(EntStruct* e, int c1) { return e->component_indexes[c1] >= 0; }

int has_all_comps_(EntStruct* e, int* arr, int n) {
    for (int i = 0; i < n; i++) {
        if (!has_comp_(e, arr[i])) return 0;
    }
    return 1;
}

int is_ent_alive(EcsWorld* world, EntId ent) {
    if (ent < 0)
        return 0;
    else if (world->entities[ent] == NULL)
        return 0;
    else
        return world->entities[ent]->status != ES_DEAD;
}

int kill_ent(EntStruct* ent) {
    ent->status = ES_DYING;
    return ent->id;
}

int destroy_ent(EcsWorld* world, EntStruct* ent) {
    EntId id = ent->id;
    if (id < 0) {
        printf("Cannot kill entity with id: %d\n", id);
        return 0;
    }
    world->entities[id]->status = ES_DEAD;
    world->num_entities--;

    // NOTE(Dustin): We should be able to reuse components
    for (int i = 0; i < CT_NumComponentTypes; i++) {
        if (world->entities[id]->component_indexes[i] >= 0) {
            int cid = world->entities[id]->component_indexes[i];
            world->entities[id]->component_indexes[i] = -1;

            free(world->components[i].list[cid]);
            world->components[i].list[cid] = NULL;
        }
    }

    world->free_spot = id;

    return 1;
}

EntId get_first_with_(EcsWorld* world, int comp) {
    for (int i = 0; i < world->max_num_entities; i++) {
        if (is_ent_alive(world, i)) {
            if (world->entities[i]->component_indexes[comp] >= 0) return i;
        }
    }
    return -1;
}

void update_and_cleanup_ecs_world(EcsWorld* world) {
    for (int i = 0; i < world->max_num_entities; i++) {
        if (is_ent_alive(world, i)) {
            if (get_ent(world, i)->status == ES_DYING) {
                destroy_ent(world, get_ent(world, i));
            }
        }
    }
}
