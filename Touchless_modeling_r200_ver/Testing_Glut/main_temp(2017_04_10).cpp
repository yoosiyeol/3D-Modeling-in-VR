//����η� ��������� �����մϴ�.
#include ".\include\GL\freeglut.h"
#include "opencv2/opencv.hpp"  
#include <iostream>  
#include <string> 

#include "mOpenCv.h"

//����� ���̺귯���� �������ݴϴ�.
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")

using namespace cv;
using namespace std;

mHand Hand;
mCamera Camera;
mTools Tools;
//------------------------//
double point1[3] = { -0.5,-0.5,-0.5 };
double point2[3] = { -0.5,0.5,-0.5 };
double point3[3] = { 0.5, 0.5, -0.5 };
double point4[3] = { 0.5, -0.5, -0.5 };

double TB[10][3];
double TL[10][3];
double TR[10][3];
double TF[10][3];


Mat Depth;
Mat Blob = cv::Mat::zeros(DepthSize, CV_8UC1);
Mat Contour = cv::Mat::zeros(DepthSize, CV_8UC1);

Point curHandPoint = CvPoint(0, 0);

int distance;

VideoCapture * capture;
Mat img_cam;
int screenW;
int screenH;

GLuint texture_background, texture_cube;
float cubeAngle = 0;
//ť�� ��ġ
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//�׽��� ���ؽ�
float ver[8][3] = {
	{-0.5f,-0.5f,-0.5f},	//�ڿ޾�
	{-0.5f,0.5f,-0.5f},	//�ڿ���
	{0.5f,0.5f,-0.5f},	//�ڿ���
	{0.5f,-0.5f,-0.5f},	//�ڿ���
	{ -0.5f,-0.5f,0.5f },	//�տ޾�
	{ -0.5f,0.5f,0.5f },	//�տ���
	{ 0.5f,0.5f,0.5f },	//�տ���
	{ 0.5f,-0.5f,0.5f }	//�տ���
};

float Tver[6][11][11][3];// ������ü�� ��, ���� X ���� Y �鳻�� (X,Y,Z)

/*
float ver0To1[9][3];
float ver1To2[9][3];
float ver2To3[9][3];
float ver3To0[9][3];
*/
void initVertext()
{
	//�� -> �� , �� -> �� , �� -> ��
	//preset vertext
	//"(x2-x1)������+(y2-y1)������+(z2-z1)�� ������ ��Ʈ ��	double tempdist = 0;
	/*TB[0][0] = -0.5f;
	TB[0][1] = 0.5f;
	TB[0][2] = -0.5f;

	TB[9][0] = 0.5f;
	TB[9][1] = 0.5f;
	TB[9][2] = -0.5f;
	for (int i = 1; i < 9; i++)
	{

	TB[i][0] = 1.0f/9 + TB[i-1][0];
	TB[i][1] = 0.5f;
	TB[i][2] = 0.5f;
	}

	TR[0][0] = 0.5f;
	TR[0][1]0.
	TR[0][2]*/
/*
	//0 TO 1
	double tempdist = 0.1f;
	for (int i = 0; i < 9; i++)
	{


		ver0To1[i][0] = ver[0][0];
		ver0To1[i][1] = tempdist+ver[0][1];
		ver0To1[i][2] = ver[0][2];
		tempdist += 0.1f;
	}
	tempdist = 0.1f;
	
	//1 TO 2
	tempdist = 0.1f;
	//cout << ver[1][0] << endl;
	for (int i = 0; i < 9; i++)
	{


		ver1To2[i][0] = tempdist + ver[1][0];
		ver1To2[i][1] = ver[1][1];
		ver1To2[i][2] = ver[1][2];
		//cout << ver1To2[i][0] << endl;
		tempdist += 0.1f;
	}
	//cout << ver[2][0] << endl;
	
	//2 TO 3
	tempdist = -0.1f;
	//cout << ver[2][0] << endl;
	for (int i = 0; i < 9; i++)
	{


		ver2To3[i][0] = ver[2][0];
		ver2To3[i][1] = tempdist + ver[2][1];
		ver2To3[i][2] = ver[2][2];
		//cout << ver2To3[i][1] << endl;
		tempdist -= 0.1f;
	}
	//cout << ver[3][1] << endl;

	//3 TO 0
	tempdist = -0.1f;
	for (int i = 0; i < 9; i++)
	{
		ver3To0[i][0] = tempdist + ver[3][0];
		ver3To0[i][1] = ver[3][1];
		ver3To0[i][2] = ver[3][2];
		//cout << ver2To3[i][1] << endl;
		tempdist -= 0.1f;
	}*/

	//        0            -> - Z �� 
	//    1   2   3      ������ü ����
	//        4            -> + Z ��
	//        5
	

	//���� -> ������ , �Ʒ� -> �� , ��-> ��
	
	//0. -Z�� 3���� ���ҷ� ������ 11x11 �迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[0][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[0][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[0][i][j][2] = -0.5f;
		}
	}

	//1. -X�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[1][i][j][0] = -0.5f;
			Tver[1][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[1][i][j][2] = (-0.5f) + ((double)i*(0.1f));
		}
	}

	//2. -Y�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[2][i][j][0] = -0.5f + ((double)i*(0.1f));
			Tver[2][i][j][1] = -0.5f;
			Tver[2][i][j][2] = (-0.5f) + ((double)j*(0.1f));
		}
	}

	//3. +X�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[3][i][j][0] = 0.5f;
			Tver[3][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[3][i][j][2] = (-0.5f) + ((double)i*(0.1f));
		}
	}

	//4. +Z�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[4][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[4][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[4][i][j][2] = 0.5f;
		}
	}

	//5. +Y�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[5][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[5][i][j][1] = 0.5f;
			Tver[5][i][j][2] = (-0.5f) + ((double)j*(0.1f));
		}
	}

}

