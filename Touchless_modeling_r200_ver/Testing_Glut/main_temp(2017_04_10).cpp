//상대경로로 헤더파일을 지정합니다.
#include ".\include\GL\freeglut.h"
#include "opencv2/opencv.hpp"  
#include <iostream>  
#include <string> 

#include "mOpenCv.h"

//사용할 라이브러리를 지정해줍니다.
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
//큐브 위치
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//테스팅 벌텍스
float ver[8][3] = {
	{-0.5f,-0.5f,-0.5f},	//뒤왼아
	{-0.5f,0.5f,-0.5f},	//뒤왼위
	{0.5f,0.5f,-0.5f},	//뒤오위
	{0.5f,-0.5f,-0.5f},	//뒤오아
	{ -0.5f,-0.5f,0.5f },	//앞왼아
	{ -0.5f,0.5f,0.5f },	//앞왼위
	{ 0.5f,0.5f,0.5f },	//앞오위
	{ 0.5f,-0.5f,0.5f }	//앞오아
};

float Tver[6][11][11][3];// 정육면체의 면, 면의 X 면의 Y 면내의 (X,Y,Z)

/*
float ver0To1[9][3];
float ver1To2[9][3];
float ver2To3[9][3];
float ver3To0[9][3];
*/
void initVertext()
{
	//왼 -> 오 , 아 -> 위 , 뒤 -> 앞
	//preset vertext
	//"(x2-x1)의제곱+(y2-y1)의제곱+(z2-z1)의 제곱에 루트 값	double tempdist = 0;
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

	//        0            -> - Z 축 
	//    1   2   3      정육면체 순번
	//        4            -> + Z 축
	//        5
	

	//왼쪽 -> 오른쪽 , 아래 -> 위 , 뒤-> 앞
	
	//0. -Z축 3개를 원소로 가지는 11x11 배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[0][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[0][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[0][i][j][2] = -0.5f;
		}
	}

	//1. -X축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[1][i][j][0] = -0.5f;
			Tver[1][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[1][i][j][2] = (-0.5f) + ((double)i*(0.1f));
		}
	}

	//2. -Y축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[2][i][j][0] = -0.5f + ((double)i*(0.1f));
			Tver[2][i][j][1] = -0.5f;
			Tver[2][i][j][2] = (-0.5f) + ((double)j*(0.1f));
		}
	}

	//3. +X축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[3][i][j][0] = 0.5f;
			Tver[3][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[3][i][j][2] = (-0.5f) + ((double)i*(0.1f));
		}
	}

	//4. +Z축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[4][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[4][i][j][1] = (-0.5f) + ((double)j*(0.1f));
			Tver[4][i][j][2] = 0.5f;
		}
	}

	//5. +Y축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 11; j++) {
			Tver[5][i][j][0] = (-0.5f) + ((double)i*(0.1f));
			Tver[5][i][j][1] = 0.5f;
			Tver[5][i][j][2] = (-0.5f) + ((double)j*(0.1f));
		}
	}

}

//회전
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;


//OpenCV Mat을 OpenGL Texture로 변환 
GLuint MatToTexture(Mat image)
{
	if (image.empty())  return -1;

	//OpenGL 텍스처 생성
	GLuint textureID;
	glGenTextures(1, &textureID);

	//텍스처 ID를 바인딩 -  사용할 텍스처 차원을 지정해준다.
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
	glRasterPos3f(x, y, z); //문자열이 그려질 위치 지정

	while (*str)
	{
		//GLUT_BITMAP_TIMES_ROMAN_24 폰트를 사용하여 문자열을 그린다.
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str);

		str++;
	}
}


//큐브의 한 면, 화면 안쪽 방향인 -Z축방향으로 0.5이동하여 정사각형을 그린다.
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

//cubebase함수에서 그린 사각형을 회전 및 이동시켜
//큐브를 완성시킨다.
void draw_cube()
{
	/*glPushMatrix();

	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향
	
	cubebase();

	glPushMatrix();

	glTranslated(1.0, 0.0, 0.0);
	glRotated(90.0, 0.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향
	cubebase();

	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.0, 0.0, 0.0);
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향
	cubebase();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 1.0, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향
	cubebase();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, -1.0, 0.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향
	cubebase();
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향
	glVertex3d(-0.5, -0.5, 0.5);
	glVertex3d(0.5, -0.5, 0.5);
	glVertex3d(0.5, 0.5, 0.5);
	glVertex3d(-0.5, 0.5, 0.5);



	glEnd();

	glPopMatrix();*/
	

	/*glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glEnd();



	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향

	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);


	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);

	glEnd();



	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향

	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);

	glEnd();


	
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향

	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);

	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향

	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);

	glEnd();*/

//0. Green, - Z축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향

		glVertex3d(Tver[0][i][j][0], Tver[0][i][j][1], Tver[0][i][j][2]);
		glVertex3d(Tver[0][i][j + 1][0], Tver[0][i][j + 1][1], Tver[0][i][j + 1][2]);
		glVertex3d(Tver[0][i + 1][j + 1][0], Tver[0][i + 1][j + 1][1], Tver[0][i + 1][j + 1][2]);
		glVertex3d(Tver[0][i + 1][j][0], Tver[0][i + 1][j][1], Tver[0][i + 1][j][2]);


		glEnd();
	}
}

//1. Orange, -X축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향

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

//2. Yellow, -Y축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향

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

//3. Blue, +X축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향


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

//4. Magenta, +Z축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향


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

