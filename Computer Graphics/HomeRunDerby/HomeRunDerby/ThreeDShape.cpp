/*******************************************************
 FILE: ThreeDShape.cpp
 AUTHOR: Nikhil Venkatesh
 DATE: 09/20/13
 DOES: Creates classes for the objects to be drawn (Cylinder, Sphere, Inverted Cone, Torus)
 PLATFORM: tested on osx
 ********************************************************/

#include "ThreeDShape.h"

class House: public ThreeDShape
{
public:
    House(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        vertices[0][0] =  Vector4D(0, 1, 0, 1);
        vertices[0][1] =  Vector4D(-0.3, 0.3, 0.3, 1);
        vertices[0][2] =  Vector4D(0.3, 0.3, 0.3, 1);
        vertices[0][3] =  Vector4D(0, 0.3, 0, 1);  // copy
        
        vertices[1][0] =  Vector4D(0.3, 0.3, -0.3, 1);
        vertices[1][1] =  Vector4D(-0.3, 0.3, -0.3, 1);
        vertices[1][2] =  Vector4D(-0.3, -0.3, 0.3, 1);
        vertices[1][3] =  Vector4D(0.3, 0.3, -0.3, 1);     //copy
        
        vertices[2][0] =  Vector4D(0.3, -0.3, 0.3, 1);
        vertices[2][1] =  Vector4D(0.3, -0.3, -0.3, 1);
        vertices[2][2] =  Vector4D(-0.3, -0.3, -0.3, 1);
        vertices[2][3] =  Vector4D(0.3, -0.3, 0.3, 1);     //copy
        
        // copy
        vertices[3][0] =  Vector4D(0.3, -0.3, 0.3, 1);
        vertices[3][1] =  Vector4D(0.3, -0.3, -0.3, 1);
        vertices[3][2] =  Vector4D(-0.3, -0.3, -0.3, 1);
        vertices[3][3] =  Vector4D(0.3, -0.3, 0.3, 1);     //copy
        
        int f_point[2] = {0, 0}, sec_point[2] = {0, 1}, t_point[2] = {0, 2}, four_point[2] = {1, 0}, five_point[2] = {1, 1}, six_point[2] = {1, 2},
        seven_point[2] = {2, 0}, e_point[2] = {2, 1}, nine_point[2] = {2, 2};
        
        //add the predetermined faces
        faces = std::vector<QuadFace*>();   //deallocate any previous faces that were calculated
        faces.push_back(new QuadFace(f_point, sec_point, t_point, f_point));
        faces.push_back(new QuadFace(f_point, t_point, four_point, f_point));
        faces.push_back(new QuadFace(f_point, four_point, five_point, f_point));
        faces.push_back(new QuadFace(f_point, five_point, sec_point, f_point));
        
        faces.push_back(new QuadFace(t_point, sec_point, six_point, seven_point));
        faces.push_back(new QuadFace(t_point, seven_point, four_point, t_point));
        faces.push_back(new QuadFace(four_point, seven_point, e_point, four_point));
        faces.push_back(new QuadFace(four_point, e_point, nine_point, four_point));
        faces.push_back(new QuadFace(nine_point, four_point, five_point, nine_point));
        faces.push_back(new QuadFace(five_point, nine_point, sec_point, five_point));
        faces.push_back(new QuadFace(sec_point, nine_point, six_point, sec_point));
    }
    
    void setBoundingBox()
    { }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        
        return result;
    }
    
    virtual ~House() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
    
};

