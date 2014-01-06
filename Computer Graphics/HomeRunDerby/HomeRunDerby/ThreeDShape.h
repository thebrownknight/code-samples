/*******************************************************
 FILE: ThreeDShape.h
 AUTHOR: Nikhil Venkatesh
 DATE: 09/20/13
 DOES: Sets up important utility classes and methods,
 and defines an abstract class, ThreeDShape that is
 the base class for the shapes being drawn.
 PLATFORM: tested on osx
 ********************************************************/
#ifndef __CameraModeler__ThreeDShape__
#define __CameraModeler__ThreeDShape__

#include <stdlib.h>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glut/glut.h>
#include "Vector4D.h"
#include "Matrix4x4.h"
#include "constants.h"

#define MAX_RS 50
#define MAX_VS 50

#define PI 3.141592653
#define PIOver2 PI/2
#define TWOPI 6.283185308

/* Utility class to hold our Vertex coordinates. */
class ThreeDVertex {
public:
    ThreeDVertex(): x(0), y(0), z(0), w(1) { }
    ThreeDVertex(float x_coord, float y_coord, float z_coord, float weight): x(x_coord), y(y_coord), z(z_coord), w(weight) { }
    float x;
    float y;
    float z;
    float w = 1;
    
    GLfloat* makeArray() {
        GLfloat *a_ret = new GLfloat[4];
        a_ret[0] = x;
        a_ret[1] = y;
        a_ret[2] = z;
        a_ret[3] = w;
        return a_ret;
    }
};

/* Utility class to hold the faces of a shape - holds 4 (row, column) vertex indices that will point into the larger vertices array. */
class QuadFace {
public:
    int first_point[2];
    int second_point[2];
    int third_point[2];
    int fourth_point[2];
    QuadFace(int f_point[], int s_point[], int t_point[], int r_point[]) {
        std::copy(f_point, f_point + 2, first_point);
        std::copy(s_point, s_point + 2, second_point);
        std::copy(t_point, t_point + 2, third_point);
        std::copy(r_point, r_point + 2, fourth_point);
    }
};

/* Set up an abstract class to hold shape information. */
class ThreeDShape {
private:
    bool draw_vert_normals;
protected:
    enum Direction {
        X,
        Y,
        Z
    };
public:
    double height, radius, radius2;
    int r_slices, v_stacks, crt_render_mode;
    Vector4D** vertices;  // a multidimensional array of pointers to ThreeDVertex structures initialized
    std::vector<QuadFace*> faces;     //vector containing pointers to QuadFace objects - these contain the row/column indices for the vertices array
    std::vector<Vector4D> face_normals;     //vector containing the an array of ThreeDVertexes
    Vector4D** vert_normals;    //multidimensional array that holds the vertex normals for each vertex
    
    //bounding box top left, top right, bottom left, bottom right
    float lx, rx, ty, by, fz, bz;
    // Object center position
    Vector3D obj_center = Vector3D(0.0,0.0,0.0);
    
    // matrix that holds the cumulative transformation for the object - initialize to identity matrix
    //    GLfloat ctm_matrix[4][4] = {
    //        {1, 0, 0, 0},
    //        {0, 1, 0, 0},
    //        {0, 0, 1, 0},
    //        {0, 0, 0, 1}
    //    };
    Matrix4x4 ctm_matrix = Matrix4x4();    // initialize with identity matrix
    // store the local axis coordinates for the object
    GLfloat x_axis = 0,  y_axis = 1,  z_axis = 0;
    
    /* Constructor for the abstract class that takes care of initializations. */
    ThreeDShape(double height, double radius, double radius2, int rs, int vs): height(height), radius(radius), radius2(radius2), r_slices(rs), v_stacks(vs) {
        vertices = new Vector4D*[MAX_RS];
        for (int i = 0; i < MAX_RS; ++i) {
            vertices[i] = new Vector4D[MAX_VS];
            for (int k = 0; k < MAX_VS; ++k) {
                vertices[i][k] = Vector4D();    // initialize the vertices array to Vector4D objects (0,0,0,1)
            }
        }
        
        vert_normals = new Vector4D*[MAX_RS];
        for (int i = 0; i < MAX_RS; ++i) {
            vert_normals[i] = new Vector4D[MAX_VS];
            for (int k = 0; k < MAX_VS; ++k) {
                vert_normals[i][k] = Vector4D();
            }
        }
        
        obj_center.setX(0);
        obj_center.setY(0);
        obj_center.setZ(0);
    }
    
