#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Angles for rotation
GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
GLfloat zoom = -10.0f;  // Camera zoom

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

void drawHouse() {
    // Base of the house
    glColor3f(0.8f, 0.4f, 0.1f); // Brown
    drawCube(0.0f, -0.5f, -2.0f, 2.5f);

    // Roof of the house
    glColor3f(0.5f, 0.0f, 0.0f); // Red
    drawPyramid(0.0f, 0.75f, -2.0f, 2.5f, 2.f);

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

void drawChristmasTree() {
    // Trunchiul copacului
    glColor3f(0.4f, 0.2f, 0.1f); // Maro
    drawCylinder(-2.0f, -1.5f, 0.0f, 0.2f, 1.0f);

    // Frunzișul copacului (conuri poziționate vertical)
    glColor3f(0.0f, 0.8f, 0.0f); // Verde
    glPushMatrix();
    glTranslatef(-2.0f, -0.7f, 0.0f);  // Poziția de bază a primului con
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotim conul pentru a fi vertical
    glutSolidCone(1.5f, 1.7f, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0f, 0.3f, 0.0f);  // Poziția celui de-al doilea con
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotim conul pentru a fi vertical
    glutSolidCone(1.2f, 1.4f, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0f, 1.2f, 0.0f);  // Poziția celui de-al treilea con
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotim conul pentru a fi vertical
    glutSolidCone(0.8f, 1.f, 50, 50);
    glPopMatrix();

    // Resetare culoare
    glColor3f(1.0f, 1.0f, 1.0f);
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
            flakes[i][1] = (rand() % 200 - 100) / 10.0f; // Y
            flakes[i][2] = (rand() % 200 - 100) / 10.0f; // Z
        }
        initialized = true;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numFlakes; ++i) {
        drawSphere(flakes[i][0], flakes[i][1], flakes[i][2], 0.05f);
        flakes[i][1] -= 0.02f; // Move down
        if (flakes[i][1] < -5.0f) flakes[i][1] = 5.0f; // Reset to top
    }
}

void drawGround() {
    glColor3f(0.3f, 0.8f, 0.3f); // Green ground
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

void initLighting() {
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightPosition[] = { 1.0f, 4.0f, 2.0f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // Dark blue background
    initLighting();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Move camera back based on zoom
    glTranslatef(0.0f, 0.0f, zoom);

    // Rotate based on user input
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    // Draw scene
    drawGround();
    drawHouse();
    drawSnowman();
    drawChristmasTree();
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

    glutMainLoop();
    return 0;
}