#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>  
#include <GL/freeglut.h>    //	Include functii pentru: 
//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <SOIL.h>
#include <ctime>
#include <iostream>

bool isSnowing = false;
struct SmokeParticle {
    GLfloat x, y, z;
    GLfloat speedY;  // Viteza de mișcare pe axa Y
    GLfloat size;    // Dimensiunea particulei
};


// Angles for rotation
GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
GLfloat zoom = -10.0f;  // Camera zoom

GLuint loadTexture(const char* filename) {
    GLuint textureID = SOIL_load_OGL_texture(
        filename,
        SOIL_LOAD_AUTO,        // Automatically determine image format
        SOIL_CREATE_NEW_ID,    // Create a new texture ID
        SOIL_FLAG_MIPMAPS      // Generate mipmaps
    );

    if (textureID == 0) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }

    // Set texture parameters
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;
}



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

void drawPyramid(GLfloat x, GLfloat y, GLfloat z, GLfloat baseSize, GLfloat height) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glBegin(GL_TRIANGLES);

    // Front face
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-baseSize / 2, 0.0f, baseSize / 2);
    glVertex3f(baseSize / 2, 0.0f, baseSize / 2);

    // Right face
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(baseSize / 2, 0.0f, baseSize / 2);
    glVertex3f(baseSize / 2, 0.0f, -baseSize / 2);

    // Back face
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(baseSize / 2, 0.0f, -baseSize / 2);
    glVertex3f(-baseSize / 2, 0.0f, -baseSize / 2);

    // Left face
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-baseSize / 2, 0.0f, -baseSize / 2);
    glVertex3f(-baseSize / 2, 0.0f, baseSize / 2);

    glEnd();
    glPopMatrix();
}

void drawSmoke() {
    static const int numParticles = 20;
    static SmokeParticle smokeParticles[numParticles];
    static bool initialized = false;  // Variabila "initialized" trebuie declarată ca static pentru a fi accesibilă în fiecare apel al funcției

    // Dacă nu a fost inițializat încă, se face inițializarea
    if (!initialized) {
        srand(static_cast<unsigned int>(time(0)));  // Inițializare aleatorie pe baza timpului curent
        // Inițializare particule
        for (int i = 0; i < numParticles; ++i) {
            smokeParticles[i].x = 0.8f;   // Poziția pe axa X (deasupra coșului de fum)
            smokeParticles[i].y = 1.8f;   // Poziția inițială pe axa Y (deasupra coșului de fum)
            smokeParticles[i].z = -2.2f;  // Poziția pe axa Z (deasupra coșului de fum)
            smokeParticles[i].speedY = (rand() % 100) / 5000.0f + 0.01f;  // Viteza particulelor (mai lent)
            smokeParticles[i].size = (rand() % 5 + 1) / 50.0f;  // Dimensiunea particulelor
        }
        initialized = true;  // După inițializare, setăm "initialized" pe true
    }

    glColor3f(0.5f, 0.5f, 0.5f);  // Culoare gri pentru fumul

    // Desenăm și actualizăm poziția fiecărei particule
    for (int i = 0; i < numParticles; ++i) {
        drawSphere(smokeParticles[i].x, smokeParticles[i].y, smokeParticles[i].z, smokeParticles[i].size);  // Desenează particula

        smokeParticles[i].y += smokeParticles[i].speedY;  // Actualizează poziția particulelor (se mișcă în sus)

        // Dacă particula iese din cadru, o resetăm
        if (smokeParticles[i].y > 3.0f) {
            smokeParticles[i].y = 1.8f;  // Resetăm poziția Y la valoarea inițială
            smokeParticles[i].speedY = (rand() % 100) / 100000.0f + 0.01f;  // Oferim o viteză mai lentă pentru revenire
        }
    }
}
void drawTexturedCube(GLfloat x, GLfloat y, GLfloat z, GLfloat size) {
    GLfloat halfSize = size / 2.0f;

    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);

    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);

    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);

    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfSize, y + halfSize, z - halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfSize, y + halfSize, z + halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfSize, y + halfSize, z + halfSize);

    // Bottom face
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

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-left

    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Top-left

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-front
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Bottom-back
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-back
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Top-front

    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-front
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Bottom-back
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-back
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Top-front

    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-left

    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Top-left

    glEnd();
}

