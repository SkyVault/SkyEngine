#include "physics_type.h"

Physics create_physics() {
    return (Physics) {
        .velocity = (Vector3){ 0 },
        .friction = 0.02f,
        .gravity_scale = 1.0f,
        .bounce_factor = 0.0f, 
        .grounded = false,
        .ground_angle = 0,
    };
}