//5. Red, +Y축 방향
for (int i = 0; i < 10; i++) {
	for (int j = 0; j < 10; j++) {
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향


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

	glPushMatrix(); //X축 붉은색
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);
	glEnd();
	drawBitmapText("+X", 0.8, 0.0, 0.0);
	drawBitmapText("-X", -0.8, 0.0, 0.0);
	glPopMatrix();

	glPushMatrix(); //Y축 녹색
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glEnd();
	drawBitmapText("+Y", 0.0, 0.8, 0.0);
	drawBitmapText("-Y", 0.0, -0.8, 0.0);
	glPopMatrix();

	glPushMatrix(); //Z축 파란색
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
// 카메라 초기화
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
	//화면을 지운다. (컬러버퍼와 깊이버퍼)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//이후 연산은 ModelView Matirx에 영향을 준다.
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

				glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향

				cubebase();

				glPushMatrix();

				glTranslated(0.1, 0.0, 0.0);
				glRotated(90.0, 0.0, 1.0, 0.0);
				glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향
				cubebase();

				glPopMatrix();

				glPushMatrix();
				glTranslated(-.1, 0.0, 0.0);
				glRotated(-90.0, 0.0, 1.0, 0.0);
				glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(-90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, -0.1, 0.0);
				glRotated(90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향
				cubebase();
				glPopMatrix();

				
				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(180.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향
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
	glColor3f(1.0f, 1.0f, 1.0f); //큐브나 좌표축 그릴 때 사용한 색의 영향을 안받을려면 필요
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -9.0);
	draw_background(); //배경그림
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	//glTranslatef(0.0, 0.0, -4.0);
	//glRotatef(cubeAngle, 1.0, 1.0, 1.0);
	//glTranslatef(cubeX, cubeY, cubeZ);
	glTranslatef(cubeX, cubeY, cubeZ-1);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x축에 대해 회전
	glRotatef(yaw, 0.0, 1.0, 0.0); //y축에 대해 회전
	glRotatef(roll, 0.0, 0.0, 1.0); //z축에 대해 회전
	//////////////////////////////////////
	//각 벌텍스마다 사각형을 그려주는 코드
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

				glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향

				cubebase();

				glPushMatrix();

				glTranslated(0.1, 0.0, 0.0);
				glRotated(90.0, 0.0, 1.0, 0.0);
				glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향
				cubebase();

				glPopMatrix();

				glPushMatrix();
				glTranslated(-.1, 0.0, 0.0);
				glRotated(-90.0, 0.0, 1.0, 0.0);
				glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(-90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향
				cubebase();
				glPopMatrix();

				glPushMatrix();
				glTranslated(0.0, -0.1, 0.0);
				glRotated(90.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향
				cubebase();
				glPopMatrix();


				glPushMatrix();
				glTranslated(0.0, 0.1, 0.0);
				glRotated(180.0, 1.0, 0.0, 0.0);
				glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향
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

	draw_cube(); //큐브
	draw_line();  //좌표축
	
	glPopMatrix();



	glutSwapBuffers();
}




void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정 

	glMatrixMode(GL_PROJECTION); //이후 연산은 Projection Matrix에 영향을 준다.
	glLoadIdentity();

	//Field of view angle(단위 degrees), 윈도우의 aspect ratio, Near와 Far Plane설정
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //이후 연산은 ModelView Matirx에 영향을 준다. 
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
		//업 버튼 아이콘 안에있는 경우
		if (Click_Point.x > 0 && Click_Point.x < MOVELOCATION.x + 100
			&& Click_Point.y>0 && Click_Point.y < MOVELOCATION.y + 100)
		{
			IsCliked = 1;
			pitch -= 90;
		}
		//IconNum = MOVE;
		// -> 버튼
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
	//웹캠으로부터 이미지 캡처
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
	

	//--------------따로 함수 뺄거임 ---------------//
	//Modify in hand
	//        0            -> - Z 축 
	//    1   2   3      정육면체 순번
	//        4            -> + Z 축
	//        5
	/*
	//정면에서 각도만 달라지는것을 생각
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
	// 위쪽에서 바라볼떄
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
	//180도 꺽여서 뒷면
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
	//쭉죽 3/4바퀴를 위로 돌려서 아랫면
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

	glutPostRedisplay();      //윈도우를 다시 그리도록 요청
	glutTimerFunc(100, timer, 0); //다음 타이머 이벤트는 1밀리세컨트 후  호출됨.
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

	//화면 지울때 사용할 색 지정
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//깊이 버퍼 지울 때 사용할 값 지정
	glClearDepth(1.0);

	//깊이 버퍼 활성화
	glEnable(GL_DEPTH_TEST);

}

void keyboard(unsigned char key, int x, int y)
{
	//ESC 키가 눌러졌다면 프로그램 종료
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
	
	glutInit(&argc, argv);  //GLUT 초기화

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //더블 버퍼와 깊이 버퍼를 사용하도록 설정, GLUT_RGB=0x00임
	initVertext();
	cameraInit();
	//-----//



	glutInitWindowSize(screenW, screenH);
	glutInitWindowPosition(100, 100); //윈도우의 위치 (x,y)
	glutCreateWindow("OpenGL Example"); //윈도우 생성


	init();

	//디스플레이 콜백 함수 등록, display함수는 윈도우 처음 생성할 때와 화면 다시 그릴 필요 있을때 호출된다. 
	glutDisplayFunc(display);

	//reshape 콜백 함수 등록, reshape함수는 윈도우 처음 생성할 때와 윈도우 크기 변경시 호출된다.
	glutReshapeFunc(reshape);
	//타이머 콜백 함수 등록, 처음에는 바로 호출됨.
	glutTimerFunc(0, timer, 0);
	//키보드 콜백 함수 등록, 키보드가 눌러지면 호출된다. 
	glutKeyboardFunc(keyboard);

	//GLUT event processing loop에 진입한다.
	//이 함수는 리턴되지 않기 때문에 다음줄에 있는 코드가 실행되지 않는다. 
	glutMainLoop();


	return 0;
}