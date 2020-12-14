#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "v2.hpp"

using namespace std;

#define DEFAULT_LIFETIME 1000

struct Particle {
    v2 pos;
    v2 velocity;
    int lifetime = DEFAULT_LIFETIME;
    bool alive = true;
};

class ParticleSystem {
    vector<Particle> particles;
public:
    ParticleSystem() {

    }
    void emit(Particle &particle) {
        // @TODO: emplace_back?
        particles.push_back(particle);
    }

    void render() {
        for (auto& particle : particles) {
            if (!particle.alive) continue;
            glColor3f(1.0, 1.0, 1.0);
            glBegin(GL_POINTS);
            glVertex2f(particle.pos.x, particle.pos.y);
            glEnd();
        }
    }

    void update() {
        for (auto& particle : particles) {
            if (!particle.alive) continue;
            particle.pos.x += particle.velocity.x;
            particle.pos.y += particle.velocity.y;

            particle.lifetime--;
            if (!particle.lifetime)
                particle.alive = false;
        }

        prune();
    }

    void prune() {
        for (int i = 0; i < particles.size();) {
            if (!particles[i].alive) {
                particles.erase(particles.begin() + i);
            } else {
                i++;
            }
        }
    }
};