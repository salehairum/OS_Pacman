#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <unordered_set>
using namespace std;

//semaphores and mutexes
sem_t startThread, s_lives, s_scoresN,s_scoresP, s_eaten,startGhost[4], key, permit, s_ghostSpeedBoost;
//s_scoresN: normal pellet, s_scoresN: power pellet
pthread_mutex_t m_game=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_ghost=PTHREAD_MUTEX_INITIALIZER;
pthread_t game, ui, ghost[4];
bool endGame=false;
//window and grid related variables
const int width = 540;
const int height = 600;

const int rows = 30;
const int cols = 27;

const float cellWidth = (float)width / cols;
const float cellHeight = (float)height / rows;

GLfloat xwcMin = 0.0 + cellWidth, xwcMax = 800.0 - cellWidth;
GLfloat ywcMin = 0.0 + cellHeight, ywcMax = 600.0 - cellHeight;

//pacman related variables
int xPos = 20;
int yPos = 480; 
int direction = 0; 

//ghost related variables
int ghostXpos[4]={270, 290, 310, 330};
int ghostYpos[4]={280, 280, 280, 280};
int ghostDirection[4];
int ghostPrevDirection[4]={-1,-1,-1,-1};
bool insideHouse[4]={true,true,true,true};
char mode='h'; //h: inside house, s: scattered, c: chase, f: frightened
int xOutside=260;
int yOutside=320;
int ghostSpeed[4];
bool isGhostBoosted[4]={false, false,false,false};
int spBoostTimer=0;
bool speedBoost=false;
int modeTimer=0;
int xTile[4]={20, 520, 20, 520};
int yTile[4]={480, 480, 100, 100};

//pellet related variables
int collideWithPellet=0;	
//0 means not collided, 1 means collided with normal pellet, 2 means collided with power pellet
bool regenerate=0;
int regenerateTimer=0;

//other variables
int tick = 0;
const int REFRESH_MS = 5;
int score=0;
int lives=3;

//for passing command line arguments to game engine thread
struct args
{
	int argc;
	char** argv;
}; 

/* TEXT TO DISPLAY ARRAY */
char arraytext[27]={'S','C','O','R','E',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','L','I','V','E','S',':',' ',' ',' ',' '};

/* MAZE ARRAY*/
int maze[rows][cols] = {
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
    {0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,0,1,1,1,1,2,1,1,1,1,0,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,2,2,1,0,1,2,2,2,1,0,1,0,1,2,2,2,1,0,1,2,2,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

/* PALLETS ARRAY*/
int pallets[rows][cols] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0},
    {0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0},
    {0,2,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,2,0},
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
    {0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

/* --------------------DRAWING MAZE--------------------------- */
void drawMaze() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j]==1) {
                glColor3f(0.0, 0.0, 1.0); // blue walls
            } else {
                glColor3f(0.0, 0.0, 0.0); // black space
            }

            glBegin(GL_QUADS);
            glVertex2f(j * cellWidth, i * cellHeight);
            glVertex2f((j + 1) * cellWidth, i * cellHeight);
            glVertex2f((j + 1) * cellWidth, (i + 1) * cellHeight);
            glVertex2f(j * cellWidth, (i + 1) * cellHeight);
            glEnd();
        }
    }
	
	string scores_str=to_string(score);
	int j=6;
	for(int i=0;scores_str[i]!='\0';i++)
	{
		arraytext[j]=scores_str[i];
		j++;
	}
	
	string lives_str=to_string(lives);
	arraytext[24]=lives_str[0];
	// Draw Score
    glColor3f(1.0, 1.0, 1.0); // white text
    glRasterPos2f(20, 550); // position to draw text
    for (int i = 0; i < 27 && arraytext[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, arraytext[i]); // adjust font size if needed
    }
    
    glFlush();
}



