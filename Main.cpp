//#include <iostream>
//#include <string>
//#include <opencv2/opencv.hpp>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "EdgeDetector.h"
#include <memory>
#include "Point.h"
#include "Rectangle.h"

using namespace cv;


namespace
{

	std::string empty_parking_path = "AutoParkingHelper/input/capture1.png";
	std::string real_parking_path = "AutoParkingHelper/input/real_time.PNG";
	std::string parking_piece_path = "AutoParkingHelper/input/capture.png";

	const char* image_window = "Source Image";
	const char* result_window = "Result window";


	Mat result;
	int match_method;
	
}

void max(Mat& src);
void find(Mat& src,Mat& piece);

std::vector<app::Point> MatchingMethod(Mat& img,Mat& templ)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	Point minLoc; Point maxLoc;
	Point matchLoc;
	double minVal; double maxVal;

	Mat general_mask = Mat::ones(result.rows, result.cols, CV_8UC1);
	std::vector<app::Point> points;
	for (int k = 0; k < 22; ++k)// look for N=5 objects
	{
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, general_mask);
		//just to visually observe centering I stay this part of code:
		result.at<float>(minLoc) = 1.0;//
		result.at<float>(maxLoc) = 0.0;//

		// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. 
		 //For all the other methods, the higher the better
		if (match_method == 0 || match_method == 1)
			matchLoc = minLoc;
		else
			matchLoc = maxLoc;
		float k_overlapping = 1.7f;//little overlapping is good for my task

		//create template size for masking objects, which have been found,
		//to be excluded in the next loop run
		int template_w = ceil(k_overlapping*templ.cols);
		int template_h = ceil(k_overlapping*templ.rows);
		int x = matchLoc.x - template_w / 2;
		int y = matchLoc.y - template_h / 2;

		//shrink template-mask size to avoid boundary violation
		if (y < 0) y = 0;
		if (x < 0) x = 0;
		//will template come beyond the mask?:if yes-cut off margin; 
		if (template_w + x > general_mask.cols)
			template_w = general_mask.cols - x;
		if (template_h + y > general_mask.rows)
			template_h = general_mask.rows - y;

		//set the negative mask to prevent repeating
		Mat template_mask = Mat::zeros(template_h, template_w, CV_8UC1);
		template_mask.copyTo(general_mask(cv::Rect(x, y, template_w, template_h)));

		/// Show me what you got on main image and on result (
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		//small correction here-size of "result" is smaller
		rectangle(result, Point(matchLoc.x - templ.cols / 2, matchLoc.y - templ.rows / 2), Point(matchLoc.x + templ.cols / 2, matchLoc.y + templ.rows / 2), Scalar::all(0), 2, 8, 0);
		points.push_back(app::Point(matchLoc.y + templ.rows / 2, matchLoc.x + templ.cols / 2));
	}//
	/*imshow(image_window, img_display);
	imshow(result_window, result);*/
	return points;
}

void draw_points(Mat& src, std::vector<app::Point>& points) {

	for (size_t i = 0; i < points.size(); i++)
	{
		int x = points[i].get_x();
		int y = points[i].get_y();

		src.at<cv::Vec3b>(x, y)[0] = 0;
		src.at<cv::Vec3b>(x, y)[1] = 0;
		src.at<cv::Vec3b>(x, y)[2] = 0;

	}

}

std::vector<app::Rectangle> get_empty_places(Mat& src,std::vector<app::Point>& points) {

	int rect_width = 40;
	int rect_height = 80;

	std::vector<app::Rectangle> result;

	for (size_t i = 0; i < points.size(); i++)
	{
		auto current_point = points[i];
		current_point.set_x(current_point.get_x());
		current_point.set_y(current_point.get_y() - 45);
		result.push_back(app::Rectangle(current_point,rect_width,rect_height));
	}

	/*for (size_t k = 0; k < result.size(); k++)
	{

		int size_x = result[k].get_left_top().get_x();
		int size_y = result[k].get_left_top().get_y();

		for (size_t i = size_x; i < result[k].get_width() + size_x; i++)
		{
			for (size_t j = size_y; j < result[k].get_height() + size_y; j++)
			{
				if (i < src.rows && j < src.cols) {
					src.at<Vec3b>(i, j)[0] = 0;
					src.at<Vec3b>(i, j)[1] = 0;
					src.at<Vec3b>(i, j)[2] = 0;
				}
			}
		}
	}*/
	return result;
}

