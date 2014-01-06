/**************************************************************************
 File: gl_c_main.cpp
 Does: Main file for HomeRunDerby
 Author: Nikhil Venkatesh and Kevin Lu
 Date: 12/09/13
 **************************************************************************/

#include "gl_c_main.h"
#include "constants.h"
#include "ThreeDShape.cpp"

#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

#define FALSE 0
#define TRUE  1
#define MAX_LIGHTS  8
#define NUM_OBJECTS 12
#define M_ZOOM_IN 1.1
#define M_ZOOM_OUT 0.9

#define PERSPECTIVE 0
#define ORTHO 1

#define LOCAL 0
#define WORLD 1

#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

// constants
const int   SCREEN_WIDTH    = 700;
const int   SCREEN_HEIGHT   = 700;
const float CAMERA_DISTANCE = 6.0f;

// global variables
int screenWidth;
int screenHeight;
int projectionType = PERSPECTIVE;
float cameraAngleX, cameraAngleY, cameraAngleZ;
float cameraDistanceX, cameraDistanceY, cameraDistanceZ;
float cameraDistance;
float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
bool isRotation, didCalcModelView, doSwingBat;
void *font = GLUT_BITMAP_HELVETICA_18;
void *times_new_roman = GLUT_BITMAP_TIMES_ROMAN_24;

float iVel = 2.0f;
float theta = 45.0f;
double time_start = 0;
double time_end = 0.5;
double deltaTime = 0.01;
int throw_pitch = 0; //is 0 when pitch is not thrown, 1 when thrown
bool collision_detected = false, first_init = true, first_bat_init = true, hit_init = false;
int num_collisions = 0, tempTest = 0;

Matrix4x4 matrixView;
Matrix4x4 matrixModel;
Matrix4x4 matrixModelView;    // = matrixView * matrixModel
Matrix4x4 matrixProjection;
Matrix4x4 matrixTranslation;
Matrix4x4 matrixRotation;
Matrix4x4 matrixScaling;

/************* Structures ****************/
typedef struct _Object {
    int sid;
    
    // example object storage arrays for vertex and normals
    GLfloat vertices_cube_smart[8][4];
    GLfloat normals_cube_smart[8][3];
    
    GLfloat shine;
    GLfloat emi[4];
    GLfloat amb[4];
    GLfloat diff[4];
    GLfloat spec[4];
    
    GLfloat translate[4];
    GLfloat scale[4];
    GLfloat rotate[4];
    
}OBJECT;

typedef struct _CAM{
    GLfloat pos[4];
    GLfloat at[4];
    GLfloat up[4];
    
    GLfloat dir[4];
}CAM;

typedef struct _LITE{
    GLfloat amb[4];
    GLfloat diff[4];
    GLfloat spec[4];
    GLfloat pos[4];
    GLfloat dir[3];
    GLfloat angle;
}LITE;

GLfloat vertices_axes[][4] = {
	{0.0, 0.0, 0.0, 1.0},  /* origin */
	{5.0, 0.0, 0.0, 1.0},  /* maxx */
	{0.0, 5.0, 0.0, 1.0}, /* maxy */
	{0.0, 0.0, 5.0, 1.0}  /* maxz */
    
};

/**************** Prototypes ******************/
void    real_translation(OBJECT *po, GLfloat x, GLfloat y, GLfloat z);
void    real_scaling(OBJECT *po, GLfloat sx, GLfloat sy, GLfloat sz);
void    real_rotation(OBJECT *po, GLfloat deg, GLfloat x, GLfloat y, GLfloat z);
Matrix4x4 setOrthoFrustum(float left, float right, float bottom, float top, float near_val, float far_val);
Matrix4x4 setFrustum(float l, float r, float b, float t, float n, float f);
Matrix4x4 setFrustum(float fovY, float aspectRatio, float front, float back);
void    calculateCameraVectors();
void    UpdateProjection(int projection);
void    buildCamTranslationMatrix();
void    buildCamRotationMatrix();
void    buildCamScalingMatrix();
void    zoom(int direction);    // e.g. 1.0 for zooming in, -1.0 for zooming out
void    translateCam(int environment, int direction, float dx, float dy, float dz);    // e.g. LOCAL, 1, 1, 1
void    rotateCam(int direction, int axis);     // e.g. CLOCKWISE, X_AXIS
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
bool checkCollision();
void initSkybox(void);
void drawScene();
void drawSkybox(double D);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void showInfo();

OBJECT my_objects[NUM_OBJECTS];
LITE my_lights[MAX_LIGHTS];
int num_objects;
int  num_lights;

// camera variables
CAM my_cam;
GLfloat camx, camy, camz;
GLfloat atx, aty, atz;
GLfloat upx, upy, upz;
Vector3D u, v, w;   // the local axes of the camera

// Bat variables
/// BAT ///
Cylinder *bat = new Cylinder(1, 0.5, 0, 40, 40);
GLfloat batPos[4];
float batAngle = 0.0;

// Baseball variables
/// BASEBALL ///
Sphere *baseball = new Sphere(0, 1, 0, 40, 40);
GLfloat baseballPos[4];
float rand_x_direction = 0.0f;

int crt_render_mode;
int crt_shape, crt_rs, crt_vs;
int crt_transform;

//// SCORING VARIABLES ////
int numHomeRuns = 0;
bool hitHomeRun = false;
bool hitFoulBall = false;
int numFouls = 0;
float distanceHit = 0.0f;
float pitchSpeed = 0.0f;
int totalPitches = 0;
float accuracy = 0.0f;
float timestep = 0.0f;

/*
 *  Initialize each of the shapes we are drawing.
 */
House *house = new House(0, 0, 0, 3, 3);
Cube *cube = new Cube(0, 0, 0, 2, 4);
// Default: height 1 unit, base radius 0.5, set 10 vertical stacks and 10 radial slices
Cylinder *cyl = new Cylinder(0.5, 0.2, 0, 40, 40);
// Default: radius: 1.0, and 10 vs and 10 rs
Sphere *sph = new Sphere(0, 0.3, 0, 40, 40);
// default is height 1 unit, 1.0 base radius, and 10 stacks and 10 slices
InvertedCone *ic = new InvertedCone(0.5, 0.4, 0, 40, 40);
/////////////////////////////////////////////////////////////////////

// Default: radius of 1.2 (distance from center of tube to center of torus
//               inner radius of 0.2 (radius of the tube)
//               number of circles along the tube is 15
//               number of circles across the tube is 40
Torus *t = new Torus(0, 0.4, 0.1, 40, 40);

// have arrays to store the multiple objects of the same type
std::vector<Cube*> my_cubes = std::vector<Cube*>();
std::vector<std::vector<float>> cube_translations = std::vector<std::vector<float>>();
std::vector<Sphere*> my_spheres = std::vector<Sphere*>();
std::vector<std::vector<float>> sphere_translations = std::vector<std::vector<float>>();
std::vector<Cylinder*> my_cylinders = std::vector<Cylinder*>();
std::vector<std::vector<float>> cylinder_translations = std::vector<std::vector<float>>();

void myabort(void) {
    abort();
    exit(1); /* exit so g++ knows we don't return. */
}

/*
 * displayProject()
 * ------
 * Sets the projection
 */
void displayProject(double fov, double asp, double dim)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov,asp,dim/16,16*dim);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*
 *  redisplayAll()
 *  ------
 *  This is called whenever we need to draw the display
 */
void redisplayAll(void)
{
    displayProject(fov,asp,dim);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);   /* for writing to stdout asap */
    glutInit(&argc, argv);
    
    my_setup(argc, argv);
    glut_setup();
    gl_setup();
    
    initSkybox();
    
    redisplayAll();
    glutMainLoop();
    return(0);
}


void glut_setup (){
    
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    
    glutInitWindowSize(700,700);
    glutInitWindowPosition(20,20);
    glutCreateWindow("Home Run Derby");
    
    /* set up callback functions */
    glutDisplayFunc(my_display);
    glutReshapeFunc(my_reshape);
    glutMouseFunc(my_mouse);
    glutSpecialFunc(my_special_keys);
    glutMotionFunc(my_mouse_drag);
    glutKeyboardFunc(my_keyboard);
    glutIdleFunc( my_idle );
    glutTimerFunc(10, my_TimeOut, 0);
    
    return;
}