/* --------------------DRAWING PALLETS--------------------------- */
void drawPallets() {
    // Calculate the radius of the dot

    float radius = min(cellWidth, cellHeight) * 0.1; // Adjust the scale as needed
    int numSegments = 20;
    for (int i = 0; i < rows-3; i++) {
        for (int j = 0; j < cols; j++) {
            // Calculate the center coordinates of the cell
            float cx = j * cellWidth + cellWidth / 2.0;
            float cy = i * cellHeight + cellHeight / 2.0;
            if (pallets[i][j]==1) {
                glColor3f(1.0, 1.0, 0.0); // yellow dots
                glBegin(GL_POLYGON);
		        for (int k = 0; k < numSegments; k++) {
		            float theta = 2.0f * 3.1415926f * float(k) / float(numSegments); // Get the current angle
		            float x = radius * cosf(theta); // Calculate the x component
		            float y = radius * sinf(theta); // Calculate the y component
		            glVertex2f(x + cx, y + cy); // Output vertex
		        }
		    }
		    else if(pallets[i][j]==2)
		    {
				glColor3f(1.0, 0.0, 0.0); // red dots
				glBegin(GL_POLYGON);
				for (int k = 0; k < numSegments; k++) {
					float theta = 2.0f * 3.1415926f * float(k) / float(numSegments); // Get the current angle
					float x = radius * cosf(theta); // Calculate the x component
					float y = radius * sinf(theta); // Calculate the y component
					glVertex2f(x + cx, y + cy); // Output vertex
				}
		    }
		    glEnd();
        }
    }

    glFlush();
}

/* --------------------DRAWING GHOSTS--------------------------- */
void drawBoxGhost(int i) {
	if(mode!='f')
	{
		if(i==0)
			glColor3f(1.0, 0.0, 0.0);
		if(i==1)
			glColor3f(1.0, 0.72, 1.0);
		if(i==2)
			glColor3f(0.0, 1.0, 1.0);
		if(i==3)
			glColor3f(1.0, 0.72, 0.32);
	}
	else	
    	glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(ghostXpos[i], ghostYpos[i]);
    glVertex2f(ghostXpos[i] + 15, ghostYpos[i]);
    glVertex2f(ghostXpos[i] + 15, ghostYpos[i] + 15);
    glVertex2f(ghostXpos[i], ghostYpos[i] + 15);
    glEnd();
}

/* --------------------DRAWING PACMAN--------------------------- */
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

/* ------------------------------------ POWER PALLET FUNCTIONS -------------------------------------------*/

void generatePowerPellet()
{
	srand(time(0));
	bool added=false;
	while(!added)
	{
		int random_x = rand() % (22) + 4; // Generates a random number between 1 and 26 inclusive
		cout<<"x:"<<random_x<<endl;
		int random_y = rand() % (25) + 1; // Generates a random number between 4 and 25 inclusive
		cout<<"y:"<<random_y<<endl;
        if (maze[random_x][random_y] != 1 && maze[random_x][random_y] != 2) {
        	if(pallets[random_x][random_y]!=1 && pallets[random_x][random_y]!=2)
        	{
        		pallets[random_x][random_y]=2;
        		added=true;
        	}
        }
    }
}


void updateScoresPowerPallet()
{
	score+=5;
}

/* ------------------------------------ PALLETS FUNCTIONS -------------------------------------------*/

void checkPacManCollsionWithPallets()
{
	if (pallets[yPos/20][xPos/20]==1 || pallets[(yPos+14)/20][(xPos+14)/20]==1 || pallets[(yPos)/20][(xPos+14)/20]==1 || pallets[(yPos+14)/20][(xPos)/20]==1)
        {
           collideWithPellet=1;
        }
	else if (pallets[yPos/20][xPos/20]==2 || pallets[(yPos+14)/20][(xPos+14)/20]==2 || pallets[(yPos)/20][(xPos+14)/20]==2 || pallets[(yPos+14)/20][(xPos)/20]==2)
        {
           collideWithPellet=2;
        }
        else collideWithPellet=0;
}

void updateScores()
{
	score+=1;
}