void threshold(Mat& src)
{
	for (size_t i = 0; i < src.rows; i++)
	{
		for (size_t j = 0; j < src.cols; j++)
		{
			if (src.at<Vec3b>(i,j)[0] > 100) {
				src.at<Vec3b>(i, j)[0] = 255;
				src.at<Vec3b>(i, j)[1] = 255;
				src.at<Vec3b>(i, j)[2] = 255;
			}
			else {
				src.at<Vec3b>(i, j)[0] = 0;
				src.at<Vec3b>(i, j)[1] = 0;
				src.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}
}

std::vector<int> get_descriptors(Mat& src,std::vector<app::Rectangle> empty_places) {
	std::vector<int> result;
	for (size_t k = 0; k < empty_places.size(); k++)
	{

		int size_x = empty_places[k].get_left_top().get_x();
		int size_y = empty_places[k].get_left_top().get_y();

		
		int count = 0;
		for (size_t i = size_x; i < empty_places[k].get_width() + size_x; i++)
		{
			
			for (size_t j = size_y; j < empty_places[k].get_height() + size_y; j++)
			{
				if (i < src.rows && j < src.cols) {
					if (src.at<Vec3b>(i,j)[0] > 250) {
						count++;
					}
				}
			}
			
		}
		result.push_back(count);
	}
	return result;
}

int main() {

	Mat src, src_gray, result;
	int match_method;

	src = imread(empty_parking_path);
	Mat parking_piece = imread(parking_piece_path);
	Mat real_parking = imread(real_parking_path);
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	app::EdgeDetector edge_parking(src);
	app::EdgeDetector edge_real_parking(real_parking);
	app::EdgeDetector edge(parking_piece);

	Mat dst_parking = edge_parking();
	Mat dst_piece = edge();	
	Mat dst_real_parking = edge_real_parking();
	


	std::vector<app::Point> points = MatchingMethod(dst_parking,dst_piece);

	//draw_points(src,points);
	std::vector<app::Rectangle> empty_places = get_empty_places(src,points);

	threshold(dst_parking);
	threshold(dst_real_parking);

	std::vector<int> empty_descriptors = get_descriptors(dst_parking,empty_places);
	std::vector<int> real_descriptors = get_descriptors(dst_real_parking,empty_places);

	//std::vector<app::Rectangle> rects;

	for (size_t i = 0; i < empty_descriptors.size(); i++)
	{
		if (std::abs(real_descriptors[i] - empty_descriptors[i]) > 10) {
			empty_places[i].set_state(false);
		}
	}

	for (size_t k = 0; k < empty_places.size(); k++)
	{

		int size_x = empty_places[k].get_left_top().get_x();
		int size_y = empty_places[k].get_left_top().get_y();

		for (size_t i = size_x; i < empty_places[k].get_width() + size_x; i++)
		{
			for (size_t j = size_y; j < empty_places[k].get_height() + size_y; j++)
			{
				if (i < src.rows && j < src.cols) {
					if (i == size_x || j == size_y || i == size_x + empty_places[k].get_width() - 1
						|| j == size_y + empty_places[k].get_height() - 1) {
						if (!(empty_places[k].isEmpty())) {
							real_parking.at<Vec3b>(i, j)[0] = 0;
							real_parking.at<Vec3b>(i, j)[1] = 0;
							real_parking.at<Vec3b>(i, j)[2] = 255;
						}
						else
						{
							real_parking.at<Vec3b>(i, j)[0] = 0;
							real_parking.at<Vec3b>(i, j)[1] = 255;
							real_parking.at<Vec3b>(i, j)[2] = 0;
						}					
					}
					
				}
			}
		}
	}

	imwrite("AutoParkingHelper/input/result.png", real_parking);
	namedWindow("image2", WINDOW_NORMAL);
	cv::imshow("image2", real_parking);
	waitKey(0);
	return 0;
}

void max(Mat& src) {

	std::vector<std::vector<int>> result;
	result.resize(src.rows);
	for (size_t i = 0; i < result.size(); i++)
	{
		result[i].resize(src.cols);
	}

	for (size_t i = 1; i < src.rows - 1; i++)
	{
		for (size_t j = 1; j < src.cols - 1; j++)
		{

			std::vector<int> kernel;

			for (size_t k = i - 1; k <= i+1; k++)
			{
				for (size_t l = j - 1; l <= j + 1; l++)
				{
					kernel.push_back(src.at<Vec3b>(k,l)[0]);
				}
			}

			auto max = std::max_element(kernel.begin() , kernel.end());

			result[i][j] = *max;

		}
	}

	for (size_t i = 0; i < src.rows; i++)
	{
		for (size_t j = 0; j < src.cols; j++)
		{
			src.at<Vec3b>(i, j)[0] = result[i][j];
			src.at<Vec3b>(i, j)[1] = result[i][j];
			src.at<Vec3b>(i, j)[2] = result[i][j];
		}
	}

}