class Cube: public ThreeDShape
{
public:
    Cube(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        vertices[0][0] =  Vector4D(-0.5, 0.5, 0.5, 1);
        vertices[0][1] =  Vector4D(0.5, 0.5, 0.5, 1);
        vertices[0][2] =  Vector4D(0.5, 0.5, -0.5, 1);
        vertices[0][3] =  Vector4D(-0.5, 0.5, -0.5, 1);
        vertices[0][4] =  Vector4D(-0.5, 0.5, 0.5, 1);
        
        vertices[1][0] =  Vector4D(-0.5, -0.5, 0.5, 1);
        vertices[1][1] =  Vector4D(0.5, -0.5, 0.5, 1);
        vertices[1][2] =  Vector4D(0.5, -0.5, -0.5, 1);
        vertices[1][3] =  Vector4D(-0.5, -0.5, -0.5, 1);
        vertices[1][4] =  Vector4D(-0.5, -0.5, 0.5, 1);
        // copy
        vertices[2][0] =  Vector4D(-0.5, 0.5, 0.5, 1);
        vertices[2][1] =  Vector4D(0.5, 0.5, 0.5, 1);
        vertices[2][2] =  Vector4D(0.5, 0.5, -0.5, 1);
        vertices[2][3] =  Vector4D(-0.5, 0.5, -0.5, 1);
        vertices[2][4] =  Vector4D(-0.5, 0.5, 0.5, 1);
        
        
        int f_point[2] = {0, 0}, sec_point[2] = {0, 1}, t_point[2] = {0, 2}, four_point[2] = {0, 3}, five_point[2] = {1, 0}, six_point[2] = {1, 1},
        seven_point[2] = {1, 2}, e_point[2] = {1, 3};
        
        //addPredeterminedFaces();
        faces = std::vector<QuadFace*>();   //deallocate any previous faces that were calculated
        faces.push_back(new QuadFace(five_point, six_point, sec_point, five_point));
        faces.push_back(new QuadFace(f_point, five_point, sec_point, f_point));
        faces.push_back(new QuadFace(six_point, seven_point, t_point, six_point));
        faces.push_back(new QuadFace(sec_point, six_point, t_point, sec_point));
        
        faces.push_back(new QuadFace(four_point, t_point, seven_point, four_point));
        faces.push_back(new QuadFace(e_point, four_point, seven_point, e_point));
        faces.push_back(new QuadFace(f_point, four_point, e_point, f_point));
        faces.push_back(new QuadFace(five_point, f_point, e_point, five_point));
        
        faces.push_back(new QuadFace(f_point, sec_point, t_point, f_point));
        faces.push_back(new QuadFace(f_point, four_point, t_point, f_point));
        faces.push_back(new QuadFace(five_point, six_point, seven_point, five_point));
        faces.push_back(new QuadFace(e_point, five_point, seven_point, e_point));
    }
    
    void setBoundingBox()
    { }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        double t;
        
        if (d.getX() != 0) {
            // bottom
            t = (-0.5 - p.getX()) / d.getX();
            result = p + d * t;
            //printf("RESULT 1: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getY() <= 0.5 && result.getY() >= -0.5 && result.getZ() <= 0.5 && result.getZ() >= -0.5)
                return result;
            
            // top
            t = (-0.5 + p.getX()) / d.getX();
            result = p + d *t;
            //printf("RESULT 2: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getX() <= 0.5 && result.getX() >= -0.5 && result.getZ() <= 0.5 && result.getZ() >= -0.5)
                return result;
        }
        
        if (d.getY() != 0) {
            // left
            t = (-0.5 - p.getY()) / d.getY();
            result = p + d * t;
            //printf("RESULT 3: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getX() <= 0.5 && result.getX() >= -0.5 && result.getZ() <= 0.5 && result.getZ() >= -0.5)
                return result;
            
            // right
            t = (-0.5 + p.getY()) / d.getY();
            result = p + d * t;
            //printf("RESULT 4: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getX() <= 0.5 && result.getX() >= -0.5 && result.getZ() <= 0.5 && result.getZ() >= -0.5)
                return result;
        }
        