void gl_setup(void) {
    
    // enable depth handling (z-buffer)
    glEnable(GL_DEPTH_TEST);
    
    // enable auto normalize
    glEnable(GL_NORMALIZE);
    
    // define the background color
    glClearColor(0,0,0,1);
    
    
    glMatrixMode(GL_PROJECTION) ;
    glLoadIdentity() ;
    matrixProjection = setFrustum(40, 1.0, 1, 200);
    glMatrixMode(GL_MODELVIEW) ;
    glLoadIdentity() ;  // init modelview to identity
    
    // toggle to smooth shading (instead of flat)
    glShadeModel(GL_SMOOTH);
    lighting_setup();
    
    
    return ;
}

/*
 *  initSkybox
 *  ------
 *  initializes all of our textures for the skybox background
 */
void initSkybox(void)
{
    /*
     SKY_FRONT 0
     SKY_RIGHT 1
     SKY_LEFT 2
     SKY_BACK 3
     SKY_UP 4
     SKY_DOWN 5
     */
    //skybox[SKY_FRONT] = loadTexBMP("/Users/Hadoop/Downloads/meshExample/txStormydays_front.bmp");
    skybox[SKY_RIGHT] = loadTexBMP("/Users/Hadoop/Documents/CS1566/Assignments/HomeRunDerby/Textures/bright_clouds.bmp", false);
    //skybox[SKY_LEFT] = loadTexBMP("/Users/Hadoop/Downloads/meshExample/txStormydays_left.bmp");
    skybox[SKY_BACK] = loadTexBMP("/Users/Hadoop/Documents/CS1566/Assignments/HomeRunDerby/Textures/bleachersfence.bmp", true);
    //skybox[SKY_UP] = loadTexBMP("/Users/Hadoop/Downloads/meshExample/txStormydays_up.bmp");
    skybox[SKY_DOWN] = loadTexBMP("/Users/Hadoop/Documents/CS1566/Assignments/HomeRunDerby/Textures/baseballdiamond.bmp", false);
}

void initBat() {
    // Create the bat object
    if(first_bat_init == false)
        bat = new Cylinder(1, 0.5, 0, 40, 40);
    
    bat->make_shape();
    if(first_bat_init == false)
        bat->calculateVertexNormals(false);
    
    bat->real_scaling(0.6, 4, 0.6);
    bat->real_rotation(-80, 0, 0, 1);
    bat->real_rotation(-60, 0, 1, 0);
    // Set the bat position to be updated later
    printf("BAT POS: %f\n", batPos[0]);
    if (batPos[0] == 0 && collision_detected) {
        bat->real_translation(0, 0, 24);
        batPos[0] = 0;
        batPos[1] = 0;
        batPos[2] = 24;
    } else {
        bat->real_translation(batPos[0], 0, 24);
        batPos[0] = batPos[0];
        batPos[1] = 0;
        batPos[2] = 24;
    }
}

void init_ball() {
    // Create the baseball object
    if (first_init == false)
        baseball = new Sphere(0, 1, 0, 40, 40);

    baseball->make_shape();
    if (first_init == false)
        baseball->calculateVertexNormals(false);
    
    baseball->acceleration[0] = 0.0;  baseball->acceleration[1] = -9.8; baseball->acceleration[2] = 0.0;
    baseball->velocity[0] = 0.0;  baseball->velocity[1] = 0.0; baseball->velocity[2] = 0.0;
    
    baseball->real_scaling(0.5, 0.5, 0.5);
    baseball->real_translation(1, 0, 2);
    baseballPos[0] = 1; baseballPos[1] = 0; baseballPos[2] = 2;
    
    int a = rand() % 2;
    baseball->velocity[0] = 2.5 + (float) (rand()) / ((float)(RAND_MAX/(0.5)));
	baseball->velocity[2] = -25 + (float) (rand()) / ((float)(RAND_MAX/(10)));
    
    baseball->velocity[1] = 0;
    
	if(a == 1)
		baseball->velocity[0] *= -1;
    
    printf("Initial velo_x: %f\n", baseball->velocity[0]);
}

void my_setup(int argc, char **argv){
    
    // Initialize random seed
    srand(time(NULL));
    num_objects = num_lights = 0;
    doSwingBat = false;
    batPos[0] = -1;
    // Initialize camera variables
    cameraAngleX  = cameraAngleY = cameraAngleZ = cameraDistanceX = cameraDistanceY = cameraDistanceZ = 0;
//    mouseLeftDown = mouseRightDown = false;
//    mouseX = mouseY = 0;
    
    // initialize global shape defaults and mode for drawing
    crt_render_mode = GL_POLYGON;
    crt_shape = 0;
    
    crt_rs = 40;
    crt_vs = 40;
    
    orthoLeft = -6.0;
    orthoRight = 6.0;
    orthoBottom = -6.0 * (GLfloat) SCREEN_HEIGHT / (GLfloat) SCREEN_WIDTH;
    orthoTop = 6.0 * (GLfloat) SCREEN_HEIGHT / (GLfloat) SCREEN_WIDTH;
    orthoNear = 1;
    orthoFar = 200;
    
    initBat();
    first_bat_init = false;
    printf("BAT CENTER trans: %f, %f, %f\n", bat->obj_center.getX(), bat->obj_center.getY(), bat->obj_center.getZ());
    
    init_ball();
    first_init = false;
    //If you want to allow the user to type in the spec file
    //then modify the following code.
    //Otherwise, the program will attempt to load the file as specified
    //on the command line:
    //EX: ./glmain spec3
    my_assert(argc >1, "need to supply a spec file");
    read_spec(argv[1]);
    for(int i=0; i<num_objects; i++){
        OBJECT *po;
        po = &my_objects[i];
        // use switch to create your objects, cube given as example
        
        switch (po->sid){
            case 1: //cube
            {
                Cube *cube = new Cube(0, 0, 0, 2, 4);
                my_cubes.push_back(cube);   // add a new cube to the global cubes vector - used for multiple of the same object type
                my_cubes.back()->make_shape();  // the item at the end of the vector is the most recent item added
                printf("Adding cube...\n");
            }
                break;
            case 2: // house
                house->make_shape();
                break;
            case 3: // sphere
            {
                Sphere *sph = new Sphere(0, 1, 0, 40, 40);
                my_spheres.push_back(sph);
                my_spheres.back()->make_shape();
            }
                break;
            case 4: // cylinder
            {
                Cylinder *cyl = new Cylinder(1, 0.5, 0, 40, 40);
                my_cylinders.push_back(cyl);
                my_cylinders.back()->make_shape();
            }
                break;
            case 5: // cone
                ic->make_shape();
                break;
            case 6: // torus
                t->make_shape();
                break;
        }
        
        // scale, rotate, translate using your real tranformations from assignment 3 depending on input from spec file
        // Call the real_scaling method on the current object
        real_scaling(po, po->scale[0], po->scale[1], po->scale[2]);
        // You need 3 separate calls to real_rotation to handle rotation about each axis
        real_rotation(po, po->rotate[0], 1, 0, 0);
        real_rotation(po, po->rotate[1], 0, 1, 0);
        real_rotation(po, po->rotate[2], 0, 0, 1);
        // Call the real_translation method on the current object
        real_translation(po, po->translate[0], po->translate[1], po->translate[2]);
        
        if (my_cubes.size() > 0)
            my_cubes.back()->calculateVertexNormals(false);
        
        house->calculateVertexNormals(false);
        
        if(my_spheres.size() > 0)
            my_spheres.back()->calculateVertexNormals(false);
        
        if (my_cylinders.size() > 0)
            my_cylinders.back()->calculateVertexNormals(false);
        
        ic->calculateVertexNormals(false);
        bat->calculateVertexNormals(false);
        baseball->calculateVertexNormals(false);
        t->calculateVertexNormals(false);
    }
    return;
}