void updatePalletsOnBoard()
{
	checkPacManCollsionWithPallets();
	if(collideWithPellet==1 && pallets[yPos/20][xPos/20]==1)
	{
			pallets[yPos/20][xPos/20]=0;
			sem_post(&s_scoresN);
			sleep(0.002);
	}
	else if(collideWithPellet==2 && pallets[yPos/20][xPos/20]==2)
	{
			mode='f';
			pallets[yPos/20][xPos/20]=0;
			regenerate=1;
			sem_post(&s_scoresP);
			//decrement speed of all ghosts
			for(int i=0;i<4;i++)
				ghostSpeed[i]--;
			//no need for sleep here, as we must immediately move to ghost for now
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

void directionInput(unsigned char key, int x, int y)
{
	switch(key){
	case 'w':
		direction = 3;
        break;
    case 's':
		direction = 1;
        break;
    case 'd':
		direction = 0;
        break;
    case 'a':
		direction = 2;
        break;
	};
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


void PacmanMainFunction()
{
	updatePos(direction);    // Update position
    updatePalletsOnBoard();
}

/* ------------------------------------ GHOST BOOST -----------------------------------------------*/
bool checkGhostIsFast(int i)
{
	//pacman left side and 4 cell difference
	if(ghostSpeed[i]>1)
	{isGhostBoosted[i]=true;
	return 1;}
	//pacman right side and 4 cell difference
	if(ghostSpeed[i]>1)
	{isGhostBoosted[i]=true;
	return 1;}
	//pacman down and 4 cell difference
	if(ghostSpeed[i]>1)
	{isGhostBoosted[i]=true;
	return 1;}
	//pacman up side and 4 cell difference
	if(ghostSpeed[i]>1)
	{isGhostBoosted[i]=true;
		return 1;}
	
	return 0;
}
void makeGhostFast(int i)
{
	ghostSpeed[i]=3;
}
void giveSpeedBoost(int i)
{
	int val;
	sem_getvalue(&s_ghostSpeedBoost, &val);
	cout<<"val "<<val<<endl;
	if(checkGhostIsFast(i)==1 && val>0)
	{
		sem_wait(&s_ghostSpeedBoost);      //decrement the s_ghostSpeedBoast
		makeGhostFast(i);
		cout<<"Ghost: "<<i<<" is fast"<<endl;
		isGhostBoosted[i]=false;
		sem_getvalue(&s_ghostSpeedBoost, &val);
		cout<<"val in if "<<val<<endl;
		sem_post(&s_ghostSpeedBoost);
	}
}

/* ------------------------------------ GHOST FUNCTIONS -------------------------------------------*/

void updatePosGhost(int i) {
	int direction=ghostDirection[i];
    switch (direction) {
    case 0:  //right
        if (maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)  //cuz window and maze have ultay coordinates of y
            ghostXpos[i] += ghostSpeed[i];
        break;
    case 1:   //down
        if (maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]-1)/20)][(ghostXpos[i]+14)/20]!=1)
            ghostYpos [i]-= ghostSpeed[i];
        break;
    case 2:   //left
        if (maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 && maze[((ghostYpos[i])/20)][(ghostXpos[i]-1)/20]!=1)
            ghostXpos [i]-= ghostSpeed[i];
        break;
    case 3:   //up
        if (maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1)
            ghostYpos[i] += ghostSpeed[i];
        break;
    };
}

