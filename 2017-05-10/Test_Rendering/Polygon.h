#pragma once
#include <cmath>
#include <vector>

#include <stdlib.h>
#include "include\GL\glut.h"

namespace Shapes {

	class Point {
	public:
		Point() {
			X = 0;
			Y = 0;
		}
		Point(float x, float y) {
			X = x;
			Y = y;
		}
		Point(float  ar[2]) {
			X = ar[0];
			Y = ar[1];
		}

		float X;
		float Y;
	};

	class Polygon
	{
	public:
		struct  Color {
			GLbyte red;
			GLbyte green;
			GLbyte blue;
		} color;

		Polygon(void);

		~Polygon(void);
		std::vector<Point*> corners;
		void finishPolygon();


		void addVertex(Point & point);
		void addVertex(float x, float y);
		void setColor(GLbyte r, GLbyte g, GLbyte b);
		void Render();
		inline int getSides();

	private:
		bool _finished;
		Point maxPoint;
		Point minPoint;
		int _sides;
		void calculateMinMaxPoints();
		void drawLine(float x1, float y1, float x2, float y2);
		bool pointInPolygon(Point * point);
		void draw();
		void fill();
		inline void drawPixel(float x, float y);

	};
};

