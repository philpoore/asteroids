#include <iostream>
#include <string>

#define WITH_ALSA 1

// #include <irrKlang.h>
#include <soloud.h>
#include <soloud_wav.h>

#include "gfx.hpp"
#include "v2.hpp"
#include "game.hpp"
#include "vector-font.hpp"

#include "ParticleSystem.hpp"

#include "Player.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"

class Game {
    GLFWwindow* window;
    ParticleSystem particles;


    // irrklang::ISoundEngine *soundEngine;
    // irrklang::ISoundSource* shootSound;

    SoLoud::Soloud soundEngine;
    SoLoud::Wav shootSound;

public:
    Game();

    void run();
    void update();
    void render();
    void nextLevel();
    void renderTitleScreen();
    void renderScore();
    void renderLevelText();
};


void Game::nextLevel() {
    std::cout << "Level: " << level << "\n";
    // Reset the player position.
    player->pos = {pixelWidth / 2.0f, pixelHeight / 2.0f};
    player->velocity = {0, 0};
    player->dir = 270;

    // Reset Level message timer.
    levelCount = 0;

    // Create some asteroids for next level.
    for (int i = 0; i < level + 3; i++) {
        asteroids.push_back(Asteroid::newRandom(3));
    }
}

void Game::update() {
    
    if (state == STATE_TITLE) {
        if (spacePressed) {
            state = STATE_GAME;
            return;
        }
    }

    // Update
    if (upPressed)
        player->accelerate();
    if (leftPressed)
        player->turnLeft();
    if (rightPressed)
        player->turnRight();
    if (spacePressed) {
        player->shoot();

#if 0
        soundEngine->play2D("sounds/fire.wav", false);
#else
        // cout << "playing sound\n";
        // soundEngine->play2D(shootSound, false);
#endif
        
    }
        

    player->update();

    // Update bullets
    for (auto& bullet : bullets) {
        bullet->update();
    }

    for (auto& asteroid : asteroids) {
        asteroid->update();
    }

    particles.update();

    /////////////////////////////
    // Collision Detection
    // Check if player has hit an asteroid.
    for (auto& asteroid : asteroids) {
        if (!asteroid->alive) continue;
        if (asteroid->collision(player->prevPos, player->pos)) {
            std::cout << "Collision with asteroid\n";
            // @TODO: Handle player dying here.
        }
    }

    // Check if bullet has hit an asteroid.
    bool done = false;
    for (auto& bullet : bullets) {
        if (!bullet->alive) continue;
        for (auto& asteroid : asteroids) {
            if (!asteroid->alive) continue;
            if (!asteroid->collision(bullet->prevPos, bullet->pos)) continue;
            
            // Increment score when an asteroid blows up.
            score += 1;
            // soundEngine->play2D(shootSound, false);
            soundEngine.play(shootSound);

            // Consume bullet.
            bullet->alive = false;
            asteroid->alive = false;

            // Emit particles
            Particle particle = {
                bullet->pos,
                {0, 0},
                100,
            };

            int numParticles = 25;
            float particleSpeed = 1.0f;
            for (int i = 0; i < numParticles; i++) {
                float vx = randomf(particleSpeed * 2) - particleSpeed;
                float vy = randomf(particleSpeed * 2) - particleSpeed;
                particle.velocity = {vx, vy};
                particles.emit(particle);
            }

            if (asteroid->size > 1) {
                // @TODO: Nicer handling of breaking of asteroids
                asteroids.push_back(
                    new Asteroid(
                        asteroid->pos,
                        asteroid->velocity,
                        asteroid->size - 1
                    )
                );
                asteroids.push_back(
                    new Asteroid(
                        asteroid->pos,
                        v2{asteroid->velocity.x + 0.1f, asteroid->velocity.y + 0.1f},
                        asteroid->size - 1
                    )
                );
            }

            done = true;
            break;
        }

        if (done) break;
    }

    // Remove dead bullets
    for (int i = 0; i < bullets.size();) {
        if (!bullets[i]->alive)
            bullets.erase(bullets.begin() + i);
        else
            i++;
    }

    // Remove dead asteroids
    for (int i = 0; i < asteroids.size();) {
        if (!asteroids[i]->alive) {
            delete asteroids[i];
            asteroids.erase(asteroids.begin() + i);
        } else {
            i++;
        }
    }

    // @TODO: Check game for win state
    bool allDead = true;
    for (int i = 0; i < asteroids.size();) {
        if (asteroids[i]->alive) {
            allDead = false;
            break;
        }
    }
    if (allDead) {
        level += 1;
        nextLevel();
    }
}