void checkDirectionForChasingGhost(int i)
{
	if(xPos>ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)   //pacman is to the right of ghost
        ghostDirection[i]=0;  //goes right
	else if(xPos<ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 && maze[((ghostYpos)[i]/20)][(ghostXpos-1)[i]/20]!=1)  //pacman is to the left of ghost
		ghostDirection[i]=2;  //goes left
	else if(yPos>ghostYpos[i] && maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
		ghostDirection[i]=3;  //goes up
	else if(yPos<ghostYpos[i] && maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]-1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
		ghostDirection[i]=1;  //goes down
}

void decrementLives()
{
	lives-=1;    //decrement the lives
}

bool checkPacManCollsionWithGhost(int i)
{
	//Pacman left ghost right
	if ((yPos-ghostYpos[i]>=-5 && yPos-ghostYpos[i]<=5) && xPos+14==ghostXpos[i]) 
        {
            return 1;
        }
        //Pacman right ghost left
	if ((yPos-ghostYpos[i]>=-5 && yPos-ghostYpos[i]<=5) && xPos==ghostXpos[i]+14)
        {
            return 1;
        }
        //Pacman up ghost down
	if(yPos==ghostYpos[i]+14 && (xPos-ghostXpos[i]>=-5 && xPos-ghostXpos[i]<=5)) 
        {
            return 1;
        }
        //Pacman down ghost up
	if (yPos+14==ghostYpos[i] && (xPos-ghostXpos[i]>=-5 && xPos-ghostXpos[i]<=5))
        {
            return 1;
        }
        if(xPos==ghostXpos[i] && yPos==ghostYpos[i])
        	return 1;
        return 0;
}
void changePacmanPositionToInitial()
{
		xPos=20;   //update the position of pacman
		yPos=480;
}

void changeGhostPositionToInitial(int i)
{
		ghostXpos[i]=290;   //update the position of pacman
		ghostYpos[i]=280;
}
void pacManCollideWithGhost(int i)
{
	if(checkPacManCollsionWithGhost(i)==1)
	{
		if(mode!='f')
		{
			sem_post(&s_lives);
			changePacmanPositionToInitial();
			sleep(0.02);
		}
		else
		{
			//ghost must be sent back home
			sem_post(&s_eaten);
		}
	}
}

void checkDirectionsForGhost(bool d[4], int i)
{
	if(maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 &&maze[((ghostYpos[i])/20)][(ghostXpos[i]-1)/20]!=1) //left
		d[2]=true;
	if(maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1) //up
		d[3]=true;
	if(maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)  //right
		d[0]=true;
	if(maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 &&maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)  //down
		d[1]=true;
}

void ghostAlgorithm(int i, int x, int y)
{
	bool d[4]={false, false, false, false};
	checkDirectionsForGhost(d, i);
	//determine next move based on destination
	if(x>ghostXpos[i]) //right
	{
		if(d[0])
		{
			ghostDirection[i]=0;
			ghostPrevDirection[i]=ghostDirection[i];
			return;
		}
	}
	if(x<ghostXpos[i] )//left
	{
		if(d[2])
			{
				ghostDirection[i]=2;
				ghostPrevDirection[i]=ghostDirection[i];
				return;
			}
	}
	if(y>ghostYpos[i] )//up
	{
		if(d[3])
			{
				ghostDirection[i]=3;
				ghostPrevDirection[i]=ghostDirection[i];
				return;
			}
	}
	if(y<ghostYpos[i])  //down
	{
		if(d[1])
			{
				ghostDirection[i]=1;
				ghostPrevDirection[i]=ghostDirection[i];
				return;
			}
	}
	if(ghostPrevDirection[i]!=-1 && d[ghostPrevDirection[i]])
	{
		ghostDirection[i]=ghostPrevDirection[i];
		return;
	}
	//else, move in any direction available
	for(int j=0;j<4;j++)
	{
		if(d[j])
		{
			ghostDirection[i]=j;
			ghostPrevDirection[i]=ghostDirection[i];
			return;
		}
	}
}

void checkDirectionForScatteredGhost(int i)
{
	//first go to top left corner
	if(!(ghostXpos[i]<270 && ghostYpos[i]>300))
	{
		if(maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 && maze[((ghostYpos[i])/20)][(ghostXpos[i]-1)/20]!=1)  //pacman is to the left of ghost
			ghostDirection[i]=2;  //goes left
		else if(maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
			ghostDirection[i]=3;  //goes up
		else if(maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)   //pacman is to the right of ghost
		    ghostDirection[i]=0;  //goes right
		else if(maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]-1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
			ghostDirection[i]=1;  //goes down
		}
	//topLeft corner
	if(ghostXpos[i]<270 && ghostYpos[i]>300)
	{
		if(xPos>ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)   //pacman is to the right of ghost
		    ghostDirection[i]=0;  //goes right
		else if(xPos<ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 && maze[((ghostYpos[i])/20)][(ghostXpos[i]-1)/20]!=1)  //pacman is to the left of ghost
			ghostDirection[i]=2;  //goes left
		else if(yPos>ghostYpos[i] && maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
			ghostDirection[i]=3;  //goes up
		else if(yPos<ghostYpos[i] && maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]-1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
			ghostDirection[i]=1;  //goes down
		}
}

void GhostExitsHouse(int i)
{
		if(xOutside>ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][((ghostXpos[i]+14+1)/20)]!=1 && maze[((ghostYpos[i])/20)][((ghostXpos[i]+14+1)/20)]!=1)   //pacman is to the right of ghost
        ghostDirection[i]=0;  //goes right
	else if(xOutside<ghostXpos[i] && maze[((ghostYpos[i]+14)/20)][(ghostXpos[i]-1)/20]!=1 && maze[((ghostYpos)[i]/20)][(ghostXpos-1)[i]/20]!=1)  //pacman is to the left of ghost
		ghostDirection[i]=2;  //goes left
	else if(yOutside>ghostYpos[i] && maze[((ghostYpos[i]+14+1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]+14+1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
		ghostDirection[i]=3;  //goes up
	else if(yOutside<ghostYpos[i] && maze[((ghostYpos[i]-1)/20)][ghostXpos[i]/20]!=1 && maze[((ghostYpos[i]-1)/20)][(ghostXpos[i]+14)/20]!=1)  //pacman is up of ghost
		ghostDirection[i]=1;  //goes down
}

/* ------------------------------------ GHOST THREAD -------------------------------------------*/
void* ghostThread(void* arg)
{	
	int* i;
	i=(int *)arg;
	
	while(true)
	{
		sem_wait(&startGhost[*i]);
		pthread_mutex_lock(&m_ghost);
	    if(mode=='f')
	    {
	    	
	    	/* algo for frightened mode is called */
	    
	    	if(sem_trywait(&s_eaten)==0)
	    	{
	    		changeGhostPositionToInitial(*i);
	    		insideHouse[*i]=true;
	    		sleep(0.1);
	    	}
	    }
	    else if(mode=='h' || insideHouse[*i]==true)
	    {
	    	GhostExitsHouse(*i);
	    }
	    else if(mode=='c'&& insideHouse[*i]==false) 
	    {
	    	ghostAlgorithm((*i), xPos, yPos);
	    }
	    else if(mode=='s' && insideHouse[*i]==false) 
	    {
	    	ghostAlgorithm((*i), xTile[*i], yTile[*i]);
	    }
		pacManCollideWithGhost(*i);
		if(speedBoost==false && ghostSpeed[*i]>2)
		{
			ghostSpeed[*i]=2;
		}
		if(speedBoost && insideHouse[*i]==false)
		{
			checkGhostIsFast(*i);
			giveSpeedBoost(*i);
		}
	    if(ghostXpos[*i]==xOutside && ghostYpos[*i]==yOutside-20)
	    //about to leave house
	    {
	    	if((*i)<3)
	    		sem_post(&startGhost[(*i)+1]);
	    	sem_wait(&key);
	    	sem_wait(&permit);
	    	//move ghost upward so that it exits ghost house
	    	ghostYpos[*i] ++;
	    	cout<<*i<<" got key and permit! "<<endl;
	    	sleep(0.02);
	    	sem_post(&key);
	    	sem_post(&permit);
	    	insideHouse[*i]=false;
	    	if(mode!='f')
	    		mode='s';
	    }
	    updatePosGhost(*i);
	   pthread_mutex_unlock(&m_ghost);
	    if((*i)<3)
	    	sem_post(&startGhost[(*i)+1]);
    }
    
    pthread_exit(0);
}


void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for (int i = 0; i < text.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// Display function
void displayGameOver() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw menu options
    glColor3f(1.0, 1.0, 1.0); // White color
    drawText(200, 400, "GAME OVER!!!");
}

/* --------------------------------UI THREAD----------------------------------- */

void* uiEngineThread(void* arg)
{
	while(true)
	{
		if(sem_trywait(&s_lives)==0)
		{
			decrementLives();
			if(lives==0)
			endGame=true;
		}
		if(sem_trywait(&s_scoresN)==0)
		{
			updateScores();
		}
		if(sem_trywait(&s_scoresP)==0)
		{
			updateScoresPowerPallet();
		}
    }
    pthread_exit(0);
}


/* ------------------------------------ GLUT FUNCTIONS -------------------------------------------*/
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}
/* ---------------------------- GAME OVER ------------------------------ */

void display() {

	glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawMaze();
	drawPallets();
   	 int i=0;
	drawBoxGhost(i);
	 i++;
	 drawBoxGhost(i);
	 i++;
	 drawBoxGhost(i);
	 i++;
	 drawBoxGhost(i);
    glPushMatrix();
    glTranslatef(xPos+7.5, yPos+7.5, 0); // Translate Pac-Man to its position
    pacMan(direction); // Draw Pac-Man
    glPopMatrix();
    tick++;
    spBoostTimer++;
    modeTimer++;
    if(modeTimer==200)
    	mode='c';
    if(modeTimer==400)
    {
    	mode='s';
    	modeTimer=0;
    }
    if(spBoostTimer==100)
    {
    	speedBoost=false;	
    }
    if(spBoostTimer==700)
    {
    	spBoostTimer=0;
    	speedBoost=true;	
    }
    if(regenerate)
    	regenerateTimer++;
    if(regenerateTimer==500)
    	//end frightened mode
    {
    	regenerateTimer=0;
    	regenerate=0;
    	mode='s';
    	generatePowerPellet();
    	for(int i=0;i<4;i++)
    	{
    		ghostSpeed[i]++;
    	}
    }
    glFlush();
    glutSwapBuffers();
}

void timer(int value) {

    glutPostRedisplay();      // Post re-paint request to activate display()
    
    //allow pacman to perform write operations
    pthread_mutex_lock(&m_game);
	PacmanMainFunction();
    pthread_mutex_unlock(&m_game);
	
	pthread_mutex_lock(&m_game);
	sem_post(&startGhost[0]);  //allow the first ghost to move
	pthread_mutex_unlock(&m_game);
	if(endGame==false)
    glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
	else
	displayGameOver();
}

void init(void) {
    /* Set color of display window to white. */
    glClearColor(0, 0, 0, 0.0);
}

/* ------------------------------GAME ENGINE THREAD------------------------------------- */

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
    //glutSpecialFunc(arrowFunc);
    glutKeyboardFunc(directionInput);
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
	srand(time(0));
    sem_init(&startThread, 0, 0);
    sem_init(&s_lives, 0, 0);
    sem_init(&s_scoresN, 0, 0);
    sem_init(&s_scoresP, 0, 0);
    sem_init(&s_eaten, 0, 0);
    sem_init(&key, 0, 2);
    sem_init(&permit, 0, 2);	//assuming 2 keys and 2 permits
    sem_init(&s_ghostSpeedBoost,0,2);
    for(int i=0;i<4;i++)
    	sem_init(&startGhost[i], 0, 0);
 	arg.argc=argc;
	arg.argv=argv;
	int threadValues[]={0,1,2,3};
	int r1=rand()%4;
	int r2;
	do
	{
		r2=rand()%4;
	}while(r2!=r1);
	for(int i=0;i<4;i++)
	{
		if(i==r1 || i==r2)
			ghostSpeed[i]=2;
		else ghostSpeed[i]=1;
	}
	pthread_create(&ghost[threadValues[0]], NULL, ghostThread,&(threadValues[0]));
	pthread_create(&ghost[threadValues[1]], NULL, ghostThread,&(threadValues[1]));
	pthread_create(&ghost[threadValues[2]], NULL, ghostThread,&(threadValues[2]));
	pthread_create(&ghost[threadValues[3]], NULL, ghostThread,&(threadValues[3]));

    pthread_create(&ui, NULL, uiEngineThread, NULL);
    pthread_create(&game, NULL, gameEngineThread, &arg);
    
    pthread_exit(0);
}
