#include "mOpenCv.h"

bool mTools::checkClicked(int numofingers, int numoftools)
{
	if (numofingers == 2) {
		if (numoftools > 0)
			if (numoftools < 5)
				return true;
	}
	else if (numofingers == 5)
		return false;
		
}

void mTools::initMappingToolsOnColor(cv::Mat *Input,cv::Point clickpoint,int numfingers)
{
	//enum ClickedIcon Icon;
	int IconNum = 0;

	check_clicked = checkClicked(ToolsNum, numfingers);

	//updateIcons(clickpoint);

	MoveROI = (*Input)(cv::Rect(MOVELOCATION.x , MOVELOCATION.y, imgMove.cols, imgMove.rows));
	cv::addWeighted(MoveROI, 1.0, imgMove, 0.8, 0.0, MoveROI);

	RotateROI = (*Input)(cv::Rect(ROTATELOCATION.x, ROTATELOCATION.y, imgRotate.cols, imgRotate.rows));
	cv::addWeighted(RotateROI, 1.0, imgRotate, 0.8, 0.0, RotateROI);

	ZoomROI = (*Input)(cv::Rect(ZOOMLOCATION.x,ZOOMLOCATION.y, imgZoom.cols, imgZoom.rows));
	cv::addWeighted(ZoomROI, 1.0, imgZoom, 0.8,0.0, ZoomROI);

	//ModifyROI = (*Input)(cv::Rect(MODIFYLOCATION.x,MODIFYLOCATION.y,imgModify.cols, imgModify.rows));
	//cv::addWeighted(ModifyROI, 1.0, imgModify, 0.3, 0.0, ModifyROI);

	//std::cout << clickpoint.x << "    .   " << clickpoint.y << std::endl;

	/*if (ToolsNum == 1 && !check_clicked)
		imgZoom = cv::imread("zoom.jpg", 1);
	else if (ToolsNum == 2 && !check_clicked)
		imgRotate = cv::imread("rotate.jpg", 1);
	else if (ToolsNum == 3 && !check_clicked)
		imgMove = cv::imread("move.jpg", 1);
	else if (ToolsNum == 4 && !check_clicked)
		imgModify = cv::imread("modify.jpg", 1);*/

}

void mTools::isClicked(int num)
{
	switch (num)
	{
	case 0:
		imgZoom = cv::imread("./icon/rotate.jpg", 1);
		break;
	case 3:
		imgZoom = cv::imread("./icon/onrotate.jpg", 1);
	}
}

void mTools::updateIcons(cv::Point clickpoint)
{
	if ((ZOOMLOCATION.x < clickpoint.x)
		&& ((ZOOMLOCATION.x + 100) > clickpoint.x)) {
		if ((ZOOMLOCATION.y < clickpoint.y)
			&& ((ZOOMLOCATION.y + 100) > clickpoint.y)) {
			imgZoom = cv::imread("onzoom.jpg", 1);
			ToolsNum = 1;
		}
	}
	else if ((ROTATELOCATION.x < clickpoint.x)
		&& ((ROTATELOCATION.x + 100) > clickpoint.x)) {
		if ((ROTATELOCATION.y < clickpoint.y)
			&& ((ROTATELOCATION.y + 100) > clickpoint.y)) {
			imgRotate = cv::imread("onrotate.jpg", 1);
			ToolsNum = 2;
		}
	}
	else if ((MOVELOCATION.x < clickpoint.x)
		&& ((MOVELOCATION.x + 100) > clickpoint.x)) {
		if ((MOVELOCATION.y < clickpoint.y)
			&& ((MOVELOCATION.y + 100) > clickpoint.y)) {
			imgMove = cv::imread("onmove.jpg", 1);
			ToolsNum = 3;
		}
	}
	else if ((MODIFYLOCATION.x < clickpoint.x)
		&& ((MODIFYLOCATION.x + 100) > clickpoint.x)) {
		if ((MODIFYLOCATION.y < clickpoint.y)
			&& ((MODIFYLOCATION.y + 100) > clickpoint.y)) {
			imgModify = cv::imread("onmodify.jpg", 1);
			ToolsNum = 4;
		}
	}
}