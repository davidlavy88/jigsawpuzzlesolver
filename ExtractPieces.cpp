/*
 * ExtractPieces.cpp
 *
 *  Created on: Oct 21, 2012
 *      Author: david
 */

#include "ExtractPieces.h"

cv::Mat ExtractPieces::thresholdPieces() {
	cv::cvtColor(Pieces, PiecesGray, CV_BGR2GRAY);
	cv::threshold(PiecesGray, BinarySegmented, 180, 200, CV_THRESH_BINARY_INV);
	BinarySegmented.convertTo(BinarySegmented,CV_8UC1);
	return BinarySegmented;
}

cv::Mat ExtractPieces::findAndDrawContours() {
	ColorContours = Pieces.clone();
	cv::findContours(BinarySegmented, contours, CV_RETR_EXTERNAL,
				  CV_CHAIN_APPROX_NONE );
	//Draw the contours
	contourImage = cv::Mat(PiecesGray.size(), CV_8UC3, cv::Scalar(0,0,0));//Black image
	cv::drawContours(contourImage, contours, -1, cv::Scalar(255,255,255));
	cv::drawContours(ColorContours, contours, -1, cv::Scalar(0,0,0));
	return contourImage;
}

void ExtractPieces::minMaxPoints() {
	cv::Point max, min;
	for (size_t i = 0; i < contours.size(); i++) {
		int max_x = 0; int max_y = 0;
		int min_x = 3000; int min_y = 3000;
		for (size_t j = 0; j < contours[i].size(); j++) {
			if (contours[i][j].x > max_x) max_x = contours[i][j].x;
			if (contours[i][j].x < min_x) min_x = contours[i][j].x;
			if (contours[i][j].y > max_y) max_y = contours[i][j].y;
			if (contours[i][j].y < min_y) min_y = contours[i][j].y;
		}
		max.x = max_x; max.y = max_y;
		min.x = min_x; min.y = min_y;
		PointMax.push_back(max);
		PointMin.push_back(min);
	}
}

std::vector<cv::Mat> ExtractPieces::extractPieces() {
	cv::Mat subimg;
	cv::Point ptContour;
	std::vector<cv::Point> pieceContour;
	cv::FileStorage fs("./config/Contours.yml",cv::FileStorage::WRITE);
	fs << "contours" << "[";
	for (size_t idx = 0; idx < PointMin.size(); idx++) {
		subimg = Pieces(cv::Rect(PointMin[idx].x - 3,PointMin[idx].y - 3,
				PointMax[idx].x + 8 - PointMin[idx].x,PointMax[idx].y + 8 - PointMin[idx].y));
		EachPiece.push_back(subimg);
		//new contours
		for (size_t i = 0; i < contours.size(); i++) {
			pieceContour.clear();
			for (size_t j = 0; j < contours[i].size(); j++) {
				ptContour.x = contours[i][j].x - PointMin[idx].x;
				ptContour.y = contours[i][j].y - PointMin[idx].y;
				pieceContour.push_back(ptContour);
			}
			new_contours.push_back(pieceContour);
		}
	}
	cv::Mat newcontourImage = cv::Mat(PiecesGray.size(), CV_8UC3, cv::Scalar(0,0,0));//Black image
	//cv::drawContours(newcontourImage, new_contours, -1, cv::Scalar(255,255,255));
	//cv::imshow("New Contours",newcontourImage);
	return EachPiece;
}
