#pragma once
#include <pxcsensemanager.h>

#include <pxchandconfiguration.h>

#include <pxcblobconfiguration.h>
#include <pxcblobdata.h>
#include <pxcblobmodule.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "math.h"

#define NUM_FINGERS 4
#define NUM_DEFECTS 8
#define THRESH 250
#define maxBlobToShow 2
#define PI 3.1415926535

#define DepthSize cv::Size(320,240)
#define ColorSize cv::Size(640,480)

#define MOVELOCATION cv::Point(0, 0)
#define ROTATELOCATION cv::Point(0, 120)
#define ZOOMLOCATION cv::Point(0, 240)
#define MODIFYLOCATION cv::Point(300,360)

enum ClikedIcon {
	MOVE = 1,
	ROTATE ,
	ZOOM 
};

class mCamera
{
private:

	PXCSession *session;
	PXCSenseManager *sm;			//SenseManager
	PXCBlobModule *blobModule;
	PXCBlobData* blobData;
	PXCBlobConfiguration* blobConfig;

	float frameRate;

	//Blob Module Preset
	void initializeBlob();
	
	//PXCImage(for realsense) Convert To Mat(opencv) 
	cv::Mat PXCImage2CVMat(PXCImage *pxcImage, PXCImage::PixelFormat format);
	
	//Find Just Blob
	void updateBlobImage(PXCImage* depthFrame, cv::Mat *input);
	
public:

	//Init Camera n Preset Blob
	void initialize();

	//Copy Color Image To Input
	void getColorImage(cv::Mat *Input);

	//Copy Blob Image To Input
	void getBlobImage(cv::Mat *Input);


	//Copy Depth Image To Input
	void getDepthImage(cv::Mat *Input);

	void release();
};

class mHand
{
private:

	cv::Mat BackGroundImage = cv::Mat::zeros(ColorSize, CV_8UC3);

	CvSeq		*LH_contourP;	// Hand contour 
	CvSeq		*RH_contourP;	// Hand contour 
	
	CvSeq		*RHHull;		// Hand convex hull
	CvSeq		*LHHull;		// Hand convex hull

	CvPoint		LHand_center;
	CvPoint		RHand_center;
	
	CvPoint		LHClickPoint;
	CvPoint		RHClickPoint;
	
	CvPoint LHTips[NUM_FINGERS];
	CvPoint RHTips[NUM_FINGERS];

	CvMemStorage	*RHHull_st=NULL;
	CvMemStorage	*LHHull_st=NULL;
	
	CvMemStorage	*RHDefects_st;
	CvMemStorage	*LHDefects_st;
	
	CvMemStorage  *LHContourSt;
	CvMemStorage  *RHContourSt;

	int		RHnum_fingers;
	int		LHnum_fingers;
	int		hand_radius;
	int windowW = 0 , windowH = 0;


	//Depth���� ���̴� Point�� ������ Color������� ����ϴ� Point ���� 
	//Input Point of Depth, Return Converted Point of Color
	CvPoint getColorPoint(CvPoint A);


	//�� �� ������ �Ÿ� ����
	//Distance between two points
	double getDistance(CvPoint A, CvPoint B);

public:

	//P2�� ���� ����
	//Return Angle(degree) of P2
	double __fastcall Meas_Angle(CvPoint P1, CvPoint P2, CvPoint P3);

	void initialize();

	void optHandPoint();

	void drawCircle(CvPoint *Tips,CvPoint HandCenter,cv::Mat *Color);

	void blob2Contour(const cv::Mat BlobImage, cv::Mat *OutContourImage);

	void run(cv::Mat *Color,cv::Mat *blob, int winW, int winH);

	CvPoint find_convex_hull(cv::Mat *Color, CvSeq *hull, CvSeq *contourP, CvMemStorage *hull_st, CvMemStorage *defects_st,CvPoint *tips, CvPoint *hand_center, int winW, int winH,int check);

	//Input�� Color �̹��� SumIamge���� Color�� �׷��� ���� ��ģ �̹����� ���� ��
	void CheckFingerNum(const cv::Mat Input, cv::Mat *OutSumImage,int num_fingers);

	int getRHFingerNum();

	int getLHFingerNum();

	cv::Point getLHFingerClickPoint();
	cv::Point getRHFingerClickPoint();

	cv::Point getLHandCenter();
	cv::Point getRHandCenter();
};

class mTools
{
private:
	cv::Mat imgModify=cv::imread("modify.jpg",1);
	cv::Mat imgMove= cv::imread("./icon/up.jpg",1);
	cv::Mat imgRotate= cv::imread("./icon/right.jpg",1);
	cv::Mat imgZoom= cv::imread("zoom.jpg",1);

	cv::Mat ModifyROI;
	cv::Mat ZoomROI;
	cv::Mat RotateROI;
	cv::Mat MoveROI;

	bool check_clicked = false;
	int ToolsNum = 0;

	void updateIcons(cv::Point clickpoint);

	bool checkClicked(int numofingers, int numoftools);

public:
	void initMappingToolsOnColor(cv::Mat *Input,cv::Point clickpoint,int numfingers);

	void isClicked(int num);
};