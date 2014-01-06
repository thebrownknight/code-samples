//
//  Textures.h
//  HomeRunDerby
//
//  Created by Hadoop on 12/9/13.
//  Copyright (c) 2013 HomeRunDerby. All rights reserved.
//

#ifndef __HomeRunDerby__Textures__
#define __HomeRunDerby__Textures__

#include <iostream>
/*  OpenGL and friends  */
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

unsigned int loadTexBMP(char* file, bool tile);

#endif /* defined(__HomeRunDerby__Textures__) */