    // Abstract methods
    virtual void make_shape() = 0;
    virtual void setBoundingBox() = 0;
    virtual Vector3D intersect(Vector3D p, Vector3D d) = 0;
    // virtual destructor
    virtual ~ThreeDShape() { }
    
    /************************************************/
    /*                       Internal Functions                      */
    /************************************************/
    
    double intersectPlane(std::string axis, double offset, Vector3D p, Vector3D d)
    {
        if(axis == "x"){
            return (offset - p.getX()) / d.getX();
        }
        else if(axis == "y"){
            return (offset - p.getY()) / d.getY();
        }
        else if(axis == "z"){
            return (offset - p.getZ()) / d.getZ();
        }
        else{
            std::cerr << "axis not specified" << std::endl;
            return -1;
        }
    }
    
    /*
     * Method name: addFaces()
     * Description: Adds the faces of the shape to the faces vector - the faces are represented by a QuadFace object
     * Params: (none)
     * Returns: nothing
     */
    void addFaces() {
        faces = std::vector<QuadFace*>();   //deallocate any previous faces that were calculated
        
        for (int m=0; m < r_slices; m++) {
            for (int l=0; l < v_stacks; l++) {
                int f_point[2] = { m, l };
                int s_point[2] = { m+1, l };
                int t_point[2] = { m+1, l +1};
                int r_point[2] = { m, l+1 };
                QuadFace *qf = new QuadFace(f_point, s_point, t_point, r_point);    // create a new QuadFace object containing four different points to connect
                faces.push_back(qf);
            }
        }
        
    }
    
    /*
     * Method name: calculateFaceNormals()
     * Description: Calculates the surface normals of the FACES of the shape.
     * Params: (none)
     * Returns: nothing
     */
    void calculateFaceNormals() {
        face_normals = std::vector<Vector4D>();
        for (int i=0; i < faces.size(); i++) {
            //Vector4D p0 = vertices[faces[i]->first_point[0]][faces[i]->first_point[1]];
            Vector4D p1 = vertices[faces[i]->second_point[0]][faces[i]->second_point[1]];  //adjacent
            Vector4D p2 = vertices[faces[i]->third_point[0]][faces[i]->third_point[1]];    //adjacent
            Vector4D p3 = vertices[faces[i]->fourth_point[0]][faces[i]->fourth_point[1]];    //adjacent
            
            //Apply the surface normal formula - take two sets of perpendicular vectors in the counter-clockwise direction,
            // subtract each set, and take the cross product
            Vector4D f_sub = (p2 - p1);
            Vector4D s_sub = (p3 - p2);
            face_normals.push_back(f_sub.cross(s_sub));
        }
        
        // Normalize each of the surface normals
        for (int k=0; k < face_normals.size(); k++) {
            face_normals[k].normalize();
        }
    }
    
