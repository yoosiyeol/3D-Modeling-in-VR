//상대경로로 헤더파일을 지정합니다.
#include ".\include\GL\freeglut.h"
#include "opencv2/opencv.hpp"  
#include <iostream>  
#include <string> 
#include <time.h>
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

Mat Depth;
Mat Blob = cv::Mat::zeros(DepthSize, CV_8UC1);
Mat Contour = cv::Mat::zeros(DepthSize, CV_8UC1);

Point curHandPoint = CvPoint(0, 0);

int distance;

VideoCapture * capture;
Mat img_cam;
Mat img_out;
int screenW;
int screenH;
int windowW;
int windowH;

GLuint texture_background, texture_cube;

//큐브 위치
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//회전
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;


//해당 물체의 위치
GLdouble objX, objY, objZ;

int numVer = 0;
float ****Tver;// 정육면체의 면, 면의 X, 면의 Y, 면 내의 (X,Y,Z)


void initVertext();

//OpenCV Mat을 OpenGL Texture로 변환 
GLuint MatToTexture(Mat image);

void draw_background();


void drawBitmapText(char *str, float x, float y, float z);


//큐브의 한 면, 화면 안쪽 방향인 -Z축방향으로 0.5이동하여 정사각형을 그린다.
static void cubebase(void);

//cubebase함수에서 그린 사각형을 회전 및 이동시켜
//큐브를 완성시킨다.
void draw_cube();

void draw_line();

void mdraw_line(CvPoint point);
// 카메라 초기화
void cameraInit();

void draw_circle();
//////////////////////////////////


//void draw_collision_point(double radius);

void find_obj(GLint x, GLint y);

void modiVertUD(float mouseX, float mouseY, float mouseZ, double cubeSize);

void modiVertLR(float mouseX, float mouseY, float mouseZ, double cubeSize);

/////////////////////////////////////

void display();

void reshape(GLsizei width, GLsizei height);

void funcHand(int finger_num, CvPoint Click_Point, char LorR);

void timer(int value);

double angle(CvPoint P1, CvPoint P2, CvPoint P3);

void init();

void keyboard(unsigned char key, int x, int y);

void MyMouseClick(GLint button, GLint state, GLint x, GLint y);

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

	//마우스 콜백 함수 등록, 마우스가 눌려지면 호출된다.
	glutMouseFunc(MyMouseClick);

	//GLUT event processing loop에 진입한다.
	//이 함수는 리턴되지 않기 때문에 다음줄에 있는 코드가 실행되지 않는다. 
	glutMainLoop();


	return 0;
}

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


	cin >> numVer;

	if (2 > numVer && numVer < 20)
		exit(0);

	//Tver 입력을 받고 그에 따른 동적 할당
	Tver = (float****)malloc(sizeof(float***) * 6);
	for (int i = 0; i < 6; i++)
	{
		Tver[i] = (float***)malloc(sizeof(float**)*numVer);
		for (int j = 0; j < numVer; j++)
		{
			Tver[i][j] = (float**)malloc(sizeof(float*)*numVer);
			for (int k = 0; k < numVer; k++)
			{
				Tver[i][j][k] = (float*)malloc(sizeof(float) * 3);
			}
		}
	}

	//왼쪽 -> 오른쪽 , 아래 -> 위 , 뒤-> 앞

	//0. -Z축 3개를 원소로 가지는 11x11 배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[0][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[0][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[0][i][j][2] = -0.5f;
		}
	}

	//1. -X축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[1][i][j][0] = -0.5f;
			Tver[1][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[1][i][j][2] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
		}
	}

	//2. -Y축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[2][i][j][0] = -0.5f + ((double)i*(1.0 / (numVer - 1)));
			Tver[2][i][j][1] = -0.5f;
			Tver[2][i][j][2] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
		}
	}

	//3. +X축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[3][i][j][0] = 0.5f;
			Tver[3][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[3][i][j][2] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
		}
	}

	//4. +Z축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[4][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[4][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[4][i][j][2] = 0.5f;
		}
	}

	//5. +Y축 3개를 원소로 가지는 11x11배열 초기화
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[5][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[5][i][j][1] = 0.5f;
			Tver[5][i][j][2] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
		}
	}
}

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
	glMatrixMode(GL_MODELVIEW);


	//0. Green, - Z축 방향
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i <numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i <numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	/*if (pitch == 0 && yaw == 0) {
		glPushMatrix();//test
		//glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_QUADS);
		glVertex3d(-100, 100, -1);
		glVertex3d(100, 100, -1);
		glVertex3d(100, -100, -1);
		glVertex3d(-100, -100, -1);
		glEnd();
		glPopMatrix();
	}*/

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

