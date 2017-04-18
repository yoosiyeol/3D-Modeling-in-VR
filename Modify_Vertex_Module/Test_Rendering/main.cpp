//상대경로로 헤더파일을 지정합니다.
#include ".\include\GL\freeglut.h"
#include <iostream>

//min max 함수
#include <algorithm>

//사용할 라이브러리를 지정해줍니다.
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")



using namespace std;

//해당 물체의 위치
GLdouble objX, objY, objZ;


//큐브 위치
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//회전
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;

float ver[8][3] = {
	{ -0.5f,-0.5f,-0.5f },	//뒤왼아
	{ -0.5f,0.5f,-0.5f },	//뒤왼위
	{ 0.5f,0.5f,-0.5f },	//뒤오위
	{ 0.5f,-0.5f,-0.5f },	//뒤오아
	{ -0.5f,-0.5f,0.5f },	//앞왼아
	{ -0.5f,0.5f,0.5f },	//앞왼위
	{ 0.5f,0.5f,0.5f },	//앞오위
	{ 0.5f,-0.5f,0.5f }	//앞오아
};

float Tver[6][11][11][3];// 정육면체의 면, 면의 X, 면의 Y, 면 내의 (X,Y,Z)


float OptLine01[11];
float OptLine02[11];
float OptLine03[11];
float OptLine05[11];
float OptLine12[11];
float OptLine14[11];
float OptLine15[11];
float OptLine23[11];
float OptLine24[11];
float OptLine34[11];
float OptLine35[11];

//float current_angle = 0.0;

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
	glBegin(GL_POLYGON);
	glVertex3d(-0.5, -0.5, -0.5);
	glVertex3d(-0.5, 0.5, -0.5);
	glVertex3d(0.5, 0.5, -0.5);
	glVertex3d(0.5, -0.5, -0.5);
	glEnd();
}

