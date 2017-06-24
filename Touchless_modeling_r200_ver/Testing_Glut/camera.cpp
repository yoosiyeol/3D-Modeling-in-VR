#include "mOpenCv.h"

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
	cv::Mat tempBlob = cv::Mat::zeros(DepthSize, CV_8UC1);
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
	//std::cout << numOfBlobs << std::endl;
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

		//temp에 i번쨰 블롭을 복사 후 Input에 겹쳐넣기
		PXCImage2CVMat(blobImage, PXCImage::PIXEL_FORMAT_ANY).copyTo(tempBlob);
		cv::addWeighted(*input, 1.0, tempBlob, 1.0, 0.0, *input);
\
		// Release Access Blob 
		blobImage->ReleaseAccess(&data);
	}
	//cv::erode(*input, *input, cv::Mat());
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
	depthRange.max = 800.f;
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
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, DepthSize.width, DepthSize.height, frameRate);


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
	
	//cv::Mat BlobROI = ColorIm(cv::Rect(10, 10, BlobROI.cols, BlobROI.rows));
	//cv::addWeighted((*Input), 1.0, BlobROI, 0.3, 0.0, (*Input));

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
