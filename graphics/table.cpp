/* ========================================================================== */
/*                                                                            */
/*   table.cpp                                                                */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Table class                                                              */
/*   methods of Table class                                                   */
/*                                                                            */
/* ========================================================================== */
#include "renderer.h"
#include "GL/GLUT.H" // OpenGL library used for calling openGL functions

Table::Table(const GLfloat& length, const GLfloat& width, const GLfloat& height, const GLfloat& size)
    : m_Length(length), m_Width(width), m_Height(height), m_Size(size)
{
    // table is initiated, private variables are loaded with given parameters
}

void Table::draw()
{
    /* start drawing table legs */
    // 1st leg
    glTranslatef(-m_Length, 0.0f, m_Width);
    leg(m_Height, m_Size);
    glTranslatef(m_Length, 0.0f, -m_Width);
    // 2nd leg
    glTranslatef(m_Length, 0.0f, m_Width);
    leg(m_Height, m_Size);
    glTranslatef(-m_Length, 0.0f, -m_Width);
    // 3rd leg
    glTranslatef(-m_Length, 0.0f, -m_Width);
    leg(m_Height, m_Size);
    glTranslatef(m_Length, 0.0f, m_Width);
    // 4th leg
    glTranslatef(m_Length, 0.0f, -m_Width);
    leg(m_Height, m_Size);
    glTranslatef(-m_Length, 0.0f, m_Width);
    /* end drawing table legs */
    /* drawing table serfice */
    serfice(m_Length, m_Width, m_Height, m_Size);
}

void Table::leg(const GLfloat& height, const GLfloat& size) const
{
    /* set leg color */
    glColor3f(0.54f, 0.37f, 0.11f);
    /* start drawing leg */
    glBegin(GL_QUADS);
    // Front
    glNormal3b(0, 0, 1);
    glVertex3f(-size, 0.0f, 0.0f);
    glVertex3f(-size, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    // Back
    glNormal3f(0, 0, -1);
    glVertex3f(-size, 0.0f, -size);
    glVertex3f(-size, height, -size);
    glVertex3f(0.0f, height, -size);
    glVertex3f(0.0f, 0.0f, -size);
    // Left
    glNormal3b(-1, 0, 0);
    glVertex3f(-size, 0.0f, -size);
    glVertex3f(-size, height, -size);
    glVertex3f(-size, height, 0.0f);
    glVertex3f(-size, 0.0f, 0.0f);
    // Right
    glNormal3b(1, 0, 0);
    glVertex3f(0.0f, 0.0f, -size);
    glVertex3f(0.0f, height, -size);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    // Top
    glNormal3b(0, 1, 0);
    glVertex3f(-size, height, -size);
    glVertex3f(-size, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(0.0f, height, -size);
    // Buttom
    glNormal3b(0, -1, 0);
    glVertex3f(-size, 0.0f, -size);
    glVertex3f(-size, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -size);

    glEnd();
    /* end drawing leg */
}

void Table::serfice(const GLfloat& length, const GLfloat& width, const GLfloat& height, const GLfloat& size) const
{
    /* set serfice color */
    glColor3f(0.32f, 0.44f, 0.59f);
    /* start drawing serfice */
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-(length + size), height, -(width + size));
    glVertex3f(-(length + size), height, width);
    glVertex3f(length, height, width);
    glVertex3f(length, height, -(width + size));
    glEnd();
    /* end drawing serfice */
}
