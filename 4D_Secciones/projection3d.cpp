#include "projection3d.hpp"

float norm4(matrix v) {
	float sum = 0.;
	v /= v(4,0);
	for(int i=0;i<4;i++)
		sum += v(i,0)*v(i,0);
	return sqrt(sum);
}

Point::Point(ifstream &stream) {
	pos = matrix(5,1);
	stream >> pos(0,0) >> pos(1,0) >> pos(2,0) >> pos(3,0);
	pos(4,0) = 1.;
	ind = points.size();
}
void Point::update() {
	translated = camera.conf * pos;
	dist_cam = norm4(translated);
	visible = translated(3,0)/translated(4,0)>=.5;
	projected = camera_matrix * translated;
	projected /= projected(3,0);
}
void Point::update2() {
	
}

Edge::Edge(ifstream &stream) {
	int p1,p2;
	stream >> p1 >> p2;
	point1 = points[p1];
	point2 = points[p2];
	point1->adjacents.push_back(point2);
	point2->adjacents.push_back(point1);
}
bool Edge::visible() {
	return point1->visible && point2->visible;
}

Face::Face(ifstream &stream){
	int p;
	Point *point;
	while(true) {
		stream >> p;
		if(p==-1)
			break;
		point = points[p];
		points.push_back(point);
		point->faces.push_back(this);	
	}	
}

matrix Camera::pos() {
	matrix m = identity5();
	for(int i=0;i<5;i++)
		m(i,4) = conf(i,4);
	return m;
}

Camera camera;
matrix camera_matrix;
int n_points;
int n_edges;
int n_faces;
vector<Point*> points;
vector<Edge*> edges;
vector<Face*> faces;
bool show_all_faces = true;

matrix identity5() {
	return matrix(5,5,{
		1,0,0,0,0,
		0,1,0,0,0,
		0,0,1,0,0,
		0,0,0,1,0,
		0,0,0,0,1
	});
}
matrix rotXY(float ang) {
	matrix R = identity5();
	R(1,1) = cos(ang);
	R(1,2) =-sin(ang);
	R(2,1) = sin(ang);
	R(2,2) = cos(ang);
	return R;
}
matrix rotYZ(float ang) {
	matrix R = identity5();
	R(0,0) = cos(ang);
	R(0,2) = sin(ang);
	R(2,0) =-sin(ang);
	R(2,2) = cos(ang);
	return R;
}
matrix rotZX(float ang) {
	matrix R = identity5();
	R(0,0) = cos(ang);
	R(0,2) = sin(ang);
	R(2,0) =-sin(ang);
	R(2,2) = cos(ang);
	return R;
}
matrix transl(float x, float y, float z, float w) {
	matrix R = identity5();
	R(0,4) = x;
	R(1,4) = y;
	R(2,4) = z;
	R(3,4) = w;
	return R;
}


vector<matrix> project() {
	vector<matrix> out;
	/*matrix invcam = camera.conf;
	matrix cam_pos = camera.pos();
	for(Point point: points) {
		matrix v = invcam * point.pos;
		float dist = norm3(v);
		if(v(2,0)/v(3,0)<.5)
			continue;
		v = camera_matrix * v;
		v /= v(2,0);
		v(2,0) = 1. * dist;
		out.push_back(v);
	}*/
	return out;
}

// Camera control
vector<bool> pressed_keys(127,false);
void check_keys() {
// X axis
	if(pressed_keys['1'])
	    camera.conf *= transl(-.001,0.,0.,0.);
	if(pressed_keys['q'])
	    camera.conf *= transl(0.001,0.,0.,0.);
	if(pressed_keys['a'])
	    camera.conf *= rotYZ(0.001);
	if(pressed_keys['z'])
	    camera.conf *= rotYZ(-0.001);
// Y axis
	if(pressed_keys['2'])
	    camera.conf *= transl(0.,-.001,0.,0.);
	if(pressed_keys['w'])
	    camera.conf *= transl(0.,0.001,0.,0.);
	if(pressed_keys['s'])
	    camera.conf *= rotZX(0.001);
	if(pressed_keys['x'])
	    camera.conf *= rotZX(-0.001);
// Z axis
	if(pressed_keys['3'])
	    camera.conf *= transl(0.,0.,-.001,0.);
	if(pressed_keys['e'])
	    camera.conf *= transl(0.,0.,0.001,0.);
	if(pressed_keys['d'])
	    camera.conf *= rotXY(-.001);
	if(pressed_keys['c'])
	    camera.conf *= rotXY(0.001);
// W axis
	if(pressed_keys['4'])
	    camera.conf *= transl(0.,0.,0.,-.001);
	if(pressed_keys['r'])
	    camera.conf *= transl(0.,0.,0.,0.001);
}

void proj_init() {
	// Read data
	ifstream data_file("data.txt");
	data_file >> n_points;
	for(int i=0;i<n_points;i++) {
		points.push_back( new Point(data_file) );
	}
	data_file >> n_edges;
	for(int i=0;i<n_edges;i++) {
		edges.push_back( new Edge(data_file) );
	}
	data_file.close();
	// Init camera
	camera.conf = identity5();
	camera_matrix = matrix(4,5,{
		2,0,0,0,0,
		0,2,0,0,0,
		0,0,2,0,0,
		0,0,0,1,0
	});
}
