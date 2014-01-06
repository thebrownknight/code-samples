//
//  constants.h
//  HomeRunDerby
//
//  Created by Hadoop on 12/2/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#ifndef HomeRunDerby_constants_h
#define HomeRunDerby_constants_h

#ifndef min //In VC++ 2008, Ryan got a warning about min redefinition, so let's not redefine it if something else already did; sigh
#define min(a,b) ((a) < (b)? a:b)
#endif

#define FALSE 0
#define TRUE  1

/* define index constants into the colors array */
#define BLACK   0
#define RED     1
#define YELLOW  2
#define MAGENTA 3
#define GREEN   4
#define CYAN    5
#define BLUE    6
#define GREY    7
#define WHITE   8


#define HOUSE    0
#define CUBE     1
#define TESCUBE  2
#define CYLINDER 3
#define SPHERE   4
#define CONE    5
#define TORUS    6
#define MESH     7
#define GEODESIC    8


#define TRANSLATION_MODE 0
#define SCALING_MODE 1
#define ROTATION_MODE 2
#define NONE_MODE 3
#define AXIS_ROTATION_MODE 4
#define REAL_TRANSLATION_MODE 5
#define REAL_SCALING_MODE 6
#define REAL_ROTATION_MODE 7
#define CAM_ZOOM_MODE 8
#define CAM_NEAR_PLANE_MODE 9
#define CAM_FAR_PLANE_MODE 10

#define SPIN_ENABLED 1
#define SPIN_DISABLED 0

/*  Skybox Textures  */
#define SKY_FRONT 0
#define SKY_RIGHT 1
#define SKY_LEFT 2
#define SKY_BACK 3
#define SKY_UP 4
#define SKY_DOWN 5

GLfloat colors [][3] = {
    {0.0, 0.0, 0.0},  /* black   */
    {1.0, 0.0, 0.0},  /* red     */
    {1.0, 1.0, 0.0},  /* yellow  */
    {1.0, 0.0, 1.0},  /* magenta */
    {0.0, 1.0, 0.0},  /* green   */
    {0.0, 1.0, 1.0},  /* cyan    */
    {0.0, 0.0, 1.0},  /* blue    */
    {0.5, 0.5, 0.5},  /* 50%grey */
    {1.0, 1.0, 1.0}   /* white   */
};

/*  Projection  */
#define DEF_ASP 1
#define DEF_DIM 25
#define DEF_TH 340
#define DEF_PH 30
#define DEF_FOV 70
#define DEF_ECX 0
#define DEF_ECY 0
#define DEF_ECZ 0

/*  PROJECTION  */
double asp=DEF_ASP;
double dim=DEF_DIM;
int th=DEF_TH;
int ph=DEF_PH;
int fov=DEF_FOV;
double ecX=DEF_ECX;
double ecY=DEF_ECY;
double ecZ=DEF_ECZ;

/*  COLORS  */
float white[]={1,1,1,1};

/*  TEXTURES  */
int skybox[6];

const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 23;

#endif