//ȸ��
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;


//OpenCV Mat�� OpenGL Texture�� ��ȯ 
GLuint MatToTexture(Mat image)
{
	if (image.empty())  return -1;

	//OpenGL �ؽ�ó ����
	GLuint textureID;
	glGenTextures(1, &textureID);

	//�ؽ�ó ID�� ���ε� -  ����� �ؽ�ó ������ �������ش�.
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
		0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());

	return textureID;
}


void draw_background()
{
	int x = screenW / 120.0;
	int y = screenH / 120.0;
	int w = screenW;
	int h = screenH;

	glBegin(GL_QUADS);
	/*glTexCoord2f(0.0, 0.0); glVertex3f(0, 0,0);
	glTexCoord2f(1.0, 0.0); glVertex3f(w, 0,0);
	glTexCoord2f(1.0, 1.0); glVertex3f(w, h,0);
	glTexCoord2f(0.0, 1.0); glVertex3f(0, h,0);*/

	glTexCoord2f(0.0, 1.0); glVertex3f(-x, -y, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(x, -y, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x, y, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-x, y, 0.0);

	/*glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);*/

	glEnd();
}


void drawBitmapText(char *str, float x, float y, float z)
{
	glRasterPos3f(x, y, z); //���ڿ��� �׷��� ��ġ ����

	while (*str)
	{
		//GLUT_BITMAP_TIMES_ROMAN_24 ��Ʈ�� ����Ͽ� ���ڿ��� �׸���.
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str);

		str++;
	}
}


//ť���� �� ��, ȭ�� ���� ������ -Z��������� 0.5�̵��Ͽ� ���簢���� �׸���.
static void cubebase(void)
{
	glBegin(GL_QUADS);
	glVertex3d(-0.05, -0.05, -0.05);
	glVertex3d(-0.05, 0.05, -0.05);
	glVertex3d(0.05, 0.05, -0.05);
	glVertex3d(0.05, -0.05, -0.05);
	

	/*glVertex3d(point1[0], point1[1], point1[2]);
	glVertex3d(point2[0], point2[1], point2[2]);
	glVertex3d(point3[0], point3[1], point3[2]);
	glVertex3d(point4[0], point4[1], point4[2]);*/
	

	glEnd();
}