        if (d.getZ() != 0) {
            // back
            t = (-0.5 - p.getZ()) / d.getZ();
            result = p + d * t;
            //printf("RESULT 6: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getX() <= 0.5 && result.getX() >= -0.5 && result.getY() <= 0.5 && result.getY() >= -0.5)
                return result;
            
            // front
            t = (-0.5 + p.getZ()) / d.getZ();
            result = p + d * t;
            //printf("RESULT 5: %f, %f, %f\n", result.getX(), result.getY(), result.getZ());
            if (result.getX() <= 0.5 && result.getX() >= -0.5 && result.getY() <= 0.5 && result.getY() >= -0.5)
                return result;
        }
        //fail
        printf("NO INTERSECTION ON THE CUBE!!!\n");
        return Vector3D(0.0,0.0,0.0);
    }
    
    Vector3D cube_ray_intersect(Vector3D p, Vector3D d, float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
        Vector3D result;
        
        // initialize the translation, rotation, and scaling matrices
        Matrix4x4 TranslationMatrix = Matrix4x4();
        TranslationMatrix.translate(tx, ty, tz);
        Matrix4x4 RotationMatrixX = Matrix4x4();
        RotationMatrixX.rotateX(rx);
        Matrix4x4 RotationMatrixY = Matrix4x4();
        RotationMatrixY.rotateY(ry);
        Matrix4x4 RotationMatrixZ = Matrix4x4();
        RotationMatrixZ.rotateZ(rz);
        Matrix4x4 ScaleMatrix = Matrix4x4();
        ScaleMatrix.scale(sx, sy, sz);
        
        // calculate the transformation matrix
        Matrix4x4 transform = TranslationMatrix * RotationMatrixX * RotationMatrixY * RotationMatrixZ * ScaleMatrix;
        
        // now calculate the inverse of the translation, rotation, and scaling matrices
        Matrix4x4 scaleInverse = Matrix4x4();
        scaleInverse.scale(1.0/sx, 1.0/sy, 1.0/sz);
        Matrix4x4 rotXInverse = Matrix4x4();  rotXInverse.rotateX(-rx);
        Matrix4x4 rotYInverse = Matrix4x4();  rotYInverse.rotateY(-ry);
        Matrix4x4 rotZInverse = Matrix4x4();  rotZInverse.rotateZ(-rz);
        Matrix4x4 rotationInverse = rotXInverse * rotYInverse * rotZInverse;
        Matrix4x4 translationInverse = Matrix4x4();
        translationInverse.translate(-tx, -ty, -tz);
        
        Matrix4x4 inverse = scaleInverse * rotationInverse * translationInverse;
        Matrix4x4 directionInverse = scaleInverse * rotationInverse;
        
        // Create temporary Vector4Ds to handle the 4x4 matrix multiplication
        Vector4D temp_d(d.getX(), d.getY(), d.getZ(), 1);
        temp_d = directionInverse * temp_d;
        
        d.setX(temp_d.getX());
        d.setY(temp_d.getY());
        d.setZ(temp_d.getZ());
        
        Vector4D temp_p(p.getX(), p.getY(), p.getZ(), 1);
        temp_p = inverse * temp_p;
        
        p.setX(temp_p.getX());
        p.setY(temp_p.getY());
        p.setZ(temp_p.getZ());
        
        // Calculate the resulting intersection point - use the same Vector4D conversion to handle the matrix multiplication
        Vector3D f_result = intersect(p, d);
        Vector4D s_result (f_result.getX(), f_result.getY(), f_result.getZ(), 1);
        // check to see if the returned result is all 0s - this is currently my default return value for no intersections
        if (s_result.getX() != 0 && s_result.getY() != 0 && s_result.getZ() != 0) {
            Vector4D my_result = transform * s_result;
            
            result.setX(my_result.getX());
            result.setY(my_result.getY());
            result.setZ(my_result.getZ());
        } else { return Vector3D(0.0, 0.0, 0.0); }      // if no intersection, return all 0s
        return result;
    }
    
    virtual ~Cube() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
    
};

class Cylinder: public ThreeDShape
{
public:
    Cylinder(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        float curTheta = 0, angThetaDelta = (360)/ (float) r_slices, height_delta = 0;
        
        for (unsigned int i=0; i < MAX_VS; i++) {
            for (unsigned int j=0; j < MAX_RS; j++) {
                vertices[j][i] = Vector4D(radius, -0.5, 0, 1);    // initialize each vertex to the initial vertex
            }
        }
        
        for (unsigned int i=0; i < v_stacks; i++) {
            for (unsigned int j=0; j < r_slices; j++) {
                vertices[j][i].RotateAboutY(curTheta);
                vertices[j][i].Translate(0, height_delta, 0);
                curTheta += angThetaDelta;
            }
            curTheta = 0;
            height_delta += height / (float) v_stacks;
        }
        
        for (unsigned int j=0; j <= r_slices; j++){
            vertices[j][v_stacks] = vertices[j][0];
        }
        for (unsigned int i=0; i <= v_stacks; i++){
            vertices[r_slices][i] = vertices[0][i];
        }
        
        addFaces();
    }
    
