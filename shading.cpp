#include <stdio.h>
#include "iostream"
#include <math.h>
#include "GL/glut.h"
//#include <GL/glu.h>
//#pragma comment(lib, "glut.lib")
//#pragma comment(lib, "glut32.lib")


using namespace std;

//******************************************************************************/
//**** You may change the initial values of some parameter in your init() ******/
//****                                                                     *****/

double xfoc = 0., yfoc = 0., zfoc = 0.0;		//Focal point position
double fovy = 40.;								//Viewing angle

double xyzmax = 9.;								//Inital length of the axes in the small viewports

double Zfar = xyzmax;							//Inital distance of the far clipping plane
double Znear = 1.;								//Initial dist'n of the near clipping plane

double delta = 0.0625;							//Amount changed in a parameter when it is adjusted.

int    winWidth = 900;							//The default width of the window
int    winHeight = 600;							//The default height of the window

bool showCamera = true;							//Flag for showing the small viewports
bool showAxis = true;							//Flag for showing the the axes

//***                                                                      *****/
//******************************************************************************/

float xcam = 0., ycam = 0., zcam = 3.;		//Camera position



//max number of vertices and triangles
#define  MAXNUMV 2000000
#define  MAXNUMT 2500000

// the mesh
int NUMV;	        //number of vertices
int NUMT;	        //number of triangles

GLfloat vertices[MAXNUMV][3];
GLfloat normals[MAXNUMV][3];
GLfloat colors[MAXNUMV][3];
int indices[MAXNUMT][3];


//*****End of object definition *******************************************
GLfloat ambient[] = { 0.22, 0.15, 0.10, 1. };
GLfloat diffuse[] = { 0.89, 0.82, 0.58,1./* .5, .0, .0, 1.*/ };
GLfloat specular[] = { 0.5, 0.5, 0.5, 1. };
float   shininess = 120.0f;

GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1. };
GLfloat light_diffuse0[] = { 0.8, .8, 0.8, 1. };
GLfloat light_specular0[] = { 0.7, 0.7, 0.7, 1. };
GLfloat light_position0[] = { 0.5, 0.5, 1.0, 1.0 };

//Bezier曲面的控制点
// 3 by 3 控制点网格，一共9个控制点
// P00, P01, P02
// P10, P11, P12
// P20, P21, P22
// P[]={P00, P01, P02, P10, P11, P12, P20, P21,P22}
double  Px[9] = { -0.5, 0.1, 0.5, -0.5, 0.2, 0.5, -0.5, 0.1, 0.5 };
double Py[9] = { 0.1, 0.4, 0.1, -0.2, 0.2, -0.2 , -0.6, -0.4, -0.3 };
double  Pz[9] = { -0.2, 0.1, 0.3, 0.2, 0.3, 0.2, 0.3, 0.5, 0.1 };


//计算S(u,v)处的点(x,y,z)
void surfacepoint(double u, double v, double& x, double& y, double& z)
{
    double a[3] = { 0 }, b[3] = { 0 }, c[3] = { 0 };

    double u_a[4] = { 0,(1.0 - u) * (1.0 - u) ,1.0 * (1 - u) * u , u * u };
    double v_a[4] = { 0,(1.0 - v) * (1.0 - v) , 1.0 * (1 - v) * v ,v * v };

    a[0] = u_a[1] * Px[0] + u_a[2] * Px[3] + u_a[3] * Px[6];
    a[1] = u_a[1] * Py[0] + u_a[2] * Py[3] + u_a[3] * Py[6];
    a[2] = u_a[1] * Pz[0] + u_a[2] * Pz[3] + u_a[3] * Pz[6];
    b[0] = u_a[1] * Px[1] + u_a[2] * Px[4] + u_a[3] * Px[7];
    b[1] = u_a[1] * Py[1] + u_a[2] * Py[4] + u_a[3] * Py[7];
    b[2] = u_a[1] * Pz[1] + u_a[2] * Pz[4] + u_a[3] * Pz[7];
    c[0] = u_a[1] * Px[2] + u_a[2] * Px[5] + u_a[3] * Px[8];
    c[1] = u_a[1] * Py[2] + u_a[2] * Py[5] + u_a[3] * Py[8];
    c[2] = u_a[1] * Pz[2] + u_a[2] * Pz[5] + u_a[3] * Pz[8];

    x = v_a[1] * a[0] + v_a[2] * b[0] + v_a[3] * c[0];
    y = v_a[1] * a[1] + v_a[2] * b[1] + v_a[3] * c[1];
    z = v_a[1] * a[2] + v_a[2] * b[2] + v_a[3] * c[2];

}