void drawTexturedCuboid(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth) {
    GLfloat halfWidth = width / 2.0f;
    GLfloat halfHeight = height / 2.0f;
    GLfloat halfDepth = depth / 2.0f;

    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-left

    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Top-left

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-front
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Bottom-back
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-back
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Top-front

    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-front
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Bottom-back
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-back
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Top-front

    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y + halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y + halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y + halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y + halfHeight, z + halfDepth); // Top-left

    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - halfWidth, y - halfHeight, z - halfDepth); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + halfWidth, y - halfHeight, z - halfDepth); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + halfWidth, y - halfHeight, z + halfDepth); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - halfWidth, y - halfHeight, z + halfDepth); // Top-left

    glEnd();
}

void drawHouse(GLuint brickTexture, GLuint doorTexture, GLuint windowTexture) {
    // Base of the house
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    drawTexturedCube(0.0f, -0.5f, -2.0f, 2.5f);

    glDisable(GL_TEXTURE_2D);

    // Roof of the house
    glColor3f(0.5f, 0.0f, 0.0f); // Red
    drawPyramid(0.0f, 0.75f, -2.0f, 2.5f, 2.f);

    // Door (door texture)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorTexture); // Door texture
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, -0.7f); // Position of the door
    glScalef(0.5f, 1.0f, 0.1f);         // Dimensions of the door
    glColor3f(1.f, 1.0f, 1.0f); 
    drawTexturedDoor(0.0f, -0.6f, -0.5f, 1.0f, 1.0f, 0.1f); // Draw the door
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Left window
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, windowTexture); // Bind window texture
    glPushMatrix();
    //glTranslatef(-0.8f, -0.3f, -0.2f); // Position of the left window
    //glScalef(1.4f, 1.4f, 0.1f);          // Window dimensions (scale)
    glColor3f(1.f, 1.0f, 1.0f);
    drawTexturedCuboid(-0.8f, -0.3f, -0.77f, 0.6f, 0.9f, 0.1f); // Draw left window
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Right window
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, windowTexture); // Bind window texture
    glPushMatrix();
    //glTranslatef(0.8f, -0.3f, -0.2f); // Position of the right window
   // glScalef(0.4f, 0.4f, 0.1f);         // Window dimensions (scale)
    glColor3f(1.f, 1.0f, 1.0f);
    drawTexturedCuboid(0.8f, -0.3f, -0.77f, 0.6f, 0.9f, 0.1f); // Draw right window
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Chimney (horn)
    glColor3f(0.5f, 0.2f, 0.2f); // Dark red
    glPushMatrix();
    glTranslatef(0.8f, 1.8f, -2.2f); // Position of the chimney on the roof
    glScalef(0.3f, 1.0f, 0.3f);     // Dimensions of the chimney
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw smoke
    drawSmoke();  // Add smoke coming out of the chimney

    // Reset color
    glColor3f(1.0f, 1.0f, 1.0f);
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

