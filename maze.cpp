#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

const int width = 800;
const int height = 600;

int xPos = 300;
int yPos = 300;

GLfloat xwcMin = 0.0, xwcMax = 800.0;
GLfloat ywcMin = 0.0, ywcMax = 600.0;

const int rows = 39;
const int cols = 32;
const float cellWidth = (float)width / cols;
const float cellHeight = (float)height / rows;

int tick = 0;
int direction = 0;
const int REFRESH_MS = 5;

bool maze[rows][cols] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1},
    {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
    {1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1},
    {1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1},
    {1,0,0,0,1,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
    {1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void arrowFunc(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        direction = 3;
        break;
    case GLUT_KEY_DOWN:
        direction = 1;
        break;
    case GLUT_KEY_LEFT:
        direction = 2;
        break;
    case GLUT_KEY_RIGHT:
        direction = 0;
        break;
    }
}

void updatePos(int direction) {
    const int SPEED = 3;
    switch (direction) {
    case 0:
        if (xPos < xwcMax + 2 )
            xPos += SPEED;
        break;
    case 1:
        if (yPos > -2 )
            yPos -= SPEED;
        break;
    case 2:
        if (xPos > -2 )
            xPos -= SPEED;
        break;
    case 3:
        if (yPos < ywcMax + 2)
            yPos += SPEED;
        break;
    };
}

void pacMan(int direction) {
    int r = 20;

    glPushMatrix();
    while (direction--) {
        glRotatef(90, 0, 0, -1);
    }
    glColor3f(1, 1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    int detail = 30;
    int startPoint = 15 * sin(tick / 15);
    for (int i = 0; i < detail; i++) {
        double deg = (i * (360 - 2 * startPoint) / detail) + startPoint;
        double x = r * cos(deg * 3.14 / 180);
        double y = r * sin(deg * 3.14 / 180);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}

void drawMaze() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

     for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j]) {
                glColor3f(0.0, 0.0, 1.0); // blue walls
            } else {
                glColor3f(0.0, 0.0, 0.0); // white space
            }

            glBegin(GL_QUADS);
            glVertex2f(j * cellWidth, (rows - i - 1) * cellHeight); // Rendering from top-left corner
            glVertex2f((j + 1) * cellWidth, (rows - i - 1) * cellHeight);
            glVertex2f((j + 1) * cellWidth, (rows - i) * cellHeight);
            glVertex2f(j * cellWidth, (rows - i) * cellHeight);
            glEnd();
        }
    }
    glFlush();
}

void display() {

    glLoadIdentity(); // Reset the transformation matrix

    drawMaze();

    glPushMatrix();
    glTranslatef(xPos, yPos, 0); // Translate Pac-Man to its position
    pacMan(direction); // Draw Pac-Man
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    glutPostRedisplay();      // Post re-paint request to activate display()
    glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pacman Maze");
    glutSpecialFunc(arrowFunc);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);  
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
