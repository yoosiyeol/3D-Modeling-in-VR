#include "mOpenCv.h"

//Depth에서 쓰이는 Point를 넣으면 Color사이즈에서 사용하는 Point 리턴 
//Input Point of Depth, Return Converted Point of Color
CvPoint mHand::getColorPoint(CvPoint A)
{
	CvPoint ColorPoint;

	ColorPoint.x = (windowW / DepthSize.width)*A.x ;
	ColorPoint.y = (windowH / DepthSize.height)*A.y;


	return ColorPoint;
}

//P2의 각도 리턴
//Return Angle(degree) of P2
double __fastcall mHand::Meas_Angle(CvPoint P1, CvPoint P2, CvPoint P3)
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

//두 점 사이의 거리 리턴
//Distance between two points
double mHand::getDistance(CvPoint A, CvPoint B)
{
	double distance = 0;

	distance = sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));

	return distance;
}

//Init mHand Module
void mHand::initialize()
{
	LHContourSt = cvCreateMemStorage(0);
	RHContourSt = cvCreateMemStorage(0);
	
	RHHull_st = cvCreateMemStorage(0);
	LHHull_st = cvCreateMemStorage(0);

	LHDefects_st = cvCreateMemStorage(0);
	RHDefects_st = cvCreateMemStorage(0);
}

//BlobImage To Contour OutContourImage
void mHand::blob2Contour(const cv::Mat BlobImage, cv::Mat *OutContourImage)
{

	cv::Mat TempBlobImage;

	// Copy & Mat 2 IplImage
	BlobImage.copyTo(TempBlobImage);
	//iplBlob = &IplImage(TempBlobImage);

	//-----------------------------------Find Contours----------------//
	if (LHContourSt == NULL) {
		LHContourSt = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(LHContourSt);
	}
	if (RHContourSt == NULL) {
		RHContourSt = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(RHContourSt);
	}

	//cvDilate(iplBlob, iplBlob); // 팽창
	//-----------------------ROI Left Right Hand --------------------//
	cv::Mat leftHandBlob = TempBlobImage(cv::Rect(0, 0, (DepthSize.width/2) - 50, DepthSize.height));
	cv::Mat rightHandBlob = TempBlobImage(cv::Rect((DepthSize.width/2)-50, 0, (DepthSize.width/2)+50, DepthSize.height));
	IplImage *iplLHBlob=&IplImage(leftHandBlob);
	IplImage *iplRHBlob=&IplImage(rightHandBlob);
	//cvDilate(iplRHBlob, iplRHBlob); // 팽창
	//cvDilate(iplLHBlob, iplLHBlob); // 팽창
	//로고의 x,y 시작좌표지정,col,row 끝점지정
	//cv::imshow("lefthand", leftHandBlob);
	//cv::imshow("righthand", rightHandBlob);
	cvShowImage("aa", iplLHBlob);
	cvShowImage("bb", iplRHBlob);

	//왼손 윤곽선 찾기
	cvFindContours(
		iplLHBlob,							// Input Picture
		LHContourSt,             //Storage for Save Contours
		&LH_contourP,             //외곽선의 좌표들이 저장될 Sequence
		sizeof(CvContour),
		CV_RETR_TREE,           //어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0, 0)
	);
	//오른손 윤곽선 찾기
	cvFindContours(
		iplRHBlob,							// Input Picture
		RHContourSt,             //Storage for Save Contours
		&RH_contourP,             //외곽선의 좌표들이 저장될 Sequence
		sizeof(CvContour),
		CV_RETR_TREE,           //어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0, 0)
	);

	// Clean iplBlob
	/*cvZero(iplLHBlob);
	if (LH_contourP) {
		//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
		cvDrawContours(
			iplLHBlob,               //외곽선이 그려질 영상
			LH_contourP,              //외곽선 트리의 루트노드
			cvScalarAll(255),      //외부 외곽선의 색상
			cvScalarAll(150),      //내부 외곽선의 색상
			100                    //외곽선을 그릴때 이동할 깊이
		);

		cv::cvarrToMat(iplLHBlob).copyTo(*OutContourImage);

		//ContourImage = cv::cvarrToMat(iplBlob);
	}
	cvZero(iplRHBlob);
	if (RH_contourP) {
		//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
		cvDrawContours(
			iplRHBlob,               //외곽선이 그려질 영상
			RH_contourP,              //외곽선 트리의 루트노드
			cvScalarAll(255),      //외부 외곽선의 색상
			cvScalarAll(150),      //내부 외곽선의 색상
			100                    //외곽선을 그릴때 이동할 깊이
		);
	}*/
	//cvShowImage("aa", iplLHBlob);
	//cvShowImage("bb", iplRHBlob);

}

