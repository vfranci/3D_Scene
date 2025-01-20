#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>  
#include <GL/freeglut.h>    //	Include functii pentru: 
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <SOIL.h>
#include <ctime>
#include <iostream>
// Variabile
bool isSnowing = false;
struct SmokeParticle {
    GLfloat x, y, z;
    GLfloat speedY;  // Viteza de mișcare pe axa Y
    GLfloat size;    // Dimensiunea particulei
};
// Unghiurile de rotație
GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
GLfloat zoom = -10.0f;  // Camera zoom
// Poziția inițială a saniei
GLfloat sleighX = -10.0f; 
// Variabile pentru texturi
GLuint brickTexture;
GLuint doorTexture;
GLuint windowTexture;
float sleighPositionX = -5.0; 

// Încărcare texturi
GLuint loadTexture(const char* filename) {
    GLuint textureID = SOIL_load_OGL_texture(
        filename,
        SOIL_LOAD_AUTO,        
        SOIL_CREATE_NEW_ID,    
        SOIL_FLAG_MIPMAPS      
    );

    if (textureID == 0) {
        std::cerr << "Nu s-a încărcat textura: " << filename << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;
}

// Funcții pentru desenarea diverselor forme
void drawSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(radius, 50, 50);
    glPopMatrix();
}

void drawCube(GLfloat x, GLfloat y, GLfloat z, GLfloat size) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidCube(size);
    glPopMatrix();
}

void drawCylinder(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat height) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Ensure the cylinder is vertical
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 50, 50);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawPyramid(GLfloat x, GLfloat y, GLfloat z, GLfloat baseSize, GLfloat height) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glBegin(GL_TRIANGLES);
    
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-baseSize / 2, 0.0f, baseSize / 2);
    glVertex3f(baseSize / 2, 0.0f, baseSize / 2);
    
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(baseSize / 2, 0.0f, baseSize / 2);
    glVertex3f(baseSize / 2, 0.0f, -baseSize / 2);

    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(baseSize / 2, 0.0f, -baseSize / 2);
    glVertex3f(-baseSize / 2, 0.0f, -baseSize / 2);

    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-baseSize / 2, 0.0f, -baseSize / 2);
    glVertex3f(-baseSize / 2, 0.0f, baseSize / 2);

    glEnd();
    glPopMatrix();
}

void drawEllipse(float cx, float cy, float cz, float rx, float ry) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.1415 / 180.0;
        float x = rx * cos(theta);
        float y = ry * sin(theta);
        glVertex3f(cx + x, cy + y, cz);
    }
    glEnd();
}

// Funcții pentru desenat formele pe care punem texturi
void drawTexturedCube(GLfloat x, GLfloat y, GLfloat z, GLfloat size) {
    GLfloat halfSize = size / 2.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);

    glEnd();
}

void drawTexturedDoor(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth) {
    GLfloat halfWidth = width / 2.0f;
    GLfloat halfHeight = height / 2.0f;
    GLfloat halfDepth = depth / 2.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 
    glEnd();
}

void drawTexturedCuboid(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth) {
    GLfloat halfWidth = width / 2.0f;
    GLfloat halfHeight = height / 2.0f;
    GLfloat halfDepth = depth / 2.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); 

    glEnd();
}

// Desenarea elementelor din scenă
void drawSmoke() {
    static const int numParticles = 20;
    static SmokeParticle smokeParticles[numParticles];
    static bool initialized = false;


    if (!initialized) {
        srand(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < numParticles; ++i) {
            smokeParticles[i].x = 0.8f;   // Poziția pe axa X (deasupra coșului de fum)
            smokeParticles[i].y = 1.8f;   // Poziția inițială pe axa Y (deasupra coșului de fum)
            smokeParticles[i].z = -2.2f;  // Poziția pe axa Z (deasupra coșului de fum)
            smokeParticles[i].speedY = (rand() % 100) / 5000.0f + 0.01f;  // Viteza particulelor (mai lent)
            smokeParticles[i].size = (rand() % 5 + 1) / 50.0f;  // Dimensiunea particulelor
        }
        initialized = true;
    }

    glColor3f(0.5f, 0.5f, 0.5f);  // Culoare gri 

    for (int i = 0; i < numParticles; ++i) {
        drawSphere(smokeParticles[i].x, smokeParticles[i].y, smokeParticles[i].z, smokeParticles[i].size);  // Desenează particula de fum
        smokeParticles[i].y += smokeParticles[i].speedY;
        // Dacă particula iese din cadru, o resetăm
        if (smokeParticles[i].y > 3.0f) {
            smokeParticles[i].y = 1.8f;
            smokeParticles[i].speedY = (rand() % 100) / 100000.0f + 0.01f;
        }
    }
}

