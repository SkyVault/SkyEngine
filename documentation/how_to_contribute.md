# How to contrubute

## Code map

| File            | Description                                         |
| --------------- | --------------------------------------------------- |
| entity_struct.h | contains the entity types                           |
| behaviours.c    | contains the unique behaviours for each entity type |
| comps.h         | contains the list of component types                |

## How to use the entity component system

Code example
 
```c
EcsWorld* ecs = create_ecs_world();

EntId id = create_ent(ecs);
EntStruct* self = get_ent(ecs, id);

add_comp(ecs, self, Transform, .translation=(Vector3){0, 0, 0});
add_comp_obj(ecs, self, Sprite, create_sprite()); 
```