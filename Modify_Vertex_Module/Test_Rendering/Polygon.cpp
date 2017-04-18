#include "Polygon.h"
using namespace Shapes;


Polygon::Polygon(void)
{
	_finished = false;
}
int Polygon::getSides() {
	return _sides = corners.size();
}
void Polygon::calculateMinMaxPoints() {

	minPoint = *corners[0];
	maxPoint = *corners[getSides() - 1];
	for (int i = 0; i<_sides; i++) {
		if (corners[i]->X > maxPoint.X)
			maxPoint.X = corners[i]->X;
		else if (corners[i]->X < minPoint.X)
			minPoint.X = corners[i]->X;

		if (corners[i]->Y > maxPoint.Y)
			maxPoint.Y = corners[i]->Y;
		else if (corners[i]->Y < minPoint.Y)
			minPoint.Y = corners[i]->Y;
	}
}
Polygon::~Polygon(void)
{

}
void Polygon::addVertex(Point & point) {
	corners.push_back(&point);
	calculateMinMaxPoints();
}
void Polygon::addVertex(float x, float y) {
	Point *point = new Point(x, y);
	addVertex(*point);
}
void Polygon::Render() {
	glColor3ub(color.red, color.green, color.blue);
	draw();
	if (_finished)
		fill();
}
void Polygon::finishPolygon() {
	_finished = true;
}
void Polygon::setColor(GLbyte r, GLbyte g, GLbyte b) {
	color.red = r;
	color.green = g;
	color.blue = b;
}
void Polygon::draw() {
	for (int i = 0; i<_sides; i++)
	{
		drawLine(corners[i]->X,
			corners[i]->Y,
			(i<_sides - 1) ? corners[i + 1]->X : corners[i]->X,
			(i<_sides - 1) ? corners[i + 1]->Y : corners[i]->Y);
	}
}
void Polygon::fill() {
	pointInPolygon(new Point(20, 20));
	for (int x = minPoint.X; x <= maxPoint.X; x++)
		for (int y = minPoint.Y; y <= maxPoint.Y; y++)
			if (pointInPolygon(new Point(x, y)))
				drawPixel(x, y);
}
void Polygon::drawPixel(float x, float y) {
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}
void Polygon::drawLine(float x1, float y1, float x2, float y2)
{
	float m = fabs((y2 - y1) / (x2 - x1));
	if (0 < m < 1) {	//Bresenham

		float dx = fabs(x2 - x1), dy = fabs(y2 - y1);

		float p = 2 * dy - dx;
		float x = 0, y = 0;


		if (x1 > x2) {
			x = x2;
			y = y2;
			x2 = x1;
		}
		else {
			x = x1;
			y = y1;
		}
		glBegin(GL_POINTS);
		glVertex2d(x, y);
		glEnd();

		while (x < x2) {
			x++;
			if (p<0) {
				p += 2 * dy;
			}
			else {
				y++;
				p += 2 * (dy - dx);
			}
			glBegin(GL_POINTS);
			glVertex2d(x, y);
			glEnd();
		}
	}
	else //DDA
	{


		float dx = x2 - x1, dy = y2 - y1;
		int steps = 0, k = 0;
		float xIncrement, yIncrement, x = x1, y = y1;


		if (fabs(dx) > fabs(dy))
			steps = fabs(dx);
		else
			steps = fabs(dy);

		xIncrement = float(dx) / float(steps);
		yIncrement = float(dy) / float(steps);

		glBegin(GL_POINTS);

		glVertex2d(floor(x), floor(y));
		glEnd();
		for (k = 0; k<steps; k++) {
			x += xIncrement;
			y += yIncrement;
			glBegin(GL_POINTS);
			glVertex2d(floor(x), floor(y));
			glEnd();
		}
	}

}
bool Polygon::pointInPolygon(Point * p) {
	bool  oddNodes = false;

	for (int start = 0, end = _sides - 1; start< _sides; start++) {

		Point Ps(corners[start]->X, corners[start]->Y);
		Point Pe(corners[end]->X, corners[end]->Y);


		bool isYBetweenLineCW = Pe.Y <  p->Y && Ps.Y >= p->Y;
		bool isYBetweenLineCCW = Ps.Y < p->Y &&  Pe.Y >= p->Y;

		float Pi = Ps.X + (((p->Y - Ps.Y) / (Pe.Y - Ps.Y)) * (Pe.X - Ps.X));
		if (isYBetweenLineCW || isYBetweenLineCCW) {


			if (p->X < Pi) {



				oddNodes = !oddNodes;
			}

		}
		end = start;
	}

	delete p;

	return oddNodes;
}