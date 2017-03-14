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

#define NUM_FINGERS 6
#define NUM_DEFECTS 8
#define THRESH 250
#define maxBlobToShow 1

cv::Mat PXCImage2CVMat(PXCImage *pxcImage, PXCImage::PixelFormat format);

class mApp {
private:
	CvCapture	*capture;	 //Capture handle 
	CvVideoWriter	*writer;	 // File recording handle 

								 //Image 
	cv::Mat	  Color;			   	// RGB image 
	cv::Mat  Depth;		 //Depth image
	cv::Mat  ContourImage;
	cv::Mat cvContourImage;
	IplImage *iplContour;
	IplImage *convDepth;		//Depth image ver.Iplimage 

								//PXC
	PXCSession *session;
	PXCSenseManager *sm;			//SenseManager
	PXCCaptureManager *cm;
	pxcStatus status;
	PXCBlobModule *blobModule;
	PXCBlobData* blobData;
	PXCBlobConfiguration* blobConfig;

	//blob point
	std::vector<PXCPointI32> points;


	cv::Size frameSize;
	cv::Size frameSizeDepth;

	float frameRate;

	CvSeq		*contourP;	// Hand contour 
	CvSeq		*hull;		// Hand convex hull 

	CvPoint		hand_center;
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

	void convMat2Ipl(cv::Mat src, IplImage *des)
	{
		IplImage temp;
		temp = src;
		des = &temp;
	}

	void initializeCv()
	{
		ContourSt = cvCreateMemStorage(0);
		convDepth = cvCreateImage(frameSizeDepth, 8, 1);
		hull_st = cvCreateMemStorage(0);
		defects_st = cvCreateMemStorage(0);

		fingers = (CvPoint*)calloc(NUM_FINGERS + 1, sizeof(CvPoint));
		defectsP = (CvPoint*)calloc(NUM_DEFECTS, sizeof(CvPoint));
	}

	void findContour()
	{
		double area, max_area = 0.0;
		CvSeq *contours, *tmp, *contour = NULL;

		int Conturs = 0;

		if (ContourSt == NULL) {
			ContourSt = cvCreateMemStorage(0);
		}
		else {
			cvClearMemStorage(ContourSt);
		}
		//Converting Mat To IplImage
		convDepth = &IplImage(Depth);
		//convMat2Ipl(Depth, convDepth);

		cvShowImage("fuckingTest", convDepth);
		cv::imshow("fucking Depth", Depth);

		//g_image영상을 BRG색공간을 그레이 스케일로 변환(BGR to Gray = BGR2GRAY)
		//cvCvtColor(ctx->image, g_gray, CV_BGR2GRAY);
		//임계값 이하:0, 임계값초과값:1 설정
		cvThreshold(convDepth, convDepth, THRESH, 255, CV_THRESH_BINARY);



		//윤곽선 찾기
		cvFindContours(
			//ctx->skin_image,                //입력영상
			convDepth,
			ContourSt,             //검출된 외곽선을 기록하기 위한 메모리 스토리지
			&contours,             //외곽선의 좌표들이 저장될 Sequence
			sizeof(CvContour),
			CV_RETR_TREE,           //어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
			CV_CHAIN_APPROX_SIMPLE,
			cvPoint(0, 0)
		);

		cvZero(convDepth);
		if (contours) {
			//if(contourP){
			//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
			cvDrawContours(
				convDepth,                //외곽선이 그려질 영상
				contours,
				//contourP               //외곽선 트리의 루트노드
				cvScalarAll(255),      //외부 외곽선의 색상
				cvScalarAll(128),      //내부 외곽선의 색상
				100                    //외곽선을 그릴때 이동할 깊이
			);
		}

		ContourImage = cv::cvarrToMat(convDepth);
	}

	void findGreatestContour()
	{

		/* Select contour having greatest area */
		/*for (tmp = contours; tmp; tmp = tmp->h_next) {
		area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
		if (area > max_area) {
		max_area = area;
		contour = tmp;
		}
		}*/

		/* Approximate contour with poly-line */
		/*if (contour) {
		contour = cvApproxPoly(contour, sizeof(CvContour),
		ContourSt, CV_POLY_APPROX_DP, 2,1);
		contourP = contour;
		}*/

	}

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
		blobConfig->EnableContourExtraction(true);
		blobConfig->EnableSegmentationImage(true);
		