    /*
     * Method name: draw_param_quad()
     * Description: Does the actual rendering of the shape using the vertices from the internal vertices array.
     Also handles drawing the vertex normals, if the user chooses to display them.
     * Params: the lines and column numbers of the 4 vertices of a face
     the color to draw the shape
     * Returns: nothing
     */
    void draw_param_quad(int line1, int col1, int line2, int col2, int line3, int col3, int line4, int col4, int ic) {
        GLfloat testAr[][4] = {
            {vertices[line1][col1].getX(), vertices[line1][col1].getY(), vertices[line1][col1].getZ(), vertices[line1][col1].getW() },
            {vertices[line2][col2].getX(), vertices[line2][col2].getY(), vertices[line2][col2].getZ(), vertices[line2][col2].getW() },
            {vertices[line3][col3].getX(), vertices[line3][col3].getY(), vertices[line3][col3].getZ(), vertices[line3][col3].getW() },
            {vertices[line4][col4].getX(), vertices[line4][col4].getY(), vertices[line4][col4].getZ(), vertices[line4][col4].getW() }
        };
        
        //        Vector4D* vert_normal_indexes[4] = {
        //            vert_normals[line1][col1],
        //            vert_normals[line2][col2],
        //            vert_normals[line3][col3],
        //            vert_normals[line4][col4]
        //        };
        
        if(draw_vert_normals) {
            
            GLfloat endpt[4];
            glBegin(GL_LINES);
            {
                glColor3fv(colors[ic]);
                //start at the vertex
                glVertex4f(vertices[line1][col1].getX(), vertices[line1][col1].getY(), vertices[line1][col1].getZ(), vertices[line1][col1].getW());
                
                //walk 0.2 in the direction of the normal
                endpt[0] = vertices[line1][col1].getX() + 0.2 * vert_normals[line1][col1].getX();
                endpt[1] = vertices[line1][col1].getY() + 0.2 * vert_normals[line1][col1].getY();
                endpt[2] = vertices[line1][col1].getZ() + 0.2 * vert_normals[line1][col1].getZ();
                endpt[3] = 1.0;
                
                // plot the end
                glVertex4fv(endpt);
            }
            glEnd();
        }
        
        glBegin(crt_render_mode);
        {
            glColor3fv(colors[ic]);
            glNormal3f(vert_normals[line1][col1].getX(), vert_normals[line1][col1].getY(), vert_normals[line1][col1].getZ());
            glVertex4fv(testAr[0]);
            glNormal3f(vert_normals[line2][col2].getX(), vert_normals[line2][col2].getY(), vert_normals[line2][col2].getZ());
            glVertex4fv(testAr[1]);
            glNormal3f(vert_normals[line3][col3].getX(), vert_normals[line3][col3].getY(), vert_normals[line3][col3].getZ());
            glVertex4fv(testAr[2]);
            glNormal3f(vert_normals[line4][col4].getX(), vert_normals[line4][col4].getY(), vert_normals[line4][col4].getZ());
            glVertex4fv(testAr[3]);
        }
        glEnd();
        
        //            // Loop through the testAr array, retrieving four arrays of x,y,z, and w values for each vertex
        //            // Draw the lines for the vertex normals
        //            for (int y = 0; y < 4; y++){
        //                glBegin(GL_LINES);
        //                glVertex4fv(testAr[y]);
        //                //glVertex4fv(calcNewVertex(testAr[y], vert_normal_indexes, y));
        //                glEnd();
        //            }
        //        }
    }
    
    /***********************************************/
    /*                       Public Functions                      */
    /***********************************************/
    
    // Setters and getters
    int getSlices() { return r_slices; }
    void setSlices(int new_slices) { r_slices = new_slices; }
    
    int getStacks() { return v_stacks; }
    void setStacks(int new_stacks) { v_stacks = new_stacks; }

    /*
     * Method name: calculateVertexNormals()
     * Description: Calculates the vertex normals using the calculated surface normals for a face and averaging them.
     * Params: bool draw_vert_norms - internal boolean that keeps track of whether the user wants to draw the vertex normals.
     * Returns: nothing
     */
    void calculateVertexNormals( bool draw_vert_norms) {
        draw_vert_normals = draw_vert_norms;    // set the internal boolean to true, the user wants to draw the vertex normals for the shape
        calculateFaceNormals();     // call the internal method to calculate the face normals
        
        int nbAdjFaces = 0;
        /* This algorithm basically goes through each of the faces of the shape, looks at each vertex and determines which faces intersect at that vertex.
         For each vertex, I calculate the average of the face normals, normalize this vector, and then add it to the vert_normals array that contains the
         vertex normals for each vertex.
         */
        for (int i = 0; i < r_slices; i++) {
            for (int k = 0; k < v_stacks; k++) {
                nbAdjFaces = 0;
                for (int l = 0; l < faces.size(); l++) {
                    if (vertices[faces[l]->first_point[0]][faces[l]->first_point[1]] == vertices[i][k]) {
                        nbAdjFaces++;
                        vert_normals[i][k] += face_normals[l];
                    }
                    else
                        if (vertices[faces[l]->second_point[0]][faces[l]->second_point[1]] == vertices[i][k]) {
                            nbAdjFaces++;
                            vert_normals[i][k] += face_normals[l];
                        }
                        else
                            if (vertices[faces[l]->third_point[0]][faces[l]->third_point[1]]  == vertices[i][k]) {
                                nbAdjFaces++;
                                vert_normals[i][k] += face_normals[l];
                            }
                            else
                                if (vertices[faces[l]->fourth_point[0]][faces[l]->fourth_point[1]]  == vertices[i][k]) {
                                    nbAdjFaces++;
                                    vert_normals[i][k] += face_normals[l];
                                }
                }
                vert_normals[i][k] /= (float) nbAdjFaces;
                
                vert_normals[i][k].normalize();
            }
        }
    }
    