void drawTexturedCone(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat height, GLint slices, GLint stacks, GLuint texture) {
    glEnable(GL_TEXTURE_2D);  // Activăm texturile
    glBindTexture(GL_TEXTURE_2D, texture);  // Leagă textura

    glPushMatrix();
    glTranslatef(x, y, z);  // Translația conului

    // Desenează baza conului
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f); // Textură pentru centru
    glVertex3f(0.0f, 0.0f, 0.0f);  // Vârful bazei

    for (int i = 0; i <= slices; i++) {
        GLfloat angle = i * 2.0f * 3.1415 / slices;  // Împărțirea cercului în segmente

        GLfloat xCoord = radius * cos(angle);
        GLfloat yCoord = radius * sin(angle);

        glTexCoord2f((xCoord / radius + 1.0f) / 2.0f, (yCoord / radius + 1.0f) / 2.0f);  // Texturarea pe margine
        glVertex3f(xCoord, 0.0f, yCoord);
    }

    glEnd();

    // Desenează fața laterală a conului
    for (int i = 0; i < slices; i++) {
        GLfloat angle1 = i * 2.0f * 3.1415 / slices;
        GLfloat angle2 = (i + 1) * 2.0f * 3.1415 / slices;

        GLfloat x1 = radius * cos(angle1);
        GLfloat y1 = radius * sin(angle1);
        GLfloat x2 = radius * cos(angle2);
        GLfloat y2 = radius * sin(angle2);

        glBegin(GL_TRIANGLES);
        glTexCoord2f((x1 / radius + 1.0f) / 2.0f, (y1 / radius + 1.0f) / 2.0f);  // Texturare pentru fața laterală
        glVertex3f(x1, 0.0f, y1);

        glTexCoord2f(0.5f, 1.0f);  // Vârful conului
        glVertex3f(0.0f, height, 0.0f);

        glTexCoord2f((x2 / radius + 1.0f) / 2.0f, (y2 / radius + 1.0f) / 2.0f);  // Texturare pentru fața laterală
        glVertex3f(x2, 0.0f, y2);
        glEnd();
    }

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);  // Dezactivează texturile
}


void drawChristmasTree(GLfloat x, GLfloat y, GLfloat z) {
    // Trunchiul copacului
    glColor3f(0.4f, 0.2f, 0.1f); // Maro
    drawCylinder(x, y, z, 0.2f, 1.0f);

    // Frunzișul copacului (conuri poziționate vertical)
    glColor3f(0.0f, 0.8f, 0.0f); // Verde
    glPushMatrix();
    glTranslatef(x, y + 0.8f, z); // Poziția de bază a primului con
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotim conul pentru a fi vertical
    glutSolidCone(1.5f, 2.7f, 50, 50);
    GLuint treeTexture =  loadTexture("tree.jpg");
    //drawTexturedCone(x, y + 0.8f, z, 1.5f, 2.7f, 50, 50, treeTexture);
    glPopMatrix();

    // Resetare culoare
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
    // Bottom sphere (body)
    drawSphere(2.0f, -1.25f, 0.0f, 0.35f);  // Reduced size

    // Middle sphere (torso)
    drawSphere(2.0f, -0.77f, 0.0f, 0.25f);   // Reduced size

    // Top sphere (head)
    drawSphere(2.0f, -0.47f, 0.0f, 0.15f);  // Reduced size

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    drawSphere(1.95f, -0.35f, 0.1f, 0.025f);
    drawSphere(2.05f, -0.35f, 0.1f, 0.025f);

    // Nose
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
    glTranslatef(2.0f, -0.45f, 0.1f);
    glutSolidCone(0.07, 0.15, 20, 20);
    glPopMatrix();

    // Reset color
    glColor3f(1.0f, 1.0f, 1.0f);
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
                flakes[i][1] = 5.0f; // Reset to top when falling below ground
            }
        }
    }
}

GLfloat sleighX = -10.0f; // Poziția inițială a saniei

void timerFunc(int value) {
    sleighX += 0.05f; // Sania se mișcă spre dreapta
    if (sleighX > 10.0f) {
        sleighX = -10.0f; // Resetează poziția saniei
    }

    glutPostRedisplay();  // Reîmprospătare ecran
    glutTimerFunc(16, timerFunc, 0);  // Apelează din nou peste 16 ms (~60 FPS)
}


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


void drawGround() {
    // Add snow texture to the ground
    glColor3f(1.0f, 1.0f, 1.0f); // White for snow
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 0.0f); // Position the ground
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

    // Desenăm un dreptunghi lung pentru potecă
    glVertex3f(-0.5f, 0.0f, 0.0f);  // Stânga jos
    glVertex3f(0.5f, 0.0f, 0.0f);   // Dreapta jos
    glVertex3f(0.5f, 0.0f, 5.0f);   // Dreapta sus
    glVertex3f(-0.5f, 0.0f, 5.0f);  // Stânga sus

    glEnd();
    glPopMatrix();

    // Resetare culoare
    glColor3f(1.0f, 1.0f, 1.0f);
}