void draw_circle()
{
	double radius = 0.02;

	if (yaw == 0 || yaw == 90 ||
		yaw == 180 || yaw == 270) {
		if (pitch == 0 || pitch == 180) {
			glPushMatrix();
			glColor3f(1.0f, 0, 0);
			glTranslated(objX, objY, 0);
			glutSolidSphere(0.05, 10, 10);
			glPopMatrix();

		}
		else if (pitch == 90 || pitch == 270)
		{
			glPushMatrix();
			glColor3f(1.0f, 0, 0);
			glTranslated(objX, 0, objZ);
			glutSolidSphere(0.05, 10, 10);
			glPopMatrix();
		}

	}
	//각 vertex 마다 원을 그려줌
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < numVer; j++)
		{
			for (int n = 0; n < numVer; n++)
			{
				glPushMatrix();
				glColor3f(0.9f, 0.9f, 0.9f);
				glTranslated(Tver[i][j][n][0], Tver[i][j][n][1], Tver[i][j][n][2]);
				glutSolidSphere(radius, 10, 10);
				glPopMatrix();

			}
		}
	}
}
///////////////////////////////


void find_obj(GLint x, GLint y)
{
	//glReadPixels(x,y,)
	GLfloat winX = (float)x;
	GLfloat winY = windowH -(float)y;		//500 is WINSIZE.height
	GLfloat zCursor;

	GLint viewPort[4];

	GLdouble projection[16];
	GLdouble modelView[16];

	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);

	if (gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, &objX, &objY, &objZ) == GLU_FALSE) {
		printf("실패\n");
	}

	std::cout << "X: " << objX << "  Y: " << objY << "  Z: " << objZ << std::endl;



	//MoveVertex(objX, objY, objZ);
}

void modiVertUD(float mouseX, float mouseY, float mouseZ, double cubeSize)
{
	if (objX == 0 && objY == 0 && objZ == 0)
		return;
	double shaveSize = 0.025;
	//Z축 픽스
	if (pitch == 0) {
		//Z축 픽스
		if (yaw == 0 || yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시
						if (mouseY > 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;

							}
						}
						//밑에서 충돌 시
						else if (mouseY < 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] > (mouseY - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;
							}
						}
					}
				}
			}
		}

		//X축 픽스
		else if (yaw == 90 || yaw == -270 || yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Y)
						if (mouseY > 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 Z)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;

							}
						}
						//밑에서 충돌 시(Y)
						else if (mouseY < 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								(Tver[i][j][n][1] > mouseY))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;
							}
						}
					}
				}
			}
		}
	}

	//Y축 픽스
	else if (pitch == 90)
	{
		//Z축 변경
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Z)
						if (mouseZ < 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;

							}
						}
						//밑에서 충돌 시(Z)
						else if (mouseZ > 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;
							}
						}
					}
				}
			}
		}
		else if (yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Z)
						if (mouseZ > 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;

							}
						}
						//밑에서 충돌 시(Z)
						else if (mouseZ < 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;
							}
						}
					}
				}
			}
		}

		//X축 변경
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(X)
						if (mouseX < 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;

							}
						}
						//밑에서 충돌 시(X)
						else if (mouseX > 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;
							}
						}
					}
				}
			}
		}
		else if (yaw == -270 || yaw == 90) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(X)
						if (mouseX > 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;

							}
						}
						//밑에서 충돌 시(X)
						else if (mouseX < 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;
							}
						}
					}
				}
			}
		}

	} // pitch 90

	  //Z축 픽스
	else if (pitch == 180)
	{
		//Z축 픽스
		if (yaw == 0 || yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시
						if (mouseY < 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;

							}
						}
						//밑에서 충돌 시
						else if (mouseY > 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] > (mouseY - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;
							}
						}
					}
				}
			}
		}

		//X축 픽스
		else if (yaw == -90 || yaw == -270 || yaw == 90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Y)
						if (mouseY < 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 Z)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;

							}
						}
						//밑에서 충돌 시(Y)
						else if (mouseY > 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								(Tver[i][j][n][1] > mouseY))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;
							}
						}
					}
				}
			}
		}
	}

	//Y축 픽스
	else if (pitch == 270)
	{
		//Z축 변경
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Z)
						if (mouseZ > 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;

							}
						}
						//밑에서 충돌 시(Z)
						else if (mouseZ < 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;
							}
						}
					}
				}
			}
		}
		else if (yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(Z)
						if (mouseZ < 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;

							}
						}
						//밑에서 충돌 시(Z)
						else if (mouseZ > 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;
							}
						}
					}
				}
			}
		}

		//X축 변경
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(X)
						if (mouseX > 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;

							}
						}
						//밑에서 충돌 시(X)
						else if (mouseX < 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;
							}
						}
					}
				}
			}
		}
		else if (yaw == -270 || yaw == 90) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//위에서 충돌 시(X)
						if (mouseX < 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;

							}
						}
						//밑에서 충돌 시(X)
						else if (mouseX > 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// 좌우 범위 설정(좌우가 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;
							}
						}
					}
				}
			}
		}

	} // pitch 270
}

