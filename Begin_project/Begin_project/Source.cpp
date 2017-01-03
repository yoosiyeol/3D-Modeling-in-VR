#include <opencv\cv.h>
#include <opencv2\highgui.hpp>
#include <opencv2/opencv.hpp>


#define VIDEO_FILE	"video.avi"
#define VIDEO_FORMAT	CV_FOURCC('M', 'J', 'P', 'G')
#define NUM_FINGERS	5
#define NUM_DEFECTS	8
#define THRESH 50

#define RED     CV_RGB(255, 0, 0)
#define GREEN   CV_RGB(0, 255, 0)
#define BLUE    CV_RGB(0, 0, 255)
#define YELLOW  CV_RGB(255, 255, 0)
#define PURPLE  CV_RGB(255, 0, 255)
#define GREY    CV_RGB(200, 200, 200)


struct ctx {
	CvCapture	*capture;	 //Capture handle 
	CvVideoWriter	*writer;	 // File recording handle 

	IplImage	*image;			   	// Input image 
	IplImage *skin_image;		 //skin_defector image
	IplImage *ContourImage;  
	IplImage	*thr_image;			// After filtering and thresholding 
	IplImage	*temp_image1;	// Temporary image (1 channel) 
	IplImage	*temp_image3;	// Temporary image (3 channels) 

	CvSeq		*contour;	// Hand contour 
	CvSeq		*hull;		// Hand convex hull 

	CvPoint		hand_center;
	CvPoint		*fingers;	// Detected fingers positions 
	CvPoint		*defects;	// Convexity defects depth points 

	CvMemStorage	*hull_st;
	CvMemStorage	*temp_st;
	CvMemStorage	*defects_st;
	CvMemStorage  *ContourSt;

	IplConvKernel	*kernel;	// Kernel for morph operations 

	int		num_fingers;
	int		hand_radius;
	int		num_defects;
};

void init_capture(struct ctx *ctx)
{
	ctx->capture = cvCaptureFromCAM(0);
	if (!ctx->capture) {
		fprintf(stderr, "Error initializing capture\n");
		exit(1);
	}
	ctx->image = cvQueryFrame(ctx->capture);

}

void init_recording(struct ctx *ctx)
{
	int fps, width, height;

	fps = cvGetCaptureProperty(ctx->capture, CV_CAP_PROP_FPS);
	width = cvGetCaptureProperty(ctx->capture, CV_CAP_PROP_FRAME_WIDTH);
	height = cvGetCaptureProperty(ctx->capture, CV_CAP_PROP_FRAME_HEIGHT);

	if (fps < 0)
		fps = 10;

	ctx->writer = cvCreateVideoWriter(VIDEO_FILE, VIDEO_FORMAT, fps,
		cvSize(width, height), 1);

	if (!ctx->writer) {
		fprintf(stderr, "Error initializing video writer\n");
		exit(1);
	}
}

void init_windows(void)
{
	cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("output", 50, 50);
	cvMoveWindow("Test", 700, 50);
}

void init_ctx(struct ctx *ctx)
{
	ctx->skin_image = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	ctx->ContourImage = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	
	ctx->thr_image = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	ctx->temp_image1 = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	ctx->temp_image3 = cvCreateImage(cvGetSize(ctx->image), 8, 3);
	ctx->kernel = cvCreateStructuringElementEx(9, 9, 4, 4, CV_SHAPE_RECT,
		NULL);
	ctx->ContourSt = cvCreateMemStorage(0);
	ctx->hull_st = cvCreateMemStorage(0);
	ctx->temp_st = cvCreateMemStorage(0);
	ctx->fingers = (CvPoint*)calloc(NUM_FINGERS + 1, sizeof(CvPoint));
	ctx->defects = (CvPoint*)calloc(NUM_DEFECTS, sizeof(CvPoint));
}

