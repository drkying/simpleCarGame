#include "gl/glut.h"   // - An interface and windows management library

#define _USE_MATH_DEFINES

#include <math.h>

//赛车相对与摄像头视角中心方向的旋转角度
float angle = 0.0;
float realAngle = 0;

//初始摄像头位置
float xcam = -5.0f, ycam = 3.0f, zcam = 25.0f;

//初始摄像头视角中心
float xfoc = 0, yfoc = 3, zfoc = 20;

/*
 * 设置相机观察模式
 * 0.俯瞰模式，从上到下观察赛车运动
 * 1.追随模式，从赛车身后观察赛车的当前位置和操作
 * 2.第三人称模式，摄像头可自由移动，视角中心始终对准赛车
 */
int cam = 1;

//赛车的坐标，x、z为赛车在地面上的坐标，y为高度
float carx = 0.0;
float carz = 20.0;

//赛车的速度
int speed = 0;

GLUquadric *quobj = gluNewQuadric();

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


//设置光照
void SetupLights() {
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};//环境光
    GLfloat diffuseLight[] = {0.9f, 0.9f, 0.9f, 1.0f};//漫反射
    GLfloat specularLight[] = {0.9f, 0.9f, 0.9f, 1.0f};//镜面光
    GLfloat lightPos[] = {20.0f, 20.0f, 80.0f, 1.0f};//光源位置

    glEnable(GL_LIGHTING); //启用光照
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);//设置环境光源
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);//设置漫反射光源
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);//设置镜面光源
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);//设置灯光位置
    glEnable(GL_LIGHT0); //打开灯光

    glEnable(GL_COLOR_MATERIAL); //启用材质的颜色跟踪
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); //指定材料着色的面
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight); //指定材料对镜面光的反应
    glMateriali(GL_FRONT, GL_SHININESS, 100); //指定反射系数
}

//绘制赛车

void drawCar() {
    glTranslatef(0, 1, 0);

    // 绘制轮胎
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            glColor3f(0.0f, 0.0f, 0.0f); // 轮胎颜色
            glPushMatrix();
            glTranslatef(i * 3.0, 0, j * 3.0);
            glutSolidTorus(0.2, 0.4, 30, 30); //圆环体绘制轮子
            glColor3f(1.0f, 1.0f, 1.0f);
            gluDisk(quobj, 0, 0.2, 30, 30); //碟盘、同心圆
            glPopMatrix();
        }
        glPushMatrix();
        glTranslatef(i * 3.0, 0, 0);
        gluCylinder(quobj, 0.2, 0.2, 3, 30, 30); // 圆柱，绘制车轮轴
        glPopMatrix();
    }

    //绘制车身
    glTranslatef(0, 0.4, 0.4);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            glColor3f(0.9f, 0.9f, 0.9f); //white
            glPushMatrix();
            glTranslatef(i * 3.0, 0, j * 2.2);
            glutSolidTorus(0.3, 0.2, 30, 30);//车身外侧
            glPopMatrix();
        }
    }
    glPushMatrix();
    glColor3f(1, 0, 0); //车身颜色 blue
    glTranslatef(1.5, 0.15, 1.1);
    glScalef(1, 0.35, 0.7);
    glutSolidSphere(2.35, 30, 30); //车身内侧
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(110, 200, 240, 100);
    glTranslatef(1.5, 0.9, 1.1);
    glScalef(1, 0.3, 1);
    glutSolidSphere(1, 30, 30); // 车顶
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(3.6, 0.1, 1.1);
    glScalef(0.3, 0.15, 1);
    glRotatef(-30, 0, 0, 1);
    glutSolidCube(2.35); // 保险杠 绘制实心立方体
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(-0.7, 0.4, 1.1);
    glScalef(0.3, 0.15, 1);
    glRotatef(-30, 0, 0, 1);
    glutSolidCube(2.35); // 保险杠 绘制实心立方体
    glPopMatrix();
}

