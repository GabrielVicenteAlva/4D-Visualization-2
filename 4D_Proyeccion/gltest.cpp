#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

const float PI = 4*atan(1);

void setColor(float dist) {
	dist = 1/(1+exp(dist-4));
	if(dist>1.) dist = 1.;
	glColor3f(dist,dist,0.);
}

void drawSphere(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColor(s);
	glutSolidSphere(.05*s,16,16);
	glPopMatrix();
}

void drawCylinder(float x1,float y1,float z1,float x2,float y2, float z2) {
	float d = sqrt(
		pow(x1-x2,2) + 
		pow(y1-y2,2) + 
		pow(z1-z2,2)
	);
	
	glLineWidth(3.);
	
	glPushMatrix();
	gluLookAt(
		x1,y1,z1,
		x2,y2,z2,
		y2-y1-z2+z1,z2-z1-x1+x1,x2-x1-y1+y2
	);
	
	glBegin(GL_LINES);
		glColor3f(0., 0., 1.);
		glVertex3f(0.,0.,0.);
		glColor3f(0., 0., 1.);
		glVertex3f(0.,0.,1.);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0., 1., 0.);
		glVertex3f(0.,0.,0.);
		glColor3f(0., 1., 0.);
		glVertex3f(0.,1.,0.);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(1., 0., 0.);
		glVertex3f(0.,0.,0.);
		glColor3f(01., 0., 0.);
		glVertex3f(1.,0.,0.);
	glEnd();
	
	glPopMatrix();
}

float t=0.;
float lightX = 0.0, lightY = 20.0, lightZ = 10.0;
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
	GLfloat Lt0pos[] = {lightX, lightY, lightZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	glLoadIdentity();
	
	gluLookAt(
		2.,2.,2.,
		0.,0.,0.,
		0.,1.,0.
	);
	glLineWidth(3.);
	
    glColor3f(0., 0., 1.);
	drawSphere(0.,0.,0.,1.);
	drawSphere(.2,.3,1.,1.);
	drawSphere(.3,.2,.5,1.);
	glBegin(GL_LINES);
		glColor3f(0., 0., 1.);
		glVertex3f(0.,0.,0.);
		glColor3f(0., 0., 1.);
		glVertex3f(0.,0.,.5);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0., 1., 0.);
		glVertex3f(0.,0.,0.);
		glColor3f(0., 1., 0.);
		glVertex3f(0.,.5,0.);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(1., 0., 0.);
		glVertex3f(0.,0.,0.);
		glColor3f(01., 0., 0.);
		glVertex3f(.5,0.,0.);
	glEnd();
	glLineWidth(5.);
	
	glBegin(GL_LINES);
		glColor3f(1., 1., 0.);
		glVertex3f(.2,.3,1.);
		glColor3f(1., 1., 0.);
		glVertex3f(.3,.2,.5);
	glEnd();
	
	
	glFlush();        
	glutSwapBuffers();
	
}

void idle() {
	// check_keys();
	glutPostRedisplay();
}

void keyDown (unsigned char key, int x, int y)
{
    // std::cout << "keydown " << key << "\n";
    // Save which key is being pressed
	// pressed_keys[key] = true;
	t += .02;
}

void keyUp (unsigned char key, int x, int y)
{
    // std::cout << "keyup " << key << "\n";
	// pressed_keys[key] = false;
}

void initRender() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, 1.0, 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
 
/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
	// proj_init();
	glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(600, 600);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	initRender();
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutIdleFunc(idle); 
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
	glutMainLoop();           // Enter the infinitely event-processing loop
	return 0;
}
