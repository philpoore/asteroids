#pragma once

#include <GLFW/glfw3.h>

#include "v2.hpp"
#include "game.hpp"
#include "Bullet.hpp"

const static float turnSpeed = 2;
const static float velocityDecay = 0.99;
const static float acceleration = 0.1;

void createBullet(v2 pos, v2 velocity, float dir);

struct Player {
    v2 pos;
    v2 prevPos = {0, 0};
    v2 velocity;
    float dir = 0;

    void update() {
        prevPos = pos;
        pos.x += velocity.x;
        pos.y += velocity.y;

        velocity.x *= velocityDecay;
        velocity.y *= velocityDecay;

        pos.clamp(pixelWidth, pixelHeight);
    }

    void accelerate() {
        velocity.x += cos(deg2rad(dir)) * acceleration;
        velocity.y += sin(deg2rad(dir)) * acceleration;
    }

    void turnLeft() {
        dir -= turnSpeed;
    }

    void turnRight() {
        dir += turnSpeed;
    }

    void shoot() {
        createBullet(pos, velocity, dir);
    }

    void draw() {
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(pos.x, pos.y, 0);
        glRotatef(dir + 90, 0, 0, 1);
        glBegin(GL_LINE_LOOP);
        glVertex2f(0, -10);
        glVertex2f(5, 5);
        glVertex2f(0, 0);
        glVertex2f(-5, 5);
        glEnd();
        glPopMatrix();
    }
};