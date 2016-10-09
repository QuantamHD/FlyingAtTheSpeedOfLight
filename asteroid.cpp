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

// Asteroid default constructor.
Asteroid::Asteroid()
{
   centerX = 0.0;
   centerY = 0.0;
   centerZ = 0.0;
   radius = 0.0; // Indicates no asteroid exists in the position.
   color[0] = 0;
   color[1] = 0;
   color[2] = 0;
}

// Asteroid constructor.
Asteroid::Asteroid(float x, float y, float z, float r, unsigned char colorR,
				   unsigned char colorG, unsigned char colorB)
{
   centerX = x;
   centerY = y;
   centerZ = z;
   radius = r;
   color[0] = colorR;
   color[1] = colorG;
   color[2] = colorB;
   colorModifier[0] = 0;
   colorModifier[1] = 0;
   colorModifier[2] = 0;
}

// Function to draw asteroid.
void Asteroid::draw()
{
   if (radius > 0.0) // If asteroid exists.
   {
      glPushMatrix();
      glTranslatef(centerX, centerY, centerZ);
      glColor3f((color[0] + colorModifier[0])/255.0, (color[1] + colorModifier[1])/255.0, (color[2] + colorModifier[2])/255.0);
      glutWireSphere(radius, (int)(radius) * 6, (int)radius * 6);
      glPopMatrix();
      std::cout << (color[0] + colorModifier[0])/255.0 << "\n";
   }
}

void Asteroid::changeColor(int amount){
    colorModifier[0] = amount;
    colorModifier[1] = amount;
    colorModifier[2] = amount;

    std::cout << (int)colorModifier[0] << "\n";
}

void Asteroid::printColor(){
    std::cout << color[0] << ", " << color[1] << ", " << color[2] <<  "\n";
}

