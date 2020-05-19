#include "particles.h"

ParticleSystem* create_particle_system() {
    ParticleSystem* sys = malloc(sizeof(ParticleSystem));
    sys->number_of_particles = 0;
    for (int i = 0; i < MAX_NUM_OF_PARTICLES; i++) {
        sys->particles[i] = (Particle){.alive = false};
    }
    return sys;
}

void spawn_particle(ParticleSystem* sys, Emitter* e) {}

void update_particle_system(ParticleSystem* sys) {}

void render_particle_system(ParticleSystem* sys) {}