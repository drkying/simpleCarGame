#include "gl/glut.h"   // - An interface and windows management library

#define _USE_MATH_DEFINES

#include <math.h>

// angle of rotation for the camera direction
float angle = 0.0;
float realAngle = 0;
// XYZ position of the camera
//摄像头位置
float xcam = -5.0f, ycam = 3.0f, zcam = 25.0f;

// XYZ position of the camera center
//摄像头视角
float xfoc = 0, yfoc = 3, zfoc = 20;

//是否俯瞰
int cam = 1;

float carx = 0.0;
float carz = 20.0;

int frame = 0;
int speed = 0;

//碰撞检测
bool willCollide(float x, float z) {

    float boxX = carx + x;
    float boxZ = carz + z;

    boxX += cos(realAngle) * 4.2;
    boxZ += -sin(realAngle) * 4.2;

    if (((boxX > -28 && boxX < 28) && (boxZ < 14 && boxZ > -14)) ||
        ((boxX > 69 || boxX < -69) || (boxZ < -35 || boxZ > 35)))
        return true;
    return false;

}





GLUquadric *quobj = gluNewQuadric();

//设置光照
void SetupLights()
{
    GLfloat ambientLight[]={0.2f,0.2f,0.2f,1.0f};//环境光
    GLfloat diffuseLight[]={0.9f,0.9f,0.9f,1.0f};//漫反射
    GLfloat specularLight[]={0.9f,0.9f,0.9f,1.0f};//镜面光
    GLfloat lightPos[]={20.0f,20.0f,80.0f,1.0f};//光源位置

    glEnable(GL_LIGHTING); //启用光照
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);//设置环境光源
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);//设置漫反射光源
    glLightfv(GL_LIGHT0,GL_SPECULAR,specularLight);//设置镜面光源
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);//设置灯光位置
    glEnable(GL_LIGHT0); //打开灯光

    glEnable(GL_COLOR_MATERIAL); //启用材质的颜色跟踪
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE); //指定材料着色的面
    glMaterialfv(GL_FRONT,GL_SPECULAR,specularLight); //指定材料对镜面光的反应
    glMateriali(GL_FRONT,GL_SHININESS,100); //指定反射系数
}
//绘制赛车
void drawCar() {
    glPushMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glTranslatef(0.25f, 2.0f, 0.2f);
    //DrawCube(1.0f, 3.0f, 0.5f);
    glPopMatrix();

    glTranslatef(0, 1, 0);

    // Draw Wheels
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            glColor3f(0.0f, 0.0f, 0.0f); // black
            glPushMatrix();
            glTranslatef(i * 3.0, 0, j * 3.0);
            glutSolidTorus(0.2, 0.4, 30, 30); // wheel
            glColor3f(1.0f, 1.0f, 1.0f);
            gluDisk(quobj, 0, 0.2, 30, 30); // kapaki
            glPopMatrix();
        }
        glPushMatrix();
        glTranslatef(i * 3.0, 0, 0);
        gluCylinder(quobj, 0.2, 0.2, 3, 30, 30); // kylindros
        glPopMatrix();
    }

    glTranslatef(0, 0.4, 0.4);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            glColor3f(0.9f, 0.9f, 0.9f); // white
            glPushMatrix();
            glTranslatef(i * 3.0, 0, j * 2.2);
            glutSolidTorus(0.3, 0.2, 30, 30); // laspotiras
            glPopMatrix();
        }
    }

    glPushMatrix();
    glColor3f(0, 0, 1); //车身颜色 blue
    glTranslatef(1.5, 0.15, 1.1);
    glScalef(1, 0.35, 0.7);
    glutSolidSphere(2.35, 30, 30); // body
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(110, 200, 240, 100); // blue
    glTranslatef(1.5, 0.9, 1.1);
    glScalef(1, 0.3, 1);
    glutSolidSphere(1, 30, 30); // glass
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 1); // white
    glTranslatef(3.6, 0.1, 1.1);
    glScalef(0.3, 0.15, 1);
    glRotatef(-30, 0, 0, 1);
    glutSolidCube(2.35); // bumper
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 1); // white
    glTranslatef(-0.7, 0.4, 1.1);
    glScalef(0.3, 0.15, 1);
    glRotatef(-30, 0, 0, 1);
    glutSolidCube(2.35); // bumper
    glPopMatrix();
}

