//
//  gl_c_main.h
//  HomeRunDerby
//
//  Created by Hadoop on 12/8/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#ifndef __HomeRunDerby__gl_c_main__
#define __HomeRunDerby__gl_c_main__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <glut/glut.h>
#include "ThreeDShape.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix4x4.cpp"
#include "Textures.h"

void glut_setup(void) ;
void gl_setup(void) ;
void my_init(int argc, char **argv);
void my_setup(int argc, char **argv);
void myabort(void);
void my_display(void) ;
void my_mouse(int button, int state, int mousex, int mousey) ;
void my_mouse_drag (int mousex, int mousey);
void my_reshape(int w, int h) ;
void my_special_keys(int key, int x, int y) ;
void my_keyboard_up( unsigned char key, int x, int y ) ;
void my_keyboard( unsigned char key, int x, int y ) ;
void my_idle(void) ;
void my_TimeOut(int id) ;

void normalize(GLfloat *p);

void lighting_setup();
void print_matrix(float my_matrix[]);

void read_spec(char *fname) ;
void parse_nums(char *buffer, GLfloat nums[]) ;
void parse_bldg(char *buffer);
void parse_light(char *buffer);

void gen_vertices(void);
void my_mult_pt(GLfloat *p);

void draw_axes( void );

#endif /* defined(__HomeRunDerby__gl_c_main__) */