//cubebase함수에서 그린 사각형을 회전 및 이동시켜
//큐브를 완성시킨다.
void draw_cube()
{
	//float ver[8][3] = {
	//	{ -0.5f,-0.5f,-0.5f },	//뒤왼아 0
	//	{ -0.5f,0.5f,-0.5f },	//뒤왼위     1
	//	{ 0.5f,0.5f,-0.5f },	//뒤오위     2
	//	{ 0.5f,-0.5f,-0.5f },	//뒤오아		3
	//	{ -0.5f,-0.5f,0.5f },	//앞왼아		4
	//	{ -0.5f,0.5f,0.5f },	//앞왼위		5
	//	{ 0.5f,0.5f,0.5f },	//앞오위		6
	//	{ 0.5f,-0.5f,0.5f }	//앞오아		7
	//};
	//왼 -> 오 , 아 -> 위 , 뒤 -> 앞
	glMatrixMode(GL_MODELVIEW);

	/*glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z축 방향

	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glEnd();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X축 방향
	glBegin(GL_QUADS);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X축 방향
	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y축 방향
	glBegin(GL_QUADS);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y축 방향
	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z축 방향
	glBegin(GL_QUADS);
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

void draw_line()
{
	glPushMatrix();

	glPushMatrix(); //X축 붉은색
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(5.0, 0.0, 0.0);
	glVertex3f(-5.0, 0.0, 0.0);
	glEnd();
	drawBitmapText("+X", 0.8, 0.0, 0.0);
	drawBitmapText("-X", -0.8, 0.0, 0.0);
	glPopMatrix();

	glPushMatrix(); //Y축 녹색
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 5.0, 0.0);
	glVertex3f(0.0, -5.0, 0.0);
	glEnd();
	drawBitmapText("+Y", 0.0, 0.8, 0.0);
	drawBitmapText("-Y", 0.0, -0.8, 0.0);
	glPopMatrix();

	glPushMatrix(); //Z축 파란색
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 5.0);
	glVertex3f(0.0, 0.0, -5.0);
	glEnd();
	drawBitmapText("+Z", 0.0, 0.0, 0.8);
	drawBitmapText("-Z", 0.0, 0.0, -0.8);
	glPopMatrix();

	glPopMatrix();

	glFlush();
}
//////////////////////////////////

bool IsCollison(float cx, float cy, float cr, const RECT* lprect)
{
	int nCenterX = static_cast<int>(cx);
	int nCenterY = static_cast<int>(cy);
	int nRadius = static_cast<int>(cr);
	if ((lprect->left <= nCenterX && nCenterX <= lprect->right) ||
		(lprect->top <= nCenterY && nCenterY <= lprect->bottom))
	{
		//반지름 만큼 확장한 원을 구한다.
		RECT rcEx = {
			lprect->left - nRadius,
			lprect->top - nRadius,
lprect->right - nRadius,
lprect->bottom - nRadius,
		};
		//확장한 사각형안에 원의 중심이 들어있는지 파악
		if ((rcEx.left < nCenterX && nCenterX < rcEx.right) &&
			(rcEx.top < nCenterY && nCenterY < rcEx.bottom)) {
			return TRUE;
		}
	}
}

bool MyIsCollision(LPRECT pIntetsect, const LPRECT pRect1, const LPRECT pRect2)
{
	bool bVertical = FALSE;	//수직 충돌 여부
	bool bHorizontal = FALSE; //수평 충돌 여부

	//수평 충돌
	if (pRect1->left < pRect2->right && pRect1->right > pRect1->left) {
		bHorizontal = TRUE;

		if (pIntetsect) {
			pIntetsect->left = (pRect1->left > pRect2->left) ? pRect1->left : pRect2->left;
			pIntetsect->right = (pRect1->right > pRect2->right) ? pRect1->right : pRect2->right;
		}
	}

	//수직 충돌
	if (pRect1->top < pRect2->bottom && pRect1->bottom > pRect2->top) {
		bVertical = TRUE;
		if (pIntetsect) {
			pIntetsect->top = max(pRect1->top, pRect2->top);
			pIntetsect->bottom = min(pRect1->bottom, pRect2->bottom);
		}
	}

	if (bVertical && bHorizontal)
		return TRUE;

	if (pIntetsect) {
		pIntetsect->left = 0;
		pIntetsect->right = 0;
		pIntetsect->top = 0;
		pIntetsect->bottom = 0;
	}

	return FALSE;

}

int SetCollisionVertex(LPRECT pRect, const LPRECT pHold)
{
	//교차 영역을 얻을 변수 선언
	RECT rcInter;

	//사각형이 출돌했다면 아래 if문이 실행되며 , rcInter의 값이 구해진다(충돌 영역)
	if (MyIsCollision(&rcInter, pRect, pHold))
	{
		int nInterW = rcInter.right - rcInter.left;
		int nInterH = rcInter.bottom - rcInter.top;

		//위아래 체크
		if (nInterW > nInterH) {

			//위에서 충돌 시
			if (rcInter.top == pHold->top)
			{
			}
			//아래서 충돌 시
			else if (rcInter.bottom == pHold->bottom)
			{
			}
		}
		//좌우 체크
		else {
			//좌에서 충돌 시
			if (rcInter.left == pHold->left)
			{
			}
			//우에서 충돌 시
			else if (rcInter.right == pHold->right)
			{
				return nInterW;
			}
		}
	}

}


void modiVert(float mouseX, float mouseY,float mouseZ, double cubeSize)
{
	if (objX == 0 && objY == 0 && objZ ==0)
		return;
	//Z축 픽스
	if (pitch == 0) {
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				for (int n = 0; n < 11; n++)
				{
					//오른쪽에서 충돌 시
					if (Tver[i][j][n][0] > (mouseX - cubeSize / 2))
					{
						if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
							Tver[i][j][n][1] >(mouseY - cubeSize / 2))
							Tver[i][j][n][0] -= cubeSize / 2;

					}
					/*//위에서 충돌 시
					else if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
						Tver[i][j][n][1] > mouseY) {
						Tver[i][j][n][1] -= cubeSize / 2;

					}
					//밑에서 충돌 시
					else if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
						Tver[i][j][n][1] < mouseY) {
						Tver[i][j][n][1] += cubeSize / 2;

					}*/
				}
			}
		}
	}
	//Y축 픽스
	if (pitch == 90) {
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				for (int n = 0; n < 11; n++)
				{
					//오른쪽에서 충돌 시
					if (Tver[i][j][n][0] >(mouseX - cubeSize / 2))
					{
						if (Tver[i][j][n][3] <(mouseZ + cubeSize / 2) &&
							Tver[i][j][n][3] >(mouseZ - cubeSize / 2))
							Tver[i][j][n][0] -= cubeSize / 2;

					}
					/*//위에서 충돌 시
					else if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
					Tver[i][j][n][1] > mouseY) {
					Tver[i][j][n][1] -= cubeSize / 2;

					}
					//밑에서 충돌 시
					else if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
					Tver[i][j][n][1] < mouseY) {
					Tver[i][j][n][1] += cubeSize / 2;

					}*/
				}
			}
		}
	}
}

