#include <GL/glut.h>
#include <math.h>
#include <unistd.h>

#include <chrono>
#include "projection4d.hpp"
using namespace std::chrono;

using Vec3D = vsr::euclidean_vector<4,float>;
using Rot3D = vsr::euclidean_rotor<4,float>;

struct finalPoint {
	Vec3D pos;
	vector<Face*> faces;
};
vector<finalPoint*> fpoints;
struct finalEdge {
	finalPoint* p1;
	finalPoint *p2;
};

int fid = 0;
float t=0.;
float lightX = 0.0, lightY = 20.0, lightZ = 10.0;
Vec3D cam_pos(4.,0.,0.);
Vec3D cam_up(0.,1.,0.);
Rot3D cam_rot(1);

void setColor(float dist) {
	dist = 1/(1+exp(1.5*dist-5));
	// cout << dist << endl;
	glColor3f(dist*1.+.00, dist*.8+.00, 0.00);
}
void setColor(float dist, float alpha) {
	dist = 1/(1+exp(1.5*dist-5));
	glColor4f(dist*1.+.00, dist*.8+.00, 0.00, alpha);
}
void drawSphere(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColor(s);
	glutSolidSphere(.25*.13/s,8,8);
	glPopMatrix();
}
Vec3D cross(Vec3D a, Vec3D b) {
    return Vec3D(a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
Edge *lastE = NULL;
float radius = 0.012;
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
	// static const float radius = .012;
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
			glNormal3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n),0);
			i++;
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), 0);
			glNormal3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n),0);
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), height);
			glNormal3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n),0);
			i--;
		    glVertex3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n), height);
			glNormal3f(radius*cos(2*PI*i/n), radius*sin(2*PI*i/n),0);
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
	
	fpoints.clear();
	for(Edge *e: edges) {
		if( !(
			(e->point1->pos[3]>W && e->point2->pos[3]<W) ||
			(e->point1->pos[3]<W && e->point2->pos[3]>W)
		) )
			continue;
		finalPoint *fp = new finalPoint();
		t = 1-(W - e->point2->pos[3])/(e->point1->pos[3] - e->point2->pos[3]);
		fp->pos = e->point1->pos*(1-t) + e->point2->pos*t;
		
		for(Face *f:faces)
			for(Edge *e2:f->edges)
				if(e==e2)
					fp->faces.push_back(f);
		
		fpoints.push_back(fp);
	}
	cout<<"W = "<<W<<'\n';
	for(finalPoint *fp: fpoints) {
		drawSphere( fp->pos[0],fp->pos[1],fp->pos[2],2. );
		for(Face *f: fp->faces)
			for(finalPoint *fp2: fpoints)
				for(Face *f2: fp2->faces)
					if(f==f2)
						drawCylinder(fp->pos,1,fp2->pos,1);
	}
	
	// Axes
	radius = 0.004;
	float u = 1.;
	for(int i=-1;i<=1;i++)
		for(int j=-1;j<=1;j++)
			{
				Vec3D x1 = Vec3D(u*i,u*j,-u);
				Vec3D x2 = Vec3D(u*i,u*j, u);
				glColor3f(1,0,0);
				drawCylinder(x1,1,x2,1,false);
				x1 = Vec3D(u*i,-u,u*j);
				x2 = Vec3D(u*i, u,u*j);
				glColor3f(0,1,0);
				drawCylinder(x1,1,x2,1,false);
				x1 = Vec3D(-u,u*i,u*j);
				x2 = Vec3D( u,u*i,u*j);
				glColor3f(0,0,1);
				drawCylinder(x1,1,x2,1,false);
			}
	radius = 0.007;
	
	glFlush();
	auto stop1 = high_resolution_clock::now();
	
// 	auto duration1 = duration_cast<microseconds>(stop1 - start1);
// 	auto duration2 = duration_cast<microseconds>(stop2 - start2);
	// cout<<duration1.count()<<endl<<duration2.count()<<endl<<endl;
}

void idle() {
	usleep(20000);
	check_keys();
	glutPostRedisplay();
}

void keyDown (unsigned char key, int x, int y)
{
    // std::cout << "keydown " << key << "\n";
    // Save which key is being pressed
	pressed_keys[key] = true;
	/*if(key=='<')
		show_all_faces ^= 1;
	if(key=='>') {
		fid ++;
		fid %= faces.size();
	}
		
	if(key=='m'){
		for(Face *F: faces)
			F->normal.print();
	}*/
}

void keyUp (unsigned char key, int x, int y)
{
    // std::cout << "keyup " << key << "\n";
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