void drawCar1() {
    glTranslatef(0, 2, 0);
    float z = 1.5;


    glColor3f(206 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, 0.25, -z);
    glVertex3f(-3.0, 0.25, z);
    glVertex3f(-3.0, -1.0, z);
    glVertex3f(-3.0, -1.0, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, -z);
    glVertex3f(-3.0, 1.5, z);
    glVertex3f(-3.0, 1.0, z);
    glVertex3f(-3.0, 1.0, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 0.25, -z);
    glVertex3f(-3.0, 0.25, -z + 0.5);
    glVertex3f(-3.0, 1.0, -z + 0.5);
    glVertex3f(-3.0, 1.0, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 0.25, z - 0.5);
    glVertex3f(-3.0, 0.25, z);
    glVertex3f(-3.0, 1.0, z);
    glVertex3f(-3.0, 1.0, z - 0.5);
    glEnd();


    glColor3f(240 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, -z);
    glVertex3f(-3.0, 1.5, z);
    glVertex3f(0.6, 1.5, z);
    glVertex3f(0.6, 1.5, -z);
    glEnd();


    glColor3f(190 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, -1.0, -z);
    glVertex3f(-3.0, -1.0, z);
    glVertex3f(3.0, -1.0, z);
    glVertex3f(3.0, -1.0, -z);
    glEnd();


    glColor3f(206 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(3.0, -1.0, -z);
    glVertex3f(3.0, 0.15, -z);
    glVertex3f(3.0, 0.15, z);
    glVertex3f(3.0, -1.0, z);


    glColor3f(0.9, 0.9, 0.9);
    glVertex3f(3.006, -0.65, -z + 0.101);
    glVertex3f(3.006, -0.35, -z + 0.101);
    glVertex3f(3.006, -0.35, -z + 0.601);
    glVertex3f(3.006, -0.65, -z + 0.601);

    glVertex3f(3.006, -0.65, z - 0.101);
    glVertex3f(3.006, -0.35, z - 0.101);
    glVertex3f(3.006, -0.35, z - 0.601);
    glVertex3f(3.006, -0.65, z - 0.601);

    glColor3f(0, 0, 0);
    glVertex3f(3.006, -0.6, -z + 1);
    glVertex3f(3.006, -0.37, -z + 1);
    glVertex3f(3.006, -0.37, z - 1);
    glVertex3f(3.006, -0.6, z - 1);

    glColor3f(0.6, 0.6, 0.6);
    glVertex3f(3.005, -0.7, -z);
    glVertex3f(3.005, -0.3, -z);
    glVertex3f(3.005, -0.3, z);
    glVertex3f(3.005, -0.7, z);

    glVertex3f(2.9, -0.3, -z - 0.0014);
    glVertex3f(3.0, -0.3, -z - 0.0014);
    glVertex3f(3.0, -0.7, -z - 0.0014);
    glVertex3f(2.9, -0.7, -z - 0.0014);

    glVertex3f(2.9, -0.3, z + 0.0014);
    glVertex3f(3.0, -0.3, z + 0.0014);
    glVertex3f(3.0, -0.7, z + 0.0014);
    glVertex3f(2.9, -0.7, z + 0.0014);

    glColor3f(226 / 255.0f, 152 / 255.0f, 22 / 255.0f);
    glVertex3f(2.95, -0.35, z + 0.0015);
    glVertex3f(2.985, -0.35, z + 0.0015);
    glVertex3f(2.985, -0.65, z + 0.0015);
    glVertex3f(2.95, -0.65, z + 0.0015);

    glVertex3f(2.95, -0.35, -z - 0.0015);
    glVertex3f(2.985, -0.35, -z - 0.0015);
    glVertex3f(2.985, -0.65, -z - 0.0015);
    glVertex3f(2.95, -0.65, -z - 0.0015);
    glEnd();

    glColor3f(230 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(3.0, 0.15, -z);
    glVertex3f(1.2, 0.25, -z);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(3.0, 0.15, z);
    glEnd();

    glColor3f(235 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(0.6, 1.5, -z);
    glVertex3f(0.6, 1.5, z);
    glVertex3f(0.65, 1.42, z);
    glVertex3f(0.65, 1.42, -z);

    glVertex3f(1.15, 0.34, -z);
    glVertex3f(1.15, 0.34, -z + 0.1);
    glVertex3f(0.65, 1.42, -z + 0.1);
    glVertex3f(0.65, 1.42, -z);

    glVertex3f(1.15, 0.34, z);
    glVertex3f(1.15, 0.34, z - 0.1);
    glVertex3f(0.65, 1.42, z - 0.1);
    glVertex3f(0.65, 1.42, z);

    glVertex3f(1.15, 0.34, -z);
    glVertex3f(1.15, 0.34, z);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(1.2, 0.25, -z);
    glEnd();


    glColor3f(206 / 255.0f, 20 / 255.0f, 55 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, -z);
    glVertex3f(0.6, 1.5, -z);
    glVertex3f(0.696, 1.3, -z);
    glVertex3f(-3.0, 1.3, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.3, -z);
    glVertex3f(-3.0, 0.25, -z);
    glVertex3f(-2.5, 0.25, -z);
    glVertex3f(-2.5, 1.3, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-1.2, 1.3, -z);
    glVertex3f(-1.2, 0.25, -z);
    glVertex3f(-1.0, 0.25, -z);
    glVertex3f(-1.0, 1.3, -z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(1.2, 0.25, -z);
    glVertex3f(0.696, 1.3, -z);
    glVertex3f(0.496, 1.3, -z);
    glVertex3f(1.0, 0.25, -z);
    glEnd();


    glBegin(GL_POLYGON);
    glVertex3f(1.2, 0.25, -z);
    glVertex3f(3.0, 0.15, -z);
    glVertex3f(3.0, -1.0, -z);
    glVertex3f(1.2, -1.0, -z);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(1.2, 0.25, -z);
    glVertex3f(1.2, -1.0, -z);
    glVertex3f(-3.0, -1.0, -z);
    glVertex3f(-3.0, 0.25, -z);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(1.2, -1.0, z);
    glVertex3f(-3.0, -1.0, z);
    glVertex3f(-3.0, 0.25, z);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(3.0, 0.15, z);
    glVertex3f(3.0, -1.0, z);
    glVertex3f(1.2, -1.0, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, z);
    glVertex3f(0.6, 1.5, z);
    glVertex3f(0.696, 1.3, z);
    glVertex3f(-3.0, 1.3, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.3, z);
    glVertex3f(-3.0, 0.25, z);
    glVertex3f(-2.5, 0.25, z);
    glVertex3f(-2.5, 1.3, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-1.2, 1.3, z);
    glVertex3f(-1.2, 0.25, z);
    glVertex3f(-1.0, 0.25, z);
    glVertex3f(-1.0, 1.3, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(0.696, 1.3, z);
    glVertex3f(0.496, 1.3, z);
    glVertex3f(1.0, 0.25, z);
    glEnd();

    glColor3f(165 / 255.0f, 8 / 255.0f, 37 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, -0.3, -z - 0.0013);
    glVertex3f(3.0, -0.3, -z - 0.0013);
    glVertex3f(3.0, -0.7, -z - 0.0013);
    glVertex3f(-3.0, -0.7, -z - 0.0013);

    glVertex3f(-3.0, -0.3, z + 0.0013);
    glVertex3f(3.0, -0.3, z + 0.0013);
    glVertex3f(3.0, -0.7, z + 0.0013);
    glVertex3f(-3.0, -0.7, z + 0.0013);
    glEnd();


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(190 / 255.0f, 190 / 255.0f, 190 / 255.0f, 0.6);
    glBegin(GL_QUADS);
    glVertex3f(0.85, 0.5, -z - 0.4);
    glVertex3f(0.85, 0.5, -z);
    glVertex3f(0.85, 0.25, -z);
    glVertex3f(0.85, 0.25, -z - 0.4);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(180 / 255.0f, 30 / 255.0f, 30 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(0.85, 0.5, -z - 0.4);
    glVertex3f(1, 0.5, -z - 0.4);
    glVertex3f(1, 0.25, -z - 0.4);
    glVertex3f(0.85, 0.25, -z - 0.4);

    glVertex3f(0.85, 0.5, -z - 0.4);
    glVertex3f(0.85, 0.5, -z);
    glVertex3f(1, 0.5, -z);
    glVertex3f(1, 0.5, -z - 0.4);

    glVertex3f(1, 0.5, -z - 0.4);
    glVertex3f(1, 0.5, -z);
    glVertex3f(1, 0.25, -z);
    glVertex3f(1, 0.25, -z - 0.4);

    glVertex3f(0.85, 0.5, -z);
    glVertex3f(1, 0.5, -z);
    glVertex3f(1, 0.25, -z);
    glVertex3f(0.85, 0.25, -z);

    glVertex3f(0.85, 0.25, -z - 0.4);
    glVertex3f(0.85, 0.25, -z);
    glVertex3f(1, 0.25, -z);
    glVertex3f(1, 0.25, -z - 0.4);

    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(190 / 255.0f, 190 / 255.0f, 190 / 255.0f, 0.6);
    glBegin(GL_QUADS);
    glVertex3f(0.85, 0.5, z + 0.4);
    glVertex3f(0.85, 0.5, z);
    glVertex3f(0.85, 0.25, z);
    glVertex3f(0.85, 0.25, z + 0.4);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(180 / 255.0f, 30 / 255.0f, 30 / 255.0f);
    glBegin(GL_QUADS);
    glVertex3f(0.85, 0.5, z + 0.4);
    glVertex3f(1, 0.5, z + 0.4);
    glVertex3f(1, 0.25, z + 0.4);
    glVertex3f(0.85, 0.25, z + 0.4);

    glVertex3f(1, 0.5, z + 0.4);
    glVertex3f(1, 0.5, z);
    glVertex3f(1, 0.25, z);
    glVertex3f(1, 0.25, z + 0.4);

    glVertex3f(0.85, 0.5, z + 0.4);
    glVertex3f(0.85, 0.5, z);
    glVertex3f(1, 0.5, z);
    glVertex3f(1, 0.5, z + 0.4);

    glVertex3f(0.85, 0.5, z);
    glVertex3f(1, 0.5, z);
    glVertex3f(1, 0.25, z);
    glVertex3f(0.85, 0.25, z);

    glVertex3f(0.85, 0.25, z + 0.4);
    glVertex3f(0.85, 0.25, z);
    glVertex3f(1, 0.25, z);
    glVertex3f(1, 0.25, z + 0.4);
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(90 / 255.0f, 90 / 255.0f, 90 / 255.0f, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, -z + 0.01);
    glVertex3f(0.5, 1.5, -z + 0.01);
    glVertex3f(1.2, 0.25, -z + 0.01);
    glVertex3f(-3.0, 0.25, -z + 0.01);
    glEnd();
    glDisable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(90 / 255.0f, 90 / 255.0f, 90 / 255.0f, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(-3.0, 1.5, z - 0.01);
    glVertex3f(0.5, 1.5, z - 0.01);
    glVertex3f(1.2, 0.25, z - 0.01);
    glVertex3f(-3.0, 0.25, z - 0.01);
    glEnd();
    glDisable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(90 / 255.0f, 90 / 255.0f, 90 / 255.0f, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(0.5, 1.5, -z);
    glVertex3f(0.5, 1.5, z);
    glVertex3f(1.2, 0.25, z);
    glVertex3f(1.2, 0.25, -z);
    glEnd();
    glDisable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(90 / 255.0f, 90 / 255.0f, 90 / 255.0f, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(-2.99, 0.25, -z + 0.5);
    glVertex3f(-2.99, 0.25, z - 0.5);
    glVertex3f(-2.99, 1.0, z - 0.5);
    glVertex3f(-2.99, 1.0, -z + 0.5);
    glEnd();
    glDisable(GL_BLEND);


    glColor3f(0.0, 0.0, 0.0);
    gluQuadricNormals(quobj, GLU_SMOOTH);
    gluQuadricTexture(quobj, GL_TRUE);
    glTranslatef(1.7, -1.0, -1.7);
    gluCylinder(quobj, 0.6, 0.6, 0.2, 15, 15);
    gluDisk(quobj, 0, 0.6, 15, 15);
    glTranslatef(0.0, 0.0, 0.2);
    gluDisk(quobj, 0, 0.6, 15, 15);

    glTranslatef(0.0, 0.0, -0.2);
    glTranslatef(-3.3, 0.0, 0.0);
    gluCylinder(quobj, 0.6, 0.6, 0.2, 15, 15);
    gluDisk(quobj, 0, 0.6, 15, 15);
    glTranslatef(0.0, 0.0, 0.2);
    gluDisk(quobj, 0, 0.6, 15, 15);

    glTranslatef(0.0, 0.0, -0.2);
    glTranslatef(0.0, 0.0, 3.2);
    gluCylinder(quobj, 0.6, 0.6, 0.2, 15, 15);
    gluDisk(quobj, 0, 0.6, 15, 15);
    glTranslatef(0.0, 0.0, 0.2);
    gluDisk(quobj, 0, 0.6, 15, 15);

    glTranslatef(0.0, 0.0, -0.2);
    glTranslatef(3.3, 0.0, 0.0);
    gluCylinder(quobj, 0.6, 0.6, 0.2, 15, 15);
    gluDisk(quobj, 0, 0.6, 15, 15);
    glTranslatef(0.0, 0.0, 0.2);
    gluDisk(quobj, 0, 0.6, 15, 15);

    glColor3f(1.0, 1.0, 1.0);
    gluDisk(quobj, 0.2, 0.4, 15, 15);
    glTranslatef(-3.3, 0.0, 0.0);
    gluDisk(quobj, 0.2, 0.4, 15, 15);
    glTranslatef(0.0, 0.0, -0.2);
    glTranslatef(0.0, 0.0, -3.2);
    gluDisk(quobj, 0.2, 0.4, 15, 15);
    glTranslatef(+3.3, 0.0, 0.0);
    gluDisk(quobj, 0.2, 0.4, 15, 15);
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

//环境背景的绘制
void renderScene(void) {
    //设置环境颜色
    glClearColor(0.1, 0.4, 0.7, 0.7); //默认颜色（天空的颜色）
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 恢复初始坐标系，即清空glRotatef、glScalef、glTranslatef等的影响
    glLoadIdentity();

    //设置摄像头位置
    gluLookAt(xcam, ycam, zcam,
              xfoc, yfoc, zfoc,
              0.0f, 1.0f, 0.0f);


    // 绘制地面
    glColor3f(0.3f, 0.3f, 0.3f);//地面颜色
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();

    glColor3f(0.52, 0.8, 0.92); //外墙颜色 淡蓝色
    glPushMatrix();
    glScalef(2, 10, 1);
    glRotatef(45, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(1, 50, 30, 4); // 绘制外墙
    glPopMatrix();


    glColor3f(0., 1., 0.); //内墙颜色
    glPushMatrix();
    glScalef(3, 8, 1.5);
    glRotatef(45, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(1, 12, 30, 4); // 绘制内墙
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

    //追随模式下移动摄像头位置
    if (cam == 1) {
        xcam = 7 * -cos(realAngle) + carx;
        zcam = 7 * sin(realAngle) + carz;
        xfoc = carx;
        zfoc = carz;
    }

    //根据车辆坐标绘制赛车
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
        } else if (cam == 0) {
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
    else if (key == 'v') {
        if (cam != 2)
            cam = 2;
        else
            cam = 1;
    }
    if (cam == 2) {
        if (key == 'a') {
            zcam -= 1.0f;
        } else if (key == 'd') {
            zcam += 1.0f;
        } else if (key == 's') {
            xcam -= 1.0f;
        } else if (key == 'w') {
            xcam += 1.0f;
        }
    }
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
            if (speed > -5)
                speed -= 1;
            break;
    }
}


int main(int argc, char **argv) {

    //初始化glut，创建窗口
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);  //使用双缓冲模式
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1800, 900);
    glutCreateWindow("赛车游戏");


    gluQuadricNormals(quobj, GLU_SMOOTH);

    //注册回调函数
    glutDisplayFunc(renderScene); //设置每次显示调用的函数
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);  //注册一个空闲时绘制操作函数
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    // 初始化OpenGL
    glEnable(GL_DEPTH_TEST);
    SetupLights(); //设置光源
    //进入GLUT事件处理周期
    glutMainLoop();

    return 1;
}
