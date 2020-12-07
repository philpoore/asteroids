#include <iostream>
#include <string>

#include "gfx.hpp"
#include "v2.hpp"
#include "game.hpp"

#include "Player.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"


void update(int count) {
    // Update
    if (upPressed)
        player->accelerate();
    if (leftPressed)
        player->turnLeft();
    if (rightPressed)
        player->turnRight();
    if (spacePressed && count % 10 == 0)
        player->shoot();

    player->update();

    // Update bullets
    for (int i = 0; i < numBullets; i++) {
        bullets[i].update();
    }

    for (auto& asteroid : asteroids) {
        asteroid->update();
    }

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
    for (int i = 0; i < numBullets; i++) {
        Bullet& bullet = bullets[i];
        if (!bullet.alive) continue;
        for (auto& asteroid : asteroids) {
            if (!asteroid->alive) continue;
            if (!asteroid->collision(bullet.prevPos, bullet.pos)) continue;
            
            // Consume bullet.
            bullet.alive = false;

            if (asteroid->size == 1) {
                asteroid->alive = false;
            } else {
                // @TODO: Nicer handling of breaking of asteroids
                asteroid = new Asteroid(asteroid->pos, asteroid->velocity, asteroid->size - 1);
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

    // @TODO: Check game for win state
}

/////////////
// Render
void render(GLFWwindow* window) {
    // Reset gfx
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(2);
    glLineWidth(1); 

    // Draw grid
    renderGrid(pixelWidth, pixelHeight, 50);

    // Render bullets
    for (int i = 0; i < numBullets; i++) {
        bullets[i].draw();
    }

    // Render asteroids
    for (auto& asteroid : asteroids) {
        asteroid->draw();
    }

    // Draw player
    player->draw();

    glfwSwapBuffers(window);
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

int main() {
    init();
    
    GLFWwindow* window = initWindow(height, width, "Asteroids");
    glOrtho(0, pixelWidth, pixelHeight, 0, 0, 1);
    glfwSetKeyCallback(window, key_callback);
    
    // Game state
    player = new Player();
    player->pos = {20, 20};
    player->velocity = {0.1, 0};
    player->dir = 90;

    bullets = new Bullet[MAX_BULLETS];

    // Create some asteroids
    // @TODO: Randomize
    asteroids.push_back(new Asteroid(v2{50, 50}, v2{0.1, 0}, 3));
    asteroids.push_back(new Asteroid(v2{150, 50}, v2{0.0, 0.1}, 2));
    asteroids.push_back(new Asteroid(v2{250, 50}, v2{0.1, 0.1}, 3));
    // asteroids.emplace_back(v2{150, 50}, v2{0.0, 0.1}, 4);
    // asteroids.emplace_back(v2{250, 50}, v2{0.1, 0}, 2);

    int count = 0;

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        count++;
        
        update(count);
        render(window);

        glfwPollEvents();
    }
    delete[] bullets;

    glfwTerminate();

    return EXIT_SUCCESS;
}