///////////////////////////////////////////
void draw_circle(double radius)
{
	
	//RECT	verRect;
	//RECT mouseRect;

	//int rCol = 0;

	if (pitch == 0) {
		glPushMatrix();
		glColor3f(1.0f, 0, 0);
		glTranslated(objX, objY, 0);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();
		//mouseRect.bottom = objY- 0.05;
		//mouseRect.top = objY - 0.05;
		//mouseRect.left = objX - 0.05;
		//mouseRect.right = objX + 0.05;
		
	}
	else if (pitch == 90)
	{
		glPushMatrix();
		glColor3f(1.0f, 0, 0);
		glTranslated(objX, 0, objZ);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();
	}
	else if (pitch == 180)
	{
		glPushMatrix();
		glColor3f(1.0f, 0, 0);
		glTranslated(objX, objY, 0);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();
	}
	else if (pitch == 270)
	{
		glPushMatrix();
		glColor3f(1.0f, 0, 0);
		glTranslated(objX, objY, 0);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();
	}


	//각 vertex 마다 원을 그려줌
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			for (int n = 0; n < 11; n++)
			{
				glPushMatrix();
				glColor3f(0.9f, 0.9f, 0.9f);
				glTranslated(Tver[i][j][n][0], Tver[i][j][n][1], Tver[i][j][n][2]);
				glutSolidSphere(radius, 10, 10);
				glPopMatrix();
				
				//verRect.bottom	=	(LONG)Tver[i][j][n][1] + 0.05;
				//verRect.top		=	(LONG)Tver[i][j][n][1] - 0.05;
				//verRect.left		=	(LONG)Tver[i][j][n][0] - 0.05;
				//verRect.right		=	(LONG)Tver[i][j][n][0] + 0.05;
				//rCol=SetCollisionVertex(&verRect, &mouseRect);
				//Tver[i][j][n][0] += rCol;
			}
		}
	}

}

void display()
{
	//화면을 지운다. (컬러버퍼와 깊이버퍼)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//이후 연산은 ModelView Matirx에 영향을 준다. 객체 조작
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 이동과 회전을 적용
	glTranslatef(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x축에 대해 회전
	glRotatef(yaw, 0.0, 1.0, 0.0); //y축에 대해 회전
	glRotatef(roll, 0.0, 0.0, 1.0); //z축에 대해 회전

									//큐브를 그림
	draw_cube();

	draw_circle(0.02);

	//좌표축을 그림
	draw_line();
	modiVert(objX, objY,objZ, 0.05);
	glutSwapBuffers();
}


void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정 

	glMatrixMode(GL_PROJECTION); //이후 연산은 Projection Matrix에 영향을 준다. 카메라로 보이는 장면 같은거 설정 
	glLoadIdentity();

	//Field of view angle(단위 degrees), 윈도우의 aspect ratio, Near와 Far Plane설정
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //이후 연산은 ModelView Matirx에 영향을 준다. 객체 조작
}


void timer(int value) {
	//current_angle += 0.5;
	//if (current_angle > 360) current_angle -= 360;



	glutPostRedisplay();      //윈도우를 다시 그리도록 요청
	glutTimerFunc(30, timer, 0); //다음 타이머 이벤트는 30밀리세컨트 후  호출됨.
}



void init()
{
	/* Set clear color */
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0);

	/* Enable the depth buffer */
	glEnable(GL_DEPTH_TEST);
}

void special(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		pitch += 1.0;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		pitch -= 1.0;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		yaw += 1.0;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		yaw -= 1.0;
	}
	cout << yaw << "," << pitch << endl;
}

void keyboard(unsigned char key, int x, int y)
{
	//cout << "다음 키가 눌러졌습니다. \"" << key << "\" ASCII: " << (int)key << endl;
	POINT point;
	//ESC 키가 눌러졌다면 프로그램 종료
	if (key == 27)
	{
		exit(0);
	}
	if (key == 119) //w
	{
		GetCursorPos(&point);	//현재 마우스 좌표를 구한다
		SetCursorPos(point.x, point.y - 10);	//지정한 좌표로 마우스를 이동시킨다.
	}
	if (key == 97) //a
	{
		GetCursorPos(&point);	//현재 마우스 좌표를 구한다
		SetCursorPos(point.x - 10, point.y);	//지정한 좌표로 마우스를 이동시킨다.
	}
	if (key == 115) //s
	{
		GetCursorPos(&point);	//현재 마우스 좌표를 구한다
		SetCursorPos(point.x, point.y + 10);	//지정한 좌표로 마우스를 이동시킨다.cubeY -= 0.1;
	}
	if (key == 100) //d
	{
		GetCursorPos(&point);	//현재 마우스 좌표를 구한다
		SetCursorPos(point.x + 10, point.y);	//지정한 좌표로 마우스를 이동시킨다.
	}

	cout << point.x <<" , "<< point.y << endl;
}