void normalize(GLfloat *p) {
    double d=0.0;
    int i;
    for(i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0) for(i=0; i<3; i++) p[i]/=d;
}

void parse_floats(char *buffer, GLfloat nums[]) {
    int i;
    char *ps;
    
    // The buffer should contain 4 values, the first three are the actual translation, rotation, or scaling values and the fourth is the w value (1.0)
    ps = strtok(buffer, " ");
    for (i=0; ps; i++) {
        nums[i] = atof(ps);     // change the value in the current token to a float, store it in the appropriate array in the OBJECT struct
        ps = strtok(NULL, " ");
        //printf("read %f ",nums[i]);
    }
    
}

void parse_obj(char *buffer){
    OBJECT *po;
    char *pshape, *pshine, *pemi, *pamb, *pdiff, *pspec, *ptranslate, *pscale, *protate;
    
    my_assert ((num_objects < NUM_OBJECTS), "too many objects");
    po = &my_objects[num_objects++];    // set the OBJECT to the current one being parsed, increment the number of objects
    
    pshape  = strtok(buffer, " ");
    printf("pshape is %s\n",pshape);
    
    ptranslate    = strtok(NULL, "()");  strtok(NULL, "()");
    pscale        = strtok(NULL, "()");  strtok(NULL, "()");
    protate       = strtok(NULL, "()");  strtok(NULL, "()");
    
    pshine  = strtok(NULL, "()");strtok(NULL, "()");
    //printf("pshine is %s\n",pshine);
    
    pemi    = strtok(NULL, "()");  strtok(NULL, "()");
    pamb    = strtok(NULL, "()");  strtok(NULL, "()");
    pdiff   = strtok(NULL, "()");  strtok(NULL, "()");
    pspec   = strtok(NULL, "()");  strtok(NULL, "()");
    
    
    po->sid  = atoi(pshape);
    po->shine = atof(pshine);
    
    parse_floats(ptranslate, po->translate);    // get the translation values and store it in the GLfloat translate[4] array
    parse_floats(pscale, po->scale);    // get the translation values and store it in the GLfloat scale[4] array
    parse_floats(protate, po->rotate);  // get the translation values and store it in the GLfloat rotate[4] array
    
    parse_floats(pemi, po->emi);
    parse_floats(pamb, po->amb);
    parse_floats(pdiff, po->diff);
    parse_floats(pspec, po->spec);
    
    // use switch to create your objects, cube given as example
    // this vector holds all the object details - for each object read through the spec file, the translation,
    // rotation, and scaling transformation information is stored and added to a larger vector that will
    // store these translations for multiple of that object (e.g. multiple cubes, or multiple spheres)
    std::vector<float> innerVector = std::vector<float>();
    innerVector.push_back(po->translate[0]);
    innerVector.push_back(po->translate[1]);
    innerVector.push_back(po->translate[2]);
    innerVector.push_back(po->rotate[0]);
    innerVector.push_back(po->rotate[1]);
    innerVector.push_back(po->rotate[2]);
    innerVector.push_back(po->scale[0]);
    innerVector.push_back(po->scale[1]);
    innerVector.push_back(po->scale[2]);
    
    switch (po->sid) {
        case 1:
            cube_translations.push_back(innerVector);
            break;
        case 2: // house
            house->make_shape();
            break;
        case 3:
            sphere_translations.push_back(innerVector);
            break;
        case 4:
            cylinder_translations.push_back(innerVector);
            break;
        case 5: //cone
            ic->make_shape();
            break;
        case 6: //torus
            t->make_shape();
            break;
        default:
            break;
    }
    printf("read object\n");
}

void parse_camera(char *buffer){
    CAM *pc;
    char *ppos, *plook, *pup;
    
    pc = &my_cam;   // get the address of the CAM object to directly manipulate it
    
    strtok(buffer, "()");
    ppos  = strtok(NULL, "()");  strtok(NULL, "()");
    plook  = strtok(NULL, "()");  strtok(NULL, "()");
    pup  = strtok(NULL, "()");  strtok(NULL, "()");
    
    parse_floats(ppos, pc->pos);
    parse_floats(plook, pc->at);
    parse_floats(pup, pc->up);
    
    // set the position of the camera and the target
    setCamera(pc->pos[0], pc->pos[1], pc->pos[2], pc->at[0], pc->at[1], pc->at[2]);
    // set the camera distance
    cameraDistanceX = pc->pos[0];
    cameraDistanceY = pc->pos[1];
    cameraDistanceZ = pc->pos[2];
    
    pc->at[0] += pc->pos[0];
    pc->at[1] += pc->pos[1];
    pc->at[2] += pc->pos[2];
    
    printf("%f, %f, %f\n", pc->at[0], pc->at[1], pc->at[2]);
    
    pc->dir[0] = pc->at[0] - pc->pos[0];
    pc->dir[1] = pc->at[1] - pc->pos[1];
    pc->dir[2] = pc->at[2] - pc->pos[2];
    normalize(pc->dir);
    
    printf("read camera\n");
}

void parse_light(char *buffer){
    LITE *pl;
    char *pamb, *pdiff, *pspec, *ppos, *pdir, *pang;
    my_assert ((num_lights < MAX_LIGHTS), "too many lights");
    pl = &my_lights[++num_lights];
    
    strtok(buffer, "()");
    pamb  = strtok(NULL, "()");  strtok(NULL, "()");
    pdiff = strtok(NULL, "()");  strtok(NULL, "()");
    pspec = strtok(NULL, "()");  strtok(NULL, "()");
    ppos  = strtok(NULL, "()");  strtok(NULL, "()");
    pdir  = strtok(NULL, "()");  strtok(NULL, "()");
    pang  = strtok(NULL, "()");
    
    parse_floats(pamb, pl->amb);
    parse_floats(pdiff, pl->diff);
    parse_floats(pspec, pl->spec);
    parse_floats(ppos, pl->pos);
    if (pdir) {
        parse_floats(pdir, pl->dir);
        pl->angle = atof(pang);
        //printf("angle %f\n", pl->angle);
    }
    else
        pl->dir[0]= pl->dir[1]= pl->dir[2] =0;
    printf("read light\n");
    
}

/* assuming the spec is going to be properly written
 not error-checking here */
void read_spec(char *fname) {
    char buffer[300];
    FILE *fp;
    //printf("%s\n\n", fname);
    fp = fopen(fname, "r");
    my_assert(fp, "can't open spec");
    while(!feof(fp)){
        fgets(buffer, 300, fp);
        //printf("read line: %s\n", buffer);
        switch (buffer[0]) {
            case '#':
                break;
            case '1':  case '2': case '3': case '4': case '5': case '6':
                //read in the objects
                parse_obj(buffer);
                break;
                //etc
                
            case 'l':
                parse_light(buffer);
                break;
                
            case 'c':
                parse_camera(buffer);
                break;
                
            default:
                break;
        }
    }
}

void lighting_setup () {
    int i;
    GLfloat globalAmb[]     = {.1, .1, .1, .1};
    
    //enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    // reflective propoerites -- global ambiant light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    
    // setup properties of lighting
    for (i=1; i<=num_lights; i++) {
        glEnable(GL_LIGHT0+i);
        glLightfv(GL_LIGHT0+i, GL_AMBIENT, my_lights[i].amb);
        glLightfv(GL_LIGHT0+i, GL_DIFFUSE, my_lights[i].diff);
        glLightfv(GL_LIGHT0+i, GL_SPECULAR, my_lights[i].spec);
        glLightfv(GL_LIGHT0+i, GL_POSITION, my_lights[i].pos);
        if ((my_lights[i].dir[0] > 0) ||  (my_lights[i].dir[1] > 0) ||  (my_lights[i].dir[2] > 0)) {
            glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, my_lights[i].angle);
            glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, my_lights[i].dir);
        }
    }
    
}

/*
 *  drawScene()
 *  ------
 *  Draw the entire Scene
 */
void drawScene()
{
    drawSkybox(3.5*25);
}

/*
 *  drawSkybox(double D)
 *  ------
 *  Draws the skybox around the entire screen
 */