//cubebase�Լ����� �׸� �簢���� ȸ�� �� �̵�����
//ť�긦 �ϼ���Ų��.
void draw_cube()
{
	/*glPushMatrix();

	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����
	
	cubebase();

	glPushMatrix();

	glTranslated(1.0, 0.0, 0.0);
	glRotated(90.0, 0.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����
	cubebase();

	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.0, 0.0, 0.0);
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����
	cubebase();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 1.0, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����
	cubebase();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, -1.0, 0.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����
	cubebase();
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����
	glVertex3d(-0.5, -0.5, 0.5);
	glVertex3d(0.5, -0.5, 0.5);
	glVertex3d(0.5, 0.5, 0.5);
	glVertex3d(-0.5, 0.5, 0.5);



	glEnd();

	glPopMatrix();*/
	

	/*glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glEnd();



	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����

	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);


	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);

	glEnd();



	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����

	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);

	glEnd();


	
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);

	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����

	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);

	glEnd();*/

//0. Green, - Z�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����

		glVertex3d(Tver[0][i][j][0], Tver[0][i][j][1], Tver[0][i][j][2]);
		glVertex3d(Tver[0][i][j + 1][0], Tver[0][i][j + 1][1], Tver[0][i][j + 1][2]);
		glVertex3d(Tver[0][i + 1][j + 1][0], Tver[0][i + 1][j + 1][1], Tver[0][i + 1][j + 1][2]);
		glVertex3d(Tver[0][i + 1][j][0], Tver[0][i + 1][j][1], Tver[0][i + 1][j][2]);


		glEnd();
	}
}

//1. Orange, -X�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����

		glVertex3d(Tver[1][i][j][0], 
			Tver[1][i][j][1], 
			Tver[1][i][j][2]);
		
		glVertex3d(Tver[1][i][j + 1][0], 
			Tver[1][i][j + 1][1], 
			Tver[1][i][j + 1][2]);
		
		glVertex3d(Tver[1][i + 1][j + 1][0], 
			Tver[1][i + 1][j + 1][1], 
			Tver[1][i + 1][j + 1][2]);
		
		glVertex3d(Tver[1][i + 1][j][0],
			Tver[1][i + 1][j][1], 
			Tver[1][i + 1][j][2]);


		glEnd();
	}
}

//2. Yellow, -Y�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����

		glVertex3d(Tver[2][i][j][0],
			Tver[2][i][j][1],
			Tver[2][i][j][2]);

		glVertex3d(Tver[2][i][j + 1][0],
			Tver[2][i][j + 1][1],
			Tver[2][i][j + 1][2]);

		glVertex3d(Tver[2][i + 1][j + 1][0],
			Tver[2][i + 1][j + 1][1],
			Tver[2][i + 1][j + 1][2]);

		glVertex3d(Tver[2][i + 1][j][0],
			Tver[2][i + 1][j][1],
			Tver[2][i + 1][j][2]);


		glEnd();
	}
}

//3. Blue, +X�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����


		glVertex3d(Tver[3][i][j][0],
			Tver[3][i][j][1],
			Tver[3][i][j][2]);

		glVertex3d(Tver[3][i][j + 1][0],
			Tver[3][i][j + 1][1],
			Tver[3][i][j + 1][2]);

		glVertex3d(Tver[3][i + 1][j + 1][0],
			Tver[3][i + 1][j + 1][1],
			Tver[3][i + 1][j + 1][2]);

		glVertex3d(Tver[3][i + 1][j][0],
			Tver[3][i + 1][j][1],
			Tver[3][i + 1][j][2]);


		glEnd();
	}
}

//4. Magenta, +Z�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����


		glVertex3d(Tver[4][i][j][0],
			Tver[4][i][j][1],
			Tver[4][i][j][2]);

		glVertex3d(Tver[4][i][j + 1][0],
			Tver[4][i][j + 1][1],
			Tver[4][i][j + 1][2]);

		glVertex3d(Tver[4][i + 1][j + 1][0],
			Tver[4][i + 1][j + 1][1],
			Tver[4][i + 1][j + 1][2]);

		glVertex3d(Tver[4][i + 1][j][0],
			Tver[4][i + 1][j][1],
			Tver[4][i + 1][j][2]);


		glEnd();
	}
}