//修改窗口大小
void changeSize(int w, int h) {


    if (h == 0) //防止窗口太小
        h = 1;
    float ratio = w * 1.0 / h;

// Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

// Reset Matrix
    glLoadIdentity();

// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

// Set the correct perspective.
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);

// Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

    // Clear Color and Depth Buffers

    //设置环境颜色
    glClearColor(0.1, 0.4, 0.7, 0.7); //默认颜色（天空的颜色）
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Set the camera
    gluLookAt(xcam, ycam, zcam, //设置摄像头位置
              xfoc, yfoc, zfoc,
              0.0f, 1.0f, 0.0f);


    // Draw ground
    glColor3f(0.3f, 0.3f, 0.3f);//地面颜色
    //glColor3f(0.f, 0.f, 0.f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();

    glColor3f(0.52, 0.8, 0.92); //外墙颜色 淡蓝色
    glPushMatrix();
    //glScalef(1.6,10,0.8);
    glScalef(2, 10, 1);
    glRotatef(45, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    //aaglutSolidTorus(1,50,30,12); // outer wall
    glutSolidTorus(1, 50, 30, 4); // outer wall
    glPopMatrix();


    glColor3f(0., 1., 0.); //内墙颜色 white
    glPushMatrix();
    //glScalef(1.7,8,0.8);
    glScalef(3, 8, 1.5);
    glRotatef(45, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    //glutSolidTorus(1,12,30,18); // inner wall
    glutSolidTorus(1, 12, 30, 4); // inner wall
    glPopMatrix();

    //判断是否碰撞
    if (willCollide(cos(realAngle) * speed * 0.01, -sin(realAngle) * speed * 0.01)) {
        speed = 0;
        angle += 90;
        if (angle >= 360) angle -= 360;
        realAngle = angle * M_PI / 180;

    } else {
        carx += cos(realAngle) * speed * 0.01;
        carz += -sin(realAngle) * speed * 0.01;
    }

    //frame++;
    //printf("frame = %d\ncam = %d\neye x = %f\neye z = %f\ncar x = %f\ncar z = %f\nspeed = %d\nangle = %f\nrealAngle = %f\nsin(angle) = %f\ncos(angle) = %f\n\n",
      //     frame, cam, xcam, zcam, carx, carz, speed, angle, realAngle, sin(realAngle), cos(realAngle));


    if (cam == 1) { //移动摄像头位置
        xcam = 7 * -cos(realAngle) + carx;
        zcam = 7 * sin(realAngle) + carz;
        xfoc = carx;
        zfoc = carz;
    }

    glTranslatef(carx, 0, carz);

    glRotatef(angle, 0, 1, 0);
    drawCar();

    glutSwapBuffers();
}

//键盘检测和功能实现
void processNormalKeys(unsigned char key, int x, int y) {

    if (key == 'k') {
        if (cam == 1) {
            xcam = 0.0f;
            ycam = 99.0f;
            zcam = 0.8f;
            xfoc = 0;
            yfoc = 0.0f;
            zfoc = 0;
            cam = 0;
        } else {
            xcam = carx - 7;
            ycam = 3.0f;
            zcam = carz + 25;
            xfoc = carx;
            yfoc = 3.0f;
            zfoc = carz + 25;
            cam = 1;
        }
    } else if (key == '+') {
        ycam -= 1.0f;
    } else if (key == '-') {
        ycam += 1.0f;
    } else if (key == 32)
        speed = 0;
    else if (key == 27)
        exit(0);
}

//控制赛车前后左右
void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {
        case GLUT_KEY_LEFT :
            if (speed > 0) {
                angle += 1;
                if (angle >= 360) angle -= 360;
                realAngle = angle * M_PI / 180;
            }
            break;
        case GLUT_KEY_RIGHT :
            if (speed > 0) {
                angle -= 1;
                if (angle <= 0) angle += 360;
                realAngle = angle * M_PI / 180;
            }
            break;
        case GLUT_KEY_UP :
            if (speed < 5)
                speed += 1;
            break;
        case GLUT_KEY_DOWN :
            if (speed > 0)
                speed -= 1;
            break;
    }
}


int main(int argc, char **argv) {

    // init GLUT and create window

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1800, 900);
    glutCreateWindow("Car Game");


    gluQuadricNormals(quobj, GLU_SMOOTH);

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    SetupLights();
    // enter GLUT event processing cycle
    glutMainLoop();

    return 1;
}
