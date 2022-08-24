#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include "matrix.hpp"

using namespace simple_matrix;
using namespace std;

struct Edge;
struct Face;
struct Point {
	matrix pos;
	matrix translated;
	matrix projected;
	bool visible;
	bool facing_cam;
	float dist_cam;
	int ind;
	Point(ifstream&);
	void update();
	void update2();
	vector<Point*> adjacents;
	vector<Face*> faces;
};
struct Edge {
	Point *point1,*point2;
	Edge(ifstream&);
	bool visible();
};

struct Face {
	vector<Point*> points;
	matrix normal;
	matrix normal_projected;
	Face(ifstream&);
	void update();
};

struct Camera{
	matrix conf;
	matrix pos();
};
extern Camera camera;
extern bool show_all_faces;

extern matrix camera_matrix;
extern vector<Point*> points;
extern vector<Edge*> edges;
extern vector<Face*> faces;

float norm4(matrix);
vector<matrix> project();
matrix identity5();
matrix rotXY(float);
matrix rotYZ(float);
matrix rotZX(float);
matrix transl(float,float,float,float);

extern vector<bool> pressed_keys;
void check_keys();

void proj_init();