void display(struct ctx *ctx)
{
	
	int i;
	cvCircle(ctx->image, ctx->hand_center, 20, CvScalar(0, 255, 0), 2);
	cvCircle(ctx->image, ctx->hand_center, 5, PURPLE, 1, CV_AA, 0);
	cvCircle(ctx->image, ctx->hand_center, ctx->hand_radius,
		RED, 1, CV_AA, 0);
		
	
	for (i = 0; i < ctx->num_fingers; i++) {

		cvCircle(ctx->image, ctx->fingers[i], 10,
			GREEN, 3, CV_AA, 0);

		cvLine(ctx->image, ctx->hand_center, ctx->fingers[i],
			YELLOW, 1, CV_AA, 0);
	}

	for (i = 0; i < ctx->num_defects; i++) {
		cvCircle(ctx->image, ctx->defects[i], 2,
			GREY, 2, CV_AA, 0);
	}

	cvShowImage("output", ctx->image);
	//cvShowImage("thresholded", ctx->thr_image);

	//cvShowImage("output", ctx->image);
	//cvShowImage("Test", ctx->thr_image);
}

void skin_detect(struct ctx *ctx) {

	//Declaration Image ( 1 channel and 3 channel)
	IplImage* skin = cvCreateImage(cvGetSize(ctx->image), 8, 3);
	IplImage* single_skin = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	IplImage* final_skin = cvCreateImage(cvGetSize(ctx->image), 8, 1);

	//Declartion Filter
	int filterSize = 3;
	int ErodeSize = 7;
	int OpenSize = 5;
	IplConvKernel *convKernel = cvCreateStructuringElementEx(filterSize, filterSize, (filterSize - 1) / 2, (filterSize - 1) / 2, CV_SHAPE_RECT, NULL);
	IplConvKernel *convErodeKernel = cvCreateStructuringElementEx(ErodeSize, ErodeSize, (ErodeSize - 1) / 2, (ErodeSize - 1) / 2, CV_SHAPE_RECT, NULL);
	IplConvKernel *convOpenKernel = cvCreateStructuringElementEx(OpenSize, OpenSize, (OpenSize - 1) / 2, (OpenSize - 1) / 2, CV_SHAPE_RECT, NULL);

	//Declartion  Low Range
	int LowH = 0;
	int LowS = 133;
	int LowV = 77;
	
	//Declartion High Range
	int HighH = 255;
	int HighS = 173;
	int HighV = 127;


	// Get YCbCr model Data and Converted image
	cvCvtColor(ctx->image, skin, CV_BGR2YCrCb);
	cvInRangeS(skin, cvScalar(LowH, LowS, LowV), cvScalar(HighH, HighS, HighV), single_skin);


	//Improving quality of image
	/*cvErode(single_skin, final_skin, convErodeKernel);
	cvMorphologyEx(final_skin, final_skin, NULL,convOpenKernel, CV_MOP_OPEN);
	cvMorphologyEx(final_skin, final_skin, NULL, convKernel, CV_MOP_CLOSE);
	*/
	/*
	cvNamedWindow("Aest");
	cvShowImage("Aest", final_skin);
	*/
	cvMorphologyEx(single_skin, final_skin, NULL, convKernel, CV_MOP_CLOSE);

	cvCopy(final_skin, ctx->skin_image);
	cvReleaseImage(&skin);
	cvReleaseImage(&final_skin);
	cvReleaseImage(&single_skin);
}