void  surface_generation()
{

    int n = 30;
    double step = 1.0 / n;

    // sampling points
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            double  u = i * step;
            double  v = j * step;

            double x, y, z;

            surfacepoint(u, v, x, y, z);
            vertices[i * (n + 1) + j][0] = x;
            vertices[i * (n + 1) + j][1] = y;
            vertices[i * (n + 1) + j][2] = z;

        }
    }

    NUMV = (n + 1) * (n + 1);
    NUMT = n * n * 2;

    int inn = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // two triangles

            indices[inn][0] = i * (n + 1) + j;
            indices[inn][1] = (i + 1) * (n + 1) + j;
            indices[inn][2] = i * (n + 1) + j + 1;


            inn++;

            indices[inn][0] = i * (n + 1) + j + 1;
            indices[inn][1] = (i + 1) * (n + 1) + j;
            indices[inn][2] = (i + 1) * (n + 1) + j + 1;

            inn++;


        }
    }


}


//void light()
//{
//	glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
//	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
//	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular0);
//	glLightfv( GL_LIGHT0, GL_POSITION, light_position0);
//}

//void materialSkin() {
//	glMaterialfv( GL_FRONT, GL_AMBIENT, ambient);
//	glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse);
//	glMaterialfv( GL_FRONT, GL_SPECULAR, specular);
//	glMaterialf( GL_FRONT, GL_SHININESS, shininess);
//}

void Normalize(float* const p)
{
    float mag = sqrtf(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
    if (mag > .0f) {
        p[0] /= mag;
        p[1] /= mag;
        p[2] /= mag;
    }
}

//To compute the normal of a surface from three points.
//   normal = (p1-p0) x (p2-p0) pointing outward in RH coordinate system.
void TriangleNormal(const GLfloat* const p0, const GLfloat* const p1,
                    const GLfloat* const p2, GLfloat* const n) {

    double  x3, y3, z3, x4, y4, z4;  //t: (x3, y3, z3),  r: (x4, y4, z4)

    x3 = p1[0] - p0[0];  y3 = p1[1] - p0[1];  z3 = p1[2] - p0[2];  //t=P1-P0
    x4 = p2[0] - p0[0];  y4 = p2[1] - p0[1];  z4 = p2[2] - p0[2];  //r=P2-P0

    // n = t x r.  X is the cross-product. u is perpendicular to the plane
    n[0] = y3 * z4 - y4 * z3;   n[1] = -(x3 * z4 - x4 * z3);   n[2] = x3 * y4 - x4 * y3;

    Normalize(n);
}

void findmeshnormal()
{
    memset(normals, 0, sizeof(float) * NUMV * 3);  //initialize

    for (int i = 0; i < NUMT; i++) {
        float n[3];
        TriangleNormal(vertices[indices[i][0]], vertices[indices[i][1]],
                       vertices[indices[i][2]], n);

        normals[indices[i][0]][0] += n[0];
        normals[indices[i][0]][1] += n[1];
        normals[indices[i][0]][2] += n[2];
        normals[indices[i][1]][0] += n[0];
        normals[indices[i][1]][1] += n[1];
        normals[indices[i][1]][2] += n[2];
        normals[indices[i][2]][0] += n[0];
        normals[indices[i][2]][1] += n[1];
        normals[indices[i][2]][2] += n[2];
    }

    for (int i = 0; i < NUMV; i++) {
        Normalize(normals[i]);
    }
}



void init()
{
    //glClearColor( 0.0, 0.0, 0.0, 0.0);  //Set the clear color to black

    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// used for the transparent view frustum
    //glEnable( GL_NORMALIZE);							// must enable this if there is scaling or shearing transform
    //glShadeModel( GL_SMOOTH );
    //
    //glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    ////shading model

    //glEnable(GL_LIGHTING);
    //light();											// the only light source
    //
    //glEnable( GL_LIGHT0);

    findmeshnormal();



}

void drawlightsphere() {
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(light_position0[0], light_position0[1], light_position0[2]);
    GLUquadricObj* q = gluNewQuadric();
    gluSphere(q, .05f, 8, 8);
    gluDeleteQuadric(q);
    glPopMatrix();
    glPopAttrib();
}



void DrawFace()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < NUMT; i++) {
        glColor3f(colors[indices[i][0]][0], colors[indices[i][0]][1], colors[indices[i][0]][2]);
        glVertex3f(vertices[indices[i][0]][0], vertices[indices[i][0]][1], vertices[indices[i][0]][2]);
        glColor3f(colors[indices[i][1]][0], colors[indices[i][1]][1], colors[indices[i][1]][2]);
        glVertex3f(vertices[indices[i][1]][0], vertices[indices[i][1]][1], vertices[indices[i][1]][2]);
        glColor3f(colors[indices[i][2]][0], colors[indices[i][2]][1], colors[indices[i][2]][2]);
        glVertex3f(vertices[indices[i][2]][0], vertices[indices[i][2]][1], vertices[indices[i][2]][2]);
    }
    glEnd();
}

