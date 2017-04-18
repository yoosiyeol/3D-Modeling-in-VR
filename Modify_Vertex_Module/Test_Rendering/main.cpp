//����η� ��������� �����մϴ�.
#include ".\include\GL\freeglut.h"
#include <iostream>

//min max �Լ�
#include <algorithm>

//����� ���̺귯���� �������ݴϴ�.
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")



using namespace std;

//�ش� ��ü�� ��ġ
GLdouble objX, objY, objZ;


//ť�� ��ġ
float cubeX = 0.0;
float cubeY = 0.0;
float cubeZ = -4.0;

//ȸ��
float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;

float ver[8][3] = {
	{ -0.5f,-0.5f,-0.5f },	//�ڿ޾�
	{ -0.5f,0.5f,-0.5f },	//�ڿ���
	{ 0.5f,0.5f,-0.5f },	//�ڿ���
	{ 0.5f,-0.5f,-0.5f },	//�ڿ���
	{ -0.5f,-0.5f,0.5f },	//�տ޾�
	{ -0.5f,0.5f,0.5f },	//�տ���
	{ 0.5f,0.5f,0.5f },	//�տ���
	{ 0.5f,-0.5f,0.5f }	//�տ���
};

float Tver[6][11][11][3];// ������ü�� ��, ���� X, ���� Y, �� ���� (X,Y,Z)


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
	glBegin(GL_POLYGON);
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
	//float ver[8][3] = {
	//	{ -0.5f,-0.5f,-0.5f },	//�ڿ޾� 0
	//	{ -0.5f,0.5f,-0.5f },	//�ڿ���     1
	//	{ 0.5f,0.5f,-0.5f },	//�ڿ���     2
	//	{ 0.5f,-0.5f,-0.5f },	//�ڿ���		3
	//	{ -0.5f,-0.5f,0.5f },	//�տ޾�		4
	//	{ -0.5f,0.5f,0.5f },	//�տ���		5
	//	{ 0.5f,0.5f,0.5f },	//�տ���		6
	//	{ 0.5f,-0.5f,0.5f }	//�տ���		7
	//};
	//�� -> �� , �� -> �� , �� -> ��
	glMatrixMode(GL_MODELVIEW);

	/*glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);     // Green, -Z�� ����

	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glEnd();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue, +X�� ����
	glBegin(GL_QUADS);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange, -X�� ����
	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);     // Red, +Y�� ����
	glBegin(GL_QUADS);
	glVertex3d(ver[1][0], ver[1][1], ver[1][2]);
	glVertex3d(ver[2][0], ver[2][1], ver[2][2]);
	glVertex3d(ver[6][0], ver[6][1], ver[6][2]);
	glVertex3d(ver[5][0], ver[5][1], ver[5][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow, -Y�� ����
	glBegin(GL_QUADS);
	glVertex3d(ver[0][0], ver[0][1], ver[0][2]);
	glVertex3d(ver[3][0], ver[3][1], ver[3][2]);
	glVertex3d(ver[7][0], ver[7][1], ver[7][2]);
	glVertex3d(ver[4][0], ver[4][1], ver[4][2]);
	glEnd();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta, +Z�� ����
	glBegin(GL_QUADS);
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

void draw_line()
{
	glPushMatrix();

	glPushMatrix(); //X�� ������
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(5.0, 0.0, 0.0);
	glVertex3f(-5.0, 0.0, 0.0);
	glEnd();
	drawBitmapText("+X", 0.8, 0.0, 0.0);
	drawBitmapText("-X", -0.8, 0.0, 0.0);
	glPopMatrix();

	glPushMatrix(); //Y�� ���
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 5.0, 0.0);
	glVertex3f(0.0, -5.0, 0.0);
	glEnd();
	drawBitmapText("+Y", 0.0, 0.8, 0.0);
	drawBitmapText("-Y", 0.0, -0.8, 0.0);
	glPopMatrix();

	glPushMatrix(); //Z�� �Ķ���
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
		//������ ��ŭ Ȯ���� ���� ���Ѵ�.
		RECT rcEx = {
			lprect->left - nRadius,
			lprect->top - nRadius,
lprect->right - nRadius,
lprect->bottom - nRadius,
		};
		//Ȯ���� �簢���ȿ� ���� �߽��� ����ִ��� �ľ�
		if ((rcEx.left < nCenterX && nCenterX < rcEx.right) &&
			(rcEx.top < nCenterY && nCenterY < rcEx.bottom)) {
			return TRUE;
		}
	}
}

bool MyIsCollision(LPRECT pIntetsect, const LPRECT pRect1, const LPRECT pRect2)
{
	bool bVertical = FALSE;	//���� �浹 ����
	bool bHorizontal = FALSE; //���� �浹 ����

	//���� �浹
	if (pRect1->left < pRect2->right && pRect1->right > pRect1->left) {
		bHorizontal = TRUE;

		if (pIntetsect) {
			pIntetsect->left = (pRect1->left > pRect2->left) ? pRect1->left : pRect2->left;
			pIntetsect->right = (pRect1->right > pRect2->right) ? pRect1->right : pRect2->right;
		}
	}

	//���� �浹
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
	//���� ������ ���� ���� ����
	RECT rcInter;

	//�簢���� �⵹�ߴٸ� �Ʒ� if���� ����Ǹ� , rcInter�� ���� ��������(�浹 ����)
	if (MyIsCollision(&rcInter, pRect, pHold))
	{
		int nInterW = rcInter.right - rcInter.left;
		int nInterH = rcInter.bottom - rcInter.top;

		//���Ʒ� üũ
		if (nInterW > nInterH) {

			//������ �浹 ��
			if (rcInter.top == pHold->top)
			{
			}
			//�Ʒ��� �浹 ��
			else if (rcInter.bottom == pHold->bottom)
			{
			}
		}
		//�¿� üũ
		else {
			//�¿��� �浹 ��
			if (rcInter.left == pHold->left)
			{
			}
			//�쿡�� �浹 ��
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
	//Z�� �Ƚ�
	if (pitch == 0) {
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				for (int n = 0; n < 11; n++)
				{
					//�����ʿ��� �浹 ��
					if (Tver[i][j][n][0] > (mouseX - cubeSize / 2))
					{
						if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
							Tver[i][j][n][1] >(mouseY - cubeSize / 2))
							Tver[i][j][n][0] -= cubeSize / 2;

					}
					/*//������ �浹 ��
					else if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
						Tver[i][j][n][1] > mouseY) {
						Tver[i][j][n][1] -= cubeSize / 2;

					}
					//�ؿ��� �浹 ��
					else if (Tver[i][j][n][1] >(mouseY - cubeSize / 2) &&
						Tver[i][j][n][1] < mouseY) {
						Tver[i][j][n][1] += cubeSize / 2;

					}*/
				}
			}
		}
	}
	//Y�� �Ƚ�
	if (pitch == 90) {
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				for (int n = 0; n < 11; n++)
				{
					//�����ʿ��� �浹 ��
					if (Tver[i][j][n][0] >(mouseX - cubeSize / 2))
					{
						if (Tver[i][j][n][3] <(mouseZ + cubeSize / 2) &&
							Tver[i][j][n][3] >(mouseZ - cubeSize / 2))
							Tver[i][j][n][0] -= cubeSize / 2;

					}
					/*//������ �浹 ��
					else if (Tver[i][j][n][1] <(mouseY + cubeSize / 2) &&
					Tver[i][j][n][1] > mouseY) {
					Tver[i][j][n][1] -= cubeSize / 2;

					}
					//�ؿ��� �浹 ��
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


	//�� vertex ���� ���� �׷���
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
	//ȭ���� �����. (�÷����ۿ� ���̹���)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//���� ������ ModelView Matirx�� ������ �ش�. ��ü ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// �̵��� ȸ���� ����
	glTranslatef(cubeX, cubeY, cubeZ);
	glRotatef(pitch, 1.0, 0.0, 0.0); //x�࿡ ���� ȸ��
	glRotatef(yaw, 0.0, 1.0, 0.0); //y�࿡ ���� ȸ��
	glRotatef(roll, 0.0, 0.0, 1.0); //z�࿡ ���� ȸ��

									//ť�긦 �׸�
	draw_cube();

	draw_circle(0.02);

	//��ǥ���� �׸�
	draw_line();
	modiVert(objX, objY,objZ, 0.05);
	glutSwapBuffers();
}


void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //������ ũ��� ������Ʈ ���� 

	glMatrixMode(GL_PROJECTION); //���� ������ Projection Matrix�� ������ �ش�. ī�޶�� ���̴� ��� ������ ���� 
	glLoadIdentity();

	//Field of view angle(���� degrees), �������� aspect ratio, Near�� Far Plane����
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //���� ������ ModelView Matirx�� ������ �ش�. ��ü ����
}


void timer(int value) {
	//current_angle += 0.5;
	//if (current_angle > 360) current_angle -= 360;



	glutPostRedisplay();      //�����츦 �ٽ� �׸����� ��û
	glutTimerFunc(30, timer, 0); //���� Ÿ�̸� �̺�Ʈ�� 30�и�����Ʈ ��  ȣ���.
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
	//cout << "���� Ű�� ���������ϴ�. \"" << key << "\" ASCII: " << (int)key << endl;
	POINT point;
	//ESC Ű�� �������ٸ� ���α׷� ����
	if (key == 27)
	{
		exit(0);
	}
	if (key == 119) //w
	{
		GetCursorPos(&point);	//���� ���콺 ��ǥ�� ���Ѵ�
		SetCursorPos(point.x, point.y - 10);	//������ ��ǥ�� ���콺�� �̵���Ų��.
	}
	if (key == 97) //a
	{
		GetCursorPos(&point);	//���� ���콺 ��ǥ�� ���Ѵ�
		SetCursorPos(point.x - 10, point.y);	//������ ��ǥ�� ���콺�� �̵���Ų��.
	}
	if (key == 115) //s
	{
		GetCursorPos(&point);	//���� ���콺 ��ǥ�� ���Ѵ�
		SetCursorPos(point.x, point.y + 10);	//������ ��ǥ�� ���콺�� �̵���Ų��.cubeY -= 0.1;
	}
	if (key == 100) //d
	{
		GetCursorPos(&point);	//���� ���콺 ��ǥ�� ���Ѵ�
		SetCursorPos(point.x + 10, point.y);	//������ ��ǥ�� ���콺�� �̵���Ų��.
	}

	cout << point.x <<" , "<< point.y << endl;
}


void MoveVertex(GLdouble winX, GLdouble winY, GLdouble winZ)
{
	double tempdist = 0.1;
	double dist;
	int index = 100;
	//(x2-x1)������+(y2-y1)������+(z2-z1)�� ������ ��Ʈ ��
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
		printf("����\n");
	}
	
	std::cout << "X: " << objX << "  Y: " << objY << "  Z: " << objZ << std::endl;

	
	
	MoveVertex(objX, objY, objZ);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);  //GLUT �ʱ�ȭ

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //���� ���ۿ� ���� ���۸� ����ϵ��� ����, GLUT_RGB=0x00��
	glutInitWindowSize(500, 500);   //�������� width�� height
	glutInitWindowPosition(100, 100); //�������� ��ġ (x,y)
	glutCreateWindow("OpenGL Example"); //������ ����


	init();

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

	//���÷��� �ݹ� �Լ� ���, display�Լ��� ������ ó�� ������ ���� ȭ�� �ٽ� �׸� �ʿ� ������ ȣ��ȴ�. 
	glutDisplayFunc(display);

	//Ű���� �ݹ� �Լ� ���, Ű���尡 �������� ȣ��ȴ�. 
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//���콺 �ݹ� �Լ� ���, ���콺�� �������� ȣ��ȴ�.
	glutMouseFunc(MyMouseClick);
	
	
	//���콺�� �����̸� ȣ��ȴ�.
	//glutMotionFunc(MyMouseMove);
	//void MyMouseMove(GLint x, GLint y)



	//reshape �ݹ� �Լ� ���, reshape�Լ��� ������ ó�� ������ ���� ������ ũ�� ����� ȣ��ȴ�.
	glutReshapeFunc(reshape);
	//Ÿ�̸� �ݹ� �Լ� ���, ó������ �ٷ� ȣ���.
	glutTimerFunc(0, timer, 0);


	//GLUT event processing loop�� �����Ѵ�.
	//�� �Լ��� ���ϵ��� �ʱ� ������ �����ٿ� �ִ� �ڵ尡 ������� �ʴ´�. 
	glutMainLoop();

	return 0;
}