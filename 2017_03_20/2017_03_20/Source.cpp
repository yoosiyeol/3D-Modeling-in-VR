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
	PXCCaptureManager *cm;
	pxcStatus status;
	PXCBlobModule *blobModule;
	PXCBlobData* blobData;
	PXCBlobConfiguration* blobConfig;

	float frameRate;

	//blob point
	std::vector<PXCPointI32> points;

	//블롭 관련 초기화
	void initializeBlob()
	{
		// Blob 모듈 데이터 셋팅
		blobModule = sm->QueryBlob();
		blobData = blobModule->CreateOutput();

		blobConfig = blobModule->CreateActiveConfiguration();
		//blobConfig->SetSegmentationSmoothing(1.0f);
		//blobConfig->SetContourSmoothing(1.0f);
		//blobConfig->EnableColorMapping(true);
		blobConfig->SetBlobSmoothing(0.5);
		blobConfig->SetMaxBlobs(maxBlobToShow);
		//blobConfig->SetMaxPixelCount(-1);
		//blobConfig->SetMaxDistance(660);
		blobConfig->SetMaxObjectDepth(100);
		blobConfig->EnableContourExtraction(true);
		blobConfig->EnableSegmentationImage(true);
		blobConfig->EnableStabilizer(true);


		//이게 최적인거 같음
		status = blobConfig->SetMaxBlobArea(0.115f);
		//blobConfig->SetMinBlobArea(0.03f);
		/*if (status == PXC_STATUS_NO_ERROR)
		printf("노에러");
		else if (status == PXC_STATUS_PARAM_UNSUPPORTED)
		printf("예스에러");*/
		blobConfig->ApplyChanges();

		// 윤곽 포인트 배열 초기화
		points.resize(maxBlobToShow * 4000);
	}
	
	//PXCImage(리얼센스스타일) 에서 Mat 형식으로 체인지
	cv::Mat PXCImage2CVMat(PXCImage *pxcImage, PXCImage::PixelFormat format)
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
	
	//블롭만 찾기
	void updateBlobImage(PXCImage* depthFrame,cv::Mat *input)
	{
		if (depthFrame == nullptr) {
			return;
		}

		// Blob 데이터 업데이트
		auto sts = blobData->Update();
		if (sts < PXC_STATUS_NO_ERROR) {
			return;
		}


		PXCImage::ImageInfo depthInfo = depthFrame->QueryInfo();
		//cv::Mat ContourImage = cv::Mat::zeros(depthInfo.height, depthInfo.width, CV_8U);
		//std::cout << "height :" << depthInfo.height << "width :" << depthInfo.width << std::endl;
		//ContourImage = cv::Mat::zeros(480, 640, CV_8U);

		auto session = sm->QuerySession();
		//depthInfo.format = PXCImage::PIXEL_FORMAT_Y16;
		PXCImage* blobImage = session->CreateImage(&depthInfo);


		// Blob 개수
		int numOfBlobs = blobData->QueryNumberOfBlobs();
		for (int i = 0; i < numOfBlobs; ++i) {
			// Blob 가까운 곳에서 먼 곳 순
			PXCBlobData::IBlob* blob;
			PXCBlobData::AccessOrderType accessOrder = PXCBlobData::ACCESS_ORDER_NEAR_TO_FAR;

			// Blob is queried according to image segmentation type (color/depth) 
			PXCBlobData::SegmentationImageType segmentationType = PXCBlobData::SegmentationImageType::SEGMENTATION_IMAGE_DEPTH;
			blobData->QueryBlob(i, segmentationType, accessOrder, blob);
			if (sts < PXC_STATUS_NO_ERROR) {
				continue;
			}

			// Blob됪몴귩롦벦궥귡
			sts = blob->QuerySegmentationImage(blobImage);
			if (sts < PXC_STATUS_NO_ERROR) {
				continue;
			}

			// Blob됪몴귩벶귒뜛귔
			PXCImage::ImageData data;
			pxcStatus sts = blobImage->AcquireAccess(PXCImage::Access::ACCESS_READ,
				PXCImage::PIXEL_FORMAT_Y16, &data);
			if (sts < PXC_STATUS_NO_ERROR) {
				continue;
			}


			// 긢??귩긓긯?궥귡
			/*for (int j = 0; j < depthInfo.height * depthInfo.width; ++j) {
				if (data.planes[0][j] != 0) {
					// 귽깛긢긞긏긚궸귝궯궲갂륡뼞귩빾궑귡
					ContourImage.data[j] = (i + 1) * 64;
				}
			}*/

			PXCImage2CVMat(blobImage, PXCImage::PIXEL_FORMAT_ANY).copyTo(*input);

			// Blob 액세스 해제
			blobImage->ReleaseAccess(&data);

			// Blob외곽선 불러오기
			//updateContoursImage(blob, i,input);
		}

		// C++경우 릴리즈 ㄴㄴ
		//blobImage->Release();
	}
	
	//리얼센스스타일 외곽선 업데이트
	/*
	void updateContoursImage(PXCBlobData::IBlob* blob, int index,cv::Mat *input)
	{
		// 블롭 찾기
		auto numOfContours = blob->QueryNumberOfContours();
		for (int i = 0; i < numOfContours; ++i) {
			// 외곽선 찾기
			PXCBlobData::IContour* contour;
			blob->QueryContour(i, contour);
			pxcI32 size = contour->QuerySize();
			if (size <= 0) {
				continue;
			}
			// 포인트 메모리 재할당
			if (points.size() < size) {
				points.reserve(size);
			}

			// 쀖둺궻?귩롦벦궥귡
			//auto sts = blob->QueryContourPoints(i, points.size(), &points[0]);
			auto sts = contour->QueryPoints(points.size(), &points[0]);
			if (sts < PXC_STATUS_NO_ERROR) {
				continue;
			}

			// 외곽선 그리기
			drawContour(&points[0], size, index,input);
		}
	}
	void drawContour(PXCPointI32* points, pxcI32 size, int index,cv::Mat *ContourImage)
	{
		// 라인 읽어들여 그리기
		for (int i = 0; i < (size - 1); ++i) {
			const auto& pt1 = points[i];
			const auto& pt2 = points[i + 1];
			cv::line(*ContourImage, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y),
				cv::Scalar(((index + 1) * 127)), 5);
		}

		// 끝과 마지막 이어주기
		const auto& pt1 = points[size - 1];
		const auto& pt2 = points[0];
		cv::line(*ContourImage, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y),
			cv::Scalar(((index + 1) * 127)), 5);


	}
	*/
