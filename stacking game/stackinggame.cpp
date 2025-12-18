#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

struct Block {
    float x, y, z;
    float width, height, depth;
    float speed;
    float r, g, b;
    int points;
};

vector<Block> blocks;

int windowWidth = 600, windowHeight = 800;
bool gameOver = false;
bool dropping = false;
int score = 0;

/* ---------- Brick Type Generator ---------- */
Block createBrick(float x, float y, float width, float speed) {
    Block b;
    b.x = x;
    b.y = y;
    b.z = 0;
    b.width = width;
    b.height = 30;
    b.depth = 20;
    b.speed = speed;

    int type = rand() % 3;
    if (type == 0) {          // Red
        b.r = 0.9f; b.g = 0.2f; b.b = 0.2f;
        b.points = 10;
    } else if (type == 1) {   // Green
        b.r = 0.2f; b.g = 0.9f; b.b = 0.3f;
        b.points = 25;
    } else {                  // Blue
        b.r = 0.2f; b.g = 0.4f; b.b = 1.0f;
        b.points = 50;
    }
    return b;
}

/* ---------- 3D Block Drawing ---------- */
void drawBlock3D(const Block &b) {
    float x = b.x, y = b.y, z = b.z;
    float w = b.width, h = b.height, d = b.depth;

    glBegin(GL_QUADS);

    // Front
    glColor3f(b.r, b.g, b.b);
    glVertex3f(x, y, z);
    glVertex3f(x + w, y, z);
    glVertex3f(x + w, y + h, z);
    glVertex3f(x, y + h, z);

    // Top (lighter)
    glColor3f(b.r + 0.1f, b.g + 0.1f, b.b + 0.1f);
    glVertex3f(x, y + h, z);
    glVertex3f(x + w, y + h, z);
    glVertex3f(x + w, y + h, z + d);
    glVertex3f(x, y + h, z + d);

    // Side (darker)
    glColor3f(b.r * 0.8f, b.g * 0.8f, b.b * 0.8f);
    glVertex3f(x + w, y, z);
    glVertex3f(x + w, y + h, z);
    glVertex3f(x + w, y + h, z + d);
    glVertex3f(x + w, y, z + d);

    glEnd();
}

/* ---------- Text ---------- */
void drawText(float x, float y, string text) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

/* ---------- Game Reset ---------- */
void resetGame() {
    blocks.clear();
    score = 0;
    gameOver = false;
    dropping = false;

    // Base block
    Block base;
    base.x = 200; base.y = 100; base.z = 0;
    base.width = 200; base.height = 30; base.depth = 20;
    base.speed = 0;
    base.r = 0.3f; base.g = 0.6f; base.b = 0.9f;
    base.points = 0;
    blocks.push_back(base);

    // First moving block
    blocks.push_back(createBrick(200, 700, 200, 0.02f));
}

/* ---------- Display ---------- */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(300, 400, 800, 300, 400, 0, 0, 1, 0);

    for (auto &b : blocks)
        drawBlock3D(b);

    // UI overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1,1,1);
    drawText(10, 770, "Score: " + to_string(score));

    if (gameOver) {
        glColor3f(1,0.2f,0.2f);
        drawText(220, 400, "GAME OVER");
        drawText(200, 360, "Press R to Restart");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

/* ---------- Update ---------- */
void update(int) {
    if (!gameOver && !dropping) {
        Block &top = blocks.back();
        float speedBoost = 0.02f + score * 0.0005f;
        top.x += top.speed * windowWidth * speedBoost;
        if (top.x < 0 || top.x + top.width > windowWidth)
            top.speed = -top.speed;
    }

    if (dropping && !gameOver) {
        Block &top = blocks.back();
        Block &below = blocks[blocks.size() - 2];
        top.y -= 6;

        if (top.y <= below.y + below.height) {
            float left = max(top.x, below.x);
            float right = min(top.x + top.width, below.x + below.width);
            float overlap = right - left;

            if (overlap > 0) {
                top.y = below.y + below.height;
                top.x = left;
                top.width = overlap;
                score += top.points;
                dropping = false;

                blocks.push_back(
                    createBrick(0, 700, overlap, 0.02f)
                );
            } else {
                gameOver = true;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

/* ---------- Input ---------- */
void keyPress(unsigned char key, int, int) {
    if (key == ' ') dropping = true;
    if (key == 'r' || key == 'R') resetGame();
}

/* ---------- Init ---------- */
void init() {
    srand(time(0));
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.12f, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)windowWidth/windowHeight, 1, 2000);
    glMatrixMode(GL_MODELVIEW);

    resetGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("3D Stacking Game");
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyPress);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