    void setBoundingBox()
    {
        lx = obj_center.getX() - radius;
        rx = obj_center.getX() + radius;
        by = obj_center.getY() - height / 2;
        ty = obj_center.getY() + height / 2;
        fz = obj_center.getZ() - radius;
        bz = obj_center.getZ() + radius;
    }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        double t[4];
        // Find descriminant
        double a = (d.getX() * d.getX()) + (d.getZ() * d.getZ());
        double b = 2.0 * ((p.getX() * d.getX()) + (p.getZ() * d.getZ()));
        double c = (p.getX() * p.getX()) + (p.getZ() * p.getZ()) - (0.5 * 0.5);
        double descriminant = (b*b)-(4*a*c);
        
        if(descriminant > 0){
            //t is positive
            t[0] = (-b + sqrt(descriminant)) / (2 * a);
            t[1] = (-b - sqrt(descriminant)) / (2 * a);
            
            Vector3D t0 = p + (d * t[0]);
            Vector3D t1 = p + (d * t[1]);
            
            if(t0.getY() > 0.5 || t0.getY() < -0.5)
                t[0] = -1;
            if(t1.getY() > 0.5 || t1.getY() < -0.5)
                t[1] = -1;
            
            if (t[0] == -1 && t[1] == -1) { return Vector3D(0.0,0.0,0.0); }
            else if ((t[0] > t[1] && t[1] != -1) || (t[0] > t[1] && t[1] != -1))
                result = p + d * t[1];
            else if ((t[0] > t[1] && t[1] == -1) || (t[1] > t[0] && t[0] != -1))
                result = p + d * t[0];
        }
        else{
            t[0] = -1;
            t[1] = -1;
            return Vector3D(0.0, 0.0, 0.0);
        }
        return result;
    }
    
    Vector3D cylinder_ray_intersect(Vector3D p, Vector3D d, float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
        Vector3D result;
        
        Matrix4x4 TranslationMatrix = Matrix4x4();
        TranslationMatrix.translate(tx, ty, tz);
        Matrix4x4 RotationMatrixX = Matrix4x4();
        RotationMatrixX.rotateX(rx);
        Matrix4x4 RotationMatrixY = Matrix4x4();
        RotationMatrixY.rotateY(ry);
        Matrix4x4 RotationMatrixZ = Matrix4x4();
        RotationMatrixZ.rotateZ(rz);
        Matrix4x4 ScaleMatrix = Matrix4x4();
        ScaleMatrix.scale(sx, sy, sz);
        
        Matrix4x4 transform = TranslationMatrix * RotationMatrixX * RotationMatrixY * RotationMatrixZ * ScaleMatrix;
        
        
        Matrix4x4 scaleInverse = Matrix4x4();
        scaleInverse.scale(1.0/sx, 1.0/sy, 1.0/sz);
        Matrix4x4 rotXInverse = Matrix4x4();  rotXInverse.rotateX(-rx);
        Matrix4x4 rotYInverse = Matrix4x4();  rotYInverse.rotateY(-ry);
        Matrix4x4 rotZInverse = Matrix4x4();  rotZInverse.rotateZ(-rz);
        Matrix4x4 rotationInverse = rotXInverse * rotYInverse * rotZInverse;
        Matrix4x4 translationInverse = Matrix4x4();
        translationInverse.translate(-tx, -ty, -tz);
        
        Matrix4x4 inverse = scaleInverse * rotationInverse * translationInverse;
        Matrix4x4 directionInverse = scaleInverse * rotationInverse;
        
        // Create temporary Vector4Ds to handle the 4x4 matrix multiplication
        Vector4D temp_d(d.getX(), d.getY(), d.getZ(), 1);
        temp_d = directionInverse * temp_d;
        
        d.setX(temp_d.getX());
        d.setY(temp_d.getY());
        d.setZ(temp_d.getZ());
        
        Vector4D temp_p(p.getX(), p.getY(), p.getZ(), 1);
        temp_p = inverse * temp_p;
        
        p.setX(temp_p.getX());
        p.setY(temp_p.getY());
        p.setZ(temp_p.getZ());
        
        // Calculate the resulting intersection point - use the same Vector4D conversion to handle the matrix multiplication
        Vector3D f_result = intersect(p, d);
        Vector4D s_result (f_result.getX(), f_result.getY(), f_result.getZ(), 1);
        if (s_result.getX() != 0 && s_result.getY() != 0 && s_result.getZ() != 0) {
            Vector4D my_result = transform * s_result;
            
            result.setX(my_result.getX());
            result.setY(my_result.getY());
            result.setZ(my_result.getZ());
        } else { return Vector3D(0.0, 0.0, 0.0); }
        return result;
    }
    
    virtual ~Cylinder() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
};

