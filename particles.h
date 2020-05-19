#ifndef BENIS_PARTICLES
#define BENIS_PARTICLES

#include "raylib.h"
#include "utils.h"

#define MAX_NUM_OF_PARTICLES (10000)

typedef struct {
    Texture2D texture;
    Rectangle region;

    Vector3 position;
    float radius;

    int amount;

    Vector2 vx_range;
    Vector2 vy_range;
    Vector2 vz_range;
} Emitter;

typedef struct {
    int type;
    int alive;
    int region;
    Vector3 position;
    Vector3 velocity;
    Color color;
} Particle;

typedef struct {
    Particle particles[MAX_NUM_OF_PARTICLES];
    int number_of_particles;

    Texture2D texture;
    Rectangle regions[100];
} ParticleSystem;

ParticleSystem* create_particle_system();

void spawn_particle(ParticleSystem* sys, Emitter* e);

void update_particle_system(ParticleSystem* sys);
void render_particle_system(ParticleSystem* sys);

#endif  // BENIS_PARTICLES