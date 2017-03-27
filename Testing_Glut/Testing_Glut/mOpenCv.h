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

#define NUM_FINGERS 5
#define NUM_DEFECTS 8
#define THRESH 250
#define maxBlobToShow 1
#define PI 3.1415926535
#define DepthSize cv::Size(320,240)
#define ColorSize cv::Size(640,480)

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

	CvSeq		*contourP;	// Hand contour 
	CvSeq		*hull;		// Hand convex hull 

	CvPoint		hand_center;
	CvPoint		ClickPoint;
	
	CvMemStorage	*hull_st;
	CvMemStorage	*defects_st;
	CvMemStorage  *ContourSt;

	int		num_fingers;
	int		hand_radius;


	//Depth���� ���̴� Point�� ������ Color������� ����ϴ� Point ���� 
	//Input Point of Depth, Return Converted Point of Color
	CvPoint getColorPoint(CvPoint A);

	//P2�� ���� ����
	//Return Angle(degree) of P2
	double __fastcall Meas_Angle(CvPoint P1, CvPoint P2, CvPoint P3);

	//�� �� ������ �Ÿ� ����
	//Distance between two points
	double getDistance(CvPoint A, CvPoint B);

public:

	void initialize();

	void blob2Contour(const cv::Mat BlobImage, cv::Mat *OutContourImage);


	void find_convex_hull(cv::Mat *Color);

	//Input�� Color �̹��� SumIamge���� Color�� �׷��� ���� ��ģ �̹����� ���� ��
	void CheckFingerNum(const cv::Mat Input, cv::Mat *OutSumImage);

	int getFingerNum();

	cv::Point getFingerClickPoint();
};