void findHandContour(struct ctx *ctx)
{

	IplImage *g_gray = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	IplImage *g_binary = cvCreateImage(cvGetSize(ctx->image), 8, 1);
	IplImage *dst = cvCreateImage(cvGetSize(ctx->image), 8, 1);

	double area, max_area = 0.0;
	CvSeq *contours, *tmp, *contour = NULL;

	int Conturs = 0;

	//-----------------------------------Find Contours----------------//
	if (ctx->ContourSt == NULL) {
		ctx->ContourSt = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(ctx->ContourSt);
	}
	//g_image영상을 BRG색공간을 그레이 스케일로 변환(BGR to Gray = BGR2GRAY)
	cvCvtColor(ctx->image, g_gray, CV_BGR2GRAY);
	//임계값 이하:0, 임계값초과값:1 설정
	cvThreshold(g_gray, g_gray, THRESH, 255, CV_THRESH_BINARY);
	cvCopy(g_gray, g_binary);
	
	
	//윤곽선 찾기
	cvFindContours(
		//ctx->skin_image,                //입력영상
	    g_gray,
		ctx->ContourSt,             //검출된 외곽선을 기록하기 위한 메모리 스토리지
		&contours,             //외곽선의 좌표들이 저장될 Sequence
		sizeof(CvContour),
		CV_RETR_TREE,           //어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0, 0)
	);
	
	/* Select contour having greatest area */
	for (tmp = contours; tmp; tmp = tmp->h_next) {
		area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
		if (area > max_area) {
			max_area = area;
			contour = tmp;
		}
	}

	/* Approximate contour with poly-line */
	if (contour) {
		contour = cvApproxPoly(contour, sizeof(CvContour),
			ctx->ContourSt, CV_POLY_APPROX_DP, 2,
			1);
		ctx->contour = contour;
	}

	cvZero(g_gray);
	if (ctx->contour) {
		//외곽선을 찾은 정보(contour)를 이용하여 외곽선을 그림
		cvDrawContours(
			g_gray,                //외곽선이 그려질 영상
			ctx->contour,              //외곽선 트리의 루트노드
			cvScalarAll(255),      //외부 외곽선의 색상
			cvScalarAll(128),      //내부 외곽선의 색상
			100                    //외곽선을 그릴때 이동할 깊이
		);
	}
	cvShowImage("Test", g_gray);
	//cvSmooth Section
	cvCopy(g_gray, dst);
	cvZero(g_gray);
	cvSmooth(dst, g_gray, CV_BLUR, 3);

	cvCopy(g_gray, ctx->ContourImage);

	cvReleaseImage(&g_gray);
	cvReleaseImage(&dst);
}

void trackingSkin(struct ctx *ctx)
{
	int i, j;
	int index;
	int nCount = 1;
	int DrawX = 0;
	int DrawY = 0;

	//Declartion  Low Range
	int LowH = 0;
	int LowS = 133;
	int LowV = 77;

	//Declartion High Range
	int HighH = 255;
	int HighS = 173;
	int HighV = 127;


	cvCvtColor(ctx->image, ctx->temp_image3, CV_BGR2YCrCb);


	for (i = 0; i < ctx->temp_image3->height; i++) {
		for (j = 0; j < ctx->temp_image3->widthStep; j += ctx->temp_image3->nChannels) {
			index = i*ctx->temp_image3->widthStep + j;
			//RGB 순으로 최소 최하 값을 검출
			if (((unsigned char)ctx->temp_image3->imageData[index + 2] > LowH)
				&& ((unsigned char)ctx->temp_image3->imageData[index + 1] > LowS)
				&& ((unsigned char)ctx->temp_image3->imageData[index] > LowV)
				&& ((unsigned char)ctx->temp_image3->imageData[index + 2] < HighH)
				&& ((unsigned char)ctx->temp_image3->imageData[index + 1] < HighS)
				&& ((unsigned char)ctx->temp_image3->imageData[index] < HighV)) {
				/*if (((unsigned char)ctx->temp_image3->imageData[index + 2] > 200)
				&& ((unsigned char)ctx->temp_image3->imageData[index + 1] > 200)
				&& ((unsigned char)ctx->temp_image3->imageData[index] > 200)){*/
				nCount++;
				DrawX += j / 3;
				DrawY += i;
			}
		}
	}
	DrawX /= nCount;
	DrawY /= nCount;
	//if (nCount > 1)	cvCircle(ctx->image, cvPoint(DrawX, DrawY), 10, CvScalar(0, 255, 0), 2);
	printf("Circle !!!: %d %d \n", DrawX, DrawY);
	
	ctx->hand_center.x = DrawX;
	ctx->hand_center.y = DrawY;

	//Test Section
	//cvShowImage("Test", ctx->temp_image3);

	cvZero(ctx->temp_image3);


}

