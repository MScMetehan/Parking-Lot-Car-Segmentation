//#include "data_preparation.h"
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <cmath>
//
//using namespace cv;
//using namespace std;
//
//float pointDistance(Point p1, Point p2) {
//    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
//}
//
//float calculateLineAngle(Vec4i line) {
//    float angle = atan2(line[3] - line[1], line[2] - line[0]) * 180 / CV_PI; 
//    if (angle < 0) angle += 180; 
//    return angle;
//}
//
//bool areLinesParallel(float angle1, float angle2, float threshold = 5.0f) { /
//    return abs(angle1 - angle2) <= threshold || abs(abs(angle1 - angle2) - 180) <= threshold;
//}
//
//Mat preprocessImage(const Mat& image) {
//    Mat gray_image;
//    cvtColor(image, gray_image, COLOR_BGR2GRAY);
//
//    Mat adaptiveThresh;
//    adaptiveThreshold(gray_image, adaptiveThresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 9, -33);
//
//    Mat edges;
//    Canny(adaptiveThresh, edges, 150, 190); 
//
//    return edges;
//}
//
//vector<Vec4i> detectLines(const Mat& edges) {
//    vector<Vec4i> lines;
//    HoughLinesP(edges, lines, 1, CV_PI / 180, 10, 15, 10);
//    return lines;
//}
//
//vector<Vec4i> filterParallelLines(const vector<Vec4i>& lines) {
//    vector<Vec4i> parallelLines;
//    for (size_t i = 0; i < lines.size(); ++i) {
//        float angle1 = calculateLineAngle(lines[i]);
//        for (size_t j = i + 1; j < lines.size(); ++j) {
//            float angle2 = calculateLineAngle(lines[j]);
//            if (areLinesParallel(angle1, angle2)) {
//                parallelLines.push_back(lines[i]);
//                parallelLines.push_back(lines[j]);
//            }
//        }
//    }
//    return parallelLines;
//}
//
//void drawLines(Mat& image, const vector<Vec4i>& lines, Scalar color = Scalar(0, 255, 0), int thickness = 2) {
//    for (const auto& aline : lines) {
//        line(image, Point(aline[0], aline[1]), Point(aline[2], aline[3]), color, thickness);
//    }
//}
//
//vector<Vec4i> processImages(const vector<Mat>& images) {
//    vector<Vec4i> allFilteredLines;
//
//    for (const auto& image : images) {
//        
//        Mat edges = preprocessImage(image);
//        imshow("Canny Edges", edges);
//        waitKey(0);
//
//        vector<Vec4i> lines = detectLines(edges);
//
//        vector<Vec4i> parallelLines = filterParallelLines(lines);
//
//        allFilteredLines.insert(allFilteredLines.end(), parallelLines.begin(), parallelLines.end());
//    }
//
//    return allFilteredLines;
//}
