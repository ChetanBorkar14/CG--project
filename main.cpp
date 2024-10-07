#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <random>

#define M_PI 3.14159265358979323846

const float PLANE_SPEED = 0.3f;
const float COLLAPSE_SPEED = 0.3f;
const float BUILDING_WIDTH = 0.15f;
const float BUILDING_HEIGHT = 1.0f;
const float BUILDING_MIN_HEIGHT = 0.1f;
const float CLOUD_SIZE = 0.1f;
const float TREE_WIDTH = 0.05f;
const float TREE_HEIGHT = 0.15f;
const float RESET_DELAY = 3.0f;
const float BLAST_DURATION = 0.5f;
const int BLAST_PARTICLES = 50;
const int NUM_CLOUDS = 5;
const int NUM_TREES = 6;

struct Plane {
    float x;
    float y;
    bool crashed;
};

struct Building {
    float x;
    float height;
    float originalHeight;
    bool collapsed;
};

struct Cloud {
    float x;
    float y;
};

struct Tree {
    float x;
    float y;
};

struct BlastParticle {
    float x;
    float y;
    float vx;
    float vy;
    float lifetime;
};

Plane plane1, plane2;
Building building1, building2;
std::vector<Cloud> clouds;
std::vector<Tree> trees;
std::vector<BlastParticle> blastParticles;
float blastTimer = 0.0f;

std::random_device rd;
std::mt19937 gen(rd());

float randomFloat(float min, float max) {
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

void initializeScene() {
    plane1 = { -1.2f, 0.0f, false };
    plane2 = { -1.2f, -0.2f, false };
    building1 = { 0.0f, BUILDING_HEIGHT, BUILDING_HEIGHT, false };
    building2 = { 0.5f, BUILDING_HEIGHT, BUILDING_HEIGHT, false };

    clouds.clear();
    for (int i = 0; i < NUM_CLOUDS; ++i) {
        clouds.push_back({ randomFloat(-1.0f, 1.0f), randomFloat(0.5f, 0.9f) });
    }

    trees.clear();
    for (int i = 0; i < NUM_TREES; ++i) {
        trees.push_back({ randomFloat(-1.0f, 1.0f), -0.7f });
    }

    blastParticles.clear();
    blastTimer = 0.0f;
}

void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawPlane(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Body
    glBegin(GL_POLYGON);
    glColor3f(0.8f, 0.8f, 0.8f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.5f, 0.05f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.5f, -0.05f);
    glEnd();

    // Wings
    glBegin(GL_TRIANGLES);
    glColor3f(0.6f, 0.6f, 0.6f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.35f, 0.15f);
    glVertex2f(0.4f, 0.0f);

    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.35f, -0.15f);
    glVertex2f(0.4f, 0.0f);
    glEnd();

    // Tail
    glBegin(GL_TRIANGLES);
    glColor3f(0.6f, 0.6f, 0.6f);
    glVertex2f(0.45f, 0.0f);
    glVertex2f(0.55f, 0.1f);
    glVertex2f(0.55f, 0.0f);

    glVertex2f(0.45f, 0.0f);
    glVertex2f(0.55f, -0.1f);
    glVertex2f(0.55f, 0.0f);
    glEnd();

    // Windows
    glColor3f(0.3f, 0.3f, 0.3f);
    for (float i = 0.1f; i < 0.4f; i += 0.1f) {
        drawRectangle(i, -0.02f, 0.05f, 0.02f);
    }

    glPopMatrix();
}

void drawBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glColor3f(0.18f, 0.55f, 0.34f);
    glVertex2f(1.0f, -0.5f);
    glVertex2f(-1.0f, -0.5f);
    glEnd();
}

void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 3; i++) {
        glBegin(GL_TRIANGLE_FAN);
        for (int j = 0; j <= 360; j += 30) {
            float theta = j * M_PI / 180.0f;
            glVertex2f(x + i * CLOUD_SIZE * 0.5f + cosf(theta) * CLOUD_SIZE,
                y + sinf(theta) * CLOUD_SIZE * 0.5f);
        }
        glEnd();
    }
}

void drawTree(float x, float y) {
    glColor3f(0.5f, 0.35f, 0.05f);
    drawRectangle(x + TREE_WIDTH / 4, y, TREE_WIDTH / 2, TREE_HEIGHT / 3);

    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + TREE_HEIGHT / 3);
    glVertex2f(x + TREE_WIDTH / 2, y + TREE_HEIGHT);
    glVertex2f(x + TREE_WIDTH, y + TREE_HEIGHT / 3);
    glEnd();
}

void drawAntenna(float x, float y, float buildingHeight) {
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(x, y);
    glVertex2f(x, y + 0.2f);
    glEnd();
}