public:
		
	//카메라 초기화 및 블롭 초기세팅
	void initialize()
	{
		//Init Session
		session = PXCSession::CreateInstance();
		PXCSession::ImplDesc desc1 = {};
		desc1.group = PXCSession::IMPL_GROUP_SENSOR;
		desc1.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
		PXCCapture* c;
		session->CreateImpl<PXCCapture>(&desc1, &c);

		//PXCCaptureManager Init
		PXCCapture::Device* device = c->CreateDevice(0);


		PXCRangeF32 depthRange;
		depthRange.min = 650.f;
		depthRange.max = 800.f;
		status = device->SetDSMinMaxZ(depthRange); // fails with PXC_STATUS_DEVICE_FAILED(-201)
		if (status != PXC_STATUS_NO_ERROR) {
			//throw std::runtime_error("깊이 카메라 최대 최소 설정 실패");
			std::cout << "깊이 카메라 깊이설정 실패" << std::endl;
			return;
		}



		//sm = PXCSenseManager::CreateInstance();
		sm = session->CreateSenseManager();
		if (sm == 0) {
			throw std::runtime_error("SenseManager!!");
		}

		frameRate = 30;

		sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR,ColorSize.width,ColorSize.height, frameRate);
		sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, DepthSize.width, DepthSize.height,
			frameRate);


		//About Blob
		status = sm->EnableBlob(0);
		if (status < PXC_STATUS_NO_ERROR) {
			//throw std::runtime_error("Blob 실패");
			std::cout << "Blob 사용 실패" << std::endl;
			return;
		}

		status = sm->Init();
		if (status != PXC_STATUS_NO_ERROR) {
			//throw std::runtime_error("센스 매니저 init 실패");
			std::cout << "센스매니저 인잇 실패" << std::endl;
			return;
		}

		//이미지 반전
		sm->QueryCaptureManager()->QueryDevice()->SetMirrorMode(
			PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);

		// Blob init
		initializeBlob();

	}

	//매개변수에 Color Image 복사
	void getColorImage(cv::Mat *Input)
	{
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

		PXCCapture::Sample *sample;
		sample = sm->QuerySample();
		if (sample->color)
			*Input = PXCImage2CVMat(sample->color, PXCImage::PIXEL_FORMAT_RGB24);
		sm->ReleaseFrame();
	}

	//매개변수에 Blob Image 복사
	void getBlobImage(cv::Mat *Input) {
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

		PXCCapture::Sample *sample;
		sample = sm->QuerySample();
		*Input = cv::Scalar(0);
		updateBlobImage(sample->depth, Input);
		sm->ReleaseFrame();
	}


	//매개변수에 Depth Image 복사
	void getDepthImage(cv::Mat *Input)
	{
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) return;

		PXCCapture::Sample *sample;
		sample = sm->QuerySample();

		if (sample->depth)
			PXCImage2CVMat(sample->depth, PXCImage::PIXEL_FORMAT_DEPTH_F32).convertTo(*Input, CV_8UC1);
		sm->ReleaseFrame();
	}

	void release()
	{
		sm->Release();
		session->Release();
	}
};

