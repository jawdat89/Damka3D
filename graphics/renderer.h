/* ========================================================================== */
/*                                                                            */
/*   renderer.h                                                               */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Environment class                                                        */
/*   Table class                                                              */
/*                                                                            */
/* ========================================================================== */
#pragma once
#include "GL/GLUT.H" // OpenGL library used for calling openGL functions
#include <cstdio> // using native c functions with c++ coding ( header includes stdio.h )
#include <malloc.h> // for allocating fixed memory ( used to create picture tixture )

/* Enviornment is drawing a room in openGL world
* handling and drawing a picture on the front wall
*/
class Environment
{
private:
    GLfloat length, height, width; // for drawing walls, floor, and the ceiling of the room
                                   /* loading picture variables */
    const char* path; // string for storing picture path
    FILE* file; // variable for loading a file
    unsigned char* picture_src; // used for allocating memory for the texture image
public:
    Environment(const GLfloat& length, const GLfloat& height, const GLfloat& width); // constructor takes walls size, and loading an image file
    ~Environment(); // destructor used for freeing alocated memory when the class is out of scope

    void draw(); // this method used for drawing environment
private:
    /* Environment drawing methods */
    /* room */
    void floor(const GLfloat& length, const GLfloat& height, const GLfloat& width) const; // drawing floor
    void room(const GLfloat& length, const GLfloat& height, const GLfloat& width) const; // drawing walls and ceiling
                                                                                         /* lamps */
    void lamp() const; // drawing lamp
    void draw_lamps() const; // drawing lamps in the environment  
                             /* picture */
    void picture(const GLfloat& length, const GLfloat& height, const GLfloat& width, unsigned char* picture) const;
    // drawing the loaded texture image
    void picture_border(const GLfloat& length, const GLfloat& height, const GLfloat& width) const;
    // drawing borders for the drawn image
};

/* Class Table is drawing a table in openGL world */
class Table
{
private:
    GLfloat m_Length, m_Width; // x, z
    GLfloat m_Height; // y
    GLfloat m_Size; // ratio
public:
    Table(const GLfloat& length, const GLfloat& width, const GLfloat& height, const GLfloat& size);
    // constructor takes table components and initiating table
    void draw(); // this method used for drawing the table
private:
    void leg(const GLfloat& height, const  GLfloat& size) const; // drawing table legs 
    void serfice(const GLfloat& length, const GLfloat& width, const GLfloat& height, const GLfloat& size) const; // drawing table serfice
};
