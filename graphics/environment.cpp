/* ========================================================================== */
/*                                                                            */
/*   environment.cpp                                                          */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Environment class                                                        */
/*   methods of Environment class                                             */
/*                                                                            */
/* ========================================================================== */

#include "renderer.h"

Environment::Environment(const GLfloat& length, const GLfloat& height, const GLfloat& width)
    :length(length), height(height), width(width) // initiate class walls
{
    /* when class is creating texture image data is created and needed memory is allocated */
    path = "src/graphics/pic/logo.bmp"; // image source path
    picture_src = (unsigned char*)malloc(256 * 256 * 3); // allocating memory for image
    file = fopen(path, "rb"); // load image file
    fread(picture_src, 256 * 256 * 3, 1, file); // read image file for allocated memory
}

Environment::~Environment()
{
    fclose(file); // close opened file
    free(picture_src); // free allocated memory
}

void Environment::draw()
{
    /* drawing environment parts */
    floor(length, height, width); // draw floor
    room(length, height, width); // draw walls and celling
    picture(length, height, width, picture_src); // drawing picture and the front wall
    picture_border(length / 3, height / 1.5, width - 1); // drawing borders for the picture
    draw_lamps(); // draw the lamps in the environment room
}

void Environment::floor(const GLfloat& length, const GLfloat& height, const GLfloat& width) const
{
    /* set floor color */
    glColor3f(0.33f, 0.33f, 0.32f);
    /* start drawing floor */
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-(length + height), 0.0f, -(width + height));
    glVertex3f(-(length + height), 0.0f, width);
    glVertex3f(length, 0.0f, width);
    glVertex3f(length, 0.0f, -(width + height));
    glEnd();
    /* end drawing floor */
}

void Environment::room(const GLfloat& length, const GLfloat& height, const GLfloat& width) const
{
    /* set walls color */
    glColor3f(0.65f, 0.6f, 0.44f);
    /* start drawing room */
    glBegin(GL_QUADS);
    /* start drawing walls */
    // Front	  		
    glNormal3f(0, 0, -1);
    glVertex3f(-length, 0, width);
    glVertex3f(-length, height, width);
    glVertex3f(length, height, width);
    glVertex3f(length, 0, width);
    // Back
    glNormal3f(0, 0, 1);
    glVertex3f(-length, 0, -width);
    glVertex3f(length, 0, -width);
    glVertex3f(length, height, -width);
    glVertex3f(-length, height, -width);
    // Left side
    glNormal3f(1, 0, 0);
    glVertex3f(-length, 0, width);
    glVertex3f(-length, height, width);
    glVertex3f(-length, height, -width);
    glVertex3f(-length, 0, -width);
    // Right side
    glNormal3f(-1, 0, 0);
    glVertex3f(length, 0, width);
    glVertex3f(length, 0, -width);
    glVertex3f(length, height, -width);
    glVertex3f(length, height, width);
    /* end drawing walls */
    /* set celling color */
    glColor3f(0.88f, 0.84f, 0.73f);
    /* start drawing celling */
    glNormal3f(0, -1, 0);
    glVertex3f(length, height, width);
    glVertex3f(length, height, -width);
    glVertex3f(-length, height, -width);
    glVertex3f(-length, height, width);
    /* end drawing celling */
    glEnd();
    /* end drawing room */
}