//5. Red, +Y�� ����
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����


		glVertex3d(Tver[5][i][j][0],
			Tver[5][i][j][1],
			Tver[5][i][j][2]);

		glVertex3d(Tver[5][i][j + 1][0],
			Tver[5][i][j + 1][1],
			Tver[5][i][j + 1][2]);

		glVertex3d(Tver[5][i + 1][j + 1][0],
			Tver[5][i + 1][j + 1][1],
			Tver[5][i + 1][j + 1][2]);

		glVertex3d(Tver[4][i + 1][j][0],
			Tver[5][i + 1][j][1],
			Tver[5][i + 1][j][2]);


		glEnd();
	}
}

	glFlush();
}

/*void sphere(double x ,double y ,double z)
{
	for (float phi = -90.0; phi <= 70.0; phi += 20.0)
	{
		glBegin(GL_QUAD_STRIP);
		for (float theta = -180.0; theta <= 180.0; theta += 20.0)
		{
			x = sin(c*theta)*cos(c*phi);
			y = cos(c*theta)*cos(c*phi);
			z = sin(c*phi);
			glVertex3d(x, y, z);
			x = sin(c*theta)*cos(c*(phi + 20.0));
			y = cos(c*theta)*cos(c*(phi + 20.0));
			z = sin(c*(phi + 20.0));
			glVertex3d(x, y, z);
		}

		glEnd();

	}
}*/

void draw_line()
{

	glPushMatrix(); //X�� ������
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);
	glEnd();
	drawBitmapText("+X", 0.8, 0.0, 0.0);
	drawBitmapText("-X", -0.8, 0.0, 0.0);
	glPopMatrix();

	glPushMatrix(); //Y�� ���
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glEnd();
	drawBitmapText("+Y", 0.0, 0.8, 0.0);
	drawBitmapText("-Y", 0.0, -0.8, 0.0);
	glPopMatrix();

	glPushMatrix(); //Z�� �Ķ���
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, -1.0);
	glEnd();
	drawBitmapText("+Z", 0.0, 0.0, 0.8);
	drawBitmapText("-Z", 0.0, 0.0, -0.8);
	glPopMatrix();


	glFlush();
}

void mdraw_line(CvPoint point)
{
	glBegin(GL_LINES);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();

}
// ī�޶� �ʱ�ȭ
void cameraInit()
{

	/*capture = new VideoCapture(0);

	if (!capture) {
		printf("Could not capture a camera\n\7");
		return;
	}

	Mat img_frame;

	capture->read(img_frame);

	screenW = img_frame.cols;
	screenH = img_frame.rows;

	cout << screenW << " " << screenH << endl;*/

	Mat img_frame;

	Camera.initialize();
	Hand.initialize();

	Camera.getColorImage(&img_frame);
	screenW = img_frame.cols;
	screenH = img_frame.rows;

	cout << screenW << " " << screenH << endl;

}