void modiVertLR(float mouseX, float mouseY, float mouseZ, double cubeSize)
{
	if (objX == 0 && objY == 0 && objZ == 0)
		return;
	double shaveSize = 0.025;
	//Z축 픽스
	if (pitch == 0 || pitch == 180) {
		//Z축 픽스
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{

						//오른쪽에서 충돌 시
						if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
							Tver[i][j][n][0] < mouseX + cubeSize / 2)
						{
							//위아래 범위 설정
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][0] -= shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
		//X축 픽스
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
							Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][2] += shaveSize;	//vertex 수정


						}
					}
				}
			}
		}

		//Z 축 픽스
		else if (yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
							Tver[i][j][n][0] >(mouseX - cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][0] += shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
		//X축 픽스 
		else if (yaw == -270 || yaw == 90) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
							Tver[i][j][n][2] <(mouseZ + cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][2] -= shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
	}
	//Y축 픽스
	else if (pitch == 90 || pitch == 270) {
		//X축 변경
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][0] >(mouseX - cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
								Tver[i][j][n][0] -= shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
		//Z축 변경
		else if (yaw == 90 || yaw == -270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] >(mouseX - cubeSize / 2))
								Tver[i][j][n][2] -= shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
		//X축 변경
		else if (yaw == 180 || yaw == -180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][0] <(mouseX + cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
								Tver[i][j][n][0] += shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
		//Z축 변경
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//오른쪽에서 충돌 시
						if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2))
						{
							//위아래 범위 설정
							if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] >(mouseX - cubeSize / 2))
								Tver[i][j][n][2] += shaveSize;	//vertex 수정

						}
					}
				}
			}
		}
	}
}
////////////////////////

void display()
{
	//화면을 지운다. (컬러버퍼와 깊이버퍼)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//이후 연산은 ModelView Matirx에 영향을 준다.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//백그라운드 불러오기
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
	
	
	glTranslatef(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x축에 대해 회전
	glRotatef(yaw, 0.0, 1.0, 0.0); //y축에 대해 회전
	glRotatef(roll, 0.0, 0.0, 1.0); //z축에 대해 회전


	draw_circle();
	draw_cube(); //큐브
	draw_line();  //좌표축


	



	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	windowW = width;
	windowH = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정 

	glMatrixMode(GL_PROJECTION); //이후 연산은 Projection Matrix에 영향을 준다.
	glLoadIdentity();

	//Field of view angle(단위 degrees), 윈도우의 aspect ratio, Near와 Far Plane설정
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //이후 연산은 ModelView Matirx에 영향을 준다. 
}

void saveVertex(Mat *output)
{
	FILE *pFile;
	struct tm *t;
	time_t timer;
	char curTime[100];

	Point myPoint;
	
	/// Text Location
	myPoint.x = 10;
	myPoint.y = 40;
	/// Font Face
	int myFontFace = 2;

	/// Font Scale
	double myFontScale = 1.2;
	timer = time(NULL);    // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	sprintf(curTime, "%04d_%02d_%02d_%02d_%02d_%02d.txt",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);
	cout << curTime << endl;
	if ((pFile=fopen(curTime, "w")) !=NULL) {
		
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < numVer; j++)
			{
				for (int k = 0; k < numVer; k++)
				{
					fprintf(pFile, "%f %f %f\n",
						Tver[i][j][k][0], Tver[i][j][k][1], Tver[i][j][k][2]);
				}
			}
		}
		putText(*output, "Save success", myPoint, myFontFace, myFontScale, Scalar::all(255));
		fclose(pFile);
		Sleep(1000);
	}
	else
	{
		putText(*output, "Save was failed", myPoint, myFontFace, myFontScale, Scalar::all(255));
		Sleep(1000);

	}//else

}

