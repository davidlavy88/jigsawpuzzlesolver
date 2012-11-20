/*
 * FeatureDetectorAndMatcher.cpp
 *
 *  Created on: Oct 21, 2012
 *      Author: david
 */

#include "FeatureDetectorAndMatcher.h"

cv::Mat FeatureDetectorAndMatcher::SIFTPuzzle () {
	cv::Mat img_keypoints;
	/*std::ofstream txt_kpPuzzle;
	char* filename = new char[200];
	std::sprintf(filename, "Good_Matches_Piece_%d.txt", (int)idx);
	txt_kpPiece.open(filename);*/
	sift(Puzzle,cv::Mat(),keypointsImage,descriptorImg);
	cv::drawKeypoints(Puzzle,keypointsImage,img_keypoints,cv::Scalar::all(-1), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	/*Get keypoints location in a txt file
	 * txt_kpPuzzle.open("Puzzle_Keypoints.txt");
	 * for (size_t idx = 0; idx < keypointsImage.size(); idx++) {
		txt_kpPuzzle << "Keypoints " << idx << ": " << keypointsImage[idx].pt.x << " " <<
				keypointsImage[idx].pt.y << std::endl;
	}*/
	return img_keypoints;

}


std::vector<cv::Mat> FeatureDetectorAndMatcher::SIFTPiecesAndMatch() {
	std::vector<cv::Mat> pieces_keypoints;
	cv::Mat piece_keypoints;
	std::vector<int> query, train;
	cv::FileStorage fs("./config/Keypoints.yml",cv::FileStorage::WRITE);
	fs << "query" << "[";
	for (size_t idx = 0; idx < Pieces.size(); idx++) {
		keypointsPieces_temp.clear(); goodMatches_temp.clear(); matches_temp.clear();
		query.clear(); train.clear();
		sift(Pieces[idx],cv::Mat(),keypointsPieces_temp,descriptorPieces_temp);
		descriptorPieces.push_back(descriptorPieces_temp); keypointsPieces.push_back(keypointsPieces_temp);
		matcher.match(descriptorPieces[idx],descriptorImg,matches_temp);
		cv::drawKeypoints(Pieces[idx],keypointsPieces_temp,piece_keypoints,cv::Scalar::all(-1), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		pieces_keypoints.push_back(piece_keypoints);
		for (int i=0; i < descriptorPieces[idx].rows; i++)
		{
		  if (matches_temp[i].distance < 144.0)
		  {
			  goodMatches_temp.push_back(matches_temp[i]);
			  query.push_back(matches_temp[i].queryIdx);
			  train.push_back(matches_temp[i].trainIdx);
		  }
		}
		goodMatches.push_back(goodMatches_temp); matches.push_back(matches_temp);
		queryPieces.push_back(query); trainImage.push_back(train);
		fs << "{:" << "Piece";
		fs << query;
		fs << "}";
		cv::Mat imgMatches_temp;
		cv::drawMatches(Pieces[idx], keypointsPieces_temp, Puzzle, keypointsImage,
				  goodMatches_temp, imgMatches_temp, cv::Scalar::all(-1),
				  cv::Scalar::all(-1), std::vector<char>(),
				  cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		imgMatches.push_back(imgMatches_temp);
	}
	fs << "]";
	fs << "train" << "[";
	for( size_t idx = 0; idx < Pieces.size(); idx++ )
	{
		fs << "{:" << "Puzzle";
		fs << trainImage[idx];
		fs << "}";
	}
	fs << "]";
	fs.release();
	return pieces_keypoints;
}

void FeatureDetectorAndMatcher::testMatches() {
	//I need queryPieces, trainImage and goodMatches
	cv::FileStorage fs2("./config/Keypoints.yml", cv::FileStorage::READ);
	cv::FileNode query = fs2["query"]; cv::FileNode train = fs2["train"];
	cv::FileNodeIterator it = query.begin(), it_end = query.end();
	cv::FileNodeIterator it2 = train.begin(), it2_end = train.end();
	int idx = 0;
	std::vector<int> matchedPiece, matchedPuzzle;
	std::vector<cv::Point> finalpu_temp, finalpi_temp;
	for ( ; it != it_end; ++it, ++it2, idx++) {
		finalpu_temp.clear(); finalpi_temp.clear();
		(*it)["Piece"] >> matchedPiece;
		(*it2)["Puzzle"] >> matchedPuzzle;
		for (int i = 0; i < (int)matchedPuzzle.size(); i++) {
			finalpu_temp.push_back(keypointsImage[matchedPuzzle[i]].pt);
			finalpi_temp.push_back(keypointsPieces[idx][matchedPiece[i]].pt);
		}
		finalpu.push_back(finalpu_temp);
		finalpi.push_back(finalpi_temp);
	}
}
void FeatureDetectorAndMatcher::getMatchedKeypoints() {
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::KeyPoint> kpPiece, kpPuzzle;
	std::ofstream txt_matchPuzzle;
	char* filename = new char[200];
	txt_matchPuzzle.open("Matched_Keypoints_Puzzle.txt");
	for (size_t idx = 0; idx < Pieces.size(); idx++) {
		std::ofstream txt_matchPiece;
		good_matches = goodMatches[idx];
		std::sprintf(filename, "Matched_Keypoints_Piece_%d.txt", (int)idx);
		txt_matchPiece.open(filename);
		txt_matchPuzzle << "Piece number " << idx << std::endl << std::endl;
		for (int i=0; i<goodMatches[idx].size(); i++)
		{
			kpPiece.push_back(keypointsPieces[idx][good_matches[i].trainIdx]);
			txt_matchPiece << "Matched Keypoints " << i << ": " << kpPiece[i].pt.x << " " <<
					kpPiece[i].pt.y << std::endl;
		}
		keypMatchedPieces.push_back(kpPiece);
		for (int i=0; i<goodMatches[idx].size(); i++)
		{
			kpPuzzle.push_back(keypointsImage[good_matches[i].queryIdx]);
			txt_matchPuzzle << "Matched Keypoints " << i << ": " << kpPuzzle[i].pt.x << " " <<
								kpPuzzle[i].pt.y << std::endl;
		}
		keypMatchedPuzzle.push_back(kpPuzzle);
	}
}

cv::Mat FeatureDetectorAndMatcher::drawContourPiecesInPuzzle() {
	//Select 1 random matched keypoint from each piece, create the rectangle that contains it
	//If the rectangle is possible, match histograms to determine whether the piece belongs or not
	cv::Point ptPiece, ptPuzzle, topleft, topright, bottomleft, bottomright;
	cv::Mat subimage_puzzle, subimage_piece;
	drawContours = Puzzle;
	for (size_t idx = 0; idx < finalpu.size(); idx++) {
		srand ( time(NULL) );
		bool range = false;
		while (!range) {
			int i = rand()%finalpi[idx].size();
			std::cout << "Keypoint Piece: (" << finalpi[idx][i].x << "," << finalpi[idx][i].y << ")" << std::endl;
			std::cout << "Keypoint Puzzle: (" << finalpu[idx][i].x << "," << finalpu[idx][i].y << ")" << std::endl;
			int corner_x = finalpu[idx][i].x - finalpi[idx][i].x + 3; int corner_y = finalpu[idx][i].y - finalpi[idx][i].y + 3;
			int width = Pieces[idx].cols - 11; int height = Pieces[idx].rows;
			std::cout << "Keypoint n." << idx << std::endl;
			std::cout << "X:" << corner_x << " Y:" << corner_y << " Width:" << width << " Height: "<< height;
			if (corner_x < 0 || corner_y < 0) {
				std::cout << "Image unable to extract, the keypoint isn't the right one ... Trying another keypoint" << std::endl;
				continue;
			}
			else {
				range = true;
				subimage_puzzle = Puzzle(cv::Rect(corner_x,corner_y,width,height));
				/*cv::imshow("Puzzle",subimage_puzzle);
				cv::imshow("Piece",Pieces[idx]);
				cv::waitKey();*/
			}
			cv::Mat img1 = hc.colorReduce(Pieces[idx],32);
			cv::Mat img2 = hc.colorReduce(subimage_puzzle,32);
			cv::MatND shist1= hc.getHistogram(img1);
			cv::MatND shist2= hc.getHistogram(img2);
			double comphist = cv::compareHist(shist1,shist2,CV_COMP_BHATTACHARYYA);
			std::cout << "COMPHIST: " << comphist << std::endl;
			if (comphist > 0.51) {
				range = false;
				std::cout << "Image Histograms doesn't match ... Trying another keypoint" << std::endl;
				continue;
			}
			else {
				//Drawing contours
				cv::Point ptContour;
				std::vector<cv::Point> pieceContour;
				for (size_t i = 0; i < contoursPieces.size(); i++) {
					for (size_t j = 0; j < contoursPieces[i].size(); j++) {
						ptContour.x = contoursPieces[i][j].x - ptPiece.x;
						ptContour.y = contoursPieces[i][j].y - ptPiece.y;
					}
					pieceContour.push_back(ptContour);
				}
			contoursPuzzle.push_back(pieceContour);
			}
		}
		cv::drawContours(drawContours, contoursPuzzle, -1, cv::Scalar(255,255,255));
	}
	return drawContours;
}

	/*for (size_t idx = 0; idx < keypMatchedPieces.size(); idx++) {
		srand ( time(NULL) );
		bool range = false;
		while (!range) {
			int i = rand()%keypMatchedPieces[idx].size();
			std::cout << "Int:" << i << std::endl;
			//random();
			ptPuzzle = keypMatchedPuzzle[idx][i].pt;
			ptPiece = keypMatchedPieces[idx][i].pt;
			std::cout << "Keypoint Piece: (" << ptPiece.x << "," << ptPiece.y << ")" << std::endl;
			std::cout << "Keypoint Puzzle: (" << ptPuzzle.x << "," << ptPuzzle.y << ")" << std::endl;
			//Coordinates of the rectangle in the puzzle image
			int corner_x = ptPuzzle.x - ptPiece.x + 3; int corner_y = ptPuzzle.y - ptPiece.y + 3;
			int width = Pieces[idx].cols - 11; int height = Pieces[idx].rows;
			std::cout << "Keypoint n." << idx << std::endl;
			std::cout << "X:" << corner_x << " Y:" << corner_y << " Width:" << width << " Height: "<< height;
			if (corner_x < 0 || corner_y < 0) {
				std::cout << "Image unable to extract, the keypoint isn't the right one ... Trying another keypoint" << std::endl;
				continue;
			}
			else {
				range = true;
				subimage_puzzle = Puzzle(cv::Rect(corner_x,corner_y,width,height));
			}
			cv::Mat img1 = hc.colorReduce(Pieces[idx],32);
			cv::Mat img2 = hc.colorReduce(subimage_puzzle,32);
			cv::MatND shist1= hc.getHistogram(img1);
			cv::MatND shist2= hc.getHistogram(img2);
			double comphist = cv::compareHist(shist1,shist2,CV_COMP_BHATTACHARYYA);
			if (comphist > 0.25) {
				range = false;
				std::cout << "Image Histograms doesn't match ... Trying another keypoint" << std::endl;
				continue;
			}
			else {
				//Drawing contours
				cv::Point ptContour;
				std::vector<cv::Point> pieceContour;
				for (size_t i = 0; i < contoursPieces.size(); i++) {
					for (size_t j = 0; j < contoursPieces[i].size(); j++) {
						ptContour.x = contoursPieces[i][j].x - ptPiece.x;
						ptContour.y = contoursPieces[i][j].y - ptPiece.y;
					}
					pieceContour.push_back(ptContour);
				}
			contoursPuzzle.push_back(pieceContour);
			}
		}
		cv::drawContours(drawContours, contoursPuzzle, -1, cv::Scalar(122,25,215));
	}
}*/