		//이게 최적인거 같음
		status = blobConfig->SetMaxBlobArea(0.115f);
		//blobConfig->SetMinBlobArea(0.03f);
		/*if (status == PXC_STATUS_NO_ERROR)
			printf("노에러");
		else if (status == PXC_STATUS_PARAM_UNSUPPORTED)
			printf("예스에러");*/
		blobConfig->ApplyChanges();
		
		// 윤곽 포인트 배열 초기화
		points.resize(maxBlobToShow*1000);
	}

	void updateBlobImage(PXCImage* depthFrame)
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
		ContourImage = cv::Mat::zeros(depthInfo.height, depthInfo.width, CV_8U);
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
			for (int j = 0; j < depthInfo.height * depthInfo.width; ++j) {
				if (data.planes[0][j] != 0) {
					// 귽깛긢긞긏긚궸귝궯궲갂륡뼞귩빾궑귡
					ContourImage.data[j] = (i + 1) * 64;
				}
			}

			// Blob 액세스 해제
			blobImage->ReleaseAccess(&data);

			// Blob외곽선 불러오기
			updateContoursImage(blob, i);
		}

		// C++경우 릴리즈 ㄴㄴ
		//blobImage->Release();
	}

	void updateContoursImage(PXCBlobData::IBlob* blob, int index)
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
			drawContour(&points[0], size, index);
		}
	}

	void drawContour(PXCPointI32* points, pxcI32 size, int index)
	{
		// 라인 읽어들여 그리기
		for (int i = 0; i < (size - 1); ++i) {
			const auto& pt1 = points[i];
			const auto& pt2 = points[i + 1];
			cv::line(ContourImage, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y),
				cv::Scalar(((index + 1) * 127)), 5);
		}

		// 끝과 마지막 이어주기
		const auto& pt1 = points[size - 1];
		const auto& pt2 = points[0];
		cv::line(ContourImage, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y),
			cv::Scalar(((index + 1) * 127)), 5);


	}

	void find_convex_hull()
	{
		CvSeq *defects;
		CvConvexityDefect *defect_array;
		int i;
		int x = 0, y = 0;
		int dist = 0;
		CvPoint pt0;
		CvPoint pt;

		hull = NULL;

		if (!contourP)
			return;

		hull = cvConvexHull2(contourP, hull_st, CV_CLOCKWISE, 0);

		int counthull = hull->total;

		pt0 = **CV_GET_SEQ_ELEM(CvPoint*, hull, counthull - 1);
		for (int i = 0; i < counthull; i++) {
			pt = **CV_GET_SEQ_ELEM(CvPoint*, hull, i);
			//cv::line(ContourImage, pt0, pt, CV_RGB(255, 255, 255));
			pt0 = pt;
		}
	
		defects = cvConvexityDefects(contourP, hull, defects_st);
		int countdefects = defects->total;

		defect_array = (CvConvexityDefect*)calloc(defects->total,
			sizeof(CvConvexityDefect));
		cvCvtSeqToArray(defects, defect_array, CV_WHOLE_SEQ);
		
		double degree = 0;
		int fromx = 0, fromy = 0;
		int tox = 0, toy = 0;

		for (int i = 0; i < countdefects; i++)
		{

			pt0 = *defect_array[i].start;
			//cv::circle(cvContourImage, pt0, 15, CV_RGB(255, 255, 255),0,8);
			///////////////////////////////////////
			fromx = defect_array[i].start->x;
			fromy = defect_array[i].start->y;
			tox = defect_array[i].end->x;
			toy = defect_array[i].end->y;
			degree = atan2(fromy - toy, tox - fromx) * 180 / 3.1415926535;
			if (cos(degree) > 0 && cos(degree) < 1)
			{
				if (defect_array[i].depth > 19) {
					printf("radian:%lf\n", degree);
					cv::circle(cvContourImage, pt0, 15, CV_RGB(255, 255, 255), 0, 8);
					cv::circle(cvContourImage, *defect_array[i].end, 15, CV_RGB(255, 255, 255), 0, 8);
					//cv::line(ContourImage, *defect_array[i].start, *defect_array[i].end, CV_RGB(255, 255, 255));

				}
			}
			/////////////////////////////////////////
		}
	

		for (i = 0; i < defects->total && i <defects->total; i++) {
			x += defect_array[i].depth_point->x;
			y += defect_array[i].depth_point->y;

			//defectsP[i] = cvPoint(defect_array[i].depth_point->x,
			//	defect_array[i].depth_point->y);
		}

		x /= defects->total;
		y /= defects->total;

		num_defects = defects->total;
		hand_center = cvPoint(x, y);

		printf("%d %d\n", hand_center.x, hand_center.y);

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
		free(defect_array);

		/*
		int j = 0;
		for (; defects; defects = defects->h_next)
		{
			int nomdef = defects->total; // defect amount  
										 //outlet_float( m_nomdef, nomdef );  

										 //printf(" defect no %d \n",nomdef);

			if (nomdef == 0)
				continue;

			// Alloc memory for defect set.     
			//fprintf(stderr,"malloc\n");  
			defect_array = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*nomdef);

			// Get defect set.  
			//fprintf(stderr,"cvCvtSeqToArray\n");  
			cvCvtSeqToArray(defects, defect_array, CV_WHOLE_SEQ);

			// Draw marks for all defects.  
			for (int i = 0; i<nomdef; i++)
			{
				printf(" defect depth for defect %d %f \n", i, defect_array[i].depth);
				cv::line(ContourImage, *(defect_array[i].start), *(defect_array[i].depth_point), CV_RGB(255, 255, 0), 1, CV_AA, 0);
				cv::circle(ContourImage, *(defect_array[i].depth_point), 5, CV_RGB(0, 0, 164), 2, 8, 0);
				cv::circle(ContourImage, *(defect_array[i].start), 5, CV_RGB(0, 0, 164), 2, 8, 0);
				cv::line(ContourImage, *(defect_array[i].depth_point), *(defect_array[i].end), CV_RGB(255, 255, 0), 1, CV_AA, 0);

			}
	
			j++;

		}
		*/
		/*
		if (hull) {

			// Get convexity defects of contour w.r.t. the convex hull 
			defects = cvConvexityDefects(contourP, hull, defects_st);

			if (defects && defects->total) {
				defect_array = (CvConvexityDefect*)calloc(defects->total,
					sizeof(CvConvexityDefect));
				cvCvtSeqToArray(defects, defect_array, CV_WHOLE_SEQ);

				// Average depth points to get hand center 

				for (i = 0; i < defects->total && i < NUM_DEFECTS; i++) {
					x += defect_array[i].depth_point->x;
					y += defect_array[i].depth_point->y;
					defectsP[i] = cvPoint(defect_array[i].depth_point->x,
						defect_array[i].depth_point->y);
				}

				x /= defects->total;
				y /= defects->total;

				num_defects = defects->total;
				hand_center = cvPoint(x, y);

				printf("%d %d\n", hand_center.x, hand_center.y);

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
				free(defect_array);
			}
		}
	*/
	}

	void eliminatesNoise()
	{
		//Declaration Image ( 1 channel and 3 channel)
		/*IplImage* skin = cvCreateImage(cvGetSize(ctx->image), 8, 3);
		IplImage* single_skin = cvCreateImage(cvGetSize(ctx->image), 8, 1);
		IplImage* final_skin = cvCreateImage(cvGetSize(ctx->image), 8, 1);*/

		IplImage* InIplBlobImage=cvCreateImage(frameSizeDepth, 8, 1);
		IplImage* OutIplBlobImage = cvCreateImage(frameSizeDepth, 8, 1);
		IplImage temp;
		cv::Mat InBlobImage;
		cv::Mat outBlobImage;
		IplConvKernel *element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, NULL);
		ContourImage.copyTo(InBlobImage);

		InIplBlobImage = &IplImage(InBlobImage);

		//Declartion Filter
		int filterSize = 3;
		int ErodeSize = 7;
		int OpenSize = 5;
	
		//Declartion  Low Range
		int LowR = 123;
		int LowG = 123;
		int LowB = 123;

		//Declartion High Range
		int HighR = 255;
		int HighG = 255;
		int HighB = 255;

		CvScalar lowerRange = cvScalar(LowR, LowG, LowB);
		CvScalar upperRange = cvScalar(HighR, HighG, HighB);

		//cvNamedWindow("InIplBlobImage", CV_WINDOW_AUTOSIZE);
		//cvShowImage("InIplBlobImage", InIplBlobImage);
		// 특정 범위의 색만 검출
		cvInRangeS(InIplBlobImage, lowerRange, upperRange,OutIplBlobImage);

		outBlobImage = cv::cvarrToMat(OutIplBlobImage);
		//cv::imshow("outIplBlobImage",outBlobImage);

		//Improving quality of image
		/*cvErode(single_skin, final_skin, convErodeKernel);
		cvMorphologyEx(final_skin, final_skin, NULL,convOpenKernel, CV_MOP_OPEN);
		cvMorphologyEx(final_skin, final_skin, NULL, convKernel, CV_MOP_CLOSE);
		*/
		/*
		cvNamedWindow("Aest");
		cvShowImage("Aest", final_skin);
		*/
		//cvMorphologyEx(OutIplBlobImage, OutIplBlobImage, NULL, convKernel, CV_MOP_CLOSE);


		//cvErode(OutIplBlobImage, OutIplBlobImage, element, 1); // 침식

		ContourImage = cv::cvarrToMat(OutIplBlobImage);
		
		//cvReleaseImage(&OutIplBlobImage);
		//cvReleaseImage(&InIplBlobImage);

	}

	void find_fingers()
	{
		int n;
		int i;
		CvPoint *points;
		CvPoint max_point;
		int dist1 = 0, dist2 = 0;

		num_fingers = 0;

		if (!contourP || !hull)
			return;

		n = contourP->total;
		points = (CvPoint*)calloc(n, sizeof(CvPoint));

		cvCvtSeqToArray(contourP, points, CV_WHOLE_SEQ);

		/*
		* Fingers are detected as points where the distance to the center
		* is a local maximum
		*/
		for (i = 0; i < n; i++) {
			int dist;
			int cx = hand_center.x;
			int cy = hand_center.y;

			dist = (cx - points[i].x) * (cx - points[i].x) +
				(cy - points[i].y) * (cy - points[i].y);

			if (dist < dist1 && dist1 > dist2 && max_point.x != 0
				&& max_point.y <frameSizeDepth.height - 10) {

				fingers[num_fingers++] = max_point;
				if (num_fingers >= NUM_FINGERS)
					break;
			}

			dist2 = dist1;
			dist1 = dist;
			max_point = points[i];
		}
		printf("Fingers NUM!!!:  %d\n", num_fingers);
		free(points);

	}

