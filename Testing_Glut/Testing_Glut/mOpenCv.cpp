#include "mOpenCv.h"


//--------------------mCamera Section--------------------//

//Blob Module Preset
void mCamera::initializeBlob() {
	// Blob Module Data Setting
	blobModule = sm->QueryBlob();
	blobData = blobModule->CreateOutput();

	blobConfig = blobModule->CreateActiveConfiguration();
	blobConfig->SetBlobSmoothing(0.5);
	blobConfig->SetMaxBlobs(maxBlobToShow);
	blobConfig->SetMaxObjectDepth(100);
	blobConfig->EnableContourExtraction(true);
	blobConfig->EnableSegmentationImage(true);
	blobConfig->EnableStabilizer(true);


	//이게 최적인거 같음
	//Set Max Range of Blob Area
	blobConfig->SetMaxBlobArea(0.115f);
	blobConfig->ApplyChanges();

}

//PXCImage(for realsense) Convert To Mat(opencv) 
cv::Mat mCamera::PXCImage2CVMat(PXCImage *pxcImage, PXCImage::PixelFormat format)

{
	PXCImage::ImageData data;
	pxcImage->AcquireAccess(PXCImage::ACCESS_READ, format, &data);

	int width = pxcImage->QueryInfo().width;
	int height = pxcImage->QueryInfo().height;

	if (!format)
		format = pxcImage->QueryInfo().format;

	int type = 0;
	if (format == PXCImage::PIXEL_FORMAT_Y8)
		type = CV_8UC1;
	else if (format == PXCImage::PIXEL_FORMAT_RGB24)
		type = CV_8UC3;
	else if (format == PXCImage::PIXEL_FORMAT_DEPTH_F32)
		type = CV_32FC1;

	cv::Mat ocvImage = cv::Mat(cv::Size(width, height), type, data.planes[0]);

	pxcImage->ReleaseAccess(&data);
	return ocvImage;
}

//Find Only Blob
void mCamera::updateBlobImage(PXCImage* depthFrame, cv::Mat *input)

{
	if (depthFrame == nullptr) {
		return;
	}

	// Blob Data Update
	auto sts = blobData->Update();
	if (sts < PXC_STATUS_NO_ERROR) {
		return;
	}


	PXCImage::ImageInfo depthInfo = depthFrame->QueryInfo();

	auto session = sm->QuerySession();
	PXCImage* blobImage = session->CreateImage(&depthInfo);


	// Blob Count 
	int numOfBlobs = blobData->QueryNumberOfBlobs();
	for (int i = 0; i < numOfBlobs; ++i) {
		// Blob Near To Far
		PXCBlobData::IBlob* blob;
		PXCBlobData::AccessOrderType accessOrder = PXCBlobData::ACCESS_ORDER_NEAR_TO_FAR;

		// Blob is queried according to image segmentation type (color/depth) 
		PXCBlobData::SegmentationImageType segmentationType = PXCBlobData::SegmentationImageType::SEGMENTATION_IMAGE_DEPTH;
		blobData->QueryBlob(i, segmentationType, accessOrder, blob);
		if (sts < PXC_STATUS_NO_ERROR) {
			continue;
		}

		// Load Blob Segment 
		sts = blob->QuerySegmentationImage(blobImage);
		if (sts < PXC_STATUS_NO_ERROR) {
			continue;
		}

		// Read Blob
		PXCImage::ImageData data;
		pxcStatus sts = blobImage->AcquireAccess(PXCImage::Access::ACCESS_READ,
			PXCImage::PIXEL_FORMAT_Y16, &data);
		if (sts < PXC_STATUS_NO_ERROR) {
			continue;
		}


		PXCImage2CVMat(blobImage, PXCImage::PIXEL_FORMAT_ANY).copyTo(*input);
		cv::erode(*input, *input, cv::Mat());

		// Release Access Blob 
		blobImage->ReleaseAccess(&data);
	}

}

//Init Camera n Preset Blob
void mCamera::initialize()

{
	//Init Session
	session = PXCSession::CreateInstance();
	PXCSession::ImplDesc desc1 = {};
	desc1.group = PXCSession::IMPL_GROUP_SENSOR;
	desc1.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
	PXCCapture* c;
	session->CreateImpl<PXCCapture>(&desc1, &c);
	auto status = 0;

	//PXCCaptureManager Init
	PXCCapture::Device* device = c->CreateDevice(0);


	PXCRangeF32 depthRange;
	depthRange.min = 650.f;
	depthRange.max = 1000.f;
	status = device->SetDSMinMaxZ(depthRange); // fails with PXC_STATUS_DEVICE_FAILED(-201)
	if (status != PXC_STATUS_NO_ERROR) {
		std::cout << "Error In Setting Range of Depth Camera" << std::endl;
		return;
	}

	sm = session->CreateSenseManager();
	if (sm == 0) {
		throw std::runtime_error("SenseManager!!");
	}

	frameRate = 30;

	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, ColorSize.width, ColorSize.height, frameRate);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, DepthSize.width, DepthSize.height,
		frameRate);


	//About Blob
	status = sm->EnableBlob(0);
	if (status < PXC_STATUS_NO_ERROR) {
		//throw std::runtime_error("Blob 실패");
		std::cout << "Error : Blob Pipe Line" << std::endl;
		return;
	}

	status = sm->Init();
	if (status != PXC_STATUS_NO_ERROR) {
		//throw std::runtime_error("센스 매니저 init 실패");
		std::cout << "Error : SenseManager Init" << std::endl;
		return;
	}

	//Flip Video
	sm->QueryCaptureManager()->QueryDevice()->SetMirrorMode(
		PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);

	// Blob init
	initializeBlob();

}