void drawSkybox(double D)
{
    glColor3fv(white);
    glEnable(GL_TEXTURE_2D);
        
        GLfloat mat_specular[]      = { 0.5, 0.5, 0.5, 0.5 };
        GLfloat mat_diffuse[]       = { 0.3, 0.3, 0.3, 0.3 };
        GLfloat mat_shininess[]     = { 25.0 };
        
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        
        glColor3f(0.2, 0.2, 0.2);
    
    // Bind the texture before you begin drawing
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_DOWN]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(+D, -5.0f, 30.0f);
    glTexCoord2f(0.0, 1.0); glVertex3f(+D, -5.0f,  -30.0f);
    glTexCoord2f(1.0, 1.0); glVertex3f( -D, -5.0f,  -30.0f);
    glTexCoord2f(1.0, 0.0); glVertex3f( -D, -5.0f, 30.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,skybox[SKY_RIGHT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-D,-D,-D);
    glTexCoord2f(1,0); glVertex3f(+D,-D,-D);
    glTexCoord2f(1,1); glVertex3f(+D,+D,-D);
    glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,skybox[SKY_RIGHT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(+D,-D,+D);
    glTexCoord2f(1,0); glVertex3f(-D,-D,+D);
    glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
    glTexCoord2f(0,1); glVertex3f(+D,+D,+D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,  skybox[SKY_BACK]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-D,-D,-D + 5);
    glTexCoord2f(1,0); glVertex3f(+D,-D,-D + 5);
    glTexCoord2f(1,1); glVertex3f(+D,+5,-D + 5);
    glTexCoord2f(0,1); glVertex3f(-D,+5,-D + 5);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

bool checkCollision()
{
    num_collisions = 0;
    
    for (int i = 0; i < 50; i++) {
         for (int j = 0; j < 50; j++) {
             if (baseball->obj_center.getX() >= bat->vertices[j][i].getX() + 0.9 && baseball->obj_center.getX() <= bat->vertices[j][i].getX() + 1.4 && (baseball->obj_center.getX() != 0.0) && bat->vertices[j][i].getX() != 0.0 && baseball->obj_center.getY() >= bat->vertices[j][i].getY() - 0.4 && baseball->obj_center.getY() <= bat->vertices[j][i].getY() + 0.4  && baseball->obj_center.getZ() >= bat->vertices[j][i].getZ() - 0.4 && baseball->obj_center.getZ() <= bat->vertices[j][i].getZ() + 0.4 && (baseball->obj_center.getZ() != 0.0) && bat->vertices[j][i].getZ() != 0.0) {
                 num_collisions++;
             }
         }
    }
    //printf("OBJ1 CENTER: %f, OBJ2 CENTER: %f\n", bat->obj_center.getZ(), baseball->obj_center.getZ());
    
    // printf("NUM COLLISIONS: %d\n", num_collisions);
    if (num_collisions > 0)
        return true;
    else
        return false;
}


///////////////////////////////////////////////////////////////////////////////
// draw a grid on XZ-plane
///////////////////////////////////////////////////////////////////////////////
void drawGrid(float size, float step)
{
    // disable lighting
    glDisable(GL_LIGHTING);
    
    // 20x20 grid
    glBegin(GL_LINES);
    
    glColor3f(0.5f, 0.5f, 0.5f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size, 0,  i);   // lines parallel to X-axis
        glVertex3f( size, 0,  i);
        glVertex3f(-size, 0, -i);   // lines parallel to X-axis
        glVertex3f( size, 0, -i);
        
        glVertex3f( i, 0, -size);   // lines parallel to Z-axis
        glVertex3f( i, 0,  size);
        glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
        glVertex3f(-i, 0,  size);
    }
    
    // x-axis
    glColor3f(1, 0, 0);
    glVertex3f(-size, 0, 0);
    glVertex3f( size, 0, 0);
    
    // z-axis
    glColor3f(0,0,1);
    glVertex3f(0, 0, -size);
    glVertex3f(0, 0,  size);
    
    glEnd();
    
    // enable lighting back
    glEnable(GL_LIGHTING);
}

void my_reshape(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    
    if (h == 0)
        h = 1;
    
    // ensure a square view port
    glViewport(0,0,min(w,h),min(w,h)) ;
    
    UpdateProjection(projectionType);
    return ;
}

// handles translating the objects read in from the spec
void real_translation(OBJECT *po, GLfloat x, GLfloat y, GLfloat z) {
    // use switch to create your objects, cube given as example
    switch (po->sid){
        case 1: //cube
            my_cubes.back()->real_translation(x, y, z);
            break;
        case 2: // house
            house->real_translation(x, y, z);
            break;
        case 3: // sphere
            my_spheres.back()->real_translation(x, y, z);
            break;
        case 4: // cylinder
            my_cylinders.back()->real_translation(x, y, z);
            break;
        case 5: // cone
            ic->real_translation(x, y, z);
            break;
        case 6: // torus
            t->real_translation(x, y, z);
            break;
    }
}

// handles scaling the objects read in from the spec
void real_scaling(OBJECT *po, GLfloat sx, GLfloat sy, GLfloat sz) {
    // use switch to create your objects, cube given as example
    switch (po->sid){
        case 1: //cube
            my_cubes.back()->real_scaling(sx, sy, sz);
            break;
        case 2: // house
            house->real_scaling(sx, sy, sz);
            break;
        case 3: // sphere
            my_spheres.back()->real_scaling(sx, sy, sz);
            break;
        case 4: // cylinder
            my_cylinders.back()->real_scaling(sx, sy, sz);
            break;
        case 5: // cone
            ic->real_scaling(sx, sy, sz);
            break;
        case 6: // torus
            t->real_scaling(sx, sy, sz);
            break;
    }
}

// handles rotating the objects read in from the spec
void real_rotation(OBJECT *po, GLfloat deg, GLfloat x, GLfloat y, GLfloat z) {
    // use a switch to rotate the particular object based on shape ID
    switch (po->sid){
        case 1: //cube
            my_cubes.back()->real_rotation(deg, x, y, z);
            break;
        case 2: // house
            house->real_rotation(deg, x, y, z);
            break;
        case 3: // sphere
            my_spheres.back()->real_rotation(deg, x, y, z);
            break;
        case 4: // cylinder
            my_cylinders.back()->real_rotation(deg, x, y, z);
            break;
        case 5: // cone
            ic->real_rotation(deg, x, y, z);
            break;
        case 6: // torus
            t->real_rotation(deg, x, y, z);
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// print the camera details - current eye point, look vector, up vector,
// height angle, aspect ratio, and world to film matrix
// (none)
///////////////////////////////////////////////////////////////////////////////
void printMatrixDetails() {
    std::cout << "===== Current Eye Point =====\n";
    printf("X: %f, Y: %f, Z: %f\n\n", my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    std::cout << "===== Look Vector =====\n";
    printf("X: %f, Y: %f, Z: %f\n\n", my_cam.dir[0], my_cam.dir[1], my_cam.dir[2]);
    std::cout << "===== Up Vector =====\n";
    printf("X: %f, Y: %f, Z: %f\n\n", my_cam.up[0], my_cam.up[1], my_cam.up[2]);
    std::cout << "===== Height Angle =====\n";
    std::cout << orthoTop - orthoBottom << std::endl << std::endl;
    std::cout << "===== Aspect Ratio =====\n";
    std::cout << orthoRight - orthoLeft << std::endl << std::endl;
    std::cout << "===== World to Film Matrix =====\n";
    std::cout << matrixProjection << std::endl;
}

void my_special_keys(int key, int x, int y)
{
    switch (key)
	{
		case GLUT_KEY_LEFT:
            // Move the batter
            if (my_spheres[1]->obj_center.getX() > -3.5) {
                my_spheres[1]->real_translation(-1, 0, 0);
                my_cubes[3]->real_translation(-1, 0, 0);
                my_cubes[4]->real_translation(-1, 0, 0);
                my_cubes[5]->real_translation(-1, 0, 0);
                my_cylinders[2]->real_translation(-1, 0, 0);
                my_cylinders[3]->real_translation(-1, 0, 0);
                bat->real_translation(-1, 0, 0);
                batPos[0]--;
            }
            
            glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
            if (my_spheres[1]->obj_center.getX() < 3.5) {
                my_spheres[1]->real_translation(1, 0, 0);
                my_cubes[3]->real_translation(1, 0, 0);
                my_cubes[4]->real_translation(1, 0, 0);
                my_cubes[5]->real_translation(1, 0, 0);
                my_cylinders[2]->real_translation(1, 0, 0);
                my_cylinders[3]->real_translation(1, 0, 0);
                bat->real_translation(1, 0, 0);
                batPos[0]++;
            }
            glutPostRedisplay();
			break;
//		case GLUT_KEY_UP:
//	        batter->real_translation(0, 0, 1);
//            batterPos[2]++;
//            glutPostRedisplay();
//			break;
//		case GLUT_KEY_DOWN:
//            batter->real_translation(0, 0, -1);
//            batterPos[2]--;
//            glutPostRedisplay();
//			break;
	}
}

void my_keyboard( unsigned char key, int x, int y ) {
    
    switch( key ) {
            // increase aspect ratio
        case 'A': {
            orthoLeft-=0.1;
            orthoRight+=0.1;
            UpdateProjection(projectionType);
            glutPostRedisplay();
        }; break;
            
            // decrease aspect ratio
        case 'a': {
            orthoLeft+=0.1;
            orthoRight-=0.1;
            UpdateProjection(projectionType);
            glutPostRedisplay();
        }; break;
            
        case 'c':
        case 'C':
            projectionType = ORTHO;
            UpdateProjection(projectionType);
            glutPostRedisplay();
            break;
        case ' ':
            //code to switch between first person view and back as appropriate
            glutPostRedisplay();
            break;
            
            // increase height angle
        case 'H': {
            orthoTop+=0.1;
            orthoBottom-=0.1;
            UpdateProjection(projectionType);
            glutPostRedisplay();
        }; break;
            
            // decrease height angle
        case 'h': {
            orthoTop-=0.1;
            orthoBottom+=0.1;
            UpdateProjection(projectionType);
            glutPostRedisplay();
        }; break;
            
        case 'F':
        case 'f': {
            crt_transform = CAM_FAR_PLANE_MODE;
        }; break;
            
        case 'N':
        case 'n': {
            crt_transform = CAM_NEAR_PLANE_MODE;
        }; break;
            
        case 'L':
        case 'l': {
            crt_transform = CAM_ZOOM_MODE;
        }; break;
            
        case 'T':
        case 't': {
            crt_transform = REAL_TRANSLATION_MODE;
        }; break;
            
        case 'R':
        case 'r': {
            crt_transform = REAL_ROTATION_MODE;
        }; break;
            
        case 'u': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, -1, 0.5, 0, 0);
                glutPostRedisplay();
            }
        }; break;
            
        case 'U': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, 1, 0.5, 0, 0);
                glutPostRedisplay();
            }
        }; break;
            
        case 'v': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, -1, 0, 0.5, 0);
                glutPostRedisplay();
            }
        }; break;
            
        case 'V': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, 1, 0, 0.5, 0);
                glutPostRedisplay();
            }
        }; break;
            
        case 'w': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, -1, 0, 0, 0.5);
                glutPostRedisplay();
            }
        }; break;
            
        case 'W': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(LOCAL, 1, 0, 0, 0.5);
                glutPostRedisplay();
            }
        }; break;
            
        case 'x': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, -1, 0.5, 0, 0);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(CLOCKWISE, X_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'X': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, 1, 0.5, 0, 0);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(COUNTERCLOCKWISE, X_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'y': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, -1, 0, 0.5, 0);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(CLOCKWISE, Y_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'Y': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, 1, 0, 0.5, 0);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(COUNTERCLOCKWISE, Y_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'z': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, -1, 0, 0, 0.5);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(CLOCKWISE, Z_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'Z': {
            if (crt_transform == REAL_TRANSLATION_MODE) {
                translateCam(WORLD, 1, 0, 0, 0.5);
            }
            else if (crt_transform == REAL_ROTATION_MODE) {
                rotateCam(COUNTERCLOCKWISE, Z_AXIS);
            }
            glutPostRedisplay();
        }; break;
            
        case 'S':
        case 's':
        {
            projectionType = PERSPECTIVE;
            UpdateProjection(projectionType);
            glutPostRedisplay();
        }; break;
            
        case 'm':
        {
            if (throw_pitch == 0) {
                my_cubes[2]->arb_rotation_point(-10, 1.5, 2.5, -1.5, 1, 0, 0);
                // Generate a random number between 1 and 3 to randomize pitches
                tempTest = rand() % 3 + 1;
                throw_pitch = 1;
                pitchSpeed = -baseball->velocity[2];
                totalPitches++;
            }
            else
                throw_pitch = 0;
            redisplayAll();
            // glutPostRedisplay();
        }; break;
            
        case 'P':
        case 'p':
        {
            printMatrixDetails();
        }; break;
            
        case '-':
            switch (crt_transform) {
                case CAM_ZOOM_MODE:
                    zoom(-1);
                    UpdateProjection(projectionType);
                    break;
                case CAM_FAR_PLANE_MODE:
                    orthoFar += 1;
                    UpdateProjection(projectionType);
                    break;
                case CAM_NEAR_PLANE_MODE:
                    if (orthoNear + 1 <= orthoFar) orthoNear += 1;
                    UpdateProjection(projectionType);
                    break;
            }
            glutPostRedisplay();
            break;
        case '+':
            switch (crt_transform) {
                case CAM_ZOOM_MODE:
                    zoom(1);
                    UpdateProjection(projectionType);
                    break;
                case CAM_FAR_PLANE_MODE:
                    if (orthoFar - 1 >= orthoNear) orthoFar -= 1;
                    UpdateProjection(projectionType);
                    break;
                case CAM_NEAR_PLANE_MODE:
                    if (orthoNear - 1 >= 0) orthoNear -= 1;
                    UpdateProjection(projectionType);
                    break;
            }
            glutPostRedisplay();
            break;
        case 'q':
        case 'Q':
            exit(0) ;
            break ;
        default: break;
    }
    
    return ;
}