    /*
     * Method name: draw_shape()
     * Description: Calls the internal draw_param_quad method to draw the shape, looping through the faces vector.
     * Params: int render_mode - the rendering mode GL_LINE_LOOP or GL_POLYGON
     const int color - the color to draw the shape
     * Returns: nothing
     */
    void draw_shape(int render_mode, const int color){
        crt_render_mode = render_mode;
        //if (draw_vert_normals)
        //calculateVertexNormals(true);
        for (int t=0; t < faces.size(); t++) {
            draw_param_quad(faces[t]->first_point[0], faces[t]->first_point[1] , faces[t]->second_point[0], faces[t]->second_point[1],
                            faces[t]->third_point[0], faces[t]->third_point[1], faces[t]->fourth_point[0], faces[t]->fourth_point[1], color);
        }
    }
    
    /************************************************/
    /*                       Utility Functions                        */
    /***********************************************/
    
    /*
     * Method name: rotate()
     * Description: Performs matrix multiplication using the rotation matrices.
     * Params: int direction - the axis about which to rotate the vertex
     *              ThreeDVertex* tdv - a pointer to a ThreeDVertex object that is the vertex being rotated
     *              float angle - the angle of rotation
     * Returns: a pointer to a new ThreeDVertex object that represents the vertex after rotation
     */
    ThreeDVertex* rotate (int direction, ThreeDVertex* tdv, float angle) {
        ThreeDVertex *ret_val = new ThreeDVertex(0,0,0,1);      // create a new default ThreeDVertex object
        GLfloat* temp = new GLfloat[4];     //temporary array that holds the calculated values
        GLfloat* input_ar = tdv->makeArray();   // convert the input ThreeDVertex to an array of floats (x,y,z,w)
        GLfloat rot_x_matrix[4][4] = {
            { 1, 0, 0, 0 },
            { 0, cosf(angle), -1*sinf(angle), 0 },
            { 0, sinf(angle), cosf(angle), 0 },
            { 0, 0, 0, 1 }
        };
        
        GLfloat rot_y_matrix[4][4] = {
            { cosf(angle), 0, sinf(angle), 0},
            { 0, 1, 0, 0},
            { -1*sinf(angle), 0, cosf(angle), 0},
            { 0, 0, 0, 1}
        };
        
        GLfloat rot_z_matrix[4][4] = {
            { cosf(angle), -1 * sinf(angle), 0, 0 },
            { sinf(angle), cosf(angle), 0, 0},
            { 0, 0, 1, 0},
            { 0, 0, 0, 1}
        };
        
        switch (direction) {
            case Direction::X:
                for (int i = 0; i < 4; i++) {
                    temp[i] = 0;
                    for (int j = 0; j < 4; j++) {
                        temp[i] = temp[i] + rot_x_matrix[i][j] * input_ar[j];
                    }
                }
                break;
            case Direction::Y:
                for (int i = 0; i < 4; i++) {
                    temp[i] = 0;
                    for (int j = 0; j < 4; j++) {
                        temp[i] = temp[i] + rot_y_matrix[i][j] * input_ar[j];
                    }
                }
                break;
            case Direction::Z:
                for (int i = 0; i < 4; i++) {
                    temp[i] = 0;
                    for (int j = 0; j < 4; j++) {
                        temp[i] = temp[i] + rot_z_matrix[i][j] * input_ar[j];
                    }
                }
                break;
            default:
                break;
        }
        
        /* Populate the ThreeDVertex object we are returning. */
        ret_val->x = temp[0];
        ret_val->y = temp[1];
        ret_val->z = temp[2];
        ret_val->w = temp[3];
        return ret_val;
    }
    
    /*
     * Method name: translate()
     * Description: Performs matrix multiplication using the translation matrices.
     * Params: ThreeDVertex* tdv - a pointer to a ThreeDVertex object that is the vertex being translated
     *              float dx - the amount translated in the x-direction
     *              float dy - the amount translated in the y-direction
     *              float dz - the amount translated in the z-direction
     * Returns: a pointer to a new ThreeDVertex object that represents the vertex after translation
     */
    ThreeDVertex* translate (ThreeDVertex* tdv, float dx, float dy, float dz) {
        ThreeDVertex *ret_val = new ThreeDVertex(0,0,0,1);      // create a new default ThreeDVertex object
        GLfloat* temp = new GLfloat[4];     //temporary array that holds the calculated values
        GLfloat* input_ar = tdv->makeArray();   // convert the input ThreeDVertex to an array of floats (x,y,z,w)
        // the translation matrix
        GLfloat trans_matrix[4][4] = {
            { 1, 0, 0, dx },
            { 0, 1, 0, dy },
            { 0, 0, 1, dz },
            { 0, 0, 0, 1 }
        };
        
        for (int i=0; i < 4; i++) {
            temp[i] = 0;
            for (int j = 0; j < 4; j++){
                temp[i] = temp[i] + trans_matrix[i][j] * input_ar[j];
            }
        }
        
        /* Populate the ThreeDVertex object we are returning. */
        ret_val->x = temp[0];
        ret_val->y = temp[1];
        ret_val->z = temp[2];
        ret_val->w = temp[3];
        
        return ret_val;
    }
    