void MoveVertex(GLdouble winX, GLdouble winY, GLdouble winZ)
{
	double tempdist = 0.1;
	double dist;
	int index = 100;
	//(x2-x1)의제곱+(y2-y1)의제곱+(z2-z1)의 제곱에 루트 값
	for (int i = 0; i < 8; i++) {
		dist = sqrt(pow((ver[i][0] - winX), 2) +
			pow((ver[i][1] - winY), 2) +
			pow((ver[i][2] - winZ), 2));

		if (dist < tempdist) {
			tempdist = dist;
			index = i;
		}
	}

	if (index != 100) {
		ver[index][0] = winX;
		ver[index][1] = winY;
		//ver[index][2] = winZ;

	}

	//X n Z
	/*if (index != 100) {
		if ((pitch == 90) && yaw == 0)
		{
			ver[2][0] = winX;
			ver[2][2] = winZ;

			ver[3][0] = winX;
			ver[3][2] = winZ;

			ver[6][0] = winX;
			ver[6][2] = winZ;

			ver[7][0] = winX;
			ver[7][2] = winZ;
		}
		//X n Y
		if ((pitch == 180) && yaw == 0)
		{
			ver[2][0] = winX;
			ver[2][1] = winY;

			ver[3][0] = winX;
			ver[3][1] = winY;

			ver[6][0] = winX;
			ver[6][1] = winY;

			ver[7][0] = winX;
			ver[7][1] = winY;
		}
		// X n Z
		if ((pitch == 270) && yaw == 0)
		{
			ver[2][0] = winX;
			ver[2][2] = winZ;

			ver[3][0] = winX;
			ver[3][2] = winZ;

			ver[6][0] = winX;
			ver[6][2] = winZ;

			ver[7][0] = winX;
			ver[7][2] = winZ;
		}
		//X n Y
		if ((pitch == 0) && yaw == 0)
		{
			ver[2][0] = winX;
			ver[2][1] = winY;

			ver[3][0] = winX;
			ver[3][1] = winY;

			ver[6][0] = winX;
			ver[6][1] = winY;

			ver[7][0] = winX;
			ver[7][1] = winY;
		}
	}*/
}

void MyMouseClick(GLint button, GLint state, GLint x, GLint y)
{
	//glReadPixels(x,y,)
	GLfloat winX = (float)x;
	GLfloat winY = 500-(float)y;		//500 is WINSIZE.height
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

	
	
	MoveVertex(objX, objY, objZ);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);  //GLUT 초기화

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //더블 버퍼와 깊이 버퍼를 사용하도록 설정, GLUT_RGB=0x00임
	glutInitWindowSize(500, 500);   //윈도우의 width와 height
	glutInitWindowPosition(100, 100); //윈도우의 위치 (x,y)
	glutCreateWindow("OpenGL Example"); //윈도우 생성


	init();

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

	//디스플레이 콜백 함수 등록, display함수는 윈도우 처음 생성할 때와 화면 다시 그릴 필요 있을때 호출된다. 
	glutDisplayFunc(display);

	//키보드 콜백 함수 등록, 키보드가 눌러지면 호출된다. 
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//마우스 콜백 함수 등록, 마우스가 눌려지면 호출된다.
	glutMouseFunc(MyMouseClick);
	
	
	//마우스가 움직이면 호출된다.
	//glutMotionFunc(MyMouseMove);
	//void MyMouseMove(GLint x, GLint y)



	//reshape 콜백 함수 등록, reshape함수는 윈도우 처음 생성할 때와 윈도우 크기 변경시 호출된다.
	glutReshapeFunc(reshape);
	//타이머 콜백 함수 등록, 처음에는 바로 호출됨.
	glutTimerFunc(0, timer, 0);


	//GLUT event processing loop에 진입한다.
	//이 함수는 리턴되지 않기 때문에 다음줄에 있는 코드가 실행되지 않는다. 
	glutMainLoop();

	return 0;
}