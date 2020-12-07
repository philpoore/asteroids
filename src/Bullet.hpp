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
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POINTS);
        glVertex2f(pos.x, pos.y);
        glEnd();
    }
};