void display()
{
	//ȭ���� �����. (�÷����ۿ� ���̹���)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//���� ������ ModelView Matirx�� ������ �ش�.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	////////////////////////////
	/*for (int n = 0; n < 6; n++) {
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				glPushMatrix();

				glTranslated(
					Tver[n][i][j][0],
					Tver[n][i][j][1],
					Tver[n][i][j][2]
				);
				////////////////////////////
				glPushMatrix();

				glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����

				cubebase();

				glPushMatrix();

				glTranslated(0.1, 0.0, 0.0);
				glRotated(90.0, 0.0, 1.0, 0.0);
				glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����
				cubebase();

				glPopMatrix();

				glPushMatrix();
				glTranslated(-.1, 0.0, 0.0);
				glRotated(-90.0, 0.0, 1.0, 0.0);
				glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(-90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, -0.1, 0.0);
				glRotated(90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����
				cubebase();
				glPopMatrix();

				
				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(180.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����
				cubebase();
				glPopMatrix();

				glPopMatrix();
				
			}
		}
	}*/
	///////////////////////////
	texture_background = MatToTexture(img_cam);
	if (texture_background < 0) return;


	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f); //ť�곪 ��ǥ�� �׸� �� ����� ���� ������ �ȹ������� �ʿ�
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -9.0);
	draw_background(); //���׸�
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	//glTranslatef(0.0, 0.0, -4.0);
	//glRotatef(cubeAngle, 1.0, 1.0, 1.0);
	//glTranslatef(cubeX, cubeY, cubeZ);
	glTranslatef(cubeX, cubeY, cubeZ-1);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x�࿡ ���� ȸ��
	glRotatef(yaw, 0.0, 1.0, 0.0); //y�࿡ ���� ȸ��
	glRotatef(roll, 0.0, 0.0, 1.0); //z�࿡ ���� ȸ��
	//////////////////////////////////////
	//�� ���ؽ����� �簢���� �׷��ִ� �ڵ�
	/*for (int n = 0; n < 6; n++) {
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				glPushMatrix();

				glTranslated(
					Tver[n][i][j][0],
					Tver[n][i][j][1],
					Tver[n][i][j][2]
				);
				////////////////////////////
				glPushMatrix();

				glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����

				cubebase();

				glPushMatrix();

				glTranslated(0.1, 0.0, 0.0);
				glRotated(90.0, 0.0, 1.0, 0.0);
				glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����
				cubebase();

				glPopMatrix();

				glPushMatrix();
				glTranslated(-.1, 0.0, 0.0);
				glRotated(-90.0, 0.0, 1.0, 0.0);
				glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(-90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, -0.1, 0.0);
				glRotated(90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����
				cubebase();
				glPopMatrix();


				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(180.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����
				cubebase();
				glPopMatrix();

				glPopMatrix();

				glPopMatrix();

			}
		}
	}*/
	/////////////////////////////////////
	
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3d(2.75, 0, 0);
	glVertex3d(0.0, 0, 0);
	glEnd();
	glPopMatrix();

	draw_cube(); //ť��
	draw_line();  //��ǥ��
	
	glPopMatrix();



	glutSwapBuffers();
}




void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //������ ũ��� ������Ʈ ���� 

	glMatrixMode(GL_PROJECTION); //���� ������ Projection Matrix�� ������ �ش�.
	glLoadIdentity();

	//Field of view angle(���� degrees), �������� aspect ratio, Near�� Far Plane����
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //���� ������ ModelView Matirx�� ������ �ش�. 
}