void drawLampPost(GLfloat x, GLfloat y, GLfloat z) {
    // Stâlp
    glColor3f(0.3f, 0.3f, 0.3f); // Gri
    drawCylinder(x, y, z, 0.1f, 2.0f); // Stâlp cilindric

    // Bec
    glColor3f(1.0f, 1.0f, 0.0f); // Galben pentru lumină
    drawSphere(x, y + 2.0f, z, 0.2f); // Becul

    // Glow efect
    GLfloat lightPos[] = { x, y + 2.0f, z, 1.0f };
    GLfloat lightColor[] = { 1.0f, 1.0f, 0.8f, 1.0f };

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
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
    GLfloat moonLightColor[] = { 0.75f, 0.75f, 0.75f, 1.0f };  // Culoare argintie

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, moonLightPosition);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, moonLightColor);

    // Setează materialele lunii pentru a reflecta lumina
    GLfloat moonMaterial[] = { 0.75f, 0.75f, 0.75f, 1.0f };  // Culoare argintie
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moonMaterial);
    glMaterialfv(GL_FRONT, GL_AMBIENT, moonMaterial);

    // Reflexii metalice
    GLfloat moonSpecular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, moonSpecular);

    // Strălucire metalică
    GLfloat moonShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, moonShininess);

    // Activare iluminare material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

GLuint brickTexture;
GLuint doorTexture;
GLuint windowTexture;

void initTextures() {
    //brickTexture = loadTexture("brick_texture.jpg");
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

// Function to draw an ellipse
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


float sleighPositionX = -5.0; // Initial sleigh position


// Function to draw antlers
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

// Function to draw a reindeer
void drawReindeer(float x, float y, float z) {
    // Body
    glColor3f(0.6, 0.3, 0.1);
    drawEllipse(x, y, z, 0.4, 0.2);

    // Head
    glColor3f(0.6, 0.3, 0.1);
    drawSphere(x + 0.5, y + 0.2, z, 0.15);

    // Legs
    glColor3f(0.4, 0.2, 0);
    for (int i = -1; i <= 1; i += 2) {
        drawSphere(x + i * 0.2, y - 0.2, z + 0.1, 0.05);
        drawSphere(x + i * 0.2, y - 0.2, z - 0.1, 0.05);
    }

    // Nose
    glColor3f(1, 0, 0);
    drawSphere(x + 0.7, y + 0.2, z, 0.05);

    // Antlers
    glColor3f(0.4, 0.2, 0);
    drawAntlers(x + 0.5, y + 0.35, z, 0.3);
}

// Function to draw the sleigh
void drawSleigh(float x, float y, float z) {
    // Base
    glColor3f(0.8, 0, 0);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(1.5, 0.5, 0.8);
    glutSolidCube(1);
    glPopMatrix();

    // Runners
    glColor3f(0.6, 0.3, 0.1);
    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glTranslatef(x, y - 0.35, z + i * 0.4);
        glScalef(1.5, 0.1, 0.1);
        glutSolidCube(1);
        glPopMatrix();
    }

    // Santa (body and head)
    glColor3f(1, 0, 0);
    drawSphere(x, y + 0.4, z, 0.2); // Body
    glColor3f(1, 0.8, 0.6);
    drawSphere(x, y + 0.6, z, 0.1); // Head

    // Hat
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(x, y + 0.65, z);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.15, 0.3, 20, 20);
    glPopMatrix();

    glColor3f(1, 1, 1);
    drawSphere(x, y + 1, z, 0.05); // Pom-pom
}

