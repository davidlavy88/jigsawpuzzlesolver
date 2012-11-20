/*
 * FeatureDetectorAndMatcher.h
 *
 *  Created on: Oct 21, 2012
 *      Author: david
 */

#ifndef FEATUREDETECTORANDMATCHER_H_
#define FEATUREDETECTORANDMATCHER_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include "colorHistogram.h"

class FeatureDetectorAndMatcher {
private:
	ColorHistogram hc;
	cv::Mat ImgPieces, Puzzle;
	std::vector<cv::Mat> descriptorPieces, Pieces;
	cv::Mat descriptorPieces_temp;
	std::vector<cv::KeyPoint> keypointsPieces_temp;
	cv::SIFT sift;
	cv::FlannBasedMatcher matcher;
	cv::Mat descriptorImg, drawContours;
	std::vector<cv::DMatch> matches_temp, goodMatches_temp;
	std::vector<std::vector<cv::Point> > contoursPieces, contoursPuzzle;
	std::vector<cv::Point> OriginPieces;

public:
	FeatureDetectorAndMatcher() {
	};

	std::vector<std::vector<int> > queryPieces, trainImage;
	std::vector<std::vector<cv::Point> > finalpu, finalpi;
	std::vector<cv::KeyPoint> keypointsImage;
	std::vector<std::vector<cv::KeyPoint> > keypointsPieces, keypMatchedPieces, keypMatchedPuzzle;
	std::vector<std::vector<cv::DMatch> > matches, goodMatches;
	std::vector<cv::Mat> imgMatches;
	cv::Mat image1, image2;;
	void SetPiecesAndPuzzle(cv::Mat imgpieces, cv::Mat puzzle, std::vector<cv::Mat> pieces) {
		ImgPieces = imgpieces;
		Puzzle = puzzle;
		Pieces = pieces;
	};
	cv::Mat SIFTPuzzle();
	std::vector<cv::Mat> SIFTPiecesAndMatch();
	void getMatchedKeypoints();
	void setParametersToAssemble(std::vector<std::vector<cv::Point> > contours) {//,
			//std::vector<cv::Point> origin) {
		contoursPieces = contours;
		//OriginPieces = origin;
	};
	cv::Mat drawContourPiecesInPuzzle();
	void testMatches();
	//void Match();

};

#endif /* FEATUREDETECTORANDMATCHER_H_ */
