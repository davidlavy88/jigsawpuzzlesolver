== jigsawpuzzlesolver

== Install prereqs

    # non-free opencv things are needed
    sudo apt-add-repository ppa:xqms/opencv-nonfree
    sudo apt-get -y install libopencv-dev
    sudo apt-get -y install libopencv-nonfree-dev

The program implements the following steps:
1. Threshold image 1, to have pieces isolated from the background.
2. Next, we find the contours around each piece, and storage in an array of contours
3. Extract each piece into single images, this is to perform, later, a matching method with the entire jigsaw puzzle.
4. Perform SIFT to each piece and the puzzle.
5. Matching SIFT Keypoints using FLANN (Explained below)
6. Filter out keypoints with greater euclidean distance
7. Positionate the pieces’ contours into the puzzle image (TO DO YET).
The steps 1,2 and 3 are performed by the file ExtractPieces.h and ExtractPieces.cpp Steps 4 - 7 are performed by the file FeatureDetectorAndMatcher.h and FeatureDetectorAndMatcher.cpp.
Step 7 is still in progress. The main idea is to select a random keypoint from the piece and find its matcher in the puzzle, then create the rectangle that contains the piece in the puzzle, and perform an histogram matcher to verify the rectangle is well positioned and we haven't chosen a mismatched keypoint.

