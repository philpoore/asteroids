#pragma once

#include <cmath>
#include <vector>
#include "utils.hpp"

class Bullet;
class Player;
class Asteroid;

int pixelWidth = 640;
int pixelHeight = 480;
int zoom = 2;

int width = pixelWidth * zoom;
int height = pixelHeight * zoom;

bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;
bool spacePressed = false;




const float bulletVelocity = 5;
const int bulletMaxLifetime = 100;
const int MAX_BULLETS = 100;
int numBullets = 0;

Bullet* bullets;
Player* player;
std::vector<Asteroid*> asteroids;