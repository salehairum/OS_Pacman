#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
using namespace std;

sem_t input, gameState,startThread;
pthread_t game, ui, ghost[4];

const int width = 540;
const int height = 600;

int xPos = 20;
int yPos = 480;  

const int rows = 30;
const int cols = 27;

const float cellWidth = (float)width / cols;
const float cellHeight = (float)height / rows;

GLfloat xwcMin = 0.0 + cellWidth, xwcMax = 800.0 - cellWidth;
GLfloat ywcMin = 0.0 + cellHeight, ywcMax = 600.0 - cellHeight;

int tick = 0;
int direction = 0;

const int REFRESH_MS = 5;

int score=0;
int lives=3;

struct args
{
	int argc;
	char** argv;
}; 

/* MAZE ARRAY*/
bool maze[rows][cols] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
/*
bool pallets[rows][cols] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};*/

/* PALLETS ARRAY*/
bool pallets[rows][cols] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0},
    {0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0},
    {0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
    {1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1},
    {0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

/* ------------------------------------ MAZE FUNCTIONS -------------------------------------------*/
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
            glVertex2f(j * cellWidth, i * cellHeight);
            glVertex2f((j + 1) * cellWidth, i * cellHeight);
            glVertex2f((j + 1) * cellWidth, (i + 1) * cellHeight);
            glVertex2f(j * cellWidth, (i + 1) * cellHeight);
            glEnd();
        }
    }

    glFlush();
}

/* ------------------------------------ PALLETS FUNCTIONS -------------------------------------------*/
void drawPallets() {
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT);

    // Calculate the radius of the dot
    float radius = min(cellWidth, cellHeight) * 0.1; // Adjust the scale as needed
    int numSegments = 20;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Calculate the center coordinates of the cell
            float cx = j * cellWidth + cellWidth / 2.0;
            float cy = i * cellHeight + cellHeight / 2.0;

            if (pallets[i][j]) {
                glColor3f(1.0, 1.0, 0.0); // yellow dots
                glBegin(GL_POLYGON);
		        for (int k = 0; k < numSegments; k++) {
		            float theta = 2.0f * 3.1415926f * float(k) / float(numSegments); // Get the current angle
		            float x = radius * cosf(theta); // Calculate the x component
		            float y = radius * sinf(theta); // Calculate the y component
		            glVertex2f(x + cx, y + cy); // Output vertex
		        }
		        glEnd();
           }
        }
    }

    glFlush();
}

bool checkPacManCollsionWithPallets()
{
	if (pallets[(yPos)/20][xPos/20]==1 || pallets[(yPos)/20][(xPos+14)/20]==1 || pallets[(yPos+14)/20][(xPos)/20]==1 || pallets[(yPos+14)/20][(xPos+14)/20]==1)
        {
            return 1;
        } 
        return 0;
}

void updateScores()
{
	score+=1;
}

void updatePalletsOnBoard()
{
	if(checkPacManCollsionWithPallets()==1)
		{
			pallets[yPos/20][xPos/20]=0;
			updateScores();
		}
}

/* ------------------------------------ PACMAN FUNCTIONS -------------------------------------------*/
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
	if(xPos==0 && (yPos>=280 || yPos<=285))
	{
		xPos=515;
		direction=0;
	}
	else if(xPos==525 && (yPos>=280 || yPos<=285))
	{
		xPos=5;
		direction=2;
	}
	else
	{
		switch (direction) {
		case 0:  //right
		    if (maze[(yPos+14)/20][((xPos+14+1)/20)]!=1 && maze[(yPos)/20][((xPos+14+1)/20)]!=1)  //cuz window and maze have ultay coordinates of y
		        {
		        xPos += SPEED;}
		    break;
		case 1:   //down
		    if (maze[(yPos-1)/20][xPos/20]!=1 && maze[(yPos-1)/20][(xPos+14)/20]!=1)
		    {
		        yPos -= SPEED;}
		    break;
		case 2:   //left
		    if (maze[(yPos+14)/20][(xPos-1)/20]!=1 && maze[(yPos)/20][(xPos-1)/20]!=1)
		    {
		        xPos -= SPEED;}
		    break;
		case 3:   //up
		    if (maze[(yPos+14+1)/20][xPos/20]!=1 && maze[(yPos+14+1)/20][(xPos+14)/20]!=1)
		    {
		        yPos += SPEED;}   
		    break;
		}; 
    }
}