class Sphere: public ThreeDShape
{
public:
    float velocity[3];
    float acceleration[3];
    Sphere(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        float curTheta = 0, curPhi = 0, angThetaDelta = (360)/ (float) v_stacks, angPhiDelta = (360)/ (float) r_slices;
        
        for (unsigned int i=0; i < MAX_VS; i++) {
            for (unsigned int j=0; j < MAX_RS; j++) {
                vertices[j][i] = Vector4D(radius, 0, 0, 1);    // initialize each vertex to the initial vertex
            }
        }
        
        for (unsigned int i=0; i < v_stacks; i++) {
            for (unsigned int j=0; j < r_slices; j++){
                vertices[j][i].RotateAboutY(curPhi);
                vertices[j][i].RotateAboutZ(curTheta);
                curPhi += angPhiDelta;
            }
            curTheta += angThetaDelta;
        }
        
        for (unsigned int j=0; j <= r_slices; j++){
            vertices[j][v_stacks] = vertices[j][0];
        }
        for (unsigned int i=0; i <= v_stacks; i++){
            vertices[r_slices][i] = vertices[0][i];
        }
        
        addFaces();
    }
    
    void setBoundingBox()
    {
        lx = obj_center.getX() - radius;
        rx = obj_center.getX() + radius;
        by = obj_center.getY() - radius;
        ty = obj_center.getY() + radius;
        fz = obj_center.getZ() - radius;
        bz = obj_center.getZ() + radius;
    }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        double a = d.dotProduct(d);
        double b = 2 * (p.dotProduct(d));
        double c = p.dotProduct(p) - 1;
        
        double descriminant  = (b * b) - (4 * a *c);
        
        double t1 = (-b + sqrt(descriminant)) / (2 * a);
        double t2 = (-b - sqrt(descriminant)) / (2 * a);
        
        if (descriminant > 0) {
            if (t1 > t2)
                result = p + d * t2;
            else
                result = p + d * t1;
        } else if (descriminant == 0){
            double test = (-1 * b) / (2 * a);
            result = p + d * test;
        } else { return Vector3D(0.0, 0.0, 0.0); }
        
        return result;
    }
    
    Vector3D sphere_ray_intersect(Vector3D p, Vector3D d, float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
        Vector3D result;
        
        Matrix4x4 TranslationMatrix = Matrix4x4();
        TranslationMatrix.translate(tx, ty, tz);
        Matrix4x4 RotationMatrixX = Matrix4x4();
        RotationMatrixX.rotateX(rx);
        Matrix4x4 RotationMatrixY = Matrix4x4();
        RotationMatrixY.rotateY(ry);
        Matrix4x4 RotationMatrixZ = Matrix4x4();
        RotationMatrixZ.rotateZ(rz);
        Matrix4x4 ScaleMatrix = Matrix4x4();
        ScaleMatrix.scale(sx, sy, sz);
        
        Matrix4x4 transform = TranslationMatrix * RotationMatrixX * RotationMatrixY * RotationMatrixZ * ScaleMatrix;
        
        
        Matrix4x4 scaleInverse = Matrix4x4();
        scaleInverse.scale(1.0/sx, 1.0/sy, 1.0/sz);
        Matrix4x4 rotXInverse = Matrix4x4();  rotXInverse.rotateX(-rx);
        Matrix4x4 rotYInverse = Matrix4x4();  rotYInverse.rotateY(-ry);
        Matrix4x4 rotZInverse = Matrix4x4();  rotZInverse.rotateZ(-rz);
        Matrix4x4 rotationInverse = rotXInverse * rotYInverse * rotZInverse;
        Matrix4x4 translationInverse = Matrix4x4();
        translationInverse.translate(-tx, -ty, -tz);
        
        Matrix4x4 inverse = scaleInverse * rotationInverse * translationInverse;
        Matrix4x4 directionInverse = scaleInverse * rotationInverse;
        
        // Create temporary Vector4Ds to handle the 4x4 matrix multiplication
        Vector4D temp_d(d.getX(), d.getY(), d.getZ(), 1);
        temp_d = directionInverse * temp_d;
        
        d.setX(temp_d.getX());
        d.setY(temp_d.getY());
        d.setZ(temp_d.getZ());
        
        Vector4D temp_p(p.getX(), p.getY(), p.getZ(), 1);
        temp_p = inverse * temp_p;
        
        p.setX(temp_p.getX());
        p.setY(temp_p.getY());
        p.setZ(temp_p.getZ());
        
        // Calculate the resulting intersection point - use the same Vector4D conversion to handle the matrix multiplication
        Vector3D f_result = intersect(p, d);
        Vector4D s_result (f_result.getX(), f_result.getY(), f_result.getZ(), 1);
        if (s_result.getX() != 0 && s_result.getY() != 0 && s_result.getZ() != 0) {
            Vector4D my_result = transform * s_result;
            
            result.setX(my_result.getX());
            result.setY(my_result.getY());
            result.setZ(my_result.getZ());
        } else { return Vector3D(0.0, 0.0, 0.0); }
        
        return result;
    }
    
    virtual ~Sphere() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
};