/////////////
// Render
void Game::renderTitleScreen() {
    glColor3f(1, 1, 1);
    renderText(pixelWidth / 2, pixelHeight / 2, "ASTEROIDS", 20);

    renderText(pixelWidth / 2, pixelHeight / 2 + 50, "PRESS SPACE", 8);
    renderText(pixelWidth / 2, pixelHeight / 2 + 150, "BY PHIL POORE - 2020", 4);
    renderText(pixelWidth / 2, pixelHeight / 2 + 160, "V 0.0.1", 3);
    renderText(10, pixelHeight - 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 4);
}

void Game::renderScore() {
    glColor3f(1, 1, 1);
    renderText(40, 32, std::to_string(score), 8);
}

void Game::renderLevelText() {
    if (levelCount >= 250) return;
    levelCount++;
    int fontSize = 6;
    std::string msg = "LEVEL " + std::to_string(level);
    int sy = (pixelHeight / 2) + 40;
    int sx = (pixelWidth / 2) - (msg.size() / 2) * fontSize * 1.5;
    float c = 1.0 - ((levelCount - 150) / 100.0);
    // @TODO: clamp
    if (c > 1.0) { c = 1.0; } else if (c < 0.0) { c = 0.0; }
    glColor3f(c, c, c);
    renderText(sx, sy, msg, fontSize);
}

void Game::render() {
    // Reset gfx
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(2);
    glLineWidth(1); 

    

    if (state == STATE_TITLE) {
        // Render asteroids
        for (auto& asteroid : asteroids) {
            asteroid->draw();
        }
        renderTitleScreen();
        return;
    }
    
    if (state == STATE_GAME) {
        // Draw grid
        renderGrid(pixelWidth, pixelHeight, 50);

        // Draw score
        renderScore();

        // Draw level text
        renderLevelText();

        // Render asteroids
        for (auto& asteroid : asteroids) {
            asteroid->draw();
        }

        // Render bullets
        for (auto& bullet : bullets) {
            bullet->draw();
        }

        particles.render();
        
        // Draw player
        player->draw();
    }
}


// Keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool isRelease = action == GLFW_RELEASE;
    bool isPressed = action == GLFW_PRESS;

    if (key == GLFW_KEY_UP && isRelease)
        upPressed = false;
    if (key == GLFW_KEY_UP && isPressed)
        upPressed = true;

    if (key == GLFW_KEY_DOWN && isRelease)
        downPressed = false;
    if (key == GLFW_KEY_DOWN && isPressed)
        downPressed = true;

    if (key == GLFW_KEY_LEFT && isRelease)
        leftPressed = false;
    if (key == GLFW_KEY_LEFT && isPressed)
        leftPressed = true;

    if (key == GLFW_KEY_RIGHT && isRelease)
        rightPressed = false;
    if (key == GLFW_KEY_RIGHT && isPressed)
        rightPressed = true;

    if (key == GLFW_KEY_SPACE && isRelease)
        spacePressed = false;
    if (key == GLFW_KEY_SPACE && isPressed)
        spacePressed = true;
}


Game::Game() {
    init();
    
    window = initWindow(height, width, "Asteroids");
    glOrtho(0, pixelWidth, pixelHeight, 0, 0, 1);
    glfwSetKeyCallback(window, key_callback);
    
    // Game state

    // Sound
    soundEngine.init();
    shootSound.load("sounds/fire2.wav");
    // soundEngine = irrklang::createIrrKlangDevice();
    // shootSound = soundEngine->addSoundSourceFromFile("sounds/fire2.wav", irrklang::ESM_AUTO_DETECT, true);

    // Place player in the middle of the screen.
    player = new Player();
    player->pos = {pixelWidth / 2.0f, pixelHeight / 2.0f};
    player->velocity = {0, 0};
    player->dir = 270;

    // Create some asteroids at the start of the same
    // @TODO: Randomize nicer
    asteroids.push_back(Asteroid::newRandom(3));
    asteroids.push_back(Asteroid::newRandom(3));
    asteroids.push_back(Asteroid::newRandom(3));
}

void Game::run() {
    // Game loop
    while (!glfwWindowShouldClose(window)) {
        count++;
        // std::cout << "Bullets.length: " << bullets.size() << "\n";
        // std::cout << "Asteroids.length: " << asteroids.size() << "\n";
        update();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
};