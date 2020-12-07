#pragma once

#include <GLFW/glfw3.h>
#include <cmath>

#include "v2.hpp"
#include "game.hpp"


const float sizeFactor = 10;

struct Asteroid {
    v2 pos = {0, 0};
    v2 prevPos = {0, 0};
    v2 velocity = {0, 0};
    v2* points = nullptr;
    int numPoints = 0;
    int size = 0;
    bool alive = false;
    Asteroid() {
        alive = false;
    }
    Asteroid(v2 _pos, v2 _velocity, int _size) {
        pos = _pos;
        velocity = _velocity;
        size = _size;
        numPoints = size + 4;
        alive = true;
        points = new v2[numPoints];
        for (int i = 0; i < numPoints; i++) {
            float a = (M_PI * 2.0 * (float)i) / numPoints;
            float rx = (random(70) / 100.0) + 0.3;
            float ry = (random(70) / 100.0) + 0.3;
            float dx = cos(a+0.1) * sizeFactor * (size * rx);
            float dy = sin(a+0.1) * sizeFactor * (size * ry);
            points[i] = {dx, dy};
        }
    }
    ~Asteroid() {
        if (points)
            delete[] points;
    }

    void update() {
        if (!alive) return;
        prevPos = pos;
        pos.x += velocity.x;
        pos.y += velocity.y;
        
        pos.clamp(pixelWidth, pixelHeight);
    }

    void draw() {
        if (!alive || numPoints < 0) return;
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(pos.x, pos.y, 0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < numPoints; i++) {
            glVertex2f(points[i].x, points[i].y);
        }
        glEnd();
        glPopMatrix();
    }

    bool collision(v2& p0, v2& p1) {
        if (!alive) return false;
        for (int i = 0; i < numPoints; i++) {
            int j = i + 1;
            if (j == numPoints) j -= numPoints;

            v2 p2 = {points[i].x + pos.x, points[i].y + pos.y};
            v2 p3 = {points[j].x + pos.x, points[j].y + pos.y};
            if (lines_intersect(p0, p1, p2, p3, nullptr)) {
                return true;
            }
        }
        return false;
    }

    static Asteroid* newRandom(int size) {
        v2 pos = {(float)random(pixelWidth), (float)random(pixelHeight)};
        v2 vel = {randomf(0.1f) - 0.05f, randomf(0.1f) - 0.05f};

        return new Asteroid(pos, vel, size);
    }
};