void drawHouse(GLuint brickTexture, GLuint doorTexture, GLuint windowTexture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    drawTexturedCube(0.0f, -0.5f, -2.0f, 2.5f);
    glDisable(GL_TEXTURE_2D);
    // Acoperiș
    glColor3f(0.5f, 0.0f, 0.0f); 
    drawPyramid(0.0f, 0.75f, -2.0f, 2.5f, 2.f);
    // Ușă 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorTexture); 
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, -0.7f); 
    glScalef(0.5f, 1.0f, 0.1f);         
    glColor3f(1.f, 1.0f, 1.0f);
    drawTexturedDoor(0.0f, -0.6f, -0.5f, 1.0f, 1.0f, 0.1f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Ferestre
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, windowTexture); 
    glPushMatrix();
    glColor3f(1.f, 1.0f, 1.0f);
    drawTexturedCuboid(-0.8f, -0.3f, -0.77f, 0.6f, 0.9f, 0.1f); 
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, windowTexture); 
    glPushMatrix();
    glColor3f(1.f, 1.0f, 1.0f);
    drawTexturedCuboid(0.8f, -0.3f, -0.77f, 0.6f, 0.9f, 0.1f); 
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Horn
    glColor3f(0.5f, 0.2f, 0.2f); 
    glPushMatrix();
    glTranslatef(0.8f, 1.8f, -2.2f); 
    glScalef(0.3f, 1.0f, 0.3f);     
    glutSolidCube(1.0f);
    glPopMatrix();

    // Fumul din horn
    drawSmoke();  

    // Reset 
    glColor3f(1.0f, 1.0f, 1.0f);
}


void drawChristmasTree(GLfloat x, GLfloat y, GLfloat z) {
    // Trunchiul copacului
    glColor3f(0.4f, 0.2f, 0.1f); 
    drawCylinder(x, y, z, 0.2f, 1.0f);

    // Frunzișul copacului 
    glColor3f(0.0f, 0.8f, 0.0f); 
    glPushMatrix();
    glTranslatef(x, y + 0.8f, z); 
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
    glutSolidCone(1.5f, 1.7f, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y + 1.3f, z); 
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
    glutSolidCone(1.2f, 1.4f, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y + 1.8f, z); 
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
    glutSolidCone(0.8f, 1.f, 50, 50);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
}


void drawForest() {
    // Desenarea mai multor brazi într-o zonă specifică
    drawChristmasTree(-3.0f, -1.5f, -3.0f); // Brad principal
    drawChristmasTree(-4.0f, -1.5f, -3.5f); // Brad adiacent
    drawChristmasTree(-3.5f, -1.5f, -4.5f); // Alt brad adiacent
    drawChristmasTree(-4.5f, -1.5f, -2.5f); // Alt brad adiacent
}


void drawSnowman() {
    // Sferele pentru corp
    drawSphere(2.0f, -1.25f, 0.0f, 0.35f);  
    drawSphere(2.0f, -0.77f, 0.0f, 0.25f);   
    drawSphere(2.0f, -0.47f, 0.0f, 0.15f);  

    // Ochi
    glColor3f(0.0f, 0.0f, 0.0f);
    drawSphere(1.95f, -0.35f, 0.1f, 0.025f);
    drawSphere(2.05f, -0.35f, 0.1f, 0.025f);

    // Nas
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
    glTranslatef(2.0f, -0.45f, 0.1f);
    glutSolidCone(0.07, 0.15, 20, 20);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
}

void drawGround() {
    glColor3f(1.0f, 1.0f, 1.0f); // Alb pentru zăpadă
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 0.0f); 
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glEnd();
    glPopMatrix();
}

void drawPath() {
    glColor3f(0.5f, 0.5f, 0.5f); // Culoare gri pentru potecă
    glPushMatrix();
    glTranslatef(0.0f, -1.49f, -2.0f); // Poziția de pornire a potecii (ușa casei)
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, 0.0f, 0.0f); 
    glVertex3f(0.5f, 0.0f, 0.0f);   
    glVertex3f(0.5f, 0.0f, 5.0f);  
    glVertex3f(-0.5f, 0.0f, 5.0f);  
    glEnd();
    glPopMatrix();
    // Resetare 
    glColor3f(1.0f, 1.0f, 1.0f);
}

