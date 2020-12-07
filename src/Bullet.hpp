#pragma once

#include <GLFW/glfw3.h>

#include "v2.hpp"
#include "game.hpp"
#include "Bullet.hpp"

struct Bullet {
    v2 pos;
    v2 prevPos = {0, 0};
    v2 velocity;
    int lifetime;
    bool alive;

    void update() {
        if (!alive) return;
        if (lifetime > bulletMaxLifetime) {
            alive = false;
            return;
        }

        prevPos = pos;
        
        pos.x += velocity.x;
        pos.y += velocity.y;
        
        pos.clamp(pixelWidth, pixelHeight);

        lifetime++;
    }

    void draw() {
        if (!alive) return;
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2f(pos.x, pos.y);
        glEnd();
    }
};

void createBullet(v2 pos, v2 velocity, float dir) {
    Bullet* bullet = new Bullet();
      
    bullet->pos = pos;
    bullet->alive = true;
    bullet->lifetime = 0;
    bullet->velocity = velocity;
    bullet->velocity.x += cos(deg2rad(dir)) * bulletVelocity;
    bullet->velocity.y += sin(deg2rad(dir)) * bulletVelocity;
    bullets.push_back(bullet);
}