double cos(GLfloat* vector1, GLfloat* vector2) {
    float a = sqrtf(vector1[0] * vector1[0] + vector1[1] * vector1[1] + vector1[2] * vector1[2]);
    float b = sqrtf(vector2[0] * vector2[0] + vector2[1] * vector2[1] + vector2[2] * vector2[2]);
    float c = sqrtf((vector1[0] - vector2[0]) * (vector1[0] - vector2[0]) + (vector1[1] - vector2[1]) * (vector1[1] - vector2[1]) + (vector1[2] - vector2[2]) * (vector1[2] - vector2[2]));
    float zi = a * a + b * b - c * c;
    return max((zi / (2 * a * b)), 0.0f);
}

double dist(const GLfloat* const X, const GLfloat* const Y)
{
    double ans = sqrtf((X[0] - Y[0]) * (X[0] - Y[0]) + (X[1] - Y[1]) * (X[1] - Y[1]) + (X[2] - Y[2]) * (X[2] - Y[2]));
    return ans;
}


void sub(GLfloat  ans[3], const GLfloat* const X, const GLfloat* const Y)
{
    ans[0] = X[0] - Y[0];
    ans[1] = X[1] - Y[1];
    ans[2] = X[2] - Y[2];
}

GLfloat* sub(const GLfloat* const X, const GLfloat* const Y)
{
    GLfloat ans[3];
    ans[0] = X[0] - Y[0];
    ans[1] = X[1] - Y[1];
    ans[2] = X[2] - Y[2];
    return ans;
}


void phong()
{
    //每一个顶点计算一个颜色
    GLfloat eye[3] = { xcam, ycam, zcam };
    for (int i = 0; i < NUMV; i++) {
        float d = dist(vertices[i], light_position0);
        GLfloat L1[3], R2[3], V2[3];
        sub(L1, light_position0, vertices[i]);
        GLfloat L[3] = { (*L1) / d, *(L1 + 1) / d, *(L1 + 2) / d };
        float cos1 = cos(L, normals[i]);


        GLfloat R1[3] = { 2 * cos1 * normals[i][0], 2 * cos1 * normals[i][1], 2 * cos1 * normals[i][2] };
        sub(R2, R1, L);
        GLfloat R[3] = { *R2, *(R2 + 1), *(R2 + 2) };
        sub(V2, eye, vertices[i]);
        GLfloat V[3] = { *V2, *(V2 + 1), *(V2 + 2) };
        double cos2 = cos(V, R);

        colors[i][0] = light_ambient[0] * ambient[0] + (light_diffuse0[0] * diffuse[0] * cos1 + light_specular0[0] * specular[0] * pow(cos2, shininess));
        colors[i][1] = light_ambient[1] * ambient[1] + (light_diffuse0[1] * diffuse[1] * cos1 + light_specular0[1] * specular[1] * pow(cos2, shininess));
        colors[i][2] = light_ambient[2] * ambient[2] + (light_diffuse0[2] * diffuse[2] * cos1 + light_specular0[2] * specular[2] * pow(cos2, shininess));
    }
}



void draw()
{
    //light();

    glDisable(GL_BLEND);		// disable blending used in side views
    glEnable(GL_DEPTH_TEST);	// enable depth test
    glEnable(GL_LIGHTING);		// enable lighting
    glEnable(GL_LIGHT0);		// turn light 0 on

    //materialSkin();		// set material

    phong();

    DrawFace();

    drawlightsphere();

}



void  hkgluPlatform(void) {
    double Extent = 9.;
    int width, h = winHeight / 2;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    width = winWidth;
    glViewport(0, 0, width, winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (double)width / winHeight, Znear, Zfar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xcam, ycam, zcam, xfoc, yfoc, zfoc, 0., 1., 0.);

    draw();



    glutSwapBuffers();

}



