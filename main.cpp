#include <GL/glut.h>
#include <math.h>
#include <unistd.h>

#include <chrono>
#include "projection4d.hpp"
using namespace std::chrono;

using Vec3D = vsr::euclidean_vector<4,float>;
using Rot3D = vsr::euclidean_rotor<4,float>;

int fid = 0;
float t=0.;
float lightX = 0.0, lightY = 30.0, lightZ = 20.0;
Vec3D cam_pos(4.,0.,0.);
Vec3D cam_up(0.,1.,0.);
Rot3D cam_rot(1);

void setColor(float dist) {
	dist = 1/(1+exp(1.0*dist-6));
	dist = 1;
	// cout << dist << endl;
	glColor3f(dist*1.+.00, dist*0.8+.00, 0.00);
}
void setColor(float dist, float alpha) {
	dist = 1/(1+exp(1.0*dist-6));
	dist = 1;
	glColor4f(dist*1.+.00, dist*0.8+.00, 0.00, alpha);
}
void drawSphere(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColor(s);
	glutSolidSphere(.13/s,8,8);
	glPopMatrix();
}
void setColorP(float dist) {
	dist = 1/(1+exp(1.5*dist-5));
	// cout << dist << endl;
	glColor3f(dist*.7+.00,0., dist*.7+.00);
}
void drawSphereP(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColorP(s);
	glutSolidSphere(.13/s,8,8);
	glPopMatrix();
}

Vec3D cross(Vec3D a, Vec3D b) {
    return Vec3D(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
Edge *lastE = NULL;
void drawCylinder (Vec3D A, float c1, Vec3D B, float c2, bool color = true) {

	Vec3D v = B - A;
	float height = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	Vec3D axis = sqrt(v[0]*v[0] + v[1]*v[1]) < 0.001 ? Vec3D(1,0,0) : cross(v, Vec3D(0,0,1));
	float angle = -atan2(sqrt(v[0]*v[0] + v[1]*v[1]), v[2])*180/PI;

	glPushMatrix();
	glTranslatef(A[0], A[1], A[2]);
	glRotatef(angle, axis[0], axis[1], axis[2]);
	// glutSolidCone(radius, height, 32, 16);
	
	static const int n = 6;
	static const float radius = .02;
	if(color)
		setColor((c1+c2)/2);
	/*for(Edge *E: faces[fid]->edges)
		if(E==lastE) {
			glColor3f(0,0,1);
			break;
		}*/
	for(int i=0;i<n;i++) {
		glBegin(GL_QUADS);
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), 0);
			glNormal3f(cos(2*PI*i/n),radius*sin(2*PI*i/n),0);
			i++;
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), 0);
			glNormal3f(cos(2*PI*i/n),radius*sin(2*PI*i/n),0);
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), height);
			glNormal3f(cos(2*PI*i/n),radius*sin(2*PI*i/n),0);
			i--;
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), height);
			glNormal3f(cos(2*PI*i/n),radius*sin(2*PI*i/n),0);
    	glEnd(); 
	}
	
	glPopMatrix();

}