void mHand::run(cv::Mat *Color,cv::Mat *blob, int winW, int winH)
{
	windowW = winW;
	windowH = winH;
	find_convex_hull(Color, RHHull, RH_contourP, LHHull_st, RHDefects_st, RHTips, &RHand_center, winW, winH, 1);
	find_convex_hull(Color, LHHull, LH_contourP, LHHull_st, LHDefects_st, LHTips, &LHand_center,winW,winH,0);
	
	optHandPoint();
	//drawCircle(LHTips, LHand_center, Color);
	//drawCircle(RHTips, RHand_center, Color);

}

void mHand::drawCircle(CvPoint *Tips,CvPoint HandCenter,cv::Mat *Color)
{
	for (int j = 0; j < 5; j++)
	{
		/*cv::circle(*blob, LHTips[j], 15, CV_RGB(255, 0, 0), 0, 8);
		cv::line(*blob, LHTips[j], hand_center, CV_RGB(255, 255, 255));
		cv::circle(*blob, RHTips[j], 15, CV_RGB(255, 0, 0), 0, 8);
		cv::line(*blob, RHTips[j], hand_center, CV_RGB(255, 255, 255));*/
		if (Tips[j].x != 0 && Tips[j].y != 0) {
			cv::circle(*Color, Tips[j], 15, CV_RGB(255, 0, 0), 0, 8);
			cv::line(*Color, Tips[j], HandCenter, CV_RGB(0, 255, 0));
		}
		Tips[j] = CvPoint(0, 0);
	}
}

void mHand::optHandPoint()
{
	for (int i = 0; i < 5; i++) {
		if (RHTips[i].x != 0 && RHTips[i].y != 0) {
			RHTips[i].x = ((windowW / DepthSize.width)*RHTips[i].x) + windowW / 2 + 60;
			RHTips[i].y = (windowH / DepthSize.height)*RHTips[i].y - 10;
		}
		if (LHTips[i].x != 0 && LHTips[i].y != 0) {
			LHTips[i].x = (windowW / DepthSize.width)*LHTips[i].x + 60;
			LHTips[i].y = (windowH / DepthSize.height)*LHTips[i].y - 10;
		}
		
	}
	LHand_center.x = ((windowW / DepthSize.width)*LHand_center.x) + 60;
	LHand_center.y = ((windowH / DepthSize.height)*LHand_center.y) - 10;
	RHand_center.x = ((windowW / DepthSize.width)*RHand_center.x) + (windowW / 2) + 60;
	RHand_center.y = ((windowH / DepthSize.height)*RHand_center.y) - 10;


}

