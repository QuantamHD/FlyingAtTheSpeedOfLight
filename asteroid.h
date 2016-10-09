#ifndef ASTEROID_H_INCLUDED
#define ASTEROID_H_INCLUDED

class Asteroid
{
public:
   Asteroid();
   Asteroid(float x, float y, float z, float r, unsigned char colorR,
		    unsigned char colorG, unsigned char colorB);
   float getCenterX() { return centerX; }
   float getCenterY() { return centerY; }
   float getCenterZ() { return centerZ; }
   float getRadius()  { return radius; }
   void draw();

private:
   float centerX, centerY, centerZ, radius;
   unsigned char color[3];
};

#endif // ASTEROID_H_INCLUDED
