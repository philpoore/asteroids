#pragma once

#include <cmath>
#include <vector>
#include "utils.hpp"

class Bullet;
class Player;
class Asteroid;

#define STATE_TITLE 1
#define STATE_GAME 2
#define STATE_GAME_OVER 3

int state = STATE_TITLE;

int count = 0;
int levelCount = 0;

// Size we want the gfx context to be.
int pixelWidth = 640;
int pixelHeight = 480;
int zoom = 2;

// Actual size of the window
int width = pixelWidth * zoom;
int height = pixelHeight * zoom;

bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;
bool spacePressed = false;


int score = 0;
int level = 1;

const float bulletVelocity = 5;
const int bulletMaxLifetime = 50;
const int MAX_BULLETS = 100;

Player* player;
std::vector<Asteroid*> asteroids;
std::vector<Bullet*> bullets;