void pacMan(int direction) {
    double r = 7.5;

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




/* ------------------------------------ GHOST FUNCTIONS -------------------------------------------*/
int ghostXpos=270;
int ghostYpos=280;
void drawBoxGhost() {
    glColor3f(1.0, 0.0, 1.0); // Yellow color for the box
    glBegin(GL_QUADS);
    glVertex2f(ghostXpos, ghostYpos);
    glVertex2f(ghostXpos + 15, ghostYpos);
    glVertex2f(ghostXpos + 15, ghostYpos + 15);
    glVertex2f(ghostXpos, ghostYpos + 15);
    glEnd();
}

void updatePosGhost(int direction) {
    const int SPEED = 1;
    switch (direction) {
    case 0:  //right
        if (maze[((ghostYpos+14)/20)][((ghostXpos+14+1)/20)]!=1 && maze[((ghostYpos)/20)][((ghostXpos+14+1)/20)]!=1)  //cuz window and maze have ultay coordinates of y
            ghostXpos += SPEED;
        break;
    case 1:   //down
        if (maze[((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos-1)/20)][(ghostXpos+14)/20]!=1)
            ghostYpos -= SPEED;
        break;
    case 2:   //left
        if (maze[((ghostYpos+14)/20)][(ghostXpos-1)/20]!=1 && maze[((ghostYpos)/20)][(ghostXpos-1)/20]!=1)
            ghostXpos -= SPEED;
        break;
    case 3:   //up
        if (maze[((ghostYpos+14+1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos+14+1)/20)][(ghostXpos+14)/20]!=1)
            ghostYpos += SPEED;
        break;
    };
}
int ghostDirection;
void checkDirectionForChasingGhost()
{
	if(xPos>ghostXpos && maze[((ghostYpos+14)/20)][((ghostXpos+14+1)/20)]!=1 && maze[((ghostYpos)/20)][((ghostXpos+14+1)/20)]!=1)   //pacman is to the right of ghost
        ghostDirection=0;  //goes right
	else if(xPos<ghostXpos && maze[((ghostYpos+14)/20)][(ghostXpos-1)/20]!=1 && maze[((ghostYpos)/20)][(ghostXpos-1)/20]!=1)  //pacman is to the left of ghost
		ghostDirection=2;  //goes left
	else if(yPos>ghostYpos && maze[((ghostYpos+14+1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos+14+1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
		ghostDirection=3;  //goes up
	else if(yPos<ghostYpos && maze[((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos-1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
		ghostDirection=1;  //goes down
}

void decrementLives()
{
	lives-=1;    //decrement the lives
}
bool checkPacManCollsionWithGhost()
{
	if (yPos==ghostYpos && xPos==ghostXpos)
        {
            return 1;
        }
        return 0;
}
void changePacmanPositionToInitial()
{
		xPos=20;   //update the position of pacman
		yPos=480;
}
void pacManCollideWithGhost()
{
	if(checkPacManCollsionWithGhost()==1)
	{
		decrementLives();
		changePacmanPositionToInitial();
	}
}


void checkDirectionForScattaredGhost()
{
	//first go to top left corner
	if(!(ghostXpos<270 && ghostYpos>300))
	{
		if(maze[((ghostYpos+14)/20)][(ghostXpos-1)/20]!=1 && maze[((ghostYpos)/20)][(ghostXpos-1)/20]!=1)  //pacman is to the left of ghost
			ghostDirection=2;  //goes left
		else if(maze[((ghostYpos+14+1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos+14+1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
			ghostDirection=3;  //goes up
		else if(maze[((ghostYpos+14)/20)][((ghostXpos+14+1)/20)]!=1 && maze[((ghostYpos)/20)][((ghostXpos+14+1)/20)]!=1)   //pacman is to the right of ghost
		    ghostDirection=0;  //goes right
		else if(maze[((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos-1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
			ghostDirection=1;  //goes down
		}
	//topLeft corner
	if(ghostXpos<270 && ghostYpos>300)
	{
		if(xPos>ghostXpos && maze[((ghostYpos+14)/20)][((ghostXpos+14+1)/20)]!=1 && maze[((ghostYpos)/20)][((ghostXpos+14+1)/20)]!=1)   //pacman is to the right of ghost
		    ghostDirection=0;  //goes right
		else if(xPos<ghostXpos && maze[((ghostYpos+14)/20)][(ghostXpos-1)/20]!=1 && maze[((ghostYpos)/20)][(ghostXpos-1)/20]!=1)  //pacman is to the left of ghost
			ghostDirection=2;  //goes left
		else if(yPos>ghostYpos && maze[((ghostYpos+14+1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos+14+1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
			ghostDirection=3;  //goes up
		else if(yPos<ghostYpos && maze[((ghostYpos-1)/20)][ghostXpos/20]!=1 && maze[((ghostYpos-1)/20)][(ghostXpos+14)/20]!=1)  //pacman is up of ghost
			ghostDirection=1;  //goes down
		}
}

void* ghostThread(void* arg)
{
	sem_wait(&startThread);
	sem_post(&startThread);
	while(true)
	{
		//cout<<"ghost: "<<endl;
    	//sem_post(&gameState);
    }
    pthread_exit(0);
}

void GhostMainFunction()
{
	checkDirectionForChasingGhost();
    checkDirectionForScatteredGhost();
    updatePosGhost(ghostDirection);
}
void PacmanMainFunction()
{
	updatePos(direction);    // Update position
    checkDirectionForChasingGhost();
    updatePalletsOnBoard();
	pacManCollideWithGhost();
}

/* ------------------------------------ WRITE FUNCTIONS -------------------------------------------*/

void writeText()
{
	 // Set the color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Given pixel coordinates
    int pixel_x = 0;
    int pixel_y = 530;

    // Given window size
    int width = 540;
    int height = 600;

    // Convert to normalized device coordinates (NDC)
    float ndc_x = (2.0f * pixel_x) / width - 1.0f;
    float ndc_y = 1.0f - (2.0f * pixel_y) / height;

    // Set raster position
    glRasterPos2f(ndc_x, ndc_y);

    // Convert the score to a string
    char scoreText[50];
    sprintf(scoreText, "Score: %d Lives: ", score);

    // Draw the score text using bitmap font
    for (int i = 0; scoreText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreText[i]);
    }

    // Set the color to red
    glColor3f(1.0f, 0.0f, 0.0f);

    // Print lives as hearts
    char heart = '❤︎'; // Unicode for heart symbol
    for (int i = 0; i < lives; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, heart);
    }

    /*// Print remaining lives as hearts
    char emptyHeart = ' ';
    for (int i = lives; i < maxLives; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, emptyHeart);
    }*/

    // Flush the OpenGL pipeline
    glFlush();
}

/* ------------------------------------ BAAKI FUNCTIONS -------------------------------------------*/
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void display() {

    //glLoadIdentity(); // Reset the transformation matrix
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawMaze();
	drawPallets();
    drawBoxGhost();
    glPushMatrix();
    glTranslatef(xPos+7.5, yPos+7.5, 0); // Translate Pac-Man to its position
    pacMan(direction); // Draw Pac-Man
    glPopMatrix();
    tick++;
    glFlush();
    glutSwapBuffers();
}

void timer(int value) {
    glutPostRedisplay();      // Post re-paint request to activate display()
    //updatePos(direction);    // Update position
    //checkDirectionForChasingGhost();
    //checkDirectionForScatteredGhost();
    //updatePosGhost(ghostDirection);
    //updatePalletsOnBoard();
	//drawPallets();
	//pacManCollideWithGhost();
	PacmanMainFunction();
	GhostMainFunction();
    glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
}

void init(void) {
    /* Set color of display window to white. */
    glClearColor(0, 0, 0, 0.0);
}

void* uiEngineThread(void* arg)
{
	sem_wait(&startThread);
	sem_post(&startThread);
	while(true)
	{
		//cout<<"ui engine!!: "<<endl;
    	//sem_post(&gameState);
    }
    pthread_exit(0);
}

void* gameEngineThread(void* arg)
{
	struct args* arguments;
	arguments=(args*)arg;
	int argc=arguments->argc;
	char** argv=arguments->argv;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pacman Maze");
    glutSpecialFunc(arrowFunc);
    glutDisplayFunc(display);
    init();
    glutReshapeFunc(reshape);  
    glutTimerFunc(0, timer, 0);
    sem_post(&startThread);
    glutMainLoop();
    pthread_exit(0);
}

/* ------------------------------------ MAIN FUNCTIONS -------------------------------------------*/
int main(int argc, char** argv) { 
	struct args arg;
    //sem_init(&input, 0, 0);
    sem_init(&startThread, 0, 0);
 	arg.argc=argc;
	arg.argv=argv;
	for(int i=0;i<4;i++)
	{
		pthread_create(&ghost[i], NULL, ghostThread, NULL);
	}
    pthread_create(&ui, NULL, uiEngineThread, NULL);
    pthread_create(&game, NULL, gameEngineThread, &arg);
    
    pthread_exit(0);
}
