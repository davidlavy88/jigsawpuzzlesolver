/*
 * ExtractPieces.h
 *
 *  Created on: Oct 21, 2012
 *      Author: david
 */

#ifndef EXTRACTPIECES_H_
#define EXTRACTPIECES_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <vector>

class ExtractPieces {
private:
	//Pieces & Puzzle = Input color images
	//PiecesGray & PuzzleGray = Input gray-color images
	//BinarySegmented = Segmented Pieces
	//ColorContours = Input color image with contours pieces
	//contourImage = B&W image with contour pieces
	cv::Mat Pieces, PiecesGray, Puzzle, PuzzleGray, contourImage, BinarySegmented, ColorContours;
	std::vector<std::vector<cv::Vec3f> > pixel_contour;//Contour's pixel of each piece
	std::vector<cv::Point> PointMax;//Boundaries of the rectangle that contains each piece
	std::vector<cv::Mat> EachPiece;

public:
	ExtractPieces() {};

	void setPiecesAndPuzzleImages(cv::Mat pieces, cv::Mat puzzle) {
		Pieces = pieces;
		Puzzle = puzzle;
	};

	std::vector<std::vector<cv::Point> > new_contours;
	std::vector<std::vector<cv::Point> > contours; //Contours point of each piece
	std::vector<cv::Point> PointMin;
	cv::Mat thresholdPieces();
	cv::Mat findAndDrawContours();
	void minMaxPoints();/*const std::vector<std::vector<cv::Point> > &contours, std::vector<cv::Point> &PointMax, std::vector<cv::Point> &PointMin*/
	std::vector<cv::Mat> extractPieces();/*const cv::Mat &image, const std::vector<cv::Point> &PointMax, const std::vector<cv::Point> &PointMin, std::vector<cv::Mat> &Pieces*/
	std::vector<std::vector<cv::Point> > newContours() {
		return new_contours;
	}

};

#endif /* EXTRACTPIECES_H_ */