public:
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
		sm= session->CreateSenseManager();
		if (sm == 0) {
			throw std::runtime_error("SenseManager!!");
		}

		frameSize = cv::Size(640, 480);
		frameSizeDepth = cv::Size(480, 360);

		frameRate = 60;
		Color = cv::Mat::zeros(frameSize, CV_8UC3);
		Depth = cv::Mat::zeros(frameSizeDepth, CV_8UC1);

		sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, frameSize.width, frameSize.height, frameRate);
		sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, frameSizeDepth.width, frameSizeDepth.height,
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
			return ;
		}
	
		//이미지 반전
		sm->QueryCaptureManager()->QueryDevice()->SetMirrorMode(
			PXCCapture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);


		initializeCv();

		// Blob init
		initializeBlob();

	}

	void run()
	{
		for (;;)
		{
			if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

			PXCCapture::Sample *sample;



			sample = sm->QuerySample();  

			//color 스트리밍
			if (sample->color)
				Color = PXCImage2CVMat(sample->color, PXCImage::PIXEL_FORMAT_RGB24);

			//depth 스트리밍
			if (sample->depth)
				PXCImage2CVMat(sample->depth, PXCImage::PIXEL_FORMAT_DEPTH_F32).convertTo(Depth, CV_8UC1);
		
			
			////////////////////////////////////
			//cv::imshow("Before depth", Depth);
			//convDepth = &IplImage(Depth);
			//cvErode(convDepth,convDepth);
			//cvDilate(convDepth, convDepth);
			//IplConvKernel *element;
		//	element = cvCreateStructuringElementEx(3, 3, 1,1, CV_SHAPE_RECT, NULL); // 필터의 크기를 11x11로 설정
			//cvMorphologyEx(convDepth, convDepth, NULL, element, CV_MOP_CLOSE, 1);


			//cvSmooth(convDepth,convDepth, CV_BLUR, 5);
		//	cv::imshow("depth", Depth);
			////////////////////////////////////////////


			// 블롭 이미지 업데이트
			if (sample) {
				updateBlobImage(sample->depth);
			}
			cv::imshow("Color", Color);
			cv::imshow("ContourImage(BlobImage)", ContourImage);
			eliminatesNoise();

			Blob2Contour();
			
			find_convex_hull();
			//find_fingers();
			convDepth = &IplImage(ContourImage);
			//cvCircle(convDepth, hand_center, hand_radius, CvScalar(255, 255, 255), 2);
			cvCircle(convDepth, hand_center, 10, CV_RGB(255, 255, 255));
			/*for (int i = 0; i < NUM_FINGERS; i++) {
				cvLine(convDepth, hand_center, fingers[i], CV_RGB(255, 255, 255));
			}*/

			/*int forROI = hand_radius * 3;
			int forROIX = hand_center.x - hand_radius*2;
			int forROIY = hand_center.y - hand_radius*2;
	
			cvSetImageROI(convDepth, cvRect(forROIX,
				forROIY,
				forROI,
				forROI
			));*/
			cv::imshow("cvContourImage(contour)", cvContourImage);
			//cv::imshow("test ROI",image);
			cvShowImage("convDepth", convDepth);
			//cvResetImageROI(convDepth);
			int key = cv::waitKey(1);

			if (key == 27)
				break;

			//blobConfiguration->Release();
			//	blobData->Release();
			sm->ReleaseFrame();
		}
		sm->Release();
		//sm->Close();
		session->Release();
	}

