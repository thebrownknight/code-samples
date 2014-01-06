//
//  raytrace.h
//  MyBabyRayTracer
//
//  Created by Hadoop on 11/13/13.
//  Copyright (c) 2013 Nikhil Venkatesh. All rights reserved.
//

#ifndef __MyBabyRayTracer__raytrace__
#define __MyBabyRayTracer__raytrace__

// platform specific includes
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <iostream>

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512


/******************************************************************/
/*                                  Raytracer declarations                                 */
/******************************************************************/

/* constants */
#define TRUE 1
#define FALSE 0

#define PI 3.14159265358979323846264338327

/* data structures */

typedef struct point {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} point;

/* a vector is just a point */
typedef point vector;

/* a ray is a start point and a direction */
typedef struct ray {
    point* start;
    vector* dir;
} ray;

typedef struct material {
    /* color */
    GLfloat r;
    GLfloat g;
    GLfloat b;
    /* ambient reflectivity */
    GLfloat amb;
} material;

typedef struct color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    /* these should be between 0 and 1 */
} color;

typedef struct sphere {
    point* c;  /* center */
    GLfloat r;  /* radius */
    material* m;
} sphere;

/* functions in raytrace.cpp */
void traceRay(ray*, color*, int);

/* functions in geometry.cpp */
sphere* makeSphere(GLfloat, GLfloat, GLfloat, GLfloat);
point* makePoint(GLfloat, GLfloat, GLfloat);
point* copyPoint(point *);
void freePoint(point *);
void calculateDirection(point*,point*,point*);
void findPointOnRay(ray*,double,point*);
int raySphereIntersect(ray*,sphere*,double*);
void findSphereNormal(sphere*,point*,vector*);

/* functions in light.cpp */
material* makeMaterial(GLfloat, GLfloat, GLfloat, GLfloat);
void shade(point*,vector*,material*,vector*,color*,int);

/* global variables */
extern int width;
extern int height;

#endif /* defined(__MyBabyRayTracer__raytrace__) */