void drawWindows(float x, float y, float width, float height) {
    glColor3f(0.0f, 0.0f, 0.0f);
    float windowWidth = width / 5.0f;
    float windowHeight = height / 10.0f;
    for (float i = 0; i < 5; ++i) {
        for (float j = 0; j < 10; ++j) {
            drawRectangle(x + i * windowWidth, y + j * windowHeight, windowWidth - 0.01f, windowHeight - 0.01f);
        }
    }
}

void createBlastParticles(float x, float y) {
    blastParticles.clear();
    for (int i = 0; i < BLAST_PARTICLES; ++i) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
        float speed = static_cast<float>(rand()) / RAND_MAX * 0.5f + 0.2f;
        blastParticles.push_back({
            x, y,
            cosf(angle) * speed, sinf(angle) * speed,
            static_cast<float>(rand()) / RAND_MAX * BLAST_DURATION
            });
    }
}

void updateBlastParticles(float deltaTime) {
    for (auto& particle : blastParticles) {
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.lifetime -= deltaTime;
    }

    blastParticles.erase(
        std::remove_if(blastParticles.begin(), blastParticles.end(),
            [](const BlastParticle& p) { return p.lifetime <= 0; }),
        blastParticles.end());
}

void drawBlastParticles() {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (const auto& particle : blastParticles) {
        float alpha = particle.lifetime / BLAST_DURATION;
        glColor4f(1.0f, 0.5f, 0.0f, alpha);
        glVertex2f(particle.x, particle.y);
    }
    glEnd();
}

void updatePlanes(float deltaTime) {
    if (plane1.x + 0.4f < building1.x && !plane1.crashed) {
        plane1.x += PLANE_SPEED * deltaTime;
    }
    else if (!plane1.crashed) {
        plane1.crashed = true;
        building1.collapsed = true;
        createBlastParticles(building1.x, 0.0f);
    }

    if (plane2.x + 0.4f < building2.x && !plane2.crashed) {
        plane2.x += PLANE_SPEED * deltaTime;
    }
    else if (!plane2.crashed) {
        plane2.crashed = true;
        building2.collapsed = true;
        createBlastParticles(building2.x, -0.2f);
    }
}

void collapseBuildings(float deltaTime) {
    if (building1.collapsed && building1.height > BUILDING_MIN_HEIGHT) {
        building1.height -= COLLAPSE_SPEED * deltaTime;
    }

    if (building2.collapsed && building2.height > BUILDING_MIN_HEIGHT) {
        building2.height -= COLLAPSE_SPEED * deltaTime;
    }
}

bool isAnimationComplete() {
    return (plane1.crashed && plane2.crashed &&
        building1.height <= BUILDING_MIN_HEIGHT &&
        building2.height <= BUILDING_MIN_HEIGHT &&
        blastParticles.empty());
}

void drawScene(bool showGameOver) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    for (const auto& cloud : clouds) {
        drawCloud(cloud.x, cloud.y);
    }

    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y);
    }

    glColor3f(0.7f, 0.7f, 0.7f);
    drawRectangle(building1.x, -0.5f, BUILDING_WIDTH, building1.height);
    drawWindows(building1.x, -0.5f, BUILDING_WIDTH, building1.height);

    glColor3f(0.7f, 0.7f, 0.7f);
    drawRectangle(building2.x, -0.5f, BUILDING_WIDTH, building2.height);
    drawWindows(building2.x, -0.5f, BUILDING_WIDTH, building2.height);

    drawAntenna(building2.x + BUILDING_WIDTH / 2, -0.5f + building2.height, building2.height);

    if (!plane1.crashed) {
        drawPlane(plane1.x, plane1.y, 0.4f);
    }

    if (!plane2.crashed) {
        drawPlane(plane2.x, plane2.y, 0.4f);
    }

    drawBlastParticles();

    glfwSwapBuffers(glfwGetCurrentContext());
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Plane Animation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

    glViewport(0, 0, 1920, 1080);
    glfwSetWindowSizeCallback(window, [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        });

    float lastTime = glfwGetTime();
    float resetTimer = 0.0f;
    bool isResetting = false;
    bool showGameOver = false;

    initializeScene();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (!isResetting) {
            updatePlanes(deltaTime);
            collapseBuildings(deltaTime);
            updateBlastParticles(deltaTime);

            if (isAnimationComplete()) {
                isResetting = true;
                resetTimer = 0.0f;
                showGameOver = true;
            }
        }
        else {
            resetTimer += deltaTime;
            if (resetTimer >= RESET_DELAY) {
                initializeScene();
                isResetting = false;
                showGameOver = false;
            }
        }

        drawScene(showGameOver);

        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    glfwTerminate();
    return 0;
}