// Camera methods
///////////////////////////////////////////////////////////////////////////////
// calculate the u,v, and w local axes of the camera object
// (none)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void calculateCameraVectors() {
    // Step 1: Calculate the w vector using the look vector
    // Construct the look vector and position vertex
    Vector3D lookAt(my_cam.at[0], my_cam.at[1], my_cam.at[2]);
    Vector3D pos(my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    
    Vector3D look = lookAt - pos;
    if(look.length() != 0) {
        look.normalize();
        w = look * -1;
    } else {
        w = look * -1;
    }
    
    // Step 2: Calculate the v vector using up and w
    Vector3D up(my_cam.up[0], my_cam.up[1], my_cam.up[2]);
    
    if (up.dotProduct(w) == 1) {
        up = w - Vector3D(0,0,1.0);
    }
    
    v = up - (up.dotProduct(w) * w);
    if(v.length() != 1)     // if the length of v does not equal 1, normalize it
        v.normalize();
    // otherwise v is already normalized
    
    // Step 3: Calculate the u vector using v and w
    u = v.crossProduct(w);
}

///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    float forward[4];
    float up[4];
    float left[4];
    float position[4];
    float invLength;
    
    // determine forward vector (direction reversed because it is camera)
    forward[0] = posX - targetX;    // x
    forward[1] = posY - targetY;    // y
    forward[2] = posZ - targetZ;    // z
    forward[3] = 0.0f;              // w
    // normalize it without w-component
    invLength = 1.0f / sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
    forward[0] *= invLength;
    forward[1] *= invLength;
    forward[2] *= invLength;
    
    // assume up direction is straight up
    up[0] = 0.0f;   // x
    up[1] = 1.0f;   // y
    up[2] = 0.0f;   // z
    up[3] = 0.0f;   // w
    
    // compute left vector with cross product
    left[0] = up[1]*forward[2] - up[2]*forward[1];  // x
    left[1] = up[2]*forward[0] - up[0]*forward[2];  // y
    left[2] = up[0]*forward[1] - up[1]*forward[0];  // z
    left[3] = 1.0f;                                 // w
    
    // re-compute orthogonal up vector
    up[0] = forward[1]*left[2] - forward[2]*left[1];    // x
    up[1] = forward[2]*left[0] - forward[0]*left[2];    // y
    up[2] = forward[0]*left[1] - forward[1]*left[0];    // z
    up[3] = 0.0f;                                       // w
    
    // camera position
    position[0] = -posX;
    position[1] = -posY;
    position[2] = -posZ;
    position[3] = 1.0f;
    
    // copy axis vectors to matrix
    matrixView.identity();
    matrixView.setColumn(0, left);
    matrixView.setColumn(1, up);
    matrixView.setColumn(2, forward);
    matrixView.setColumn(3, position);
}

