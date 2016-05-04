#include <GL/gl.h>           
#include <GL/glu.h>         
#include <GL/glut.h>    

#include <stdio.h>      
#include <stdlib.h>     
#include <math.h>
#include <tiffio.h>     /* Sam Leffler's libtiff library. */
int writetiff(char *filename, char *description,
		int x, int y, int width, int height, int compression);

#define    windowWidth 600
#define    windowHeight 600

#define RED   0
#define GREEN 0
#define BLUE  0
#define ALPHA 1

#define true  1
#define false 0

#define KEY_ESC 27

#define PI 3.1415926535898

#define position_Ini -60.0

#define triangle 1
#define tetraedre 2

float t = 0.f;
float delta = 10.f;
float k = 0.001f;
float K = 0.002f;
int IdleRunning = true;
int TraceEcran = false;
int RangFichierStockage = 0;
float position = position_Ini;

int Ma_Tete; //une sphère
int oreille; //un tétraèdre pour modèle oreille
int patte_ant; //modèle patte antérieure
int Mon_Repere;


int angle = 0;
float dx = 0;
float dy = 0;

float sin_k_t = 0;

static GLfloat light_position0[] = { 15.0 , 15.0 , 15.0 , 0.0 };
static GLfloat light_position1[] = { 15.0 , 15.0 , -15.0 , 0.0 };

static GLfloat ambient_light0[] = { 0.0 , 0.0 , 0.0 , 0.0 };
static GLfloat diffuse_light0[] = { 0.7 , 0.7 , 0.7 , 1.0 };
static GLfloat specular_light0[] = { 0.1 , 0.1 , 0.1 , 0.1 };

static GLfloat ambient_light1[] = { 0.50 , 0.50 , 0.50 , 1.0 };
static GLfloat diffuse_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };
static GLfloat specular_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };

int Step = 0;
int latence =4;

void init_scene();
void render_scene();
void init_angles();
GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height); 
GLvoid window_key(unsigned char key, int x, int y); 
GLvoid window_timer(); 
GLvoid special_key();
void Faire_Composantes();
void faire_tetraedre();
void Dessine_Repere();

int main(int argc, char **argv) 
{  
	// initialisation  des paramètres de GLUT en fonction
	// des arguments sur la ligne de commande
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// définition et création de la fenêtre graphique
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("MIAOU");

	// initialisation de OpenGL et de la scène
	initGL();  
	init_scene();

	// choix des procédures de callback pour 
	// le tracé graphique
	glutDisplayFunc(&window_display);
	// le redimensionnement de la fenêtre
	glutReshapeFunc(&window_reshape);
	// la gestion des événements clavier
	glutKeyboardFunc(&window_key);
	// fonction appelée régulièrement entre deux gestions d'événements
	glutTimerFunc(latence,&window_timer,Step);
    
    glutSpecialFunc(&special_key);
	// la boucle prinicipale de gestion des événements utilisateur
	glutMainLoop();  

	return 1;
}

// initialisation du fond de la fenêtre graphique : noir opaque

GLvoid initGL() {
	// initialisation de l'éclairement

	// définition de deux souces  lumineuses
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	// activation de l'éclairement
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// propriétés matérielles des objets
	// glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambientanddiffuse);
	// glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	// glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glShadeModel( GL_SMOOTH );
	glEnable(GL_COLOR_MATERIAL);

	// initialisation du fond
	glClearColor(RED, GREEN, BLUE, ALPHA);        
	// z-buffer
	glEnable(GL_DEPTH_TEST);
}

void init_scene(){
	// initialise des display lists des composantes cylindriques du corps
	Faire_Composantes();
}

// fonction de call-back pour lŽaffichage dans la fenêtre

GLvoid window_display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	render_scene();

	// trace la scène grapnique qui vient juste d'être définie
	glFlush();
}

// fonction de call-back pour le redimensionnement de la fenêtre

GLvoid window_reshape(GLsizei width, GLsizei height){  
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20, 20, -20, 20, -1000, 1000);
	// glFrustum(-20, 20, -20, 20, 10, 1000);
	// glScalef(10, 10, 10);

	// toutes les transformations suivantes sŽappliquent au modèle de vue 
	glMatrixMode(GL_MODELVIEW);
}

// fonction de call-back pour la gestion des événements clavier

GLvoid window_key(unsigned char key, int x, int y) {  
	switch (key) {    
		case KEY_ESC:  
			exit(1);                    
			break; 
		case ' ':  
			if (IdleRunning) {
				glutTimerFunc(latence,NULL,Step);
				IdleRunning = false;
			} 
			else {
				glutTimerFunc(latence,&window_timer,Step);
				IdleRunning = true;
			}
			break; 
		case '+':  
            angle++;
			delta *= 1.05;
            glutPostRedisplay();
			break; 
		case '-':  
            angle--;
			delta /= 1.05;
            glutPostRedisplay();
			break; 
		default:
			printf ("La touche %d n'est pas active.\n", key);
			break;
	}     
}

GLvoid special_key(int key, int x, int y){
	switch(key){
		case GLUT_KEY_LEFT :
			dx -= 0.3;
			glutPostRedisplay();
			break;

		case GLUT_KEY_RIGHT :
			dx += 0.3;
			glutPostRedisplay();
			break;

		case GLUT_KEY_DOWN :
			dy -= 0.3;
			glutPostRedisplay();
			break;

		case GLUT_KEY_UP :
			dy += 0.3;
			glutPostRedisplay();
			break;

		default:
			printf ("La touche %d n'est pas active.\n", key);
			break;
	}

}

