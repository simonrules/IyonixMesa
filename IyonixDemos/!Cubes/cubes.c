/* Modified from a demo found on http://nehe.gamedev.net
   Don't complain about the bad coding. My parts are all
   perfect. ;o)
 */

#include <stdio.h>
#include <stdlib.h>
#include "GL/iyonixmesa.h"
#include <GL/glu.h>
#include <stdlib.h>
#include "glaux.h"
#include <swis.h>
#include <kernel.h>

GLuint texture;
GLuint list;
_kernel_swi_regs r;

/* Image type - contains height, width, and data */
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // used to convert bgr to rgb color.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }

    /* Yes, I know we're not using a struct here. Sue the author. */

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }

    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    /* Does red/blue swapping for the texture to go from BMP
       to OpenGL ordering.
     */
    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }

    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures(GLvoid) {
    // Load Texture
    Image *image1;

    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("<Obey$Dir>.crate/bmp", image1)) {
	exit(1);
    }

    // Create Textures
    glGenTextures(1, &texture);

    // texture 2 (linear scaling)
    glBindTexture(GL_TEXTURE_2D, texture);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // scale linearly when image smalled than texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    free(image1);
};

/*  Initialize light source.
 */
void myinit(void)
{
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glFrontFace (GL_CCW);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable (GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(1.0f, 0.7f, 0.9f, 1.0f);
}

void displayCube(void)
{
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    glNormal3f( 0.0f, 0.0f,-1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

    glNormal3f( 0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

    glNormal3f( 0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

    glNormal3f( 1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void display(void)
{
    static float angle = 0.0f;
    int i, j, k;

    glBindTexture(GL_TEXTURE_2D, texture);

    for(i = -4;i <= 5;i += 3)
    {
       for(j = -5;j <= 4;j += 3)
       {
          for(k = -4;k <= 5;k += 3)
          {
             glPushMatrix();
             glTranslatef((float)i, (float)j, (float)k);
             displayCube();
             glPopMatrix();
          }
       }
    }
}

void myReshape(int w, int h)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glViewport(0, 0, w, h);

   gluPerspective(60, (float)w / (float)h, 1, 100);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   gluLookAt(0, 0, 15, 0, 0, 0, 0.0f, 1.0f, 0.0f);
}

int main(int argc, char** argv)
{
   int i;
   int *mode_block;
   int centre_x, centre_y;

   /* Get screen dimensions */
   r.r[0] = 1;
   _kernel_swi(OS_ScreenMode, &r, &r);
   mode_block = (int *)r.r[1];

   if(mode_block[3] != 5)
   {
      printf("Only 32bpp modes supported at the moment, sorry\n");
      exit(1);
   }

   centre_x = mode_block[1] / 2;
   centre_y = mode_block[2] / 2;

   IyonixMesaInitialise();

   _kernel_swi(OS_RemoveCursors, &r, &r);

   myinit();
   myReshape(mode_block[1], mode_block[2]);
   LoadGLTextures();

   /* Create display list and render cubes to the list */
   list = glGenLists(1);
   glNewList(list, GL_COMPILE);
   display();
   glEndList();

   /* Loop till a mouse button is pressed */
   do
   {
      _kernel_swi(OS_Mouse, &r, &r);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glPushMatrix();

      glRotatef((float)(r.r[0] - centre_x), 0.0, 1.0, 0.0);
      glRotatef((float)(r.r[1] - centre_y), 1.0, 0.0, 0.0);

      /* Render the display list */
      glCallList(list);

      glPopMatrix();

      IyonixMesaSwapBuffers();
   }
   while(r.r[2] == 0);

   IyonixMesaCloseDown();

   return 0;
}