void drawLampPost(GLfloat x, GLfloat y, GLfloat z) {
    // Stâlp
    glColor3f(0.3f, 0.3f, 0.3f); // Gri
    drawCylinder(x, y, z, 0.1f, 2.0f); 

    // Bec
    glColor3f(1.0f, 1.0f, 0.0f); // Galben pentru lumină
    drawSphere(x, y + 2.0f, z, 0.2f); 

    // Efect de glow
    GLfloat lightPos[] = { x, y + 2.0f, z, 1.0f };
    GLfloat lightColor[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
}


void drawSnowflakes() {
    static const int numFlakes = 200;
    static GLfloat flakes[numFlakes][3];
    static bool initialized = false;

    if (!initialized) {
        srand(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < numFlakes; ++i) {
            flakes[i][0] = (rand() % 200 - 100) / 10.0f; // X
            flakes[i][1] = (rand() % 200) / 10.0f;       // Y
            flakes[i][2] = (rand() % 200 - 100) / 10.0f; // Z
        }
        initialized = true;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numFlakes; ++i) {
        drawSphere(flakes[i][0], flakes[i][1], flakes[i][2], 0.05f);
        if (isSnowing) {
            flakes[i][1] -= 0.02f; // Move down
            if (flakes[i][1] < -1.5f) {
                flakes[i][1] = 5.0f; // Resetează poziția fulgilor care au coborât sub pământ
            }
        }
    }
}

// Funcție pentru controlul ninsorii din mouse
void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isSnowing = true;
        }
        else if (state == GLUT_UP) {
            isSnowing = false;
        }
    }
    glutPostRedisplay();
}

void initLighting() {
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightPosition[] = { 1.0f, 4.0f, 2.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Lumină pentru lună cu culoare argintie
    GLfloat moonLightPosition[] = { 7.0f, 10.0f, -1.0f, 1.0f };  // Poziția lunii mai sus
    GLfloat moonLightColor[] = { 0.75f, 0.75f, 0.75f, 1.0f };  
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, moonLightPosition);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, moonLightColor);
    GLfloat moonMaterial[] = { 0.75f, 0.75f, 0.75f, 1.0f };  // Culoare argintie
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moonMaterial);
    glMaterialfv(GL_FRONT, GL_AMBIENT, moonMaterial);
    GLfloat moonSpecular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, moonSpecular);
    GLfloat moonShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, moonShininess);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


void drawDecorativeLights(GLfloat startX, GLfloat startY, GLfloat startZ, GLfloat endX, GLfloat endY, GLfloat endZ, int numLights) {
    glColor3f(1.0f, 0.0f, 0.0f); // Roșu pentru luminile decorative
    for (int i = 0; i < numLights; i++) {
        GLfloat t = (GLfloat)i / (numLights - 1);
        GLfloat x = startX + t * (endX - startX);
        GLfloat y = startY + t * (endY - startY);
        GLfloat z = startZ + t * (endZ - startZ);
        drawSphere(x, y, z, 0.05f);
    }
}

// Desenarea coarnelor renilor
void drawAntlers(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0.2, 0.3, 0);

    glVertex3f(0.2, 0.3, 0);
    glVertex3f(0.3, 0.4, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(-0.2, 0.3, 0);

    glVertex3f(-0.2, 0.3, 0);
    glVertex3f(-0.3, 0.4, 0);
    glEnd();

    glPopMatrix();
}

// Desenarea unui ren
void drawReindeer(float x, float y, float z) {
    // Corp
    glColor3f(0.6, 0.3, 0.1);
    drawEllipse(x, y, z, 0.4, 0.2);

    // Cap
    glColor3f(0.6, 0.3, 0.1);
    drawSphere(x + 0.5, y + 0.2, z, 0.15);

    // Picioare
    glColor3f(0.4, 0.2, 0);
    for (int i = -1; i <= 1; i += 2) {
        drawSphere(x + i * 0.2, y - 0.2, z + 0.1, 0.05);
        drawSphere(x + i * 0.2, y - 0.2, z - 0.1, 0.05);
    }

    // Nas
    glColor3f(1, 0, 0);
    drawSphere(x + 0.7, y + 0.2, z, 0.05);

    // Coarne
    glColor3f(0.4, 0.2, 0);
    drawAntlers(x + 0.5, y + 0.35, z, 0.3);
}