// fonction de call-back appelée régulièrement

GLvoid window_timer() {
	// On effecture une variation des angles de chaque membre
	// de l'amplitude associée et de la position médiane
	// ********* A FAIRE **************

	// On déplace la position de l'avatar pour qu'il avance
	// ********* A FAIRE **************
	position = K*t;

	glutTimerFunc(latence,&window_timer,++Step);

	glutPostRedisplay();
}


void Faire_Composantes() {
	GLUquadricObj* GLAPIENTRY qobj;

	// attribution des indentificateurs de display lists
	Ma_Tete =  glGenLists(6);

	// compilation de la display list de la sphère
	glNewList(Ma_Tete,GL_COMPILE);
        glutSolidSphere(5,20,20);
	glEndList();
    
    faire_tetraedre();

	// allocation d'une description de quadrique
	qobj = gluNewQuadric();
	// la quadrique est pleine 
	gluQuadricDrawStyle(qobj, GLU_FILL); 
	// les ombrages, s'il y en a, sont doux
	gluQuadricNormals(qobj, GLU_SMOOTH);

}

void faire_tetraedre(){    
    glNewList(tetraedre, GL_COMPILE);
		//ABC
        glBegin(GL_TRIANGLES); 
            // Sommet A
            glVertex3f(0, 4, 0);
            // Sommet B
            glVertex3f(-1.5, 0, 0);
            // Sommet C
            glVertex3f(1.5, 0, 0);
        glEnd();
		
        //ACD
            glColor3f(1,0,0);
            glBegin(GL_TRIANGLES); 
            // Sommet A
            glVertex3f(0, 4, 0);
            // Sommet C
            glVertex3f(1.5, 0, 0);
            // Sommet D
            glVertex3f(0, -1.5, -2);
        glEnd();
        
        //ABD
        glColor3f(0,1,0);
         glBegin(GL_TRIANGLES); 
            // Sommet A
            glVertex3f(0, 4, 0);
            // Sommet B
            glVertex3f(-1.5, 0, 0);
            // Sommet D
            glVertex3f(0, -1.5, -2);
        glEnd();

        //BCD
         glColor3f(1, 0, 1);
         glBegin(GL_TRIANGLES); 
            // Sommet B
            glVertex3f(-1.5, 0, 0);
            // Sommet C
            glVertex3f(1.5, 0, 0);
            // Sommet D
            glVertex3f(0, -1, -2);
        glEnd();
        
	glEndList();
    
}

/*une patte = 
 *      un tétraèdre pour la partie épaule,
 *      une boule de liaison,
 *      une pyramide à base carrée (base côté épaule),
 *      un polygone à 6 côtés*/
void faire_patte_ant(){
    GLUquadric *quad;
    quad = gluNewQuadric();
    glNewList(patte_ant, GL_COMPILE);
    
        //tétraèdre pour la partie épaule
        glBegin(GL_TRIANGLES); 
            // Sommet A
            glVertex3f(0, 0, 0.3);
            // Sommet B
            glVertex3f(0.26, 0, 0);
            // Sommet C
            glVertex3f(1.5, 0, -0.11);
            // Sommet D
            glVertex3f(0.16, -0.19, 0)
        glEnd();
        
        
        
        
        
    glEndList();
}


void  Dessine_Repere() {
	glNewList(Mon_Repere, GL_COMPILE);
        glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0); //rouge = axe x
            glVertex3f(-10 , 0 , 0);
            glVertex3f(10 , 0 , 0);
        glEnd();
        glBegin(GL_LINES); //vert = axe y
            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(0 , -10 , 0);
            glVertex3f(0 , 10 , 0);
        glEnd();
        glBegin(GL_LINES); //bleu = axe z
            glColor3f(0.0, 0, 1.0);
            glVertex3f(0 , 0 , -10);
            glVertex3f(0 , 0 , 10);
        glEnd();
        glPointSize( 10.0 );
        glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 1.0);
            glVertex3f(10.0 , 0 , 0);
        glEnd();
        glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 1.0);
            glVertex3f(0 , 10.0 , 0);
        glEnd(); 
        glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 1.0);
            glVertex3f(0 , 0 , 10);
        glEnd();
	glEndList();
}

void render_scene()
{
   

    glRotatef(angle, 0, 1, 0);
    glTranslatef(dx, dy, 0);
    
     Dessine_Repere();
	// déplacement horizontal selon l'axe Oy pour donner 
	// une impression de déplacement horizontal accompagnant
	// la marche
	//  glTranslatef( 0, position, 0);

	// tracé de la tête avec une translation positive
	// selon Oz pour la placer au-dessus du tronc
	// les appels à glPushMatrix et glPopMatrix servent 
	// à sauvegarder et restaurer le contexte graphique
	glColor3f(1.0, 1.0, 0.0);
    
    
    //TETE
    /*
    glPushMatrix();
        glCallList(Ma_Tete);
    glPopMatrix();
    */
    //OREILLES
    /*
    glPushMatrix();
        glColor3f(1.0, 1.0, 0.0);
        glRotatef(20, 0, 0, 1);
        glTranslatef(-2, 3.5, 0);
        glCallList(tetraedre);
    glPopMatrix();
    
    glPushMatrix();
        glColor3f(1.0, 1.0, 0.0);
        glRotatef(-20, 0, 0, 1);
        glTranslatef(2, 3.5, 0);
        glCallList(tetraedre);
    glPopMatrix();
     */



	// permutation des buffers lorsque le tracé est achevé
	glutSwapBuffers();
}