void drawLake() {
    glColor3f(0.7f, 0.8f, 1.0f); // Culoare mai deschisă pentru gheață
    glPushMatrix();
    glTranslatef(4.7f, -1.45f, -2.0f); // Mută lacul în dreapta (modifică primul parametru)

    // Desenează un oval folosind ecuația elipsei
    glBegin(GL_POLYGON);
    GLfloat radiusX = 3.0f; // Rază mai mare pe axa X pentru a crea forma ovală
    GLfloat radiusZ = 1.f; // Rază mai mică pe axa Z pentru a păstra proporția ovală
    for (int i = 0; i <= 100; ++i) {
        GLfloat angle = 2.0f * 3.1415f * i / 100; // Împărțim cercul în 100 de segmente
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
        GLfloat angle = 2.0f * 3.1415f * i / 100; // Împărțim cercul în 100 de segmente
        GLfloat x = radiusX * cos(angle);
        GLfloat z = radiusZ * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();
}


void drawMoon() {
    glPushMatrix();
    glTranslatef(5.0f, 4.0f, -5.0f);  // Position of the moon in the top-right
    glColor3f(0.8f, 0.8f, 0.5f);      // Light yellow for the moon
    glutSolidSphere(0.5f, 50, 50);    // Draw a sphere to represent the moon
    glPopMatrix();
}

void drawRabbit1() {
    // Corpul iepurelui (sfera mare)
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawSphere(-3.0f, -1.2f, 0.0f, 0.08f);  // Corp mare

    // Capul iepurelui (sfera mai mică)
    drawSphere(-3.0f, -1.1f, 0.0f, 0.05f);  // Cap mic

    // Urechile (cilindri)
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawCylinder(-3.02f, -1.1f, 0.0f, 0.01f, 0.11f);  // Ureche dreapta
    drawCylinder(-2.98f, -1.1f, 0.0f, 0.01f, 0.11f);  // Ureche stânga

    // Picioarele
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawCylinder(-3.02f, -1.3f, 0.0f, 0.025f, 0.05f);  // Picior dreapta
    drawCylinder(-2.98f, -1.3f, 0.0f, 0.025f, 0.05f);  // Picior stânga
}

void drawRabbit2() {
    // Corpul iepurelui (sfera mare)
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawSphere(-3.5f, -1.2f, 0.0f, 0.08f);  // Corp mare

    // Capul iepurelui (sfera mai mică)
    drawSphere(-3.5f, -1.1f, 0.0f, 0.05f);  // Cap mic

    // Urechile (cilindri)
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawCylinder(-3.52f, -1.1f, 0.0f, 0.01f, 0.11f);  // Ureche dreapta
    drawCylinder(-3.48f, -1.1f, 0.0f, 0.01f, 0.11f);  // Ureche stânga

    // Picioarele
    glColor3f(1.0f, 1.0f, 1.0f);  // Alb
    drawCylinder(-3.52f, -1.3f, 0.0f, 0.025f, 0.05f);  // Picior dreapta
    drawCylinder(-3.48f, -1.3f, 0.0f, 0.025f, 0.05f);  // Picior stânga
}

void drawDog() {
    // Corpul câinelui (sfera mare)
    glColor3f(0.8f, 0.4f, 0.0f);  // Maro deschis
    drawSphere(1.0f, -1.3f, 1.0f, 0.15f);  // Corp mare

    // Capul câinelui (sfera mai mică)
    drawSphere(1.0f, -1.07f, 1.0f, 0.1f);  // Cap mic

    // Urechile câinelui (cilindri inclinați)
    glColor3f(0.8f, 0.4f, 0.0f);  // Maro deschis
    drawCylinder(1.05f, -1.05f, 1.0f, 0.02f, 0.15f);  // Ureche dreapta
    drawCylinder(0.95f, -1.05f, 1.0f, 0.02f, 0.15f);  // Ureche stânga

    // Coada câinelui (cilindru mic)
    glColor3f(0.8f, 0.4f, 0.0f);  // Maro deschis
    drawCylinder(1.13f, -1.3f, 1.0f, 0.01f, 0.15f);  // Coada

    // Picioarele câinelui
    glColor3f(0.8f, 0.4f, 0.0f);  // Maro deschis
    drawCylinder(1.05f, -1.5f, 1.06f, 0.03f, 0.1f);  // Picior dreapta față
    drawCylinder(0.94f, -1.5f, 1.06f, 0.03f, 0.1f);  // Picior stânga față
    drawCylinder(1.05f, -1.5f, 0.94f, 0.03f, 0.1f);  // Picior dreapta spate
    drawCylinder(0.94f, -1.5f, 0.94f, 0.03f, 0.1f);  // Picior stânga spate
}




void initMoonLighting() {
    // Lumină foarte slabă pentru lună
    GLfloat moonLightPos[] = { 7.0f, 5.0f, -10.0f, 1.0f };  // Poziționăm lumina în locul lunii
    GLfloat moonLightColor[] = { 0.2f, 0.2f, 0.5f, 1.0f };  // Lumină rece și slabă (albastru deschis)

    glEnable(GL_LIGHT2);  // Activăm un alt canal de lumină (GL_LIGHT2)
    glLightfv(GL_LIGHT2, GL_POSITION, moonLightPos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, moonLightColor);
    glLightfv(GL_LIGHT2, GL_SPECULAR, moonLightColor);
}


void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // Dark blue background
    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    initLighting();
    isSnowing = true;  // Pornește zăpada automat
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Setăm poziția camerei
    gluLookAt(0.0f, 0.0f, 10.0f,  // Poziția camerei
        0.0f, 0.0f, 0.0f,   // Punctul de interes (ceea ce camera privește)
        0.0f, 1.0f, 0.0f);  // Direcția "sus"

    glEnable(GL_LIGHTING);  // Activează iluminarea
    glEnable(GL_LIGHT0);    // Activează prima sursă de lumină
    glEnable(GL_LIGHT1);    // Activează a doua sursă de lumină (pentru luna ta)
    initTextures();

}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Inițializarea iluminării pentru lună
    initMoonLighting();  // Adăugăm lumina lunii


    // Move camera back based on zoom
    glTranslatef(0.0f, 0.0f, zoom);

    // Rotate based on user input
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    // Activăm iluminarea globală
    glEnable(GL_LIGHTING);

    // Draw scene
    drawGround();
    drawPath();
    drawLake(); // Lac în stânga scenei
    drawMoon();  // Poziționează luna la o distanță mai mare și sus (5,5,-10)


    drawLampPost(-1.2f, -1.5f, 0.0f); // Felinar stânga
    drawLampPost(1.2f, -1.5f, 0.0f); // Felinar dreapta
    GLuint brickTexture = loadTexture("brick_texture.jpg"); 
    GLuint doorTexture = loadTexture("door.jpg");
    GLuint windowTexture = loadTexture("window1.jpg");
    drawHouse(brickTexture, doorTexture, windowTexture); // Draw the house with the texture
    drawSnowman();
    drawDecorativeLights(-1.2f, 0.8f, -0.75f, 1.2f, 0.8f, -0.75f, 10); // Luminile pe acoperiș


    drawForest();
    // Draw sleigh
    // Move sleigh
    sleighPositionX += 0.08;
    if (sleighPositionX > 10.0) {
        sleighPositionX = -10.0;
    }
    // Desenăm animalele
    drawRabbit1();  // Desenează iepurele
    drawRabbit2();  // Desenează iepurele
    drawDog();     // Desenează câinele
    // Draw sleigh
    drawSleigh(sleighPositionX, 3.5, -3);

    // Draw reindeer
    float spacing = 1.2;
    for (int i = 0; i < 3; ++i) {
        drawReindeer(sleighPositionX + spacing * (i + 1), 3.5, -2.5);
    }
    drawSnowflakes();

    glutSwapBuffers();
}




void keyboardSpecial(int key, int x, int y) {
    // Adjust rotation angles based on arrow keys
    if (key == GLUT_KEY_UP) {
        zoom += 1.0f;  // Zoom in
    }
    else if (key == GLUT_KEY_DOWN) {
        zoom -= 1.0f;  // Zoom out
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
    glutCreateWindow("3D Snowman Scene at Night");

    initOpenGL();

    glutDisplayFunc(display);
    glutSpecialFunc(keyboardSpecial);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(16, timerFunc, 0);


    glutMainLoop();
    return 0;
}