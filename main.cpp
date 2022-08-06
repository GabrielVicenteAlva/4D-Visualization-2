#include <GL/glut.h>
#include <math.h>
#include <unistd.h>

float lightX = 0.0, lightY = 20.0, lightZ = 10.0;
void setColor(float dist, float alpha=1) {
	dist = 1/(1+exp(1.5*dist-5));
	glColor4f(dist*1.+.00, dist*1.+.00, 0.00, alpha);
}
void drawSphere(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColor(s);
	glutSolidSphere(.25*.13/s,8,8);
	glPopMatrix();
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
	GLfloat Lt0pos[] = {lightX, lightY, lightZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);
	
    glLoadIdentity();
    glViewport(0, 0, 600,600);
    glMatrixMode(GL_MODELVIEW);
    
	gluLookAt(
		0,0,2,
		0.,0.,0.,
		0,1,0
	);
	
	drawSphere(0,0,0,.5);
	
    glLoadIdentity();
    glViewport(600, 0, 600,600);
    glMatrixMode(GL_MODELVIEW);
    
	gluLookAt(
		2,0,0,
		0.,0.,0.,
		0,1,0
	);
	
	drawSphere(0,0,0,.5);
	
	glFlush();
	
}

void idle() {
	usleep(20000);
// 	check_keys();
	glutPostRedisplay();
}

void keyDown (unsigned char key, int x, int y)
{
//     std::cout << "keydown " << key << "\n";
//     // Save which key is being pressed
// 	pressed_keys[key] = true;
// 	if(key=='<')
// 		show_all_faces ^= 1;
}

void keyUp (unsigned char key, int x, int y)
{
//     std::cout << "keyup " << key << "\n";
// 	pressed_keys[key] = false;
}

void motionFunc(int x,int y) {
// 	float difX = x - lastX;
// 	float difY = y - lastY;
// 	// cout<<difX<<' '<<difY<<endl;
// 	if(abs(difX)<8 && abs(difY)<8) {
// 		cam_rot = cam_rot*Vec3D(-difX/200.,difY/200.,1.)*Vec3D(0,0,1.);
// 		cam_pos = cam_rot*Vec3D(0.,0.,4.)*!cam_rot;
// 		cam_up = cam_rot*Vec3D(0.,1.,0.)*!cam_rot;
// 	}
// 	lastX = x;
// 	lastY = y;
}
void initRender() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.00, 0.00, 0.00, 1.0);
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
// 	proj_init();
	glutInit(&argc, argv);                 // Initialize GLUT
	glutInitWindowSize(1200, 600);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutIdleFunc(idle); 
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(motionFunc);
    initRender();
	glutMainLoop();          // Enter the infinitely event-processing loop
	return 0;
}
