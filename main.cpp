///////////////////////////////////////////////////////////////////////////////////
// spacetravel.cpp
//
// This program draws a conical spacecraft that can travel and an array of
// fixed spherical asteroids. The view in the left viewport is from a fixed
// camera; the view in the right viewport is from the spacecraft.
// There is approximate collision detection.
//
// User-defined constants:
// ROW is the number of rows of  asteroids.
// COLUMN is the number of columns of asteroids.
// FILL_PROBABILITY is the percentage probability that a particular row-column slot
// will be filled with an asteroid.
//
// Interaction:
// Press the left/right arrow keys to turn the craft.
// Press the up/down arrow keys to move the craft.
//
// Sumanta Guha.
///////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "asteroid.h"

#ifdef __APPLE__
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib")
#endif

#define PI 3.14159265

const int ROWS = 8;  // Number of rows of asteroids.
const int COLUMNS = 5; // Number of columns of asteroids.
const int FILL_PROBABILITY = 100; // Percentage probability that a particular row-column slot will be
                             // filled with an asteroid. It should be an integer between 0 and 100.

using namespace std;

// Globals.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static int width, height; // Size of the OpenGL window.
static float angle = 0.0; // Angle of the spacecraft.
static float xVal = 0, zVal = 0; // Co-ordinates of the spacecraft.
static bool isCollision = false; // Is there collision between the spacecraft and an asteroid?
static bool isGoldenCollision = false;
static unsigned int spacecraft; // Display lists base index.
static float goldenX = 30.0, goldenY = 0.0, goldenZ = -40.0;
static int goldenRow = 2, goldenCol = 3;

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
   char *c;

   for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

Asteroid arrayAsteroids[ROWS][COLUMNS]; // Global array of asteroids.

void makeBigGoldenAsteroid()
{
    int i = goldenRow;
    int j = goldenCol;
    arrayAsteroids[i][j] = Asteroid( goldenX, goldenY, goldenZ, 10.0,
			                                    239 , 239 , 23 );
}

void initAsteroids()
{
    int i, j;
    for (j = 0; j < COLUMNS; j++)
      for (i = 0; i < ROWS; i++)
	     if (rand() % 100 < FILL_PROBABILITY)
         // If rand()%100 >= FILL_PROBABILITY the default constructor asteroid remains in the slot
		 // which indicates that there is no asteroid there because the default's radius is 0.
		 {
			// Position the asteroids depending on if there is an even or odd number of columns
			// so that the spacecraft faces the middle of the asteroid field.
			if (COLUMNS % 2) // Odd number of columns.
	           arrayAsteroids[i][j] = Asteroid( 30.0 * (-COLUMNS / 2 + j), 0.0, -40.0 - 30.0 * i, 3.0,
			                                    rand() % 256, rand() % 256, rand() % 256);
			else // Even number of columns.
			   arrayAsteroids[i][j] = Asteroid( 15 + 30.0 * (-COLUMNS / 2 + j), 0.0, -40.0 - 30.0 * i, 3.0,
			                                    rand() % 256, rand() % 256, rand() % 256);
		 }


   makeBigGoldenAsteroid();
}

// Initialization routine.
void setup(void)
{

   spacecraft = glGenLists(1);
   glNewList(spacecraft, GL_COMPILE);
	  glPushMatrix();
	  glRotatef(180.0, 0.0, 1.0, 0.0); // To make the spacecraft point down the $z$-axis initially.
	  glColor3f (1.0, 1.0, 1.0);
      glutWireCone(5.0, 10.0, 10, 10);

      glRotatef(-90, 1, 0, 0);
      glutWireCylinder(0.3, 30.0, 10, 10);
	  glPopMatrix();
   glEndList();

   // Initialize global arrayAsteroids.
   initAsteroids();

   glEnable(GL_DEPTH_TEST);
   glClearColor (0.0, 0.0, 0.0, 0.0);
}

// Function to check if two spheres centered at (x1,y1,z1) and (x2,y2,z2) with
// radius r1 and r2 intersect.
int checkSpheresIntersection(float x1, float y1, float z1, float r1,
						     float x2, float y2, float z2, float r2)
{
   return ( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2) <= (r1 + r2) * (r1 + r2) );
}

// Function to check if the spacecraft collides with an asteroid when the center of the base
// of the craft is at (x, 0, z) and it is aligned at an angle a to to the -z direction.
// Collision detection is approximate as instead of the spacecraft we use a bounding sphere.
int asteroidCraftCollision(float x, float z, float a)
{
   int i,j;

   // Check for collision with each asteroid.
   for (j = 0; j < COLUMNS; j++)
      for (i = 0; i < ROWS; i++)
		 if (arrayAsteroids[i][j].getRadius() > 0 ) // If asteroid exists.
            if ( checkSpheresIntersection( x - 5 * sin( (PI / 180.0) * a), 0.0,
		         z - 5 * cos( (PI / 180.0) * a), 7.072,
		         arrayAsteroids[i][j].getCenterX(), arrayAsteroids[i][j].getCenterY(),
		         arrayAsteroids[i][j].getCenterZ(), arrayAsteroids[i][j].getRadius() ) )
		       return 1;
   return 0;
}