CvPoint mHand::find_convex_hull(cv::Mat *Color, CvSeq *hull,CvSeq *contourP,CvMemStorage *hull_st,CvMemStorage *defects_st,CvPoint *tips,CvPoint *hand_center,int winW,int winH,int check)
{
	CvSeq *defects;
	CvConvexityDefect *defect_array;
	int i,j=0;
	double x = 0, y = 0;
	double dist = 0;
	double distance = 0;
	double degree = 0;
	CvPoint ColorPoint;
	CvPoint ColorHandCenter;
	CvPoint ColorClickPoint = cvPoint(640, 480);
	CvPoint ForComparePoint;
	CvPoint Temp;
	int FingerPosition = 0;

	hull = NULL;

	if (!contourP)
		return NULL;
	if (!hull_st)
		return NULL;
	// find the convexhull
	hull = cvConvexHull2(contourP, hull_st, CV_CLOCKWISE, 0);

	int counthull = hull->total;

	//Find the convexity defects
	defects = cvConvexityDefects(contourP, hull, defects_st);
	int countdefects = defects->total;

	defect_array = (CvConvexityDefect*)calloc(defects->total,
		sizeof(CvConvexityDefect));
	cvCvtSeqToArray(defects, defect_array, CV_WHOLE_SEQ);


	//Find Hand Center & radius
	for (i = 0; i < defects->total && i <defects->total; i++) {
		x += defect_array[i].depth_point->x;
		y += defect_array[i].depth_point->y;
	}

	x /= defects->total;
	y /= defects->total;

	*hand_center = cvPoint(x, y);

	// Compute hand radius as mean of distances of
	//defects' depth point to hand center 
	for (i = 0; i < defects->total; i++) {
		int d = (x - defect_array[i].depth_point->x) *
			(x - defect_array[i].depth_point->x) +
			(y - defect_array[i].depth_point->y) *
			(y - defect_array[i].depth_point->y);

		dist += sqrt(d);
	}

	hand_radius = dist / defects->total;

	// Count Fingers		
	for (int i = 0; i < countdefects; i++)
	{
		degree = Meas_Angle(*defect_array[i].start, *defect_array[i].depth_point, *defect_array[i].end);

		if (degree < 100)
		{
			//	if (defect_array[i].depth > 5) {
			//if (i < countdefects)
			distance = getDistance(*defect_array[i].start, *defect_array[i].end);
			if (distance > 5) {

				
				ColorHandCenter = getColorPoint(*hand_center);
				ColorPoint = getColorPoint(*defect_array[i].end);
				if (check == 1) {
					ColorHandCenter.x += windowW /2 - 40 ;
					ColorPoint.x += windowW / 2 - 40;
					ColorHandCenter.y -= 10;
					ColorPoint.y -= 10;
				}
				else
				{
					ColorHandCenter.x += 45;
					ColorPoint.x += 45;
					ColorHandCenter.y -=10;
					ColorPoint.y -= 10;
				}
				tips[j] = ColorPoint;
				if (0 <= j && j < NUM_FINGERS)
					j++;
				cv::circle(*Color, ColorPoint, 15, CV_RGB(255, 0, 0), 0, 8);
				cv::line(*Color, ColorPoint, ColorHandCenter, CV_RGB(0, 255, 0));
				//Catch Click Point On Longest Finger 
				/*if (getDistance(*defect_array[i].end, hand_center) > LongestDistance) {
				ColorClickPoint = getColorPoint(*defect_array[i].end);
				LongestDistance = getDistance(*defect_array[i].end, hand_center);
				}*/
				//Catch Click Point On Leftmost Finger

				//ForComparePoint = ColorPoint;
				if (ColorClickPoint.x > ColorPoint.x)
					ColorClickPoint = ColorPoint;

				FingerPosition++;
			}
		}
	}
	if (check == 1) {
		RHnum_fingers = FingerPosition;
		RHClickPoint = ColorClickPoint;
	}
	else {
		LHnum_fingers = FingerPosition;
		LHClickPoint = ColorClickPoint;
	}
	cv::circle(*Color, ColorClickPoint, 10, CV_RGB(0, 0, 255), 0, 8);
	free(defect_array);
	return ColorClickPoint;
	//CheckCircle();
}


//Input에 Color 이미지 SumIamge에는 Color와 그려진 것이 합친 이미지가 나갈 것
void mHand::CheckFingerNum(const cv::Mat Input, cv::Mat *OutSumImage,int num_fingers)
{
	cv::String mystr;
	if (num_fingers == 4) {
		mystr = "5 fingers";
		BackGroundImage = cv::Mat::zeros(ColorSize, CV_8UC3);
	}
	else if (num_fingers == 3)
		mystr = "4 fingers";
	else if (num_fingers == 2)
		mystr = "3 fingers";
	else if (num_fingers == 1)
		mystr = "2 fingers";
	else if (num_fingers == 0)
		mystr = "Doesn't exist";

	cv::putText(Input, mystr, CvPoint(20, 20), 2, 1.2, cvScalarAll(255));

	if (num_fingers == 2) {
		//cv::circle(Input, ClickPoint, 10, CV_RGB(255, 255, 255), 0, 8);
		//cv::circle(BackGroundImage, ClickPoint, 5, CV_RGB(255, 255, 255), 10);
	}
	cv::add(Input, BackGroundImage, *OutSumImage);
}

//손가락 개수 리턴
int mHand::getRHFingerNum() {
	int RH_fing = RHnum_fingers;
	RHnum_fingers = 0;
	return RH_fing;
}

int mHand::getLHFingerNum() {
	int LH_fing = LHnum_fingers;
	LHnum_fingers = 0;
	return LH_fing;
}

cv::Point mHand::getLHFingerClickPoint()
{
	CvPoint click = LHClickPoint;
	LHClickPoint = CvPoint(0, 0);
	return click;
}

cv::Point mHand::getRHFingerClickPoint()
{
	CvPoint click = RHClickPoint;
	RHClickPoint = CvPoint(0, 0);
	return click;
}

cv::Point mHand::getLHandCenter()
{
	return LHand_center;
}
cv::Point mHand::getRHandCenter()
{
	return RHand_center;
}