class mHand
{
private:

	CvSeq		*contourP;	// Hand contour 
	CvSeq		*hull;		// Hand convex hull 

	CvPoint		hand_center;
	CvPoint		ClickPoint;
	CvPoint		*fingers;	// Detected fingers positions 
	CvPoint		*defectsP;	// Convexity defects depth points 

	CvMemStorage	*hull_st;
	CvMemStorage	*temp_st;
	CvMemStorage	*defects_st;
	CvMemStorage  *ContourSt;

	IplConvKernel	*kernel;	// Kernel for morph operations 

	int		num_fingers;
	int		hand_radius;
	int		num_defects;

	
	//Depth에서 쓰이는 Point를 넣으면 Color사이즈에서 사용하는 Point 리턴 
	CvPoint getColorPoint(CvPoint A)
	{
		CvPoint ColorPoint;

		ColorPoint.x = (640 / 320)*A.x + 60;
		ColorPoint.y = (480 / 240)*A.y - 20;


		return ColorPoint;
	}

	//P2의 각도 리턴
	double __fastcall Meas_Angle(CvPoint P1, CvPoint P2, CvPoint P3)
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
	double getDistance(CvPoint A, CvPoint B)
	{
		double distance = 0;

		distance = sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));

		return distance;
	}

	/*
	void CheckCircle()
	{
		if (num_fingers == 2) {
			cv::circle(Color, ClickPoint, 10, CV_RGB(255, 255, 255), 0, 8);
			cvCircle(BackImage, ClickPoint, 5, CV_RGB(255, 255, 255), 10);
		}
	}*/


public:

	void initialize()
	{
		ContourSt = cvCreateMemStorage(0);
		hull_st = cvCreateMemStorage(0);
		defects_st = cvCreateMemStorage(0);
		/*
		convDepth = cvCreateImage(frameSizeDepth, 8, 1);
		IplColor = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
		ForSumImage = cvCreateImage(cvGetSize(IplColor), 8, 3);
		BackImage = cvCreateImage(cvGetSize(IplColor), 8, 3);*/

		fingers = (CvPoint*)calloc(NUM_FINGERS + 1, sizeof(CvPoint));
		defectsP = (CvPoint*)calloc(NUM_DEFECTS, sizeof(CvPoint));
	}

	void blob2Contour(cv::Mat BlobImage,cv::Mat *OutContourImage)
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

		//cvErode(iplContour, iplContour);
		//cvErode(iplContour, iplContour);
		cvDilate(iplContour, iplContour); // 팽창

		//cv::imshow("InBlob2Conotur", cvContourImage);


		//윤곽선 찾기
		cvFindContours(
			iplContour,							// 입력 영상
			ContourSt,             //검출된 외곽선을 기록하기 위한 메모리 스토리지
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

		//ContourImage = cv::cvarrToMat(iplContour);
		//cvShowImage("iplContour", iplContour);
		//cvSmooth Section
		/*cvCopy(g_gray, dst);
		cvZero(g_gray);
		cvSmooth(dst, g_gray, CV_BLUR, 3);

		cvCopy(g_gray, ctx->ContourImage);

		cvReleaseImage(&g_gray);
		cvReleaseImage(&dst);*/
	}

};

int main()
{
	mCamera Camera;
	mHand Hand;
	cv::Mat Color;
	cv::Mat Depth;
	cv::Mat Blob = cv::Mat::zeros(DepthSize, CV_8UC1);
	
	cv::Mat Contour = cv::Mat::zeros(DepthSize, CV_8UC1);

	Camera.initialize();
	Hand.initialize();

	while (1) {
		//---------------test Camera part ------------//
		Camera.getColorImage(&Color);
		Camera.getDepthImage(&Depth);
		Camera.getBlobImage(&Blob);

		cv::imshow("Blob", Blob);
		cv::imshow("Color", Color);
		cv::imshow("Depth", Depth);
		//---------------------------------------------//

		//--------------test Hand part ---------------//
		Hand.blob2Contour(Blob, &Contour);
		cv::imshow("Contour", Contour);
		//---------------------------------------------//

		int key = cv::waitKey(1);
		if (key == 27)	break;
	}

	Camera.release();

}