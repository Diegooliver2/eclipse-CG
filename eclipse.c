#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


static GLuint sunTexture; // ID da textura do Sol
static GLuint earthTexture; // ID da textura da Terra
static GLuint moonTexture; // ID da textura da Lua

static int spin = 0;
static int controllerMoonRotation = 0;
static int eixoXT = -30.0;
static int eixoZT = -75.0;
static int eixoXR = 0.0;
static int eixoYR = 0.0;

void drawTexturedSphere(GLfloat radius, GLint slices, GLint stacks) {
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluSphere(quadric, radius, slices, stacks);
    gluDeleteQuadric(quadric);
}

void loadTexture(const char* filename, GLuint* texture) {
    int width, height, numColCh;
    unsigned char* image = stbi_load(filename, &width, &height, &numColCh, 0);

    if (image != NULL) {
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Configuração de filtragem e repetição da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(image);
    } else {
        fprintf(stderr, "Falha ao carregar a textura.\n");
    }
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
    
   loadTexture("textures/sun_texture.png", &sunTexture);
   loadTexture("textures/earth_texture.png", &earthTexture);
   loadTexture("textures/moon_texture.png", &moonTexture);
   
   // Considera a luz ambiente e as propriedades dos materiais
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
void display(void)
{
   GLfloat position[] = { 0.0, 0.0, 0.0, 1.0 };

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix ();
   glTranslatef ((GLdouble) eixoXT, 0.0, (GLdouble) eixoZT); //Perspectiva
   glRotated ((GLdouble) eixoXR, 1.0, 0.0, 0.0);
   glRotated ((GLdouble) (GLdouble) eixoYR, 0.0, 1.0, 0.0);

// Sol
   glPushMatrix ();
   glTranslated (50.0, 0.0, 0.0);
   glDisable (GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sunTexture);
   glDisable(GL_BLEND);
   glColor3f (1.0, 1.0, 1.0);
   drawTexturedSphere (10, 100, 100);
   glDisable(GL_TEXTURE_2D);
   glEnable (GL_LIGHTING);
   glLightfv (GL_LIGHT0, GL_POSITION, position);
   glPopMatrix ();

// Lua
   glPushMatrix ();
   glRotated ((GLdouble) spin, 0.0, 1.0, 0.0);
   glTranslated (0.0, 0.0, 8.0);
	   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, moonTexture);
   
   GLfloat moon_diffuse[] = {0.5, 0.9, 0.8, 1.0};
   GLfloat moon_ambient[] = {0.2, 0.2, 0.2, 1.0};

   glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_diffuse);
   glMaterialfv(GL_FRONT, GL_AMBIENT, moon_ambient);
   
   drawTexturedSphere (1.15, 100, 100);
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix ();

// Terra
   glPushMatrix ();
   glTranslated (0.0, 0.0, 0.0);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, earthTexture);
   
   GLfloat earth_diffuse[] = {0.5, 0.9, 0.8, 1.0};
   GLfloat earth_ambient[] = {0.2, 0.2, 0.2, 1.0};

   glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_diffuse);
   glMaterialfv(GL_FRONT, GL_AMBIENT, earth_ambient);
   
   drawTexturedSphere (3, 100, 100);
   
   glDisable(GL_TEXTURE_2D);
   glPopMatrix ();
   
// Renderiza
   glPopMatrix ();
   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 500.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
	 if (state == GLUT_DOWN) {
	    spin = (spin + 1) % 360;
	    glutPostRedisplay();
	 }
         break;
      case GLUT_RIGHT_BUTTON:
      	 if (state == GLUT_DOWN) {
      	    spin = (spin - 1) % 360;
    	    glutPostRedisplay();
      	 }
         break;
      case 3:
         eixoZT = eixoZT + 1;
         glutPostRedisplay();
         break;
      case 4:
         eixoZT = eixoZT - 1;
         glutPostRedisplay();
         break;
      default:
         break;
   }
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'r':
      case 'R':
	 eixoXT = -30.0;
	 eixoZT = -75.0;
	 eixoXR = 0.0;
	 eixoYR = 0.0;
         glutPostRedisplay();
         break;
      case 't':
      case 'T':
	 eixoXT = 0.0;
	 eixoZT = 2.0;
	 eixoXR = 0.0;
	 eixoYR = 90;
         glutPostRedisplay();
         break;
      case 'w':
      case 'W':
      	 eixoZT = eixoZT + 1;
         glutPostRedisplay();
         break;
      case 's':
      case 'S':
	 	 eixoZT = eixoZT - 1;
         glutPostRedisplay();
         break;
      case 'a':
      case 'A':
         eixoXT = eixoXT + 1;
         glutPostRedisplay();
         break;
      case 'd':
      case 'D':
         eixoXT = eixoXT - 1;
         glutPostRedisplay();
         break;
      case 'q':
      case 'Q':
         eixoYR = (eixoYR + 1) % 360;
         if (eixoXT < 0.0) {
             eixoXT = eixoXT + 1;
         }         
         glutPostRedisplay();
         break;
      case 'e':
      case 'E':
         eixoYR = (eixoYR - 1) % 360;
         if (eixoXT < 0.0) {
             eixoXT = eixoXT + 1;
         }         
         glutPostRedisplay();
         break;
      case 'f':
      case 'F':
         eixoXR = (eixoXR + 5) % 360;
         glutPostRedisplay();
         break;
      case 'v':
      case 'V':
      	 eixoXR = (eixoXR - 5) % 360;
         glutPostRedisplay();
         break;
      case '1':
 	 spin = (spin + 1) % 360;
 	 glutPostRedisplay();
         break;
      case '3':
	 spin = (spin - 1) % 360;
	 glutPostRedisplay();
         break;
      case 'm':
      case 'M':
	if (controllerMoonRotation == 0) {
	   controllerMoonRotation = 1;
	} else {
	   controllerMoonRotation = 0;
	}
 	glutPostRedisplay();
	break;
      default:
         break;
   }
}

void moonRotation () {
	if (controllerMoonRotation == 0) {
	   spin = (spin + 1) % 360;
	}

	glutPostRedisplay();
	glutTimerFunc(50, moonRotation, 1);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (1200, 600); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyboard);
   glutTimerFunc(50, moonRotation, 1);
   glutMainLoop();
   return 0;
}

