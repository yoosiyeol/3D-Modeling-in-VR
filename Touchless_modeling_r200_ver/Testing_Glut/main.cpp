//����η� ��������� �����մϴ�.
#include ".\include\GL\freeglut.h"
#include "opencv2/opencv.hpp"  
#include <iostream>  
#include <string> 
#include <time.h>
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

//ť�� ��ġ
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//ȸ��
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;


//�ش� ��ü�� ��ġ
GLdouble objX, objY, objZ;

int numVer = 0;
float ****Tver;// ������ü�� ��, ���� X, ���� Y, �� ���� (X,Y,Z)


void initVertext();

//OpenCV Mat�� OpenGL Texture�� ��ȯ 
GLuint MatToTexture(Mat image);

void draw_background();


void drawBitmapText(char *str, float x, float y, float z);


//ť���� �� ��, ȭ�� ���� ������ -Z��������� 0.5�̵��Ͽ� ���簢���� �׸���.
static void cubebase(void);

//cubebase�Լ����� �׸� �簢���� ȸ�� �� �̵�����
//ť�긦 �ϼ���Ų��.
void draw_cube();

void draw_line();

void mdraw_line(CvPoint point);
// ī�޶� �ʱ�ȭ
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

	//���콺 �ݹ� �Լ� ���, ���콺�� �������� ȣ��ȴ�.
	glutMouseFunc(MyMouseClick);

	//GLUT event processing loop�� �����Ѵ�.
	//�� �Լ��� ���ϵ��� �ʱ� ������ �����ٿ� �ִ� �ڵ尡 ������� �ʴ´�. 
	glutMainLoop();


	return 0;
}

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


	cin >> numVer;

	if (2 > numVer && numVer < 20)
		exit(0);

	//Tver �Է��� �ް� �׿� ���� ���� �Ҵ�
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

	//���� -> ������ , �Ʒ� -> �� , ��-> ��

	//0. -Z�� 3���� ���ҷ� ������ 11x11 �迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[0][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[0][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[0][i][j][2] = -0.5f;
		}
	}

	//1. -X�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[1][i][j][0] = -0.5f;
			Tver[1][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[1][i][j][2] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
		}
	}

	//2. -Y�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[2][i][j][0] = -0.5f + ((double)i*(1.0 / (numVer - 1)));
			Tver[2][i][j][1] = -0.5f;
			Tver[2][i][j][2] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
		}
	}

	//3. +X�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[3][i][j][0] = 0.5f;
			Tver[3][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[3][i][j][2] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
		}
	}

	//4. +Z�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[4][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[4][i][j][1] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
			Tver[4][i][j][2] = 0.5f;
		}
	}

	//5. +Y�� 3���� ���ҷ� ������ 11x11�迭 �ʱ�ȭ
	for (int i = 0; i < numVer; i++) {
		for (int j = 0; j < numVer; j++) {
			Tver[5][i][j][0] = (-0.5f) + ((double)i*(1.0 / (numVer - 1)));
			Tver[5][i][j][1] = 0.5f;
			Tver[5][i][j][2] = (-0.5f) + ((double)j*(1.0 / (numVer - 1)));
		}
	}
}

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
	glMatrixMode(GL_MODELVIEW);


	//0. Green, - Z�� ����
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i <numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i <numVer - 1; i++) {
		for (int j = 0; j <numVer - 1; j++) {
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
	for (int i = 0; i < numVer - 1; i++) {
		for (int j = 0; j < numVer - 1; j++) {
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
	//�� vertex ���� ���� �׷���
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
		printf("����\n");
	}

	std::cout << "X: " << objX << "  Y: " << objY << "  Z: " << objZ << std::endl;



	//MoveVertex(objX, objY, objZ);
}

void modiVertUD(float mouseX, float mouseY, float mouseZ, double cubeSize)
{
	if (objX == 0 && objY == 0 && objZ == 0)
		return;
	double shaveSize = 0.025;
	//Z�� �Ƚ�
	if (pitch == 0) {
		//Z�� �Ƚ�
		if (yaw == 0 || yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��
						if (mouseY > 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��
						else if (mouseY < 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] > (mouseY - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;
							}
						}
					}
				}
			}
		}

		//X�� �Ƚ�
		else if (yaw == 90 || yaw == -270 || yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(Y)
						if (mouseY > 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 Z)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Y)
						else if (mouseY < 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								(Tver[i][j][n][1] > mouseY))
							{
								// �¿� ���� ����(�¿찡 X)
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

	//Y�� �Ƚ�
	else if (pitch == 90)
	{
		//Z�� ����
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(Z)
						if (mouseZ < 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Z)
						else if (mouseZ > 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
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
						//������ �浹 ��(Z)
						if (mouseZ > 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Z)
						else if (mouseZ < 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;
							}
						}
					}
				}
			}
		}

		//X�� ����
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(X)
						if (mouseX < 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��(X)
						else if (mouseX > 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
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
						//������ �浹 ��(X)
						if (mouseX > 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��(X)
						else if (mouseX < 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
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

	  //Z�� �Ƚ�
	else if (pitch == 180)
	{
		//Z�� �Ƚ�
		if (yaw == 0 || yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��
						if (mouseY < 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��
						else if (mouseY > 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] > (mouseY - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][1] -= shaveSize;
							}
						}
					}
				}
			}
		}

		//X�� �Ƚ�
		else if (yaw == -90 || yaw == -270 || yaw == 90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(Y)
						if (mouseY < 0) {

							if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
								Tver[i][j][n][1] < (mouseY + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 Z)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][1] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Y)
						else if (mouseY > 0) {
							if (Tver[i][j][n][1] < (mouseY + cubeSize / 2) &&
								(Tver[i][j][n][1] > mouseY))
							{
								// �¿� ���� ����(�¿찡 X)
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

	//Y�� �Ƚ�
	else if (pitch == 270)
	{
		//Z�� ����
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(Z)
						if (mouseZ > 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Z)
						else if (mouseZ < 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
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
						//������ �浹 ��(Z)
						if (mouseZ < 0) {

							if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
								Tver[i][j][n][2] < (mouseZ + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 -X+)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��(Z)
						else if (mouseZ > 0) {
							if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 X)
								if ((Tver[i][j][n][0] < mouseX + cubeSize / 2) &&
									(Tver[i][j][n][0] > mouseX - cubeSize / 2))
									Tver[i][j][n][2] -= shaveSize;
							}
						}
					}
				}
			}
		}

		//X�� ����
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//������ �浹 ��(X)
						if (mouseX > 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] -= shaveSize;

							}
						}
						//�ؿ��� �浹 ��(X)
						else if (mouseX < 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
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
						//������ �浹 ��(X)
						if (mouseX < 0) {

							if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
								Tver[i][j][n][0] < (mouseX + cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
								if ((Tver[i][j][n][2] < mouseZ + cubeSize / 2) &&
									(Tver[i][j][n][2] > mouseZ - cubeSize / 2))
									Tver[i][j][n][0] += shaveSize;

							}
						}
						//�ؿ��� �浹 ��(X)
						else if (mouseX > 0) {
							if (Tver[i][j][n][0] < (mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] > (mouseX - cubeSize / 2))
							{
								// �¿� ���� ����(�¿찡 +Z-)
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
	//Z�� �Ƚ�
	if (pitch == 0 || pitch == 180) {
		//Z�� �Ƚ�
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{

						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][0] >(mouseX - cubeSize / 2) &&
							Tver[i][j][n][0] < mouseX + cubeSize / 2)
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][0] -= shaveSize;	//vertex ����

						}
					}
				}
			}
		}
		//X�� �Ƚ�
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][2] < (mouseZ + cubeSize / 2) &&
							Tver[i][j][n][2] > (mouseZ - cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][2] += shaveSize;	//vertex ����


						}
					}
				}
			}
		}

		//Z �� �Ƚ�
		else if (yaw == -180 || yaw == 180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
							Tver[i][j][n][0] >(mouseX - cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][0] += shaveSize;	//vertex ����

						}
					}
				}
			}
		}
		//X�� �Ƚ� 
		else if (yaw == -270 || yaw == 90) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2) &&
							Tver[i][j][n][2] <(mouseZ + cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
								Tver[i][j][n][1] >(mouseY - cubeSize / 2))
								Tver[i][j][n][2] -= shaveSize;	//vertex ����

						}
					}
				}
			}
		}
	}
	//Y�� �Ƚ�
	else if (pitch == 90 || pitch == 270) {
		//X�� ����
		if (yaw == 0) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][0] >(mouseX - cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
								Tver[i][j][n][0] -= shaveSize;	//vertex ����

						}
					}
				}
			}
		}
		//Z�� ����
		else if (yaw == 90 || yaw == -270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] >(mouseX - cubeSize / 2))
								Tver[i][j][n][2] -= shaveSize;	//vertex ����

						}
					}
				}
			}
		}
		//X�� ����
		else if (yaw == 180 || yaw == -180) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][0] <(mouseX + cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2) &&
								Tver[i][j][n][2] >(mouseZ - cubeSize / 2))
								Tver[i][j][n][0] += shaveSize;	//vertex ����

						}
					}
				}
			}
		}
		//Z�� ����
		else if (yaw == -90 || yaw == 270) {
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < numVer; j++)
				{
					for (int n = 0; n < numVer; n++)
					{
						//�����ʿ��� �浹 ��
						if (Tver[i][j][n][2] <(mouseZ + cubeSize / 2))
						{
							//���Ʒ� ���� ����
							if (Tver[i][j][n][0] <(mouseX + cubeSize / 2) &&
								Tver[i][j][n][0] >(mouseX - cubeSize / 2))
								Tver[i][j][n][2] += shaveSize;	//vertex ����

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
	//ȭ���� �����. (�÷����ۿ� ���̹���)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//���� ������ ModelView Matirx�� ������ �ش�.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//��׶��� �ҷ�����
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
	
	
	glTranslatef(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x�࿡ ���� ȸ��
	glRotatef(yaw, 0.0, 1.0, 0.0); //y�࿡ ���� ȸ��
	glRotatef(roll, 0.0, 0.0, 1.0); //z�࿡ ���� ȸ��


	draw_circle();
	draw_cube(); //ť��
	draw_line();  //��ǥ��


	



	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	windowW = width;
	windowH = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //������ ũ��� ������Ʈ ���� 

	glMatrixMode(GL_PROJECTION); //���� ������ Projection Matrix�� ������ �ش�.
	glLoadIdentity();

	//Field of view angle(���� degrees), �������� aspect ratio, Near�� Far Plane����
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //���� ������ ModelView Matirx�� ������ �ش�. 
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
	timer = time(NULL);    // ���� �ð��� �� ������ ���
	t = localtime(&timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ�
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

			//�� ��ư ������ �ȿ��ִ� ���
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
			// -> ��ư
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
	funcHand(LH_finger_num, LH_Click_Point, 'L');
	funcHand(RH_finger_num, RH_Click_Point, 'R');
	if (LH_finger_num != 0)
		curHandPoint = LH_Click_Point;
	else {
		curHandPoint.x = 0;
		curHandPoint.y = 0;
	}

	glutPostRedisplay();      //�����츦 �ٽ� �׸����� ��û
	glutTimerFunc(100, timer, 0); //���� Ÿ�̸� �̺�Ʈ�� 1�и�����Ʈ ��  ȣ���.
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
		printf("����\n");
	}

	std::cout << "X: " << objX << "  Y: " << objY << "  Z: " << objZ << std::endl;


	//MoveVertex(objX, objY, objZ);
}