void funcHand(int finger_num, CvPoint Click_Point)
{
	/*double gapX = 0;
	double gapY = 0;
	if (curHandPoint.x == 0 && curHandPoint.y == 0)
		curHandPoint = Click_Point;
	
	gapX = curHandPoint.x - Click_Point.x;
	gapY = curHandPoint.y - Click_Point.y;
	if (pitch >= 360 || pitch <= -360)
		pitch = 0;
	if (yaw >= 360 || yaw <= -360)
		yaw = 0;
	//5 Fingers
	if (finger_num == 1)
	{
		/*cubeX += gapX *(9 / 1280);	//640:4.5
		cubeY += gapY *(11 / 1600); //480:3.3
		cout << "X : " << cubeX << endl;
		cout << "Y : " << cubeY << endl;
		cout << "GAPX : " << gapX << endl;
		cout << "GAPY : " << gapY << endl;*/
		/*/
		/*if (gapX > 20) {
			yaw += 90;
		}
		else if (gapX < -20) {
			yaw -= 90;
		}
		else if (gapY > 20) {
			pitch += 90;
		}

		else if (gapX < -20) {
			pitch -= 90;
		}*/
	/*else {
		curHandPoint = CvPoint(0, 0);
	}*/
	/*//4 Fingers
	if (finger_num == 3) {

		cubeX += gapX *(1/100);	//640:4.5
		cubeY += gapY *(1/100); //480:3.3
	}
	else if (finger_num == 2) {
		//roll -= 10;
		if (roll > 360)
			roll -= 360;

	}*/
	static int IsCliked = 0;
	double gapX = 0;
	double gapY = 0;
	if (curHandPoint.x == 0 && curHandPoint.y == 0)
		curHandPoint = Click_Point;

	gapX = curHandPoint.x - Click_Point.x;
	gapY = curHandPoint.y - Click_Point.y;

	if (pitch >= 360 || pitch <=-360)
		pitch = 0;
	if (yaw >= 360 || yaw <= -360)
		yaw = 0;
	
	
	
	if (finger_num == 1)
	{
		//�� ��ư ������ �ȿ��ִ� ���
		if (Click_Point.x > 0 && Click_Point.x < MOVELOCATION.x + 100
			&& Click_Point.y>0 && Click_Point.y < MOVELOCATION.y + 100)
		{
			IsCliked = 1;
			pitch -= 90;
		}
		//IconNum = MOVE;
		// -> ��ư
		else if (Click_Point.x > 0 && Click_Point.x < ROTATELOCATION.x + 100
			&& Click_Point.y>0 && Click_Point.y < ROTATELOCATION.y + 100)
		{
			IsCliked = 2;
			yaw += 90;
		}
		else if (Click_Point.x > 0 && Click_Point.x < ZOOMLOCATION.x + 100
			&& Click_Point.y>0 && Click_Point.y < ZOOMLOCATION.y + 100)
		{
			if (IsCliked)
				IsCliked = 0;
			else
				IsCliked = 3;
		}

	}
	cout << IsCliked << endl;
	Tools.isClicked(IsCliked);
	switch (IsCliked)
	{
	case 0:
		yaw = 0;
		pitch = 0;
		return;
	case 3:
		if (finger_num == 4){
			yaw -= gapX;
			pitch -= gapY;
		}

	}
}


void timer(int value) {
	//��ķ���κ��� �̹��� ĸó
//	capture->read(img_cam);
//	cvtColor(img_cam, img_cam, COLOR_BGR2RGB);

	Point CurPoint = CvPoint(0, 0);
	
	CvPoint LH_Click_Point = Hand.getLHFingerClickPoint();
	CvPoint RH_Click_Point = Hand.getRHFingerClickPoint();

	CvPoint LH_Hand_Center = Hand.getLHandCenter();
	CvPoint RH_Hand_Center = Hand.getRHandCenter();

	int LH_finger_num = Hand.getLHFingerNum();
	int RH_finger_num = Hand.getRHFingerNum();


	Camera.getBlobImage(&Blob);
	Hand.blob2Contour(Blob, &Contour);
	Camera.getColorImage(&img_cam);
	cvtColor(img_cam, img_cam, COLOR_BGR2RGB);
	Hand.run(&img_cam,&Blob);
	Tools.initMappingToolsOnColor(&img_cam,LH_Click_Point,LH_finger_num);
	//Camera.getDepthImage(&Depth);
	
	//imshow("contour", Contour);
	imshow("Blob", Blob);
	//imshow("mapping tools", img_cam);
	

	//--------------���� �Լ� ������ ---------------//
	//Modify in hand
	//        0            -> - Z �� 
	//    1   2   3      ������ü ����
	//        4            -> + Z ��
	//        5
	/*
	//���鿡�� ������ �޶����°��� ����
	if (pitch == 0)
	{
		// -Z +Y
		//  -X   +X
		//    -Y +Z
		if (yaw == 0) 
		{
		}
		// +X +Y
		//  -Z   +Z
		//    -Y -X
		else if (yaw == 90)
		{
		}
		// -Z +Y
		//  -X   +X
		//    -Y +Z
		else if (yaw == 180)
		{
		}
		// -Z +Y
		//  -X   +X
		//    -Y +Z
		else if (yaw == 270)
		{
		}
	}
	// ���ʿ��� �ٶ󺼋�
	else if (pitch == 90)
	{
		if(yaw == 0){}
		else if (yaw == 90)
		{ }
		else if(yaw == 180)
		{ }
		else if (yaw == 270)
		{ }
	}
	//180�� ������ �޸�
	else if (pitch == 180)
	{
		if (yaw == 0) {}
		else if (yaw == 90)
		{
		}
		else if (yaw == 180)
		{
		}
		else if (yaw == 270)
		{
		}
	}
	//���� 3/4������ ���� ������ �Ʒ���
	else if (pitch == 270)
	{
		if (yaw == 0) {}
		else if (yaw == 90)
		{
		}
		else if (yaw == 180)
		{
		}
		else if (yaw == 270)
		{
		}
	}*/


	//cout << Hand.getLHFingerNum() << endl;
	funcHand(LH_finger_num, LH_Click_Point);
	if (LH_finger_num != 0)
		curHandPoint = LH_Click_Point;
	else {
		curHandPoint.x = 0;
		curHandPoint.y = 0;
	}

	glutPostRedisplay();      //�����츦 �ٽ� �׸����� ��û
	glutTimerFunc(100, timer, 0); //���� Ÿ�̸� �̺�Ʈ�� 1�и�����Ʈ ��  ȣ���.
}