// Desenarea saniei
void drawSleigh(float x, float y, float z) {
    // Baza
    glColor3f(0.8, 0, 0);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(1.5, 0.5, 0.8);
    glutSolidCube(1);
    glPopMatrix();

    // Schiuri
    glColor3f(0.6, 0.3, 0.1);
    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glTranslatef(x, y - 0.35, z + i * 0.4);
        glScalef(1.5, 0.1, 0.1);
        glutSolidCube(1);
        glPopMatrix();
    }

    // Moș Crăciun
    glColor3f(1, 0, 0);
    drawSphere(x, y + 0.4, z, 0.2); 
    glColor3f(1, 0.8, 0.6);
    drawSphere(x, y + 0.6, z, 0.1); 

    // Căciula moșului
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(x, y + 0.65, z);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.15, 0.3, 20, 20);
    glPopMatrix();
    glColor3f(1, 1, 1);
    drawSphere(x, y + 1, z, 0.05); // Pom-pom
}

// Desenarea lacului
void drawLake() {
    glColor3f(0.7f, 0.8f, 1.0f); // Culoare mai deschisă pentru gheață
    glPushMatrix();
    glTranslatef(4.7f, -1.45f, -2.0f); 
    glBegin(GL_POLYGON);
    GLfloat radiusX = 3.0f; 
    GLfloat radiusZ = 1.f; 
    for (int i = 0; i <= 100; ++i) {
        GLfloat angle = 2.0f * 3.1415f * i / 100; 
        GLfloat x = radiusX * cos(angle);
        GLfloat z = radiusZ * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // Adăugăm o reflexie palidă sub lac pentru a sugera gheața
    glPushMatrix();
    glTranslatef(0.0f, -0.01f, 0.0f); // Depășim ușor pe axa Y pentru reflexie
    glColor4f(0.6f, 0.7f, 0.9f, 0.5f); // Reflexie transparentă
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 100; ++i) {
        GLfloat angle = 2.0f * 3.1415f * i / 100; 
        GLfloat x = radiusX * cos(angle);
        GLfloat z = radiusZ * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

// Desenarea lunii
void drawMoon() {
    glPushMatrix();
    glTranslatef(5.0f, 4.0f, -5.0f);  
    glColor3f(0.8f, 0.8f, 0.5f);      
    glutSolidSphere(0.5f, 50, 50);    
    glPopMatrix();
}

// Desenarea animalelor
void drawRabbit1() {
    // Corpul iepurelui 
    glColor3f(1.0f, 1.0f, 1.0f);  
    drawSphere(-3.0f, -1.2f, 0.0f, 0.08f);  

    // Capul 
    drawSphere(-3.0f, -1.1f, 0.0f, 0.05f);  

    // Urechile
    glColor3f(1.0f, 1.0f, 1.0f);  
    drawCylinder(-3.02f, -1.1f, 0.0f, 0.01f, 0.11f);  
    drawCylinder(-2.98f, -1.1f, 0.0f, 0.01f, 0.11f);  

    // Picioarele
    glColor3f(1.0f, 1.0f, 1.0f);  
    drawCylinder(-3.02f, -1.3f, 0.0f, 0.025f, 0.05f);  
    drawCylinder(-2.98f, -1.3f, 0.0f, 0.025f, 0.05f);  
}

void drawRabbit2() {
    glColor3f(1.0f, 1.0f, 1.0f); 
    drawSphere(-3.5f, -1.2f, 0.0f, 0.08f);  
    drawSphere(-3.5f, -1.1f, 0.0f, 0.05f); 
    glColor3f(1.0f, 1.0f, 1.0f);  
    drawCylinder(-3.52f, -1.1f, 0.0f, 0.01f, 0.11f);  
    drawCylinder(-3.48f, -1.1f, 0.0f, 0.01f, 0.11f);  
    glColor3f(1.0f, 1.0f, 1.0f);  
    drawCylinder(-3.52f, -1.3f, 0.0f, 0.025f, 0.05f);  
    drawCylinder(-3.48f, -1.3f, 0.0f, 0.025f, 0.05f);  
}

void drawDog() {
    glColor3f(0.8f, 0.4f, 0.0f);  
    drawSphere(1.0f, -1.3f, 1.0f, 0.15f);  
    drawSphere(1.0f, -1.07f, 1.0f, 0.1f);  
    glColor3f(0.8f, 0.4f, 0.0f);  
    drawCylinder(1.05f, -1.05f, 1.0f, 0.02f, 0.15f); 
    drawCylinder(0.95f, -1.05f, 1.0f, 0.02f, 0.15f);  
    glColor3f(0.8f, 0.4f, 0.0f);  
    drawCylinder(1.13f, -1.3f, 1.0f, 0.01f, 0.15f); 
    glColor3f(0.8f, 0.4f, 0.0f);  
    drawCylinder(1.05f, -1.5f, 1.06f, 0.03f, 0.1f);  
    drawCylinder(0.94f, -1.5f, 1.06f, 0.03f, 0.1f);  
    drawCylinder(1.05f, -1.5f, 0.94f, 0.03f, 0.1f);  
    drawCylinder(0.94f, -1.5f, 0.94f, 0.03f, 0.1f);  
}

void initMoonLighting() {
    // Lumină foarte slabă pentru lună
    GLfloat moonLightPos[] = { 7.0f, 5.0f, -10.0f, 1.0f };  
    GLfloat moonLightColor[] = { 0.2f, 0.2f, 0.5f, 1.0f };  

    glEnable(GL_LIGHT2);  // Activăm un alt canal de lumină (GL_LIGHT2)
    glLightfv(GL_LIGHT2, GL_POSITION, moonLightPos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, moonLightColor);
    glLightfv(GL_LIGHT2, GL_SPECULAR, moonLightColor);
}


void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // Fundal albastru închis
    glEnable(GL_TEXTURE_2D); 
    initLighting();
    isSnowing = true;  // Pornește zăpada automat
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Setăm poziția camerei
    gluLookAt(0.0f, 0.0f, 10.0f,  
        0.0f, 0.0f, 0.0f,   
        0.0f, 1.0f, 0.0f);  
    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0);    
    glEnable(GL_LIGHT1);    

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    initMoonLighting();  // Adăugăm lumina lunii
    glTranslatef(0.0f, 0.0f, zoom); // Mișcăm camera
    glRotatef(angleX, 1.0f, 0.0f, 0.0f); // Rotim scena
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glEnable(GL_LIGHTING); // Activăm iluminarea globală

    //Desenarea scenei
    drawGround();
    drawPath();
    drawLake(); 
    drawMoon();  
    drawLampPost(-1.2f, -1.5f, 0.0f); // Felinar stânga
    drawLampPost(1.2f, -1.5f, 0.0f); // Felinar dreapta
    GLuint brickTexture = loadTexture("brick_texture.jpg");
    GLuint doorTexture = loadTexture("door.jpg");
    GLuint windowTexture = loadTexture("window1.jpg");
    drawHouse(brickTexture, doorTexture, windowTexture); 
    drawSnowman();
    drawDecorativeLights(-1.2f, 0.8f, -0.75f, 1.2f, 0.8f, -0.75f, 10); // Luminile pe acoperiș
    drawForest();
    
    // Desenăm animalele
    drawRabbit1();  
    drawRabbit2();  
    drawDog();     
    drawSleigh(sleighPositionX, 3.5, -3);

    // Renii
    float spacing = 1.2;
    for (int i = 0; i < 3; ++i) {
        drawReindeer(sleighPositionX + spacing * (i + 1), 3.5, -2.5);
    }
    drawSnowflakes();

    glutSwapBuffers();
}

// Funcție pentru animația saniei
void timerFunc(int value) {
    sleighPositionX += 0.08;
    if (sleighPositionX > 10.0) {
        sleighPositionX = -10.0;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timerFunc, 0);
}

// Zoom din taste
void keyboardSpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        zoom += 1.0f;  
    }
    else if (key == GLUT_KEY_DOWN) {
        zoom -= 1.0f;  
    }
    else if (key == GLUT_KEY_LEFT) {
        angleY -= 5.0f;
    }
    else if (key == GLUT_KEY_RIGHT) {
        angleY += 5.0f;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Peisaj de iarna in 3D");
    initOpenGL();
    glutDisplayFunc(display);
    glutSpecialFunc(keyboardSpecial);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(16, timerFunc, 0);
    glutMainLoop();
    return 0;
}