    /*
     * Method name: scale()
     * Description: Performs matrix multiplication using the scaling matrices.
     * Params: ThreeDVertex* tdv - a pointer to a ThreeDVertex object that is the vertex being translated
     *              float sx - the amount to scale in the x-direction
     *              float sy - the amount to scale in the y-direction
     *              float sz - the amount to scale in the z-direction
     * Returns: a pointer to a new ThreeDVertex object that represents the vertex after scaling.
     */
    ThreeDVertex* scale (ThreeDVertex* tdv, float sx, float sy, float sz) {
        ThreeDVertex *ret_val = new ThreeDVertex(0,0,0,1);      // create a new default ThreeDVertex object
        GLfloat* temp = new GLfloat[4];     //temporary array that holds the calculated values
        GLfloat* input_ar = tdv->makeArray();   // convert the input ThreeDVertex to an array of floats (x,y,z,w)
        
        // the scaling matrix
        GLfloat scale_matrix[4][4] = {
            { sx, 0, 0, 0 },
            { 0, sy, 0, 0 },
            { 0, 0, sz, 0 },
            { 0, 0, 0, 1 }
        };
        
        float temp_x = tdv->x;
        float opp_x = -1 * tdv->x;
        float temp_y = tdv->y;
        float opp_y = -1 * tdv->y;
        float temp_z = tdv->z;
        float opp_z = -1 * tdv->z;
        
        // negative translation first
        GLfloat neg_trans_matrix[4][4] = {
            { 1, 0, 0, opp_x },
            { 0, 1, 0, opp_y },
            { 0, 0, 1, opp_z },
            { 0, 0, 0, 1 }
        };
        // positive translation after the scaling
        GLfloat pos_trans_matrix[4][4] = {
            { 1, 0, 0, temp_x },
            { 0, 1, 0, temp_y },
            { 0, 0, 1, temp_z },
            { 0, 0, 0, 1 }
        };
        
        // perform the matrix multiplication -> when scaling, make sure to translate back to origin first, scale, and then translate back to original pos.
        for (int i=0; i < 4; i++) {
            temp[i] = 0;
            for (int j = 0; j < 4; j++){
                temp[i] = temp[i] + pos_trans_matrix[i][j] * scale_matrix[i][j] * neg_trans_matrix[i][j] * input_ar[j];
            }
        }
        
        /* Populate the ThreeDVertex object we are returning. */
        ret_val->x = temp[0];
        ret_val->y = temp[1];
        ret_val->z = temp[2];
        ret_val->w = temp[3];
        
        return ret_val;
    }
    