//Copy Color Image To Input
void mCamera::getColorImage(cv::Mat *Input)

{
	if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

	PXCCapture::Sample *sample;
	sample = sm->QuerySample();
	if (sample->color)
		*Input = PXCImage2CVMat(sample->color, PXCImage::PIXEL_FORMAT_RGB24);
	sm->ReleaseFrame();
}

//Copy Blob Image To Input
void mCamera::getBlobImage(cv::Mat *Input) {
	if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

	PXCCapture::Sample *sample;
	sample = sm->QuerySample();
	*Input = cv::Scalar(0);
	updateBlobImage(sample->depth, Input);
	sm->ReleaseFrame();
}

//Copy Depth Image To Input
void mCamera::getDepthImage(cv::Mat *Input)
{
	if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

	PXCCapture::Sample *sample;
	sample = sm->QuerySample();

	if (sample->depth)
		PXCImage2CVMat(sample->depth, PXCImage::PIXEL_FORMAT_DEPTH_F32).convertTo(*Input, CV_8UC1);
	sm->ReleaseFrame();
}

void mCamera::release()
{
	sm->Release();
	session->Release();
}


//--------------------mHand Section----------------------//

//Depth에서 쓰이는 Point를 넣으면 Color사이즈에서 사용하는 Point 리턴 
//Input Point of Depth, Return Converted Point of Color
CvPoint mHand::getColorPoint(CvPoint A)
{
	CvPoint ColorPoint;

	ColorPoint.x = (640 / 320)*A.x + 60;
	ColorPoint.y = (480 / 240)*A.y - 20;


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
	ContourSt = cvCreateMemStorage(0);
	hull_st = cvCreateMemStorage(0);
	defects_st = cvCreateMemStorage(0);
}

//BlobImage To Contour OutContourImage
void mHand::blob2Contour(const cv::Mat BlobImage, cv::Mat *OutContourImage)
{

	cv::Mat TempBlobImage;
	IplImage *iplContour;

	// Copy & Mat 2 IplImage
	BlobImage.copyTo(TempBlobImage);
	iplContour = &IplImage(TempBlobImage);

	//-----------------------------------Find Contours----------------//
	if (ContourSt == NULL) {
		ContourSt = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(ContourSt);
	}

	cvDilate(iplContour, iplContour); // 팽창

									 


	//윤곽선 찾기
	cvFindContours(
		iplContour,							// Input Picture
		ContourSt,             //Storage for Save Contours
		&contourP,             //외곽선의 좌표들이 저장될 Sequence
		sizeof(CvContour),
		CV_RETR_TREE,           //어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0, 0)
	);

	// Clean IplContour
	cvZero(iplContour);
	if (contourP) {
		//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
		cvDrawContours(
			iplContour,               //외곽선이 그려질 영상
			contourP,              //외곽선 트리의 루트노드
			cvScalarAll(255),      //외부 외곽선의 색상
			cvScalarAll(150),      //내부 외곽선의 색상
			100                    //외곽선을 그릴때 이동할 깊이
		);

		cv::cvarrToMat(iplContour).copyTo(*OutContourImage);


		//ContourImage = cv::cvarrToMat(iplContour);
	}

}


void mHand::find_convex_hull(cv::Mat *Color)
{
	CvSeq *defects;
	CvConvexityDefect *defect_array;
	int i;
	double x = 0, y = 0;
	double dist = 0;
	double distance = 0;
	double degree = 0;
	CvPoint ColorPoint;
	CvPoint ColorHandCenter;
	CvPoint ColorClickPoint = cvPoint(640, 480);
	CvPoint ForComparePoint;
	int FingerPosition = 0;

	hull = NULL;

	if (!contourP)
		return;
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

	hand_center = cvPoint(x, y);

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
			if (i < countdefects)
				distance = getDistance(*defect_array[i].start, *defect_array[i].end);
			if (distance > 5) {

				ColorHandCenter = getColorPoint(hand_center);
				ColorPoint = getColorPoint(*defect_array[i].end);
				cv::circle(*Color, ColorPoint, 15, CV_RGB(255, 0, 0), 0, 8);
				cv::line(*Color, ColorPoint, ColorHandCenter, CV_RGB(0, 255, 0));
				//Catch Click Point On Longest Finger 
				/*if (getDistance(*defect_array[i].end, hand_center) > LongestDistance) {
				ColorClickPoint = getColorPoint(*defect_array[i].end);
				LongestDistance = getDistance(*defect_array[i].end, hand_center);
				}*/
				//Catch Click Point On Leftmost Finger
				ForComparePoint = getColorPoint(*defect_array[i].end);
				if (ColorClickPoint.x > ForComparePoint.x)
					ColorClickPoint = ForComparePoint;



				FingerPosition++;
			}
		}
	}


	num_fingers = FingerPosition;
	cv::circle(*Color, ColorClickPoint, 10, CV_RGB(0, 0, 255), 0, 8);
	ClickPoint = ColorClickPoint;
	free(defect_array);
	//CheckCircle();
}


//Input에 Color 이미지 SumIamge에는 Color와 그려진 것이 합친 이미지가 나갈 것
void mHand::CheckFingerNum(const cv::Mat Input, cv::Mat *OutSumImage)
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
		cv::circle(Input, ClickPoint, 10, CV_RGB(255, 255, 255), 0, 8);
		cv::circle(BackGroundImage, ClickPoint, 5, CV_RGB(255, 255, 255), 10);
	}
	cv::add(Input, BackGroundImage, *OutSumImage);
}

//손가락 개수 리턴
int mHand::getFingerNum() {
	return num_fingers;
}

cv::Point mHand::getFingerClickPoint()
{
	return ClickPoint;
}