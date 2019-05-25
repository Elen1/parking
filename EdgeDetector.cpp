#include "EdgeDetector.h"
using namespace cv;
using namespace app;


EdgeDetector::EdgeDetector(cv::Mat& image)
	:image_(image)
{
	dst_.create(image_.size(), image_.type());
	cvtColor(image_, src_gray_, COLOR_BGR2GRAY);
}


EdgeDetector::~EdgeDetector()
{
}

Mat EdgeDetector::operator()() {
	

	blur(src_gray_, detected_edges_, Size(3, 3));

	Canny(detected_edges_, detected_edges_, lowThreshold, lowThreshold*ratio, kernel_size);

	dst_ = Scalar::all(0);

	image_.copyTo(dst_, detected_edges_);
	return dst_;
}