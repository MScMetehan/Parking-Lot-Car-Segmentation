//data_preparation.h

#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "data_preparation.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

struct BoundingBox {
    cv::RotatedRect rect;
    bool occupied;
};

void saveBoundingBoxesToXML(const std::string& filePath, const std::vector<BoundingBox>& boundingBoxes);

std::vector<cv::Mat> load_images(const std::string& directory);

std::vector<std::vector<BoundingBox>> load_bounding_boxes(const std::string& directory);

std::vector<cv::Mat> load_masks(const std::string& directory);

void load_sequences(const std::vector<std::string>& image_dirs, const std::vector<std::string>& bbox_dirs,std::vector<std::vector<cv::Mat>>& all_images, std::vector<std::vector<std::vector<BoundingBox>>>& all_bounding_boxes);

std::vector<std::vector<cv::Mat>> load_all_masks(const std::vector<std::string>& directories);

void draw_rotated_rects_with_ids(cv::Mat& img, const std::vector<BoundingBox>& bboxes, const std::string& savePath, int imageIndex);

void apply_segmentation_mask(const cv::Mat& image, const cv::Mat& mask, const std::string& savePath, int imageIndex);


void applyThreshold(Mat& inputImage, double thresholdValue);


void removeNoiseAndKeepCars(Mat& binaryImage);

void preprocessImages(const vector<Mat>& inputImages, vector<Mat>& outputImages, double alpha, int beta);

void drawBoundingBoxes(Mat& image, const vector<BoundingBox>& detectedRegions, const vector<int>& ids, const Scalar& color);

int extractIDFromFilename(const string& filename);

bool isBoundingBoxOccupied(const Mat& binaryImage, const RotatedRect& boundingBox, double whitePixelThreshold);

std::vector<cv::Mat> loadImagesFromDirectory(const std::string& directory);

cv::Mat createAverageBackground(const std::vector<cv::Mat>& images);

std::vector<BoundingBox> detectParkingSpaces(const cv::Mat& firstImage, const std::vector<cv::Mat>& templates, double threshold, std::vector<int>& ids);