bool checkForGoldenCollision(float x, float z, float a)
{
  float centerX = arrayAsteroids[goldenRow][goldenCol].getCenterX();
  float centerY = arrayAsteroids[goldenRow][goldenCol].getCenterY();
  float centerZ = arrayAsteroids[goldenRow][goldenCol].getCenterZ();

  return checkSpheresIntersection(
     x - 5 * sin( (PI / 180.0) * a),
     0.0,
     z - 5 * cos( (PI / 180.0) * a),
     7.072,
     centerX,
     centerY,
	 centerZ,
     arrayAsteroids[goldenRow][goldenCol].getRadius() );
}

void goldenCameraViewPort()
{
    glViewport(width / 2.0, 0, width / 2.0, height);
    glLoadIdentity();

    glPushMatrix();
   glColor3f(1.0, 0.0, 0.0);
   glRasterPos3f(-28.0, 25.0, -30.0);
   if (isGoldenCollision) writeBitmapString((void*)font, "You have found Gold!");
   glPopMatrix();

    gluLookAt(goldenX, goldenY, goldenZ, xVal, 0.0, zVal, 0.0, 1.0, 0.0);

    // Draw all the asteroids in arrayAsteroids.
    for (int j = 0; j < COLUMNS; j++)
       for (int i = 0; i < ROWS; i++) {
         if (i != goldenRow && j != goldenCol)
            arrayAsteroids[i][j].draw();
       }

    glPushMatrix();
   glTranslatef(xVal, 0.0, zVal);
   glRotatef(angle, 0.0, 1.0, 0.0);
   glCallList(spacecraft);
   glPopMatrix();
}

void firstpersonViewPort()
{
   int i, j;
   glViewport (0, 0, width / 2.0,  height);
   glLoadIdentity();

   // Write text in isolated (i.e., before gluLookAt) translate block.
   glPushMatrix();
   glColor3f(1.0, 0.0, 0.0);
   glRasterPos3f(-28.0, 25.0, -30.0);
   if (isCollision) writeBitmapString((void*)font, "Cannot - will crash!");
   glPopMatrix();

   // Draw a vertical line on the left of the viewport to separate the two viewports
   glColor3f(1.0, 1.0, 1.0);
   glLineWidth(2.0);
   glBegin(GL_LINES);
      glVertex3f(5.0, -5.0, -5.0);
      glVertex3f(5.0, 5.0, -5.0);
   glEnd();
   glLineWidth(1.0);

   // Locate the camera at the tip of the cone and pointing in the direction of the cone.
   gluLookAt(xVal - 10 * sin( (PI / 180.0) * angle),
	         0.0,
			 zVal - 10 * cos( (PI / 180.0) * angle),
	         xVal - 11 * sin( (PI / 180.0) * angle),
			 0.0,
             zVal - 11 * cos( (PI / 180.0) * angle),
             0.0,
			 1.0,
			 0.0);

   // Draw all the asteroids in arrayAsteroids.
   for (j = 0; j < COLUMNS; j++)
      for (i = 0; i < ROWS; i++)
         arrayAsteroids[i][j].draw();
}

// Drawing routine.
void drawScene(void)
{
   int i, j;
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   firstpersonViewPort(); //left view port
   goldenCameraViewPort(); //Right view port

   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport (0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 250.0);
   glMatrixMode(GL_MODELVIEW);

   // Pass the size of the OpenGL window.
   width = w;
   height = h;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch (key)
   {
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
   float tempxVal = xVal, tempzVal = zVal, tempAngle = angle;

   // Compute next position.
   if (key == GLUT_KEY_LEFT) tempAngle = angle + 5.0;
   if (key == GLUT_KEY_RIGHT) tempAngle = angle - 5.0;
   if( key == GLUT_KEY_UP)
   {
	  tempxVal = xVal - sin(angle * PI / 180.0);
	  tempzVal = zVal - cos(angle * PI / 180.0);
   }
   if( key == GLUT_KEY_DOWN)
   {
	  tempxVal = xVal + sin(angle * PI / 180.0);
	  tempzVal = zVal + cos(angle * PI / 180.0);
   }

   // Angle correction.
   if (tempAngle > 360.0) tempAngle -= 360.0;
   if (tempAngle < 0.0) tempAngle += 360.0;

   // Move spacecraft to next position only if there will not be collision with an asteroid.
   if (!asteroidCraftCollision(tempxVal, tempzVal, tempAngle) )
   {
      isCollision = 0;
      xVal = tempxVal;
	  zVal = tempzVal;
	  angle = tempAngle;
   }
   else isCollision = true;


   if (!checkForGoldenCollision(tempxVal, tempzVal, tempAngle))
   {
       isGoldenCollision = false;
   }
   else {
       isGoldenCollision = true;
   }

   glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   cout << "Interaction:" << endl;
   cout << "Press the left/right arrow keys to turn the craft." << endl
        << "Press the up/down arrow keys to move the craft." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
   printInteraction();
   glutInit(&argc, argv);

   glutInitContextVersion(4, 3);
   glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(800, 400);
   glutInitWindowPosition(100, 100);
   glutCreateWindow("Space Travel");
   glutDisplayFunc(drawScene);
   glutReshapeFunc(resize);
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup();

   glutMainLoop();
}
