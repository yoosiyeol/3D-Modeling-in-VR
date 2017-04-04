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
	/*glVertex3d(-0.5, -0.5, -0.5);
	glVertex3d(-0.5, 0.5, -0.5);
	glVertex3d(0.5, 0.5, -0.5);
	glVertex3d(0.5, -0.5, -0.5);*/
	glVertex3d(point1[0], point1[1], point1[2]);
	glVertex3d(point2[0], point2[1], point2[2]);
	glVertex3d(point3[0], point3[1], point3[2]);
	glVertex3d(point4[0], point4[1], point4[2]);
	glEnd();
}

//cubebase함수에서 그린 사각형을 회전 및 이동시켜
//큐브를 완성시킨다.
void draw_cube()
{
	glPushMatrix();

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

	glPopMatrix();

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


	glFlush();
}

// 카메라 초기화
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
	//화면을 지운다. (컬러버퍼와 깊이버퍼)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//이후 연산은 ModelView Matirx에 영향을 준다.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	glTranslated(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x축에 대해 회전
	glRotatef(yaw, 0.0, 1.0, 0.0); //y축에 대해 회전
	glRotatef(roll, 0.0, 0.0, 1.0); //z축에 대해 회전



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
	double gap = 0;

	if (curHandPoint.x == 0 && curHandPoint.y == 0)
		curHandPoint = Click_Point;

	//5 Fingers
	if (finger_num == 4) {
		gap = curHandPoint.x - Click_Point.x;
		yaw -= gap;
		cout << gap << endl;
		if (yaw > 360)
			yaw -= 360;
		
		gap = curHandPoint.y - Click_Point.y;
		pitch -= gap;
		if (pitch > 360)
			pitch -= 360;
	}
	if (finger_num == 3) {

		//cubeX += (curHandPoint.x - Click_Point.x) / 50;
		//cubeY += (curHandPoint.y - Click_Point.y) / 50;
	}
	else if (finger_num == 2) {
		//roll -= 10;
		if (roll > 360)
			roll -= 360;

	}
	else if (finger_num == 0)
		curHandPoint = CvPoint(0, 0);

	curHandPoint = Click_Point;

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
	//Tools.initMappingToolsOnColor(&img_cam,Hand.getFingerClickPoint(),Hand.getFingerNum());
	//Camera.getDepthImage(&Depth);
	
	//imshow("contour", Contour);
	imshow("Blob", Blob);
	//imshow("mapping tools", img_cam);
	

	//--------------따로 함수 뺄거임 ---------------//
	//cout << Hand.getLHFingerNum() << endl;
	funcHand(LH_finger_num, LH_Click_Point);
	
	//----------------------------------------------------//
	
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
}


int main(int argc, char** argv)
{
	
	glutInit(&argc, argv);  //GLUT 초기화

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //더블 버퍼와 깊이 버퍼를 사용하도록 설정, GLUT_RGB=0x00임

	//cameraInit();
	//-----//


	screenW = ColorSize.width;
	screenH = ColorSize.height;

	Camera.initialize();
	Hand.initialize();

	glutInitWindowSize(screenW+110, screenH);
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