///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs to transpose it
///////////////////////////////////////////////////////////////////////////////
Matrix4x4 setFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4x4 mat;
    mat[0]  =  2 * n / (r - l);
    mat[2]  =  (r + l) / (r - l);
    mat[5]  =  2 * n / (t - b);
    mat[6]  =  (t + b) / (t - b);
    mat[10] = -(f + n) / (f - n);
    mat[11] = -(2 * f * n) / (f - n);
    mat[14] = -1;
    mat[15] =  0;
    return mat;
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
Matrix4x4 setFrustum(float fovY, float aspectRatio, float front, float back)
{
    float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane
    
    // params: left, right, bottom, top, near, far
    return setFrustum(-width, width, -height, height, front, back);
}

///////////////////////////////////////////////////////////////////////////////
// set an orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
Matrix4x4 setOrthoFrustum(float left, float right, float bottom, float top, float near_val, float far_val)
{
    Matrix4x4 mat;
    mat[0]  =  2 / (right - left);
    mat[3]  =  -(right + left) / (right - left);
    mat[5]  =  2 / (top - bottom);
    mat[7]  =  -(top + bottom) / (top - bottom);
    mat[10] = -2 / (far_val - near_val);
    mat[11] = -(far_val + near_val) / (far_val - near_val);
    return mat;
}

///////////////////////////////////////////////////////////////////////////////
// zoom into and away from the lookAt point using camera lens
// params: direction of zoom
// Note: ZOOM IN DOES NOT WORK CORRECTLY
///////////////////////////////////////////////////////////////////////////////
void zoom(int direction) {
    if (direction == -1) {
        orthoLeft /= M_ZOOM_OUT;
        orthoRight /= M_ZOOM_OUT;
        
        orthoTop /= M_ZOOM_OUT;
        orthoBottom /= M_ZOOM_OUT;
    } else {
        orthoLeft /= M_ZOOM_IN;
        orthoRight /= M_ZOOM_IN;
        
        orthoTop /= M_ZOOM_IN;
        orthoBottom /= M_ZOOM_IN;
        
    }
}