void find_convex_hull(struct ctx *ctx)
{
	CvSeq *defects;
	CvConvexityDefect *defect_array;
	int i;
	int x = 0, y = 0;
	int dist = 0;

	ctx->hull = NULL;

	if (!ctx->contour)
		return;

	ctx->hull = cvConvexHull2(ctx->contour, ctx->hull_st, CV_CLOCKWISE, 0);

	if (ctx->hull) {

		/* Get convexity defects of contour w.r.t. the convex hull */
		defects = cvConvexityDefects(ctx->contour, ctx->hull,
			ctx->defects_st);

		if (defects && defects->total) {
			defect_array = (CvConvexityDefect*)calloc(defects->total,
				sizeof(CvConvexityDefect));
			cvCvtSeqToArray(defects, defect_array, CV_WHOLE_SEQ);

			/* Average depth points to get hand center */
			
			for (i = 0; i < defects->total && i < NUM_DEFECTS; i++) {
				x += defect_array[i].depth_point->x;
				y += defect_array[i].depth_point->y;
				ctx->defects[i] = cvPoint(defect_array[i].depth_point->x,
					defect_array[i].depth_point->y);
			}

			x /= defects->total;
			y /= defects->total;

			ctx->num_defects = defects->total;
			ctx->hand_center = cvPoint(x, y);

			printf("%d %d\n", ctx->hand_center.x,ctx->hand_center.y);
			
			/* Compute hand radius as mean of distances of
			defects' depth point to hand center */
			for (i = 0; i < defects->total; i++) {
				int d = (x - defect_array[i].depth_point->x) *
					(x - defect_array[i].depth_point->x) +
					(y - defect_array[i].depth_point->y) *
					(y - defect_array[i].depth_point->y);

				dist += sqrt(d);
			}

			ctx->hand_radius = dist / defects->total;
			free(defect_array);
		}
	}
}

void find_fingers(struct ctx *ctx)
{
	int n;
	int i;
	CvPoint *points;
	CvPoint max_point;
	int dist1 = 0, dist2 = 0;

	ctx->num_fingers = 0;

	if (!ctx->contour || !ctx->hull)
		return;

	n = ctx->contour->total;
	points = (CvPoint*)calloc(n, sizeof(CvPoint));

	cvCvtSeqToArray(ctx->contour, points, CV_WHOLE_SEQ);

	/*
	* Fingers are detected as points where the distance to the center
	* is a local maximum
	*/
	for (i = 0; i < n; i++) {
		int dist;
		int cx = ctx->hand_center.x;
		int cy = ctx->hand_center.y;

		dist = (cx - points[i].x) * (cx - points[i].x) +
			(cy - points[i].y) * (cy - points[i].y);

		if (dist < dist1 && dist1 > dist2 && max_point.x != 0
			&& max_point.y < cvGetSize(ctx->image).height - 10) {

			ctx->fingers[ctx->num_fingers++] = max_point;
			if (ctx->num_fingers >= NUM_FINGERS )
				break;
		}

		dist2 = dist1;
		dist1 = dist;
		max_point = points[i];
	}
	printf("Fingers NUM!!!:  %d\n", ctx->num_fingers);
	free(points);

}

void ruler_func(struct ctx *ctx)
{
	IplImage *ruler = cvLoadImage("ruler.bmp", 1);
	//cvAdd(ctx->image, ruler, ctx->image, NULL);

	CvRect ruler_rect= cvRect(600, 400, ruler->width, ruler->height);
	
}



int main() {
	struct ctx ctx = {};
	int key;

	init_capture(&ctx);
	init_recording(&ctx);
	init_windows();
	init_ctx(&ctx);

	do {
		ctx.image = cvQueryFrame(ctx.capture);
		cvFlip(ctx.image, 0, 1);				//image를 좌우 반전


		//---------Test Skin_detection Section----------//
		
		skin_detect(&ctx);
		find_convex_hull(&ctx);
		//cvShowImage("Test", ctx.skin_image);
		//-------------------------------//
		
		trackingSkin(&ctx);
		//---------Test findHandContour Section----------//
		
		findHandContour(&ctx);
		//cvShowImage("Test", ctx.ContourImage);
		//-------------------------------//
		
		//---------Test skinTracking Section----------//
		
		//trackingSkin(&ctx);
		//---------Test find_fingers Section------------//
		find_fingers(&ctx);
		
		ruler_func(&ctx);

		display(&ctx);
		cvWriteFrame(ctx.writer, ctx.image);

		key = cvWaitKey(1);
	}   while (1);

	return 0;
}