    /*
     * Method name: arb_rotation()
     * Description: Performs the rotation about an arbitrary axis.
     * Params: ThreeDVertex* tdv - a pointer to a ThreeDVertex object that is the vertex being translated
     *              float angle - the angle of rotation
     *              float ax - the amount translated in the x-direction
     *              float ay - the amount translated in the y-direction
     *              float az - the amount translated in the z-direction
     * Returns: a pointer to a new ThreeDVertex object that represents the vertex after rotation
     */
    ThreeDVertex* arb_rotation (ThreeDVertex* tdv, float angle, float ax, float ay, float az) {
        ThreeDVertex *ret_val = new ThreeDVertex(0,0,0,1);      // create a new default ThreeDVertex object
        GLfloat* temp = new GLfloat[4];     //temporary array that holds the calculated values
        ThreeDVertex *temp_vertex = normalize(new ThreeDVertex(ax, ay, az, 1));   // normalize the axis first
        GLfloat* input_ar = tdv->makeArray();   // convert the input ThreeDVertex to an array of floats (x,y,z,w)
        
        // create the monster matrix (Rodrigues' Formula)
        GLfloat M[4][4];
        GLfloat axis[3] = { temp_vertex->x, temp_vertex->y, temp_vertex->z };
        M[0][0] = powf(axis[0], 2) + cosf(angle) * (1 - powf(axis[0], 2));
        M[0][1] = axis[0] * (axis[1] * (1 - cosf(angle))) - axis[2] * sinf(angle);
        M[0][2] = axis[2] * (axis[0] * (1 - cosf(angle))) + axis[1] * sinf(angle);
        M[0][3] = 0;
        
        M[1][0] = axis[0] * (axis[1] * (1 - cosf(angle))) + axis[2] * sinf(angle);
        M[1][1] = powf(axis[1], 2) + cosf(angle)*(1 - powf(axis[1], 2));
        M[1][2] = axis[1] * (axis[2] * (1 - cosf(angle))) - axis[0] * sinf(angle);
        M[1][3] = 0;
        
        M[2][0] = axis[2] * (axis[0] * (1 - cosf(angle))) - axis[1] * sinf(angle);
        M[2][1] = axis[1] * (axis[2]*(1 - cosf(angle))) + axis[0] * sinf(angle);
        M[2][2] = powf(axis[2], 2) + cosf(angle)*(1 - powf(axis[2], 2));
        M[2][3] = 0;
        
        M[3][0] = 0;
        M[3][1] = 0;
        M[3][2] = 0;
        M[3][3] = 1;
        
        // perform the matrix multiplication between the Rodrigues matrix and the input vertex
        for (int i=0; i < 4; i++) {
            temp[i] = 0;
            for (int j = 0; j < 4; j++){
                temp[i] = temp[i] + M[i][j] * input_ar[j];
            }
        }
        
        /* Populate the ThreeDVertex object we are returning. */
        ret_val->x = temp[0];
        ret_val->y = temp[1];
        ret_val->z = temp[2];
        ret_val->w = temp[3];
        
        return ret_val;
    }
    
    /*
     * Method name: arb_rotation_point()
     * Description: Performs the rotation about an arbitrary axis (ax, ay, az) located at point C(cx, cy, cz).
     * Params: ThreeDVertex* tdv - a pointer to a ThreeDVertex object that is the vertex being translated
     *              float angle - the angle of rotation
     *              float cx - the arbitrary point's x-value
     *              float cy - the arbitrary point's y-value
     *              float cz - the arbitrary point-s z-value
     *              float ax - the arbitrary axis's x-value
     *              float ay - the arbitrary axis's y-value
     *              float az - the arbitrary axis's z-value
     * Returns: nothing
     */
    void arb_rotation_point (float angle, float cx, float cy, float cz, float ax, float ay, float az) {
        
        real_translation(-1 * cx, -1 * cy, -1 * cz);
        real_rev(angle, ax, ay, az);
        real_translation(cx, cy, cz);
        
    }
    
    /*
     * Method name: real_translation()
     * Description: Performs the real translation of an object.
     * Params:
     *              GLfloat x - the amount translated in the x-direction
     *              GLfloat y - the amount translated in the y-direction
     *              GLfloat z - the amount translated in the z-direction
     * Returns: nothing
     */
    void real_translation(GLfloat x, GLfloat y, GLfloat z) {
        
        ctm_matrix.translate(x, y, z);    // update the current transformation matrix
        
        // loop through the vertices and translate all of them by the cumulative transformation matrix
        for (unsigned int i=0; i <= v_stacks; i++) {
            for (unsigned int j=0; j <= r_slices; j++) {
                //vertices[j][i] = translate(vertices[j][i], x, y, z);
                vertices[j][i].Translate(x, y, z);
            }
        }
        
        //printf("BAT CENTER IN real translation: %f, %f, %f\n", obj_center.getX(), obj_center.getY(), obj_center.getZ());

        obj_center.setX(obj_center.getX() + x);
        obj_center.setY(obj_center.getY() + y);
        obj_center.setZ(obj_center.getZ() + z);
        
        //printf("BAT CENTER IN real translation: %f, %f, %f\n", obj_center.getX(), obj_center.getY(), obj_center.getZ());
        
        setBoundingBox();
        // set the x-, y-, and z-axes
        x_axis += x;
        y_axis += y;
        z_axis += z;
    }
    
    /*
     * Method name: real_rotation()
     * Description: Performs the real rotation of an object.
     * Params: GLfloat deg - the number of degrees to rotate
     *              GLfloat x - if 1, then rotate about the x-axis
     *              GLfloat y - if 1, then rotate about the y-axis
     *              GLfloat z - if 1, then rotate about the z-axis
     * Returns: nothing
     */
    void real_rotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
        