double angle(CvPoint P1,CvPoint P2,CvPoint P3)
{
	double a, b, c;
	double Angle, temp;

	a = sqrt(pow(P1.x - P3.x, 2) + pow(P1.y - P3.y, 2));
	b = sqrt(pow(P1.x - P2.x, 2) + pow(P1.y - P2.y, 2));
	c = sqrt(pow(P2.x - P3.x, 2) + pow(P2.y - P3.y, 2));

	temp = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b*c);

	Angle = acos(temp);
	Angle = Angle * (180 / PI);

	return Angle;
}

void init()
{
	glGenTextures(1, &texture_background);

	//ȭ�� ���ﶧ ����� �� ����
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//���� ���� ���� �� ����� �� ����
	glClearDepth(1.0);

	//���� ���� Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

}

void keyboard(unsigned char key, int x, int y)
{
	//ESC Ű�� �������ٸ� ���α׷� ����
	if (key == 27)
	{
		capture->release();
		exit(0);
	}
	else if (key == 'a')
	{
		yaw += 45;
		if (yaw >= 360)
			yaw = 0;
	}
	else if (key == 'd')
	{
		yaw -= 45;
		if (yaw >= 360)
			yaw = 0;
	}
	else if (key == 'w') {

		pitch += 45;
		if (pitch >= 360 )
			pitch = 0;
	}
	else if (key == 's')
	{
		pitch -= 45;
		if (yaw <= -360)
			yaw = 0;
	}
	
}


int main(int argc, char** argv)
{
	
	glutInit(&argc, argv);  //GLUT �ʱ�ȭ

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //���� ���ۿ� ���� ���۸� ����ϵ��� ����, GLUT_RGB=0x00��
	initVertext();
	cameraInit();
	//-----//



	glutInitWindowSize(screenW, screenH);
	glutInitWindowPosition(100, 100); //�������� ��ġ (x,y)
	glutCreateWindow("OpenGL Example"); //������ ����


	init();

	//���÷��� �ݹ� �Լ� ���, display�Լ��� ������ ó�� ������ ���� ȭ�� �ٽ� �׸� �ʿ� ������ ȣ��ȴ�. 
	glutDisplayFunc(display);

	//reshape �ݹ� �Լ� ���, reshape�Լ��� ������ ó�� ������ ���� ������ ũ�� ����� ȣ��ȴ�.
	glutReshapeFunc(reshape);
	//Ÿ�̸� �ݹ� �Լ� ���, ó������ �ٷ� ȣ���.
	glutTimerFunc(0, timer, 0);
	//Ű���� �ݹ� �Լ� ���, Ű���尡 �������� ȣ��ȴ�. 
	glutKeyboardFunc(keyboard);

	//GLUT event processing loop�� �����Ѵ�.
	//�� �Լ��� ���ϵ��� �ʱ� ������ �����ٿ� �ִ� �ڵ尡 ������� �ʴ´�. 
	glutMainLoop();


	return 0;
}