#define SQUARE(x) (x*x)
int initial_mouse_x, initial_mouse_y;
double initial_x, initial_y, initial_z;
bool is_leftbutton_pressed = false;
const double ROTATION_RATIO = 3.1416 / 1600.; // 90 deg per 800 pixel displacement

//A call back that perform some viewing function with mouse

void mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON) {
        //if (!is_rightbutton_pressed) {
        if (state == GLUT_DOWN) {
            initial_mouse_x = x;
            initial_mouse_y = y;
            initial_x = xcam;
            initial_y = ycam;
            initial_z = zcam;
            is_leftbutton_pressed = true;
        }
        else { // assume state == GLUT_UP
            is_leftbutton_pressed = false;
        }
        //}
    }
}


//A call back that change the position of camera with mouse
void mouse_motion(int mouse_x, int mouse_y) {

    if (is_leftbutton_pressed) {
        int x_displacement = initial_mouse_x - mouse_x; // -(mouse_x - initial_mouse_x);
        int y_displacement = mouse_y - initial_mouse_y; // mouse_y - initial_mouse_y;

        double center_x, center_y, center_z;			// coordinate of origin of rotation
        double point_x, point_y, point_z;				// final coordinate after rotation

        center_x = xfoc;
        center_y = yfoc;
        center_z = zfoc;

        /*  perform rotation about y axis  */

        double theta = static_cast<double>(x_displacement) * ROTATION_RATIO;  // rotation angle about y
        double sin_theta = sin(theta);
        double cos_theta = cos(theta);
        double init_rel_x = initial_x - center_x;		// initial relative point_x when center is translated to origin
        double init_rel_z = initial_z - center_z;		// initial relative point_z when center is translated to origin

        double relative_x = init_rel_x * cos_theta + init_rel_z * sin_theta;
        double relative_z = init_rel_z * cos_theta - init_rel_x * sin_theta;

        /*  perform rotation about a horizontal line  */

        double relative_y = initial_y - center_y;

        double alpha = static_cast<double>(y_displacement) * ROTATION_RATIO;  // rotation angle about a line on x-z plane
        double sin_alpha = sin(alpha);
        double cos_alpha = cos(alpha);

        double hyp = sqrt(SQUARE(relative_x) + SQUARE(relative_z));
        double cos_gamma = relative_x / hyp;
        double sin_gamma = relative_z / hyp;
        double cos_gamma_square = SQUARE(cos_gamma);
        double sin_gamma_square = SQUARE(sin_gamma);
        double sin_alpha_times_cos_gamma = sin_alpha * cos_gamma;
        double sin_alpha_times_sin_gamma = sin_alpha * sin_gamma;
        double camotsgcg = (cos_alpha - 1.) * sin_gamma * cos_gamma; // Cos_Alpha_Minus_One_Times_Sin_Gamma_Cos_Gamma

        point_x = center_x + (relative_x * (cos_alpha * cos_gamma_square + sin_gamma_square) + relative_y * (-sin_alpha_times_cos_gamma) + relative_z * (camotsgcg));
        point_y = center_y + (relative_x * (sin_alpha_times_cos_gamma)+relative_y * (cos_alpha)+relative_z * (sin_alpha_times_sin_gamma));
        point_z = center_z + (relative_x * (camotsgcg)+relative_y * (-sin_alpha_times_sin_gamma) + relative_z * (cos_alpha * sin_gamma_square + cos_gamma_square));

        xcam = point_x;
        ycam = point_y;
        zcam = point_z;
    }

    glutPostRedisplay();
}

void menuf(int i) {
}


void reShape(int w, int h)
{

    winWidth = w;
    winHeight = h;
    if (showCamera && (w < h)) winHeight = w;

    glutPostRedisplay();
}


//****** The following are needed for starting hkgluPlatform1   ***/
void hkgluPlatformInit() {

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowSize(winWidth, winHeight);

    glutCreateWindow("Mesh Shading");

    glutReshapeFunc(reShape);

    glutDisplayFunc(hkgluPlatform);

    glutMouseFunc(mouse);
    glutMotionFunc(mouse_motion);



    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


int main(int argc, _TCHAR* argv[])
{
    glutInit(&argc, argv);

    hkgluPlatformInit();

    surface_generation();

    init();

    glutMainLoop();

    return 0;
}