/*	static void setColorImage(PXCImage* rgbImage)
	{
		PXCImage::ImageInfo info = rgbImage->QueryInfo();
		PXCImage::ImageData data;
		int m_bufferSize = 0;
		pxcI32* m_colorBuffer = NULL;
		pxcI32 *m_buffer = new pxcI32[m_bufferSize];
		pxcI32 g_lut[5] = { 0,255,170,130,1 };

		if (rgbImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data) == PXC_STATUS_NO_ERROR)
		{

			int bufferSize = info.width * info.height;
			if (bufferSize != m_bufferSize)
			{
				m_bufferSize = bufferSize;
				//if (m_colorBuffer) delete[] m_colorBuffer;
				m_colorBuffer = new pxcI32[m_bufferSize];
			}

			memcpy_s(m_colorBuffer, m_bufferSize * sizeof(pxcI32), (pxcI32*)data.planes[0], m_bufferSize * sizeof(pxcI32));

			pxcI32 pitch = data.pitches[0];
			pxcBYTE* blobRow = (pxcBYTE*)m_buffer;
			pxcBYTE* dst = (pxcBYTE*)m_colorBuffer;

			for (int i = 0; i < info.height; i++)
			{
				for (int j = 0; j < info.width; j++)
				{
					if (blobRow[i * pitch + j * 4] != 0)
					{
						dst[i*pitch + j * 4] = g_lut[blobRow[i * pitch + j * 4]];
						dst[i*pitch + j * 4 + 1] = g_lut[blobRow[i * pitch + j * 4]] / 2;
					}
				}
			}

			rgbImage->ReleaseAccess(&data);
		}
	}
	*/
	void Blob2Contour()
	{

		double area, max_area = 0.0;
		CvSeq *tmp = NULL;
		//IplImage *iplContour;
		cv::Mat matContour;
		int Conturs = 0;

		// Copy & Mat 2 IplImage
		ContourImage.copyTo(cvContourImage);
		iplContour = &IplImage(cvContourImage);
		
		//-----------------------------------Find Contours----------------//
		if (ContourSt == NULL) {
			ContourSt = cvCreateMemStorage(0);
		}
		else {
			cvClearMemStorage(ContourSt);
		}
		
		cvErode(iplContour, iplContour);
		cvErode(iplContour, iplContour);


		cv::imshow("InBlob2Conotur", cvContourImage);
		

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

		// Mat 2 IplImage
		//cvZero(iplContour);
		if (contourP) {
			//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
			cvDrawContours(
				iplContour,               //외곽선이 그려질 영상
				contourP,              //외곽선 트리의 루트노드
				cvScalarAll(255),      //외부 외곽선의 색상
				cvScalarAll(150),      //내부 외곽선의 색상
				100                    //외곽선을 그릴때 이동할 깊이
			);
			
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

	/*static void DisplayPicture(PXCImage *image, PXCBlobData* blobData)
	{
		if (!image) return;
		PXCImage::ImageInfo info = image->QueryInfo();

		PXCImage::ImageData new_bdata;
		image->AcquireAccess(PXCImage::ACCESS_WRITE, &new_bdata);

		memset(new_bdata.planes[0], 0, new_bdata.pitches[0] * info.height);
		image->ReleaseAccess(&new_bdata);

		int numOfBlobs = blobData->QueryNumberOfBlobs();

		std::vector<std::vector<PXCPointI32*>> points;
		std::vector<std::vector<int>> pointsAccualSize;
		std::vector<PXCBlobData::IBlob*> blobDataList;

		points.resize(2);
		pointsAccualSize.resize(2);
		blobDataList.resize(2);

		PXCBlobData::AccessOrderType accessOrder = PXCBlobData::ACCESS_ORDER_LARGE_TO_SMALL;
		accessOrder = PXCBlobData::ACCESS_ORDER_NEAR_TO_FAR;

		int validNumOfBlobs = 0;
		for (int i = 0; i < 4; i++)
		{
			// Blob is queried according to image segmentation type (color/depth) 
			PXCBlobData::IBlob* blob;
			PXCBlobData::SegmentationImageType segmentationType = PXCBlobData::SegmentationImageType::SEGMENTATION_IMAGE_COLOR;
			blobData->QueryBlob(i, segmentationType, accessOrder, blob);

			validNumOfBlobs += 1;
			blobDataList[i] = blob;

			if (blob->QuerySegmentationImage(image) == PXC_STATUS_NO_ERROR)
			{
				// Choose color according to blob order
				pxcI32 color = i + 1;
				//SetMask(image, color);

				//we don't know
				int numberOfContours = blob->QueryNumberOfContours();
				points[i].resize(numberOfContours);
				std::vector<int> blobPointsSize = pointsAccualSize[i];
				pointsAccualSize[i].resize(numberOfContours);
				////
				for (int j = 0; j < numberOfContours; ++j)
				{
					PXCBlobData::IContour* contour;
					if (blob->QueryContour(j, contour) == pxcStatus::PXC_STATUS_NO_ERROR)
					{
						pointsAccualSize[i].at(j) = 0;
						int contourSize = contour->QuerySize();
						points[i].at(j) = 0;
						if (contourSize > 0)
						{
							points[i].at(j) = new PXCPointI32[contourSize];
							contour->QueryPoints(contourSize, points[i].at(j));
							pointsAccualSize[i].at(j) = contourSize;
						}
					}

				}
			}
		}



		setColorImage(image);

		for (int i = 0; i < validNumOfBlobs; i++)
		{
			if (!blobDataList[i])
				continue;
			std::vector<PXCPointI32*> blobPoints = points[i];
			std::vector<int> blobPointsSize = pointsAccualSize[i];


			blobDataList[i]->QueryExtremityPoint(PXCBlobData::EXTREMITY_CENTER);
		}

		//clear contour points
		size_t pointsLen = points.size();
		for (int k = 0; k < pointsLen; ++k)
		{
			for (int h = 0; h < points[k].size(); ++h)
			{
				if (points[k].at(h) != NULL)
				{
					delete[] points[k].at(h);
					points[k].at(h) = NULL;
				}

			}
			points[k].clear();
		}
	}*/
};

int main(int argc, char* argv[])
{
	mApp app;

	app.initialize();
	app.run();
}



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