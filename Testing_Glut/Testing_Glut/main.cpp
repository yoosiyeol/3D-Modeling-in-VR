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

Mat Depth;
Mat Blob = cv::Mat::zeros(DepthSize, CV_8UC1);
Mat Contour = cv::Mat::zeros(DepthSize, CV_8UC1);

Point tempHandPoint = CvPoint(0, 0);

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
	int x = screenW / 100.0;
	int y = screenH / 100.0;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-x, -y, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(x, -y, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x, y, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-x, y, 0.0);
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
	glVertex3d(-0.5, -0.5, -0.5);
	glVertex3d(-0.5, 0.5, -0.5);
	glVertex3d(0.5, 0.5, -0.5);
	glVertex3d(0.5, -0.5, -0.5);
	glEnd();
}

//cubebase�Լ����� �׸� �簢���� ȸ�� �� �̵�����
//ť�긦 �ϼ���Ų��.
void draw_cube()
{
	glPushMatrix();

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

	glPopMatrix();

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


	glFlush();
}

// ī�޶� �ʱ�ȭ
void cameraInit()
{

	capture = new VideoCapture(0);

	if (!capture) {
		printf("Could not capture a camera\n\7");
		return;
	}

	Mat img_frame;

	capture->read(img_frame);

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
	glTranslatef(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x�࿡ ���� ȸ��
	glRotatef(yaw, 0.0, 1.0, 0.0); //y�࿡ ���� ȸ��
	glRotatef(roll, 0.0, 0.0, 1.0); //z�࿡ ���� ȸ��

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


void timer(int value) {
	//��ķ���κ��� �̹��� ĸó
	//capture->read(img_cam);
	//cvtColor(img_cam, img_cam, COLOR_BGR2RGB);

	static Point CurPoint = CvPoint(0, 0);

	Camera.getColorImage(&img_cam);
	//Camera.getDepthImage(&Depth);
	Camera.getBlobImage(&Blob);
	Hand.blob2Contour(Blob, &Contour);
	Hand.find_convex_hull(&img_cam);

	imshow("Blob", Blob);
	

	
	//5 Fingers
	if ((Hand.getFingerNum()) == 4) {
		/*cubeAngle += 1.0f;
		if (cubeAngle > 360) {
			cubeAngle -= 360;
		}*/
		if (tempHandPoint == Point(0, 0)) {
			tempHandPoint = Hand.getFingerClickPoint();
		}
		yaw -= tempHandPoint.x - Hand.getFingerClickPoint().x;
		if (pitch > 360)
			pitch -= 360;
		pitch -= tempHandPoint.y - Hand.getFingerClickPoint().y;
		if (yaw > 360)
			yaw -= 360;
		tempHandPoint = Hand.getFingerClickPoint();
	}
	else if ((Hand.getFingerNum()) == 0)
		tempHandPoint = Point(0, 0);


	glutPostRedisplay();      //�����츦 �ٽ� �׸����� ��û
	glutTimerFunc(1, timer, 0); //���� Ÿ�̸� �̺�Ʈ�� 1�и�����Ʈ ��  ȣ���.
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
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);  //GLUT �ʱ�ȭ

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //���� ���ۿ� ���� ���۸� ����ϵ��� ����, GLUT_RGB=0x00��

	//cameraInit();
	//-----//


	screenW = ColorSize.width;
	screenH = ColorSize.height;

	Camera.initialize();
	Hand.initialize();

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