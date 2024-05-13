#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <string.h>
using namespace std;

const int width = 800;
const int height = 600;

int xPos = 20;
int yPos = 560;    //570 is best due to pacman size

const int rows = 30;
const int cols = 40;
const float cellWidth = (float)width / cols;
const float cellHeight = (float)height / rows;

GLfloat xwcMin = 0.0 + cellWidth, xwcMax = 800.0 - cellWidth;
GLfloat ywcMin = 0.0 + cellHeight, ywcMax = 600.0 - cellHeight;

int tick = 0;
int direction = 0;
const int REFRESH_MS = 5;

bool maze[30][40] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
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
    const int SPEED = 1;
    cout<<"col: "<<xPos/20<<endl;
    cout<<"row: "<<29-(yPos/20)<<endl;
    switch (direction) {
    case 0:  //right
    	cout<<"right"<<endl;
        if (maze[29-((yPos+19)/20)][((xPos+19+1)/20)]!=1 && maze[29-((yPos)/20)][((xPos+19+1)/20)]!=1)  //cuz window and maze have ultay coordinates of y
            {cout<<"xwxMax: "<<xwcMax<<endl;
            cout<<"xPos/20: "<<xPos/20<<endl;
            xPos += SPEED;}
        break;
    case 1:   //down
    	cout<<"down"<<endl;
        if (maze[29-((yPos-1)/20)][xPos/20]!=1 && maze[29-((yPos-1)/20)][(xPos+19)/20]!=1)
        {cout<<"xwxMax: "<<xwcMax<<endl;
            cout<<"xPos/20: "<<xPos/20<<endl;
            yPos -= SPEED;}
        break;
    case 2:   //left
    	cout<<"left"<<endl;
        if (maze[29-((yPos+19)/20)][(xPos-1)/20]!=1 && maze[29-((yPos)/20)][(xPos-1)/20]!=1)
        {cout<<"xwxMax: "<<xwcMax<<endl;
            cout<<"xPos/20: "<<xPos/20<<endl;
            xPos -= SPEED;}
        break;
    case 3:   //up
    	cout<<"up"<<endl;
        if (maze[29-((yPos+19+1)/20)][xPos/20]!=1 && maze[29-((yPos+19+1)/20)][(xPos+19)/20]!=1)
        {cout<<"xwxMax: "<<xwcMax<<endl;
            cout<<"xPos/20: "<<xPos/20<<endl;
            yPos += SPEED;}   
        break;
    };
}
void pacMan(int direction) {
    int r = 10;

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

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}


/*
void displayFcn(void) {

    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(xPos, yPos, 0);

    pacMan(direction);
    updatePos(direction);

    glPopMatrix();
    glFlush();
    tick++;

}
*/
void drawBox() {
    glColor3f(1.0, 1.0, 0.0); // Yellow color for the box
    glBegin(GL_QUADS);
    glVertex2f(xPos, yPos);
    glVertex2f(xPos + 20, yPos);
    glVertex2f(xPos + 20, yPos + 20);
    glVertex2f(xPos, yPos + 20);
    glEnd();
}


int ghostXpos=400;
int ghostYpos=300;
void drawBoxGhost() {
    glColor3f(1.0, 0.0, 1.0); // Yellow color for the box
    glBegin(GL_QUADS);
    glVertex2f(ghostXpos, ghostYpos);
    glVertex2f(ghostXpos + 20, ghostYpos);
    glVertex2f(ghostXpos + 20, ghostYpos + 20);
    glVertex2f(ghostXpos, ghostYpos + 20);
    glEnd();
}

void updatePosGhost(int direction) {
    const int SPEED = 1;
    switch (direction) {
    case 0:  //right
    	cout<<"right"<<endl;
        if (maze[29-((ghostYpos+19)/20)][((ghostXpos+19+1)/20)]!=1 && maze[29-((ghostYpos)/20)][((ghostXpos+19+1)/20)]!=1)  //cuz window and maze have ultay coordinates of y
            ghostXpos += SPEED;
        break;
    case 1:   //down
    	cout<<"down"<<endl;
        if (maze[29-((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[29-((ghostYpos-1)/20)][(ghostXpos+19)/20]!=1)
            ghostYpos -= SPEED;
        break;
    case 2:   //left
    	cout<<"left"<<endl;
        if (maze[29-((ghostYpos+19)/20)][(ghostXpos-1)/20]!=1 && maze[29-((ghostYpos)/20)][(ghostXpos-1)/20]!=1)
            ghostXpos -= SPEED;
        break;
    case 3:   //up
    	cout<<"up"<<endl;
        if (maze[29-((ghostYpos+19+1)/20)][ghostXpos/20]!=1 && maze[29-((ghostYpos+19+1)/20)][(ghostXpos+19)/20]!=1)
            ghostYpos += SPEED;
        break;
    };
}
int ghostDirection;
void checkDirectionForGhost()
{
	cout<<"GhostXPos"<<ghostXpos<<endl;
	cout<<"GhostYPos"<<ghostYpos<<endl;
	if(xPos>ghostXpos && maze[29-((ghostYpos+19)/20)][((ghostXpos+19+1)/20)]!=1 && maze[29-((ghostYpos)/20)][((ghostXpos+19+1)/20)]!=1)   //pacman is to the right of ghost
        ghostDirection=0;  //goes right
	else if(xPos<ghostXpos && maze[29-((ghostYpos+19)/20)][(ghostXpos-1)/20]!=1 && maze[29-((ghostYpos)/20)][(ghostXpos-1)/20]!=1)  //pacman is to the left of ghost
		ghostDirection=2;  //goes left
	else if(yPos>ghostYpos && maze[29-((ghostYpos+19+1)/20)][ghostXpos/20]!=1 && maze[29-((ghostYpos+19+1)/20)][(ghostXpos+19)/20]!=1)  //pacman is up of ghost
		ghostDirection=3;  //goes up
	else if(yPos<ghostYpos && maze[29-((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[29-((ghostYpos-1)/20)][(ghostXpos+19)/20]!=1)  //pacman is up of ghost
		ghostDirection=1;  //goes down
}

void timer(int value) {
    glutPostRedisplay();      // Post re-paint request to activate display()
    updatePos(direction);    // Update position
    checkDirectionForGhost();
    updatePosGhost(ghostDirection);
    glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
}
void display() {

    /*glLoadIdentity(); // Reset the transformation matrix

    drawMaze();

    glPushMatrix();
    glTranslatef(xPos, yPos, 0); // Translate Pac-Man to its position
    pacMan(direction); // Draw Pac-Man
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
    
   */
   glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity(); // Reset the transformation matrix

    drawMaze();
	drawBox();
	drawBoxGhost();	
    glPushMatrix();
    glTranslatef(xPos, yPos, 0); // Translate Pac-Man to its position
    cout<<"Pac xPos: "<<xPos<<endl;
    cout<<"Pac yPos: "<<yPos<<endl;
    //pacMan(direction); // Draw Pac-Man
    glPopMatrix();

    glFlush();
    tick++;
    glutSwapBuffers();
}

void init(void) {
    /* Set color of display window to white. */
    glClearColor(0, 0, 0, 0.0);
}

int main(int argc, char** argv) {
    cout<<"Cell width: "<<cellWidth<<endl;
    cout<<"Cell height: "<<cellHeight<<endl;
    cout<<"xwxMax: "<<xwcMax<<endl;
    cout<<"ywxMax: "<<ywcMax<<endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pacman Maze");
    glutSpecialFunc(arrowFunc);
    glutDisplayFunc(display);
    init();
    glutReshapeFunc(reshape);  
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
