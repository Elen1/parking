#ifndef _EDGE_DETECTOR_H__
#define _EDGE_DETECTOR_H__

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

namespace app
{
	class EdgeDetector
	{
	public:
		EdgeDetector(cv::Mat& image);
		~EdgeDetector();

	public:

		cv::Mat operator()();

	private:
		cv::Mat& image_;
		cv::Mat src_gray_;
		cv::Mat dst_, detected_edges_;
		const int edgeThresh = 1;
		const int lowThreshold = 60;
		const int max_lowThreshold = 100;
		const int ratio = 3;
		const int kernel_size = 3;
	};
}



#endif // !_EDGE_DETECTOR_H__