void Environment::picture(const GLfloat& length, const GLfloat& height, const GLfloat& width, unsigned char* picture) const
{
    /* start drawing textured image in openGL world*/
    glTranslatef((length / 3), (height / 1.5), -(width - 1)); // translate position to the front wall
    glRotatef(90, 1, 0, 0); // rotate image
                            /* drawing texture image (blank) */
    glBegin(GL_QUADS);
    glTexCoord3d(0.0, 0.0, 0.0); glVertex3d(-4.0, 0.0, -4.0);
    glTexCoord3d(1.0, 0.0, 0.0); glVertex3d(4.0, 0.0, -4.0);
    glTexCoord3d(1.0, 1.0, 1.0); glVertex3d(4.0, 0.0, 4.0);
    glTexCoord3d(0.0, 1.0, 1.0); glVertex3d(-4.0, 0.0, 4.0);
    glEnd();
    /* loading the picture from loaded file */
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_BGR_EXT, GL_UNSIGNED_BYTE, picture);
    glRotatef(-90, 1, 0, 0); // rotate back to origin
    glTranslatef(-(length / 3), -(height / 1.5), (width - 1)); // translate position to the origin point
                                                               /* end drawing textured image in openGL world*/
}

void Environment::picture_border(const GLfloat& length, const GLfloat& height, const GLfloat& width) const
{
    glTranslatef((length / 3), (height / 1.5), -(width - 1)); // translate position to the front wall
                                                              /* set picture border color */
    glColor3f(0.6f, 0.4f, 0.2f);
    /* start drawing picture borders */
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    // 1st border
    glVertex3f(4.3, 2.3, 0);
    glVertex3f(4.3, height - height / 2 + 0.5, 0);
    glVertex3f(4.3 + 1.5, height - height / 2 + 0.5, 0);
    glVertex3f(4.3 + 1.5, 2.3, 0);
    // 2nd border
    glVertex3f(4.3, height - height / 2 + 1.5, 0);
    glVertex3f(4.3, height - height / 2, 0);
    glVertex3f(12.6 + 1.5, height - height / 2, 0);
    glVertex3f(12.6 + 1.5, height - height / 2 + 1.5, 0);
    // 3rd border
    glVertex3f(12.6, 2.3, 0);
    glVertex3f(12.6, height - height / 2 + 0.5, 0);
    glVertex3f(12.6 + 1.5, height - height / 2 + 0.5, 0);
    glVertex3f(12.6 + 1.5, 2.3, 0);
    // 4th border
    glVertex3f(4.3, 2.3 + 1.5, 0);
    glVertex3f(4.3, 2.3, 0);
    glVertex3f(12.6 + 1.5, 2.3, 0);
    glVertex3f(12.6 + 1.5, 2.3 + 1.5, 0);
    glEnd();
    /* end drawing picture borders */
    glTranslatef(-(length / 3), -(height / 1.5), (width - 1)); // translate position to the origin point
}

void Environment::lamp() const
{
    /* start drawing a lamp */
    /* set lamp sides color */
    glColor3f(0.75f, 0.75f, 0.75f);
    /* start drawing lamp sides */
    glBegin(GL_TRIANGLES);
    // Front
    glVertex3f(0.0f, -2.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    // Right
    glVertex3f(0.0f, -2.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    // Back
    glVertex3f(0.0f, -2.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    // Left
    glVertex3f(0.0f, -2.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();
    // Top
    glBegin(GL_QUADS);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();
    /* end drawing lamp sides */
    /* start drawing lamp ball */
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, -1.5f, 0.0f);
    glutSolidSphere(0.5, 32, 32);
    glTranslatef(0.0f, 1.5f, 0.0f);
    /* end drawing lamp ball */
    /* end drawing a lamp */
}

void Environment::draw_lamps() const
{
    /* start drawing lamps in environment room */
    GLfloat xp = 0.0f, yp = 21.5f, zp = 0.0f; // lamps drawing position
                                              /* start drawing 3 lamps in the room */
    glTranslatef(xp, yp, (zp - 7.0f));
    lamp();
    glTranslatef(-xp, -yp, -(zp - 7.0f));
    glTranslatef((xp - 7.0f), yp, zp);
    lamp();
    glTranslatef(-(xp - 7.0f), -yp, -zp);
    glTranslatef((xp + 7.0f), yp, zp);
    lamp();
    glTranslatef(-(xp + 7.0f), -yp, -zp);
    /* end drawing 3 lamps in the room */
    /* end drawing lamps in environment room */
}