void    buildCamTranslationMatrix() {
    Vector3D pos(my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    // Step 5: Compute the camera rotation * translation matrix
    Vector3D Pn = pos + w*orthoNear;
    
    matrixTranslation = Matrix4x4(1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1, 0,
                                  -1 * Pn.getX(), -1 * Pn.getY(), -1 * Pn.getZ(), 1);
}

void    buildCamRotationMatrix() {
    // Step 4: Compute the camera rotation matrix
    matrixRotation = Matrix4x4(u.getX(), v.getX(), w.getX(), 0,
                               u.getY(), v.getY(), w.getY(), 0,
                               u.getZ(), v.getZ(), w.getZ(), 0,
                               0, 0, 0, 1);
}

void    buildCamScalingMatrix() {
    
    matrixScaling = Matrix4x4(2/(orthoRight - orthoLeft), 0, 0, 0,
                              0, 2/(orthoTop - orthoBottom), 0, 0,
                              0, 0, 1/orthoFar, 0,
                              0, 0, 0, 1);
}

void    translateCam(int environment, int direction, float dx, float dy, float dz){
    Vector3D pos(my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    Vector3D lookAt(my_cam.at[0], my_cam.at[1], my_cam.at[2]);
    
    Vector3D camDistance(cameraDistanceX, cameraDistanceY, cameraDistanceZ);
    if (environment == LOCAL) {
        if (direction == -1) {
            if (dy == 0 && dz == 0) {
                pos -= (dx * u);
                lookAt -= (dx * u);
                camDistance -= (dx * u);
            }
            if (dx == 0 && dz == 0) {
                pos -= (dy * v);
                lookAt -= (dy * v);
                camDistance -= (dy * v);
            }
            if (dx == 0 && dy == 0) {
                pos -= (dz * w);
                lookAt -= (dz * w);
                camDistance -= (dz * w);
            }
        }
        else if (direction == 1) {
            if (dy == 0 && dz == 0) {
                pos += (dx * u);
                lookAt += (dx * u);
                camDistance += (dx * u);
            }
            if (dx == 0 && dz == 0) {
                pos += (dy * v);
                lookAt += (dy * v);
                camDistance += (dy * v);
            }
            if (dx == 0 && dy == 0) {
                pos += (dz * w);
                lookAt += (dz * w);
                camDistance += (dz * w);
            }
        }
        my_cam.pos[0] = pos.getX(); my_cam.pos[1] = pos.getY(); my_cam.pos[2] = pos.getZ();
        my_cam.at[0] = lookAt.getX(); my_cam.at[1] = lookAt.getY(); my_cam.at[2] = lookAt.getZ();
        cameraDistanceX = camDistance.getX();
        cameraDistanceY = camDistance.getY();
        cameraDistanceZ = camDistance.getZ();
        
    } else if (environment == WORLD) {
        if (direction == -1) {
            if (dy == 0 && dz == 0) {   // x-axis
                my_cam.pos[0]-=dx;
                my_cam.at[0]-=dx;
                cameraDistanceX-=dx;
            }
            if (dx == 0 && dz == 0) {   // y-axis
                my_cam.pos[1]-=dy;
                my_cam.at[1]-=dy;
                cameraDistanceY-=dy;
            }
            if (dx == 0 && dy == 0) {   // z-axis
                my_cam.pos[2]-=dz;
                my_cam.at[2]-=dz;
                cameraDistanceZ -=dz;
            }
        }
        else if (direction == 1) {
            if (dy == 0 && dz == 0) {
                my_cam.pos[0]+=dx;
                my_cam.at[0]+=dx;
                cameraDistanceX+=dx;
            }
            if (dx == 0 && dz == 0) {
                my_cam.pos[1]+=dy;
                my_cam.at[1]+=dy;
                cameraDistanceY+=dy;
            }
            if (dx == 0 && dy == 0) {
                my_cam.pos[2]+=dz;
                my_cam.at[2]+=dz;
                cameraDistanceZ+=dz;
            }
        }
    }
    // set the position and lookAt points to the newly calculated ones
    pos = Vector3D(my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    lookAt = Vector3D(my_cam.at[0], my_cam.at[1], my_cam.at[2]);
    
    Vector3D dir = lookAt - pos;
    dir.normalize();
    my_cam.dir[0] = dir.getX();
    my_cam.dir[1] = dir.getY();
    my_cam.dir[2] = dir.getZ();
}

void    rotateCam(int direction, int axis){
    isRotation = true;
    
    Vector3D pos(my_cam.pos[0], my_cam.pos[1], my_cam.pos[2]);
    Vector3D lookAt(my_cam.at[0], my_cam.at[1], my_cam.at[2]);
    Vector3D up(my_cam.up[0], my_cam.up[1], my_cam.up[2]);
    
    if (direction == CLOCKWISE) {
        switch (axis) {
            case X_AXIS:
                lookAt.RotateAboutX(5);
                cameraAngleX+=5;
                break;
            case Y_AXIS:
                lookAt.RotateAboutY(5);
                cameraAngleY+=5;
                break;
            case Z_AXIS:
                lookAt.RotateAboutZ(5);
                cameraAngleZ+=5;
                break;
            default:
                break;
        }
    }
    else if (direction == COUNTERCLOCKWISE) {
        switch (axis) {
            case X_AXIS:
                lookAt.RotateAboutX(-5);
                cameraAngleX-=5;
                break;
            case Y_AXIS:
                lookAt.RotateAboutY(-5);
                cameraAngleY-=5;
                break;
            case Z_AXIS:
                lookAt.RotateAboutZ(-5);
                cameraAngleZ-=5;
                break;
            default:
                break;
        }
    }
    // update the cam direction and up, position stays the same
    //my_cam.up[0] = up.getX(); my_cam.up[1] = up.getY(); my_cam.up[2] = up.getZ();
    my_cam.at[0] = lookAt.getX(); my_cam.at[1] = lookAt.getY(); my_cam.at[2] = lookAt.getZ();
    lookAt = Vector3D(my_cam.at[0], my_cam.at[1], my_cam.at[2]);
    
    Vector3D dir = lookAt - pos;
    dir.normalize();
    my_cam.dir[0] = dir.getX();
    my_cam.dir[1] = dir.getY();
    my_cam.dir[2] = dir.getZ();
}

/****************************************************************************
 UpdateProjection() - used to create the projection matrix, equivalent to
 putWorldToFilm() method from project description
 
 Sets the current projection mode. If toggle is set to GL_TRUE, then the
 projection will be toggled between perspective and orthograpic. Otherwise,
 the previous selection will be used again.
 *****************************************************************************/
void UpdateProjection(int projection)
{
    // select the projection matrix and clear it out
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // choose the appropriate projection based on the currently toggled mode
    if (projection == 0)    // perspective projection
    {
        // set the perspective with the appropriate aspect ratio
        matrixProjection = setFrustum(70, 1.0, 1, 200);     //this are the default settings for the gluPerspective function call above
        
        glMultMatrixf(matrixProjection.getTranspose());
    }
    else if (projection == 1)   //orthographic projection
    {
        // set up an orthographic projection with the same near clip plane
        //glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
        matrixProjection = setOrthoFrustum(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
        
        glMultMatrixf(matrixProjection.getTranspose());
    }
    
    // select modelview matrix and clear it out
    glMatrixMode(GL_MODELVIEW);
} // end UpdateProjection

void my_mouse_drag(int x, int y) {
}

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void swingBat() {
        bat->arb_rotation_point(50, batPos[0] - 1, batPos[1], batPos[2], 0, 1, 0);
    if(batAngle >= 185) {
        doSwingBat = false;
        batAngle = 0.0f;
        //if (collision_detected)
            initBat();
    }
}

float tempVar = 0;
void hitBall() {
    float gravity = 9.80665;
    // We will implement a rudimentary form of randomization
    // Depending on the number of vertices that collided with the baseball,
    // we will make the ball go further and at a higher angle.
    if (hit_init) {
        rand_x_direction = RandomFloat(-3, 3);
        if (num_collisions > 0 && num_collisions < 50) {
            iVel = RandomFloat(0.1, 0.9);
            theta = RandomFloat(-20, -1);
        }
        else if (num_collisions > 50 && num_collisions < 100) {
            iVel = RandomFloat(1.0, 1.9);
            theta = RandomFloat(1, 40);
        }
        else if (num_collisions > 100 && num_collisions < 200) {
            iVel = RandomFloat(2.0, 2.9);
            theta = RandomFloat(41, 60);
        }
        else if (num_collisions > 200) {
            iVel = RandomFloat(3.0, 3.9);
            theta = RandomFloat(61, 70);
        }
        hit_init = false;
    }
    timestep++;
    
    // printf("IVEL: %f, THETA: %f, RAND_X_DIRECTION: %f\n", iVel, theta, rand_x_direction);
    
    
    // Calculate the incremenation values for each iteration of the timeout
    float z_increment = iVel * cosf(theta * DEGS2RADIANS);
    float y_increment = iVel * sinf(theta * DEGS2RADIANS) - (gravity * deltaTime);
    z_increment = -1 * z_increment;     // negate the z_increment to have the ball go in the right direction
    deltaTime += 0.01;  // increment the delta time
    baseball->real_translation(rand_x_direction, y_increment, z_increment);    // translate the ball by the calculated amount
    
    baseballPos[0] += rand_x_direction;
    baseballPos[1] += y_increment;
    baseballPos[2] += z_increment;
    
    if (baseball->obj_center.getZ() < -6.0 && baseball->obj_center.getY() > 10)
        hitHomeRun = true;
    
    if (baseball->obj_center.getZ() > cameraDistanceZ) {
        if (!hitHomeRun)
            hitFoulBall = true;
    }
    
    // Check to see if the ball has hit the ground
    if (baseball->obj_center.getY() <= 0 || (baseball->obj_center.getY() < 2 && baseball->obj_center.getY() > 0 && baseball->obj_center.getZ() < 1 && baseball->obj_center.getZ() > 0)) {
        if (timestep > 1) {
            // printf("BASEBALL CENTER Z: %f\n", baseball->obj_center.getZ());
            timestep = 0.0f;
            if (hitHomeRun)
                numHomeRuns++;
            if (hitFoulBall)
                numFouls++;
        
            hitHomeRun = false; hitFoulBall = false;
            my_cubes[2]->arb_rotation_point(10, 1.5, 2.5, -1.5, 1, 0, 0);
            init_ball();
            throw_pitch = 0;
            collision_detected = false;
            deltaTime = 0.01;
            cameraDistanceX = my_cam.pos[0]; cameraDistanceY = my_cam.pos[1]; cameraDistanceZ = my_cam.pos[2];
            return;
        }
    }
    // Velocity equations:
    // X-component of velocity
//    float vel_x = baseball->velocity[0] + baseball->acceleration[0] * deltaTime;
//    float vel_y = baseball->velocity[1] + baseball->acceleration[1] * deltaTime;
//    float vel_z = baseball->velocity[2] + baseball->acceleration[2] * deltaTime;
    
    //deltaTime += 0.1;
//    float ballX = baseballPos[0] + baseball->velocity[0] * deltaTime + (0.5) * baseball->acceleration[0] * pow(deltaTime, 2);
//    float ballY = baseballPos[1] + baseball->velocity[1] * deltaTime + (0.5) * baseball->acceleration[1] * pow(deltaTime, 2);
//    float ballZ = baseballPos[2] + baseball->velocity[2] * deltaTime + (0.5) * baseball->acceleration[2] * pow(deltaTime, 2);
//    float ballZ = (5.0) * cosf(45 * PI / 180) * deltaTime;
//    float ballX = 0;
//    float ballY = ((5.0) * sinf(45 * PI / 180) * deltaTime) - (0.5) * (-9.8) * powf(deltaTime, 2) ;
//    baseball
//    printf("CHANGE: %f\n", ballY - tempVar);
//    tempVar = ballY;
//    ballZ = -1 * ballZ;
    
    //baseball->real_translation(ballX , ballY, ballZ);
    if (!(baseball->obj_center.getZ() > my_cam.pos[0])) {
        cameraDistanceX += rand_x_direction;
        cameraDistanceY += y_increment;
        cameraDistanceZ += z_increment;
    }
    glutPostRedisplay();
}

void my_mouse(int button, int state, int mousex, int mousey) {
    
    switch( button ) {
            
        case GLUT_LEFT_BUTTON:
            if( state == GLUT_DOWN ) {
                
                // my_raytrace(mousex, mousey);
                //swingBat();
                // TODO: toggle bat, make it stop
                std::cout << doSwingBat << std::endl;
                if (doSwingBat)
                    doSwingBat = false;
                else
                    doSwingBat = true;
            }
            
            if( state == GLUT_UP ) {
            }
            break ;
            
        case GLUT_RIGHT_BUTTON:
            if ( state == GLUT_DOWN ) {
            }
            
            if( state == GLUT_UP ) {
            }
            break ;
    }
    
    
    
    return ;
}

/***********************************
 FUNCTION: draw_axes
 ARGS: none
 RETURN: none
 DOES: draws main X, Y, Z axes
 ************************************/
void draw_axes( void ) {
    glLineWidth( 5.0 );
    
    glDisable(GL_LIGHTING);
    
    glBegin(GL_LINES);
    {
        glColor3fv(colors[1]);
        glVertex4fv(vertices_axes[0]);
        glVertex4fv(vertices_axes[1]);
		
        glColor3fv(colors[4]);
        glVertex4fv(vertices_axes[0]);
        glVertex4fv(vertices_axes[2]);
		
        glColor3fv(colors[6]);
        glVertex4fv(vertices_axes[0]);
        glVertex4fv(vertices_axes[3]);
    }
    glEnd();
    glLineWidth( 1.0 );
    
    glEnable(GL_LIGHTING);
	
}

void draw_objects() {
    int i;
    // draw the bat
    bat->draw_shape(crt_render_mode, RED);
    // draw the baseball
    baseball->draw_shape(crt_render_mode, WHITE);
    
    for(i=0; i<num_objects; i++){
        OBJECT *cur;
        cur = &my_objects[i];
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, cur->amb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, cur->diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, cur->spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, &cur->shine);
        //glMaterialfv(GL_FRONT, GL_EMISSION, cur->emi);
        
        switch(cur->sid){
            case 1: //cube
                //printf("Drawing the cube...");
                for (int i = 0; i < my_cubes.size(); i++) {
                    my_cubes[i]->draw_shape(crt_render_mode, CYAN);
                }
                break;
            case 2: // house
                //printf("Drawing the house...");
                house->draw_shape(crt_render_mode, WHITE);
                break;
            case 3: // sphere
                //printf("Drawing the sphere...");
                for (int i = 0; i < my_spheres.size(); i++) {
                    my_spheres[i]->draw_shape(crt_render_mode, GREY);
                }
                break;
            case 4: // cylinder
                //printf("Drawing the cylinder...");
                for (int i = 0; i < my_cylinders.size(); i++) {
                    my_cylinders[i]->draw_shape(crt_render_mode, GREEN);
                }
                
                break;
            case 5: // cone
                //printf("Drawing the cone...");
                ic->draw_shape(crt_render_mode, RED);
                break;
            case 6: // torus
                //printf("Drawing the torus...");
                t->draw_shape(crt_render_mode, YELLOW);
                break;
            default: break;
        }
        
    }
}

void my_display() {
    
    calculateCameraVectors();
    buildCamTranslationMatrix();
    buildCamRotationMatrix();
    
    matrixModelView = matrixRotation * matrixTranslation;
    
    // clear all pixels, reset depth
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
    
    glLoadIdentity();
    
    // save the initial ModelView matrix before modifying ModelView matrix
    // Transform the camera
    glPushMatrix();
    matrixView.identity();
    matrixView.translate(-cameraDistanceX, -cameraDistanceY, -cameraDistanceZ);
    matrixView.rotateZ(cameraAngleZ);
    matrixView.rotateY(cameraAngleY);
    matrixView.rotateX(cameraAngleX);
    
    // copy view matrix to OpenGL
    glLoadMatrixf(matrixView.getTranspose());
    //@@ the equivalent code for using OpenGL routine is:
    //@@ glTranslatef(0, 0, -cameraDistance);
    //@@ glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    //@@ glRotatef(cameraAngleY, 0, 1, 0);   // heading
    //@@ glRotatef(cameraAngleZ, 0, 0, 1);  // roll
    // drawGrid();     // draw a grid on the x-z plane
    
    // compute the ModelView matrix and load it into OpenGL
    matrixModelView = matrixView * matrixModel;
    glLoadMatrixf(matrixModelView.getTranspose());
    
    //draw the objects, rays, and axes
    // draw_axes();
    
    // copy view matrix to OpenGL
    //glLoadMatrixf(matrixView.getTranspose());
    // glMultMatrixf(matrixModelView.get());
    
    // compute model matrix
    //matrixModel.identity();
    
    // compute modelview matrix
    //matrixModelView = matrixView * matrixModel;
    
    // copy modelview matrix to OpenGL
    //glLoadMatrixf(matrixModelView.getTranspose());
    
    //glLoadMatrixf(matrixView.get());
    
    //update the flashlight to follow the person
    
    //draw the objects
    //draw_axes();
    
    drawScene();    // draw the skybox
    
    draw_objects();
    
    // draw info messages
    showInfo();
    
    glPopMatrix();
    
    // this buffer is ready
    glutSwapBuffers();
}

/***********************************
 FUNCTION: my_TimeOut
 ARGS: timer id
 RETURN: none
 DOES: handles "timer" events
 ************************************/
void my_TimeOut(int id) {
    /* spin counter-clockwise*/
    /* schedule next timer event, 0.2 secs from now */
    if (doSwingBat) {
            swingBat();
            batAngle += 45;
            glutPostRedisplay();
    }
    if(throw_pitch == 1)
 	{
        // Handle the pitching
        if (tempTest == 1)
            baseball->real_translation(baseball->velocity[0] * deltaTime, 0, baseballPos[2] -=baseball->velocity[1] * deltaTime);
        else if (tempTest == 2)
            baseball->real_translation(0, 0, baseballPos[2] -=baseball->velocity[1] * deltaTime);
        else if (tempTest == 3)
            baseball->real_translation(baseball->velocity[0] * deltaTime, 0, -baseball->velocity[2] * deltaTime);
        
		glutPostRedisplay();
		//printf("posx: %f, posy: %f\n", baseball.obj_verts[0][0][0], baseball.obj_verts[0][0][1]);
		baseball->velocity[0] += baseball->acceleration[0] * deltaTime;
        baseball->velocity[1] += baseball->acceleration[1] * deltaTime;
        baseball->velocity[2] += baseball->acceleration[2] * deltaTime;
        
	  	if(baseball->obj_center.getZ() > 35)
	  	{
            my_cubes[2]->arb_rotation_point(10, 1.5, 2.5, -1.5, 1, 0, 0);
	  		init_ball();
            glutPostRedisplay();
            
            // Reset pitch and bat
            throw_pitch = 0;
            doSwingBat = false;
            initBat();
	  	}
        
        if(checkCollision())
	  	{
	  		printf("Collision detected.\n");
            collision_detected = true;
            doSwingBat = false;
            hit_init = true;
            glutPostRedisplay();
	  	}
        if (collision_detected)
            hitBall();
	}
    glutTimerFunc(10, my_TimeOut, 0);
    
    return ;
}

///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);
    
    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position
    
    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);
    
    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position
    
    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix
    
    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
    
    float color[4] = {1, 1, 1, 1};
    
    accuracy = (totalPitches > 0) ? ((float)numHomeRuns / (float)totalPitches) : 0;
    accuracy *= 100;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    
    drawString(" HOME RUN DERBY ", screenWidth/2 - 50, screenHeight - TEXT_HEIGHT, color, times_new_roman);
    drawString(" By Nikhil Venkatesh and Kevin Lu ", screenWidth/2 - 50, screenHeight - 2*TEXT_HEIGHT, color, times_new_roman);
    drawString("  === Stats ===", 0, screenHeight-TEXT_HEIGHT, color, font);
    ss << "  Number of Home Runs: " << numHomeRuns << std::ends;
    drawString(ss.str().c_str(), 0, screenHeight-(2*TEXT_HEIGHT), color, font);
    ss.str("");
    ss << "  Number of Foul Balls: " << numFouls << std::ends;
    drawString(ss.str().c_str(), 0, screenHeight-(3*TEXT_HEIGHT), color, font);
    ss.str("");
    ss << "  Total Number of Pitches: " << totalPitches << std::ends;
    drawString(ss.str().c_str(), 0, screenHeight-(4*TEXT_HEIGHT), color, font);
    ss.str("");
    ss << "  Pitch Speed: " << pitchSpeed << " mph " << std::ends;
    drawString(ss.str().c_str(), 0, screenHeight-(5*TEXT_HEIGHT), color, font);
    ss.str("");
    ss << "  Accuracy: " << accuracy << "%" << std::ends;
    drawString(ss.str().c_str(), 0, screenHeight-(6*TEXT_HEIGHT), color, font);
    ss.str("");
    
    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
    
    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix
    
    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}

void my_idle(void) {
    return ;
}