void display() {
	auto start1 = high_resolution_clock::now();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
	GLfloat Lt0pos[] = {lightX, lightY, lightZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	glLoadIdentity();
	// camera.pos.print();
	// cout<<"\n\n";
	gluLookAt(
		cam_pos[0],cam_pos[1],cam_pos[2],
		0.,0.,0.,
		cam_up[0],cam_up[1],cam_up[2]
	);
	auto start2 = high_resolution_clock::now();
	for(Point *P: points)
		P->update();
	if(!show_all_faces) {
		for(Face *F: faces)
			F->update();
		for(Point *P: points)
			P->visible = P->visible && P->facing_cam;
	}
	int n = 0;
	for(Point *p: points)
		if(!p->visible)
			n++;
// 	std::cout<<n<<'\n';
	auto stop2 = high_resolution_clock::now();
	// Edges
	glLineWidth(5.);
	for(Edge *E: edges) {
		if(!E->visible() || (!show_all_faces && !E->facing_cam))
			continue;
// 		glBegin(GL_LINES);
// 			setColor(E->point1->dist_cam);
// 			glVertex3f(
// 				E->point1->projected[0],
// 				E->point1->projected[1],
// 				E->point1->projected[2]
// 			);
// 			setColor(E->point2->dist_cam);
// 			glVertex3f(
// 				E->point2->projected[0],
// 				E->point2->projected[1],
// 				E->point2->projected[2]
// 			);
// 		glEnd();
		lastE = E;
		drawCylinder(E->point1->projected, E->point1->dist_cam, E->point2->projected, E->point2->dist_cam);
	}
	// Points
	for(Point *P: points) {
		if(!P->visible)
			continue;
		setColor(P->dist_cam);
		// cout << P->projected(0,0) << ' ' << P->projected[1] << ' ' << P->projected[2] << ' ' << P->projected[3] << endl;
		drawSphere(
			P->projected[0],
			P->projected[1],
			P->projected[2],
			P->dist_cam
		);
	}
	/*
	glColor4f(1, 1, 0, 0.5);
	
	glBegin(GL_TRIANGLES);
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
		glVertex3f(1,1,0);
	glEnd();
	*/
	// Axes
	while(1) {
	break;
	Vec3D offset(-.5,-.5,-.5,-.5); 
	float l = .5;
	Vec3D O = camera.apply(offset);
	if(O[3]==0.) break;
	O *= 2./O[3];
	Vec3D X = camera.apply(offset+Vec3D(l,0,0,0));
	if(X[3]==0.) break;
	X *= 2./X[3];
	Vec3D Y = camera.apply(offset+Vec3D(0,l,0,0));
	if(Y[3]==0.) break;
	Y *= 2./Y[3];
	Vec3D Z = camera.apply(offset+Vec3D(0,0,l,0));
	if(Z[3]==0.) break;
	Z *= 2./Z[3];
	Vec3D W = camera.apply(offset+Vec3D(0,0,0,l));
	if(W[3]==0.) break;
	W *= 2./W[3];
	
	lastE = NULL;
	glColor3f(1,0,0);
	drawCylinder(O,1,X,1,false);
	glColor3f(0,1,0);
	drawCylinder(O,1,Y,1,false);
	glColor3f(0,0,1);
	drawCylinder(O,1,Z,1,false);
	glColor3f(.5,.5,1);
	drawCylinder(O,1,W,1,false);
	
	}
	float fl = 1.;
	/*
	Vec3D v111(fl,fl,fl);
	Vec3D v110(fl,fl,-fl);
	Vec3D v100(fl,-fl,-fl);
	Vec3D v101(fl,-fl,fl);
	Vec3D v001(-fl,-fl,fl);
	Vec3D v000(-fl,-fl,-fl);
	Vec3D v010(-fl,fl,-fl);
	Vec3D v011(-fl,fl,fl);
	
	glColor4f(1, 1, 0, 0.5);
	glBegin(GL_QUADS);
	glVertex3f(v111[0],v111[1],v111[2]);
	glVertex3f(v110[0],v110[1],v110[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v110[0],v110[1],v110[2]);
	glVertex3f(v111[0],v111[1],v111[2]);
	glEnd();
	
	
	glBegin(GL_QUADS);
	glVertex3f(v111[0],v111[1],v111[2]);
	glVertex3f(v110[0],v110[1],v110[2]);
	glVertex3f(v010[0],v010[1],v010[2]);
	glVertex3f(v011[0],v011[1],v011[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v011[0],v011[1],v011[2]);
	glVertex3f(v010[0],v010[1],v010[2]);
	glVertex3f(v110[0],v110[1],v110[2]);
	glVertex3f(v111[0],v111[1],v111[2]);
	glEnd();
	
	
	glBegin(GL_QUADS);
	glVertex3f(v111[0],v111[1],v111[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v001[0],v001[1],v001[2]);
	glVertex3f(v011[0],v011[1],v011[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v011[0],v011[1],v011[2]);
	glVertex3f(v001[0],v001[1],v001[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v111[0],v111[1],v111[2]);
	glEnd();
	
	
	
	glBegin(GL_QUADS);
	glVertex3f(v011[0],v011[1],v011[2]);
	glVertex3f(v010[0],v010[1],v010[2]);
	glVertex3f(v000[0],v000[1],v000[2]);
	glVertex3f(v001[0],v001[1],v001[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v001[0],v001[1],v001[2]);
	glVertex3f(v000[0],v000[1],v000[2]);
	glVertex3f(v010[0],v010[1],v010[2]);
	glVertex3f(v011[0],v011[1],v011[2]);
	glEnd();
	
	
	glBegin(GL_QUADS);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v000[0],v000[1],v000[2]);
	glVertex3f(v001[0],v001[1],v001[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v001[0],v001[1],v001[2]);
	glVertex3f(v000[0],v000[1],v000[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glEnd();
	
	
	glBegin(GL_QUADS);
	glVertex3f(v111[0],v111[1],v111[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v110[0],v110[1],v110[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(v110[0],v110[1],v110[2]);
	glVertex3f(v100[0],v100[1],v100[2]);
	glVertex3f(v101[0],v101[1],v101[2]);
	glVertex3f(v111[0],v111[1],v111[2]);
	glEnd();
	*/
	// 
	/*
	Face *f = faces[fid];
	for(int i=0;i<4;i++) {
		glColor4f(1, 1, 0, 0.5);
		glBegin(GL_TRIANGLES);
		for(int j=0;j<4;j++) {
			if(i!=j)
				glVertex3f(f->points[j]->projected[0],f->points[j]->projected[1],f->points[j]->projected[2]);
		}
		for(int j=3;j>=0;j--) {
			if(i!=j)
				glVertex3f(f->points[j]->projected[0],f->points[j]->projected[1],f->points[j]->projected[2]);
		}
		glEnd();
	}*/
	
	
	
	glFlush();
	auto stop1 = high_resolution_clock::now();
	
	auto duration1 = duration_cast<microseconds>(stop1 - start1);
	auto duration2 = duration_cast<microseconds>(stop2 - start2);
	// cout<<duration1.count()<<endl<<duration2.count()<<endl<<endl;
}

void idle() {
	usleep(20000);
	check_keys();
	glutPostRedisplay();
}

void keyDown (unsigned char key, int x, int y)
{
    std::cout << "keydown " << key << "\n";
    // Save which key is being pressed
	pressed_keys[key] = true;
	if(key=='<')
		show_all_faces ^= 1;
	if(key=='>') {
		fid ++;
		fid %= faces.size();
	}
		
	if(key=='m'){
		for(Face *F: faces)
			F->normal.print();
	}
}

void keyUp (unsigned char key, int x, int y)
{
    std::cout << "keyup " << key << "\n";
	pressed_keys[key] = false;
}

void initRender() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(1.00, 1.00, 1.00, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, 1.0, 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int lastX=0.,lastY=0.;
void motionFunc(int x,int y) {
	float difX = x - lastX;
	float difY = y - lastY;
	// cout<<difX<<' '<<difY<<endl;
	if(abs(difX)<8 && abs(difY)<8) {
		cam_rot = cam_rot*Vec3D(-difX/200.,difY/200.,1.)*Vec3D(0,0,1.);
		cam_pos = cam_rot*Vec3D(0.,0.,4.)*!cam_rot;
		cam_up = cam_rot*Vec3D(0.,1.,0.)*!cam_rot;
	}
	lastX = x;
	lastY = y;
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
	proj_init();
	glutInit(&argc, argv);                 // Initialize GLUT
	glutInitWindowSize(600, 600);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutIdleFunc(idle); 
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(motionFunc);
    initRender();
	glutMainLoop();           // Enter the infinitely event-processing loop
	return 0;
}
