#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <GLFW/glfw3.h>

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

inline float deg2rad(float deg) {
    return deg * 0.0174533;
}

inline int random(int n) {
    return rand() % n;
}

struct v2 {
    float x;
    float y;

    void clamp(int width, int height) {
        if (x > width)
            x -= width;
        else if (x < 0)
            x += width;

        if (y > height)
            y -= height;
        else if (y < 0)
            y += height;
    }
};

bool lines_intersect(v2 p0, v2 p1, v2 p2, v2 p3, v2* i)
{
    v2 s1 = {p1.x - p0.x, p1.y - p0.y};
    v2 s2 = {p3.x - p2.x, p3.y - p2.y};

    float s, t;
    s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i != nullptr) {
            i->x = p0.x + (t * s1.x);
            i->y = p0.y + (t * s1.y);
        }
        return true;
    }

    return false; // No collision
}



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
};


const float bulletVelocity = 5;
const int bulletMaxLifetime = 100;
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


const int MAX_BULLETS = 100;
int numBullets = 0;
Bullet* bullets;

void createBullet(v2 pos, v2 velocity, float dir) {
    Bullet* bullet = nullptr;
    for (int i = 0; i < numBullets; i++) {
        if (!bullets[i].alive) {
            bullet = &bullets[i];
        }
    }
    if (!bullet) {
        bullet = &bullets[numBullets++];
    }
      
    bullet->pos = pos;
    bullet->alive = true;
    bullet->lifetime = 0;
    bullet->velocity = velocity;
    bullet->velocity.x += cos(deg2rad(dir)) * bulletVelocity;
    bullet->velocity.y += sin(deg2rad(dir)) * bulletVelocity;
}


const static float turnSpeed = 1;
const static float velocityDecay = 0.99;
const static float acceleration = 0.1;
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


// //////////////////////////////////////////////////////////////////
// Window setup
void glInfo() {
    using std::cout;
    using std::endl;
    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void init() {
    if (!glfwInit()) {
        std::cout << "Can't init GLFW\n";
        exit(1);
    }
}

GLFWwindow* initWindow() {
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    char* title = (char*)"Asteroids";
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        const char* description;
        int code = glfwGetError(&description);
        std::cout << "Can't create window : " << code << " : " << std::string(description) << "\n";
        exit(2);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup 2D rendering
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, pixelWidth, pixelHeight, 0, 0, 1);
    glEnable(GL_MULTISAMPLE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return window;
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
    
    GLFWwindow* window = initWindow();
    glfwSetKeyCallback(window, key_callback);

    // Game state
    Player player;
    player.pos = {20, 20};
    player.velocity = {0.1, 0};
    player.dir = 90;

    bullets = new Bullet[MAX_BULLETS];
    std::vector<Asteroid*> asteroids;

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
        // Update
        if (upPressed)
            player.accelerate();
        if (leftPressed)
            player.turnLeft();
        if (rightPressed)
            player.turnRight();
        if (spacePressed && count % 10 == 0)
            player.shoot();

        player.update();

        // Update bullets
        for (int i = 0; i < numBullets; i++) {
            bullets[i].update();
        }

        for (auto& asteroid : asteroids) {
            asteroid->update();
        }

        // Collision Detection
        // Check if player has hit an asteroid.
        for (auto& asteroid : asteroids) {
            if (!asteroid->alive) continue;
            if (asteroid->collision(player.prevPos, player.pos)) {
                std::cout << "Collision with asteroid\n";
                // @TODO: Handle player dying here.
            }
        }

        // Check if bullet has hit an asteroid.
        for (int i = 0; i < numBullets; i++) {
            Bullet& bullet = bullets[i];
            if (!bullet.alive) continue;
            for (auto& asteroid : asteroids) {
                if (!asteroid->alive) continue;
                if (asteroid->collision(bullet.prevPos, bullet.pos)) {
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
                }
            }
        }

        // @TODO: Check game for win state

        /////////////
        // Render
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPointSize(2);
        glLineWidth(1); 

        // Render grid
        glColor3f(0.1, 0.1, 0.1);
        glBegin(GL_LINES);
        for (int i = 0; i < pixelWidth; i += 50) {
            glVertex2f(i, 0);
            glVertex2f(i, pixelHeight);
        }
        for (int j = 0; j < pixelHeight; j += 50) {
            glVertex2f(0, j);
            glVertex2f(pixelWidth, j);
        }
        glEnd();

        // Draw player
        player.draw();

        // Render bullets
        for (int i = 0; i < numBullets; i++) {
            bullets[i].draw();
        }

        // Render asteroids
        for (auto& asteroid : asteroids) {
            asteroid->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete[] bullets;

    glfwTerminate();

    return EXIT_SUCCESS;
}