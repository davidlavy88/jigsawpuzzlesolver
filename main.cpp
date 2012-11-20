/*
 * main.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: david
 */

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <vector>
#include "ExtractPieces.h"
#include "FeatureDetectorAndMatcher.h"

int main( int argc, char** argv ) {
	ExtractPieces extractpieces;
	FeatureDetectorAndMatcher fdm;
	cv::Mat threshold, contourImage, puzzleKeypoints;
	std::vector<cv::Mat> piecesKeypoints, Pieces, pieceMatches;
	std::vector<std::vector<cv::Point> > newcontours;
	int index = 0;
	//Program
	if (argc==1){
	        std::cout << "No images to load!" << std::endl;
	        std::cin.get();
	        return 0;
	    }
	int image_num = argc-1;
	cv::Mat *img = new cv::Mat[image_num]; // allocates table on heap instead of stack
	// Load the images from command line:
	for (index = 0; index < image_num; index++) {
	  img[index] = cv::imread(argv[index+1]);
	  if (!img[index].data){
		  std::cout << "Image data not loaded properly" << std::endl;
		  std::cin.get();
		  return -1;
	  }
	}
	//img[0]: Pieces, img[1]: Puzzle
	extractpieces.setPiecesAndPuzzleImages(img[0],img[1]);
	threshold = extractpieces.thresholdPieces();
	contourImage = extractpieces.findAndDrawContours();
	extractpieces.minMaxPoints();
	Pieces = extractpieces.extractPieces();
	newcontours = extractpieces.new_contours;

	//Extract each piece from image and find SIFT Keypoints
	fdm.SetPiecesAndPuzzle(img[0],img[1],Pieces);
	fdm.setParametersToAssemble(newcontours);
	puzzleKeypoints = fdm.SIFTPuzzle();
	cv::imshow("Puzzle Keypoints",puzzleKeypoints);
	std::cout << "Number of keypoints" << fdm.keypointsImage.size() << std::endl;
	piecesKeypoints = fdm.SIFTPiecesAndMatch();
	pieceMatches = fdm.imgMatches;
	std::cout << "This: " << piecesKeypoints.size() << std::endl;
	for (size_t idx = 0; idx < Pieces.size(); idx++) {
		cv::imshow("Piece Keypoints",piecesKeypoints[idx]);
		cv::imshow("Matches between Puzzle and Pieces",pieceMatches[idx]);
		std::cout << "Number of keypoints in piece " << idx+1 << ": " <<
				fdm.keypointsPieces[idx].size() << std::endl;
		cv::waitKey();
	}
	//fdm.testMatches();
	//fdm.getMatchedKeypoints();
	//cv::Mat final = fdm.drawContourPiecesInPuzzle();


	//cv::imshow("Pieces and Puzzle",final);
	//cv::waitKey();
	cv::imshow("Input Image", img[0]);
	cvMoveWindow("Input Image", 0, 0);
	cv::imshow("Contours", contourImage);
	cvMoveWindow("Contours", 200, 0);
	cv::waitKey();

	return 0;
}