        if (x == 1) {
            // loop through the vertices and rotate all of them by the cumulative transformation matrix
            //temp = matrix_multiply(rot_x_matrix, ctm_matrix);
            ctm_matrix.rotateX(angle);
            
            for (unsigned int i=0; i <= v_stacks; i++) {
                for (unsigned int j=0; j <= r_slices; j++) {
                    vertices[j][i].RotateAboutX(angle);
                }
            }
            obj_center.RotateAboutX(angle);
            // set the y- and z-axes (the x-pos does not move when rotating around it)
            ThreeDVertex* temp1 = rotate(Direction::X, new ThreeDVertex(x_axis, y_axis, z_axis, 1), angle);
            y_axis = temp1->y;
            z_axis = temp1->z;
        }
        if (y == 1) {
            // loop through the vertices and rotate all of them by the cumulative transformation matrix
            ctm_matrix.rotateY(angle);
            
            for (unsigned int i=0; i <= v_stacks; i++) {
                for (unsigned int j=0; j <= r_slices; j++) {
                    vertices[j][i].RotateAboutY(angle);
                }
            }
            obj_center.RotateAboutY(angle);
            // set the x- and z-axes (the y-pos does not move when rotating around it)
            ThreeDVertex* temp1 = rotate(Direction::Y, new ThreeDVertex(x_axis, y_axis, z_axis, 1), angle);
            x_axis = temp1->x;
            z_axis = temp1->z;
        }
        if (z == 1) {
            // loop through the vertices and rotate all of them by the cumulative transformation matrix
            ctm_matrix.rotateZ(angle);
            
            for (unsigned int i=0; i <= v_stacks; i++) {
                for (unsigned int j=0; j <= r_slices; j++) {
                    vertices[j][i].RotateAboutZ(angle);
                }
            }
            obj_center.RotateAboutZ(angle);
            // set the x- and y-axes (the z-pos does not move when rotating around it)
            ThreeDVertex* temp1 = rotate(Direction::Z, new ThreeDVertex(x_axis, y_axis, z_axis, 1), angle);
            x_axis = temp1->x;
            y_axis = temp1->y;
        }
    }
    
    /*
     * Method name: real_scaling()
     * Description: Performs the real translation of an object.
     * Params:
     *              GLfloat sx - the amount scaled in the x-direction
     *              GLfloat sy - the amount scaled in the y-direction
     *              GLfloat sz - the amount scaled in the z-direction
     * Returns: nothing
     */
    void real_scaling(GLfloat sx, GLfloat sy, GLfloat sz) {
        
        ctm_matrix.scale(sx, sy, sz);
        
        // loop through the vertices and scale all of them by the cumulative transformation matrix
        for (unsigned int i=0; i <= v_stacks; i++) {
            for (unsigned int j=0; j <= r_slices; j++) {
                //vertices[j][i] = scale(vertices[j][i], sx, sy, sz);
                vertices[j][i].Scale(sx, sy, sz);
            }
        }
        
        height *= sy;
        radius *= sx;
        
        // set the x-, y-, and z-axes
        x_axis *= sx;
        y_axis *= sy;
        z_axis *= sz;
    }
    
    /*
     * Method name: real_rev()
     * Description: Performs the rotation of an object about an arbitrary axis.
     * Params:
     *              GLfloat x - revolve about the x-axis
     *              GLfloat y - revolve about the y-axis
     *              GLfloat z - revolve about the z-axis
     * Returns: nothing
     */
    void real_rev(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
        // Perform the multiplication
        // loop through the vertices and scale all of them by the cumulative transformation matrix
        for (unsigned int i=0; i <= v_stacks; i++) {
            for (unsigned int j=0; j <= r_slices; j++) {
                //vertices[j][i] = arb_rotation(vertices[j][i], angle, x, y, z);
                vertices[j][i].RotateAboutAxis(angle, Vector3D(x, y, z));
            }
        }
        obj_center.RotateAboutAxis(angle, Vector3D(x, y, z));
        // printf("BAT CENTER IN a r b ROTATION: %f, %f, %f\n", obj_center.getX(), obj_center.getY(), obj_center.getZ());
    }
    
    
    /*
     * Method name: reset()
     * Description: Resets the object back to the start position.
     * Params:
     * Returns: nothing
     */
    void reset() {
        // reset the CTM matrix
        //memset(ctm_matrix, 0, sizeof(ctm_matrix));
        ctm_matrix.identity();
        // reset the axes
        x_axis = 0.0;
        y_axis = 1.0;
        z_axis = 0.0;
        make_shape();
    }
    
    /*
     * Method name: subtract()
     * Description: Simple function that subtracts two vectors.
     * Params: ThreeDVertex* tdv1 - a pointer to the first ThreeDVertex object
     *              ThreeDVertex* tdv2 - a pointer to the second ThreeDVertex object
     * Returns: a pointer to a new ThreeDVertex object that results from the subtraction
     */
    ThreeDVertex* subtract(ThreeDVertex* tdv1, ThreeDVertex* tdv2) {
        ThreeDVertex* temp = new ThreeDVertex();
        temp->x = tdv2->x - tdv1->x;
        temp->y = tdv2->y - tdv1->y;
        temp->z = tdv2->z - tdv1->z;
        temp->w = 1;
        return temp;
    }
    
    /*
     * Method name: matrix_multiply()
     * Description: Simple function that multiplies two 4x4 matrices (need to change for arbitrary number)
     * Params: GLfloat f_mat[4][4] - the first matrix in the multiplication
     *              GLfloat s_mat[4][4]- the second matrix in the multiplication
     * Returns: a 4x4 matrix with the result
     */
    GLfloat ** matrix_multiply(GLfloat f_mat[4][4], GLfloat s_mat[4][4]) {
        //Initialize
        GLfloat** res = new GLfloat*[4];
        for (int i = 0; i < 4; i++) {
            res[i] = new GLfloat[4];
            for (int j = 0; j < 4; j++)
                res[i][j] = 0;
        }
        
        // Calculate
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                for(int m = 0; m < 4; m++) {
                    res[i][j] += f_mat[i][m] * s_mat[m][j];
                }
            }
        }
        
        return res;
    }
    
    /*
     * Method name: cross_product()
     * Description: Simple function that takes the cross product of two vectors.
     * Params: ThreeDVertex* tdv1 - a pointer to the first ThreeDVertex object
     *              ThreeDVertex* tdv2 - a pointer to the second ThreeDVertex object
     * Returns: a pointer to a new ThreeDVertex object that results from the cross product
     */
    ThreeDVertex* cross_product(ThreeDVertex* tdv1, ThreeDVertex* tdv2) {
        ThreeDVertex* temp = new ThreeDVertex();
        temp->x = (tdv1->y * tdv2->z) - (tdv1->z * tdv2->y);
        temp->y = (tdv1->z * tdv2->x) - (tdv1->x * tdv2->z);
        temp->z = (tdv1->x * tdv2->y) - (tdv1->y * tdv2->x);
        temp->w = 1;
        return temp;
    }
    
    /*
     * Method name: normalize()
     * Description: Simple function that normalizes a vector.
     * Params: ThreeDVertex* test - a pointer to the vector to be normalized
     * Returns: a pointer to a new ThreeDVertex object that results from the normalization
     */
    ThreeDVertex* normalize(ThreeDVertex* test) {
        ThreeDVertex* temp = new ThreeDVertex();
        float length = sqrtf(powf(test->x, 2.0) + powf(test->y, 2.0) + powf(test->z, 2.0));
        temp->x = test->x / length;
        temp->y = test->y / length;
        temp->z = test->z / length;
        return temp;
    }
    
    /*
     * Method name: calcNewVertex()
     * Description: Calculates the new vertex to draw a line to for the vertex normals (V' = V + 0.4 * N)
     *                      where N is the normalized vertex normal.
     * Params: GLfloat testAr[] - an array containing the vertex coordinates (x, y, z, w)
     *              ThreeDVertex* tdv_n1[] - an array containing the vertex normals  in a face
     *              int vert_normal_index - the appropriate vertex normal to use
     * Returns: an array of GLfloats, used to draw the new vertex for the vertex normal line
     */
    GLfloat* calcNewVertex(GLfloat testAr[], ThreeDVertex* tdv_n1[], int vert_normal_index) {
        GLfloat* ret_val = new GLfloat[4];  // create a new GLfloat array
        ThreeDVertex* cur_vert_normal = tdv_n1[vert_normal_index];  // cache the appropriate vertex_normal
        
        ret_val[0] = testAr[0] + (cur_vert_normal->x * 0.5);
        ret_val[1] = testAr[1] + (cur_vert_normal->y * 0.5);
        ret_val[2] = testAr[2] + (cur_vert_normal->z * 0.5);
        ret_val[3] = 1;
        
        return ret_val;
    }
};


#endif /* defined(__CameraModeler__ThreeDShape__) */