void loadVertex(Mat *output)
{
	FILE *pFile;
	char readVer[100];
	char readFile[100];
	char *tokVer;
	cout << "Input FIle :";
	cin >> readFile;
	if ((pFile = fopen(readFile,"r")) != NULL)
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < numVer; j++)
			{
				for (int k = 0; k < numVer; k++)
				{
					fgets(readVer, 100, pFile);

					tokVer = strtok(readVer, " ");
					while (tokVer != NULL)
					{
		
						Tver[i][j][k][0] = atof(tokVer);
						tokVer = strtok(NULL, " ");
						Tver[i][j][k][1] = atof(tokVer);
						tokVer = strtok(NULL, " ");
						Tver[i][j][k][2] = atof(tokVer);
						tokVer = strtok(NULL, " ");
					}
				}
			}

		}
	}
}

void funcHand(int finger_num, CvPoint Click_Point, char LorR)
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

	if (pitch >= 360 || pitch <= -360)
		pitch = 0;
	if (yaw >= 360 || yaw <= -360)
		yaw = 0;


	if (LorR == 'L') {
		static int count = 0;
		count++;

		if (finger_num == 1)
		{
			static int term = 0;

			//업 버튼 아이콘 안에있는 경우
			if (Click_Point.x > 0 && Click_Point.x < MOVELOCATION.x + 100
				&& Click_Point.y>0 && Click_Point.y < MOVELOCATION.y + 100)
			{
				term++;
				if (term > 5) {
					IsCliked = 1;
					pitch += 90;
					term = 0;
				}
			}
			//IconNum = MOVE;
			// -> 버튼
			else if (Click_Point.x > 0 && Click_Point.x < ROTATELOCATION.x + 100
				&& Click_Point.y>0 && Click_Point.y < ROTATELOCATION.y + 100)
			{
				term++;
				if (term > 5) {
					IsCliked = 2;
					yaw += 90;
					term = 0;
				}
			}
			else if (Click_Point.x > 0 && Click_Point.x < ZOOMLOCATION.x + 100
				&& Click_Point.y>0 && Click_Point.y < ZOOMLOCATION.y + 100)
			{
				term++;
				if (term > 5) {
					if (IsCliked)
						IsCliked = 0;
					else
						IsCliked = 3;
					term = 0;
				}
			}

		}
		//cout << IsCliked << endl;
		Tools.isClicked(IsCliked);
		switch (IsCliked)
		{
		case 0:
			yaw = 0;
			pitch = 0;
			return;
		case 3:
			if (finger_num == 4) {
				yaw -= gapX;
				pitch -= gapY;
			}

		}
	}
	else if (LorR == 'R')
	{

		if (finger_num == 1) {
			find_obj(Click_Point.x, Click_Point.y);
			modiVertLR(objX, objY, objZ, 0.1);
		}
		else if (finger_num == 2) {
			find_obj(Click_Point.x, Click_Point.y);
			modiVertUD(objX, objY, objZ, 0.1);
		}
		else if (finger_num == 3)
		{
			saveVertex(&img_cam);
		}
		else if (finger_num == 4)
		{
			loadVertex(&img_cam);
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

	objX = 0;
	objY = 0;
	objZ = 0;
	Camera.getBlobImage(&Blob);
	Hand.blob2Contour(Blob, &Contour);
	Camera.getColorImage(&img_cam);
	cvtColor(img_cam, img_cam, COLOR_BGR2RGB);
	Hand.run(&img_cam, &Blob,windowW,windowH);
	Tools.initMappingToolsOnColor(&img_cam, LH_Click_Point, LH_finger_num);
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
	funcHand(LH_finger_num, LH_Click_Point, 'L');
	funcHand(RH_finger_num, RH_Click_Point, 'R');
	if (LH_finger_num != 0)
		curHandPoint = LH_Click_Point;
	else {
		curHandPoint.x = 0;
		curHandPoint.y = 0;
	}

	glutPostRedisplay();      //윈도우를 다시 그리도록 요청
	glutTimerFunc(100, timer, 0); //다음 타이머 이벤트는 1밀리세컨트 후  호출됨.
}

double angle(CvPoint P1, CvPoint P2, CvPoint P3)
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
		if (pitch >= 360)
			pitch = 0;
	}
	else if (key == 's')
	{
		pitch -= 45;
		if (yaw <= -360)
			yaw = 0;
	}

}

void MyMouseClick(GLint button, GLint state, GLint x, GLint y)
{
	//glReadPixels(x,y,)
	GLfloat winX = (float)x;
	GLfloat winY = 500 - (float)y;		//500 is WINSIZE.height
	GLfloat zCursor;

	GLint viewPort[4];

	GLdouble objX, objY, objZ;

	GLdouble projection[16];
	GLdouble modelView[16];

	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);

	if (gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, &objX, &objY, &objZ) == GLU_FALSE) {
		printf("실패\n");
	}

	std::cout << "X: " << objX << "  Y: " << objY << "  Z: " << objZ << std::endl;


	//MoveVertex(objX, objY, objZ);
}