class InvertedCone: public ThreeDShape
{
public:
    InvertedCone(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        // angPhiDelta is the full circle divided by the number of radial slices - get equal angles between each slice
        float curPhi = 0, angPhiDelta = (360)/ (float) r_slices, t = 0;
        
        for (unsigned int i=0; i < MAX_VS; i++) {
            for (unsigned int j=0; j < MAX_RS; j++) {
                vertices[j][i] = Vector4D();    // initialize each vertex to the initial vertex
            }
        }
        
        for (unsigned int i=0; i < v_stacks; i++) {
            t =  i/(float) v_stacks;      // increment the interval between [0, 1] that is multiplied by the radius as a scaling factor
            for (unsigned int j=0; j < r_slices; j++){
                //vertices[j][i] = rotate(Direction::Y, translate(new ThreeDVertex(0,0,0,1), radius*(1-t), height - (height * t), 0), curPhi);
                vertices[j][i].Translate(radius*(1-t), height - (height * t), 0);
                vertices[j][i].RotateAboutY(curPhi);
                curPhi += angPhiDelta;
            }
        }
        
        for (unsigned int j=0; j <= r_slices; j++){
            vertices[j][v_stacks] = vertices[j][0];
        }
        for (unsigned int i=0; i <= v_stacks; i++){
            vertices[r_slices][i] = vertices[0][i];
        }
        
        addFaces();
    }
    
    void setBoundingBox()
    { }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        
        return result;
    }
    
    virtual ~InvertedCone() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
};

class Torus: public ThreeDShape
{
public:
    Torus(double height, double radius, double radius2, int rs, int vs): ThreeDShape(height, radius, radius2, rs, vs) { }
    
    void make_shape() {
        // angPhiDelta is the full circle divided by the number of radial slices - get equal angles between each slice
        float curTheta = 0, curPhi = 0, angPhiDelta = (360)/ (float) r_slices, angThetaDelta = (360)/ (float) v_stacks;
        
        for (unsigned int i=0; i < MAX_VS; i++) {
            for (unsigned int j=0; j < MAX_RS; j++) {
                vertices[j][i] = Vector4D(radius2, 0, 0, 1);    // initialize each vertex to the initial vertex
            }
        }
        
        for (unsigned int i=0; i <= v_stacks; i++) {
            for (unsigned int j=0; j <= r_slices; j++){
                vertices[j][i].RotateAboutZ(curTheta);
                vertices[j][i].Translate((float) radius, 0, 0);
                vertices[j][i].RotateAboutY(curPhi);
                curTheta += angThetaDelta;
                curPhi += angPhiDelta;
            }
            curTheta = 0;
        }
        
        addFaces();
    }
    
    void setBoundingBox()
    { }
    
    Vector3D intersect(Vector3D p, Vector3D d) {
        Vector3D result;
        
        return result;
    }
    
    virtual ~Torus() {
        delete[] vertices;
        face_normals = std::vector<Vector4D>();
        faces = std::vector<QuadFace*>();   // deallocate all the storage in the vector
    }
};