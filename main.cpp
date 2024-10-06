#include <GLFW/glfw3.h>  
#include <cmath>  
#include <iostream>  
#include <chrono>
#include <thread>

const float PLANE_SPEED = 0.3f;
const float COLLAPSE_SPEED = 0.3f;
const float BUILDING_WIDTH = 0.15f;
const float BUILDING_HEIGHT = 1.0f;
const float BUILDING_MIN_HEIGHT = 0.1f;
const float CLOUD_SIZE = 0.2f;
const float TREE_WIDTH = 0.05f;
const float TREE_HEIGHT = 0.15f;
const float RESET_DELAY = 3.0f;  
struct Plane {
    float x;
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

Plane plane1, plane2;
Building building1, building2;
Cloud cloud1, cloud2;
Tree tree1, tree2;

void initializeScene() {
    plane1 = { -1.2f, false };
    plane2 = { -1.2f, false };
    building1 = { 0.0f, BUILDING_HEIGHT, BUILDING_HEIGHT, false };
    building2 = { 0.5f, BUILDING_HEIGHT, BUILDING_HEIGHT, false };
    cloud1 = { -0.5f, 0.8f };
    cloud2 = { 0.2f, 0.6f };
    tree1 = { -0.8f, -0.7f };
    tree2 = { 0.4f, -0.7f };
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

    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);  
    glVertex2f(0.0f, 0.2f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.0f, -0.2f);

    glColor3f(0.6f, 0.6f, 0.6f);
    glVertex2f(0.05f, 0.2f);
    glVertex2f(0.1f, 0.3f);
    glVertex2f(0.0f, 0.2f);

    glVertex2f(0.05f, -0.2f);
    glVertex2f(0.1f, -0.3f);
    glVertex2f(0.0f, -0.2f);

    glEnd();
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
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f); 
    glVertex2f(x, y);
    glVertex2f(x + CLOUD_SIZE, y);
    glVertex2f(x + CLOUD_SIZE, y + CLOUD_SIZE);
    glVertex2f(x, y + CLOUD_SIZE);
    glEnd();
}

void drawTree(float x, float y) {
    glColor3f(0.5f, 0.35f, 0.05f);
    drawRectangle(x + TREE_WIDTH / 4, y, TREE_WIDTH / 2, TREE_HEIGHT / 3);

    glColor3f(0.0f, 0.5f, 0.0f); // Dark green
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + TREE_HEIGHT / 3);
    glVertex2f(x + TREE_WIDTH / 2, y + TREE_HEIGHT);
    glVertex2f(x + TREE_WIDTH, y + TREE_HEIGHT / 3);
    glEnd();
}

// Function to draw an antenna (connected to building)
void drawAntenna(float x, float y, float buildingHeight) {
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f); // Gray  
    glVertex2f(x, y); // Start from the top of the building
    glVertex2f(x, y + 0.2f); // Extend 0.2 units above the building
    glEnd();
}

// Function to update the planes' position and trigger building collapse  
void updatePlanes(float deltaTime) {
    if (plane1.x + 0.2f < building1.x) {
        plane1.x += PLANE_SPEED * deltaTime;
    }
    else {
        plane1.crashed = true;
        building1.collapsed = true;
    }

    if (plane2.x + 0.2f < building2.x) {
        plane2.x += PLANE_SPEED * deltaTime;
    }
    else {
        plane2.crashed = true;
        building2.collapsed = true;
    }
}

// Function to collapse the buildings gradually  
void collapseBuildings(float deltaTime) {
    if (building1.collapsed && building1.height > BUILDING_MIN_HEIGHT) {
        building1.height -= COLLAPSE_SPEED * deltaTime;
    }

    if (building2.collapsed && building2.height > BUILDING_MIN_HEIGHT) {
        building2.height -= COLLAPSE_SPEED * deltaTime;
    }
}

// Function to check if the animation is complete
bool isAnimationComplete() {
    return (plane1.crashed && plane2.crashed &&
        building1.height <= BUILDING_MIN_HEIGHT &&
        building2.height <= BUILDING_MIN_HEIGHT);
}

// Function to draw the scene  
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background (sky and ground)  
    drawBackground();

    // Draw building 1 (collapses if hit by plane)  
    glColor3f(0.7f, 0.7f, 0.7f); // Uniform building color  
    drawRectangle(building1.x, -0.5f, BUILDING_WIDTH, building1.height);

    // Draw building 2 (collapses if hit by plane)  
    glColor3f(0.7f, 0.7f, 0.7f); // Uniform building color  
    drawRectangle(building2.x, -0.5f, BUILDING_WIDTH, building2.height);

    // Draw antenna on building 2 (connected to the building)
    drawAntenna(building2.x + BUILDING_WIDTH / 2, -0.5f + building2.height, building2.height);

    // Draw plane 1  
    if (!plane1.crashed) {
        drawPlane(plane1.x, 0.0f, 0.4f); // Plane 1 with a 3D scaling effect  
    }

    // Draw plane 2  
    if (!plane2.crashed) {
        drawPlane(plane2.x, -0.2f, 0.4f); // Plane 2 with a 3D scaling effect  
    }

    // Draw clouds  
    drawCloud(cloud1.x, cloud1.y);
    drawCloud(cloud2.x, cloud2.y);

    // Draw trees  
    drawTree(tree1.x, tree1.y);
    drawTree(tree2.x, tree2.y);

    glfwSwapBuffers(glfwGetCurrentContext());
}

int main() {
    // Initialize GLFW  
    if (!glfwInit()) {
        return -1;
    }

    // Create window  
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Repeating Plane Crash Educational Animation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set the clear color (sky blue)  
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

    // Set up the viewport to handle window resizing
    glViewport(0, 0, 1920, 1080);
    glfwSetWindowSizeCallback(window, [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        });

    float lastTime = glfwGetTime();
    float resetTimer = 0.0f;
    bool isResetting = false;

    initializeScene();

    // Main loop  
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time  
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (!isResetting) {
            // Update plane positions and check for crashes  
            updatePlanes(deltaTime);

            // Collapse buildings if planes crash into them  
            collapseBuildings(deltaTime);

            // Check if animation is complete
            if (isAnimationComplete()) {
                isResetting = true;
                resetTimer = 0.0f;
            }
        }
        else {
            // Count down the reset timer
            resetTimer += deltaTime;
            if (resetTimer >= RESET_DELAY) {
                // Reset the scene
                initializeScene();
                isResetting = false;
            }
        }

        // Render the scene  
        drawScene();

        // Poll events  
        glfwPollEvents();

        // Cap the frame rate (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    glfwTerminate();
    return 0;
}