void drawCylinder(float x1, float y1, float z1, float s1, float x2, float y2, float z2, float s2) {
  int X = 0,Y = 1,Z = 2;
  // Get components of difference vector
  float x = x1-x2;
  float y = y1-y2,
        z = z1-z2;
  float[] firstPerp = getFirstPerpVector(x,y,z);
  // Get the second perp vector by cross product
  float[] secondPerp = new float[3];
  secondPerp[X] = y*firstPerp[Z]-z*firstPerp[Y];
  secondPerp[Y] = z*firstPerp[X]-x*firstPerp[Z];
  secondPerp[Z] = x*firstPerp[Y]-y*firstPerp[X];
  // Normalize vector
  float length = 0.0f;
  for (float f : secondPerp)
    length += f*f;
  length = (float) Math.sqrt(length);
  for (int i=0; i<3; i++)
    secondPerp[i] /= length;

  // Having now our vectors, here we go:
  // First points; you can have a cone if you change the radius R1

  final int ANZ = 32;  // number of vertices
  final float FULL = (float) (2.0f*Math.PI),
              R1   = 4.0f; // radius
  float[][] points = new float[ANZ+1][3];
  for (int i=0; i<ANZ; i++) {
    float angle = FULL*(i/(float) ANZ);

    points[i][X] = (float) (R1*(Math.cos(angle)*firstPerp[X]+Math.sin(angle)*secondPerp[X]));
    points[i][Y] = (float) (R1*(Math.cos(angle)*firstPerp[Y]+Math.sin(angle)*secondPerp[Y]));
    points[i][Z] = (float) (R1*(Math.cos(angle)*firstPerp[Z]+Math.sin(angle)*secondPerp[Z]));
  }
  // Set last to first
  System.arraycopy(points[0],0,points[ANZ],0,3);

  gl.glColor3f(1.0f,0.0f,0.0f);
  gl.glBegin(GL.GL_TRIANGLE_FAN);
  gl.glVertex3f(x1,y1,z1);
  for (int i=0; i<=ANZ; i++) {
    gl.glVertex3f(x1+points[i][X],
                  y1+points[i][Y],
                  z1+points[i][Z]);
  }
  gl.glEnd();

  gl.glBegin(GL.GL_TRIANGLE_FAN);
  gl.glVertex3f(x2,y2,z2);
  for (int i=0; i<=ANZ; i++) {
    gl.glVertex3f(x2+points[i][X],
                  y2+points[i][Y],
                  z2+points[i][Z]);
  }
  gl.glEnd();

  gl.glBegin(GL.GL_QUAD_STRIP);
  for (int i=0; i<=ANZ; i++) {
    gl.glVertex3f(x1+points[i][X],
                  y1+points[i][Y],
                  z1+points[i][Z]);
    gl.glVertex3f(x2+points[i][X],
                  y2+points[i][Y],
                  z2+points[i][Z]);
  }
  gl.glEnd();      
}
