//data_preparation.cpp

#include "data_preparation.h"
#include <filesystem>
#include <fstream>
#include <sstream>
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

void saveBoundingBoxesToXML(const std::string& filePath, const std::vector<BoundingBox>& boundingBoxes) {
    
    cv::FileStorage fs(filePath, cv::FileStorage::WRITE | cv::FileStorage::FORMAT_XML);

    if (!fs.isOpened()) {
        std::cerr << "Error opening XML file for writing: " << filePath << std::endl;
        return;
    }

    
    fs << "parking" << "["; 

    for (size_t i = 0; i < boundingBoxes.size(); ++i) {
        const auto& box = boundingBoxes[i];

        
        fs << "space" << "{"; 
        fs << "id" << static_cast<int>(i) + 1;
        fs << "occupied" << (box.occupied ? 1 : 0);

        fs << "rotatedRect" << "{";
        fs << "center" << "{";
        fs << "x" << static_cast<int>(box.rect.center.x);
        fs << "y" << static_cast<int>(box.rect.center.y);
        fs << "}";
        fs << "size" << "{";
        fs << "w" << static_cast<int>(box.rect.size.width);
        fs << "h" << static_cast<int>(box.rect.size.height);
        fs << "}";
        fs << "angle" << box.rect.angle;
        fs << "}";

        
        fs << "contour" << "[";
        cv::Point2f vertices[4];
        box.rect.points(vertices);
        for (int j = 0; j < 4; ++j) {
            fs << "point" << "{";
            fs << "x" << static_cast<int>(vertices[j].x);
            fs << "y" << static_cast<int>(vertices[j].y);
            fs << "}";
        }
        fs << "]"; 

        fs << "}"; 
    }

    fs << "]"; 
    fs.release(); 
}



std::vector<cv::Mat> load_images(const std::string& directory) {
    std::vector<cv::Mat> images;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        cv::Mat image = cv::imread(entry.path().string());
        if (!image.empty()) {
            images.push_back(image);
        }
    }
    return images;
}


std::vector<std::vector<BoundingBox>> load_bounding_boxes(const std::string& directory) {
    std::vector<std::vector<BoundingBox>> all_bounding_boxes;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream file(entry.path().string());
        std::string line;
        std::vector<BoundingBox> bounding_boxes;

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << entry.path().string() << std::endl;
            continue;
        }

        while (std::getline(file, line)) {
            if (line.find("<space id=") != std::string::npos) {
                BoundingBox box;
                int occupied = 0;  

                // Parse the occupied attribute
                size_t pos = line.find("occupied=\"");
                if (pos != std::string::npos) {
                    sscanf_s(line.c_str() + pos, "occupied=\"%d\"", &occupied);
                    box.occupied = (occupied == 1);  
                }

                while (std::getline(file, line) && line.find("</space>") == std::string::npos) {
                    if (line.find("<center x=") != std::string::npos) {
                        int cx, cy;
                        sscanf_s(line.c_str(), " <center x=\"%d\" y=\"%d\"/>", &cx, &cy);
                        box.rect.center = cv::Point2f(static_cast<float>(cx), static_cast<float>(cy));
                    }
                    else if (line.find("<size w=") != std::string::npos) {
                        int width, height;
                        sscanf_s(line.c_str(), " <size w=\"%d\" h=\"%d\"/>", &width, &height);
                        box.rect.size = cv::Size2f(static_cast<float>(width), static_cast<float>(height));
                    }
                    else if (line.find("<angle d=") != std::string::npos) {
                        float angle;
                        sscanf_s(line.c_str(), " <angle d=\"%f\"/>", &angle);
                        box.rect.angle = angle;
                    }
                }

                bounding_boxes.push_back(box);
            }
        }

        all_bounding_boxes.push_back(bounding_boxes);
    }
    return all_bounding_boxes;
}

std::vector<cv::Mat> load_masks(const std::string& directory) {
    std::vector<cv::Mat> masks;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        cv::Mat mask = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        if (!mask.empty()) {
            masks.push_back(mask);
        }
    }
    return masks;
}

void load_sequences(const std::vector<std::string>& image_dirs, const std::vector<std::string>& bbox_dirs,
    std::vector<std::vector<cv::Mat>>& all_images, std::vector<std::vector<std::vector<BoundingBox>>>& all_bounding_boxes) {
    for (size_t i = 0; i < image_dirs.size(); ++i) {
        auto images = load_images(image_dirs[i]);
        auto bounding_boxes = load_bounding_boxes(bbox_dirs[i]);
        if (!images.empty() && !bounding_boxes.empty()) {
            all_images.push_back(images);
            all_bounding_boxes.push_back(bounding_boxes);
        }
    }
}

std::vector<std::vector<cv::Mat>> load_all_masks(const std::vector<std::string>& directories) {
    std::vector<std::vector<cv::Mat>> all_masks;
    for (const auto& directory : directories) {
        std::vector<cv::Mat> masks;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            cv::Mat mask = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
            if (!mask.empty()) {
                masks.push_back(mask);
            }
        }
        all_masks.push_back(masks);
    }
    return all_masks;
}

void draw_rotated_rects_with_ids(cv::Mat& img, const std::vector<BoundingBox>& bboxes, const std::string& savePath, int imageIndex) {
    for (size_t i = 0; i < bboxes.size(); ++i) {
        const auto& box = bboxes[i];
        cv::Point2f vertices[4];
        box.rect.points(vertices);

        cv::Scalar color = box.occupied ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0);

        for (int j = 0; j < 4; j++) {
            line(img, vertices[j], vertices[(j + 1) % 4], color, 2);
        }

        cv::putText(img, std::to_string(i + 1), vertices[1], cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    }

    std::string outputFilePath = savePath + "\\image" + std::to_string(imageIndex) + ".png";
    cv::imwrite(outputFilePath, img);
}

void apply_segmentation_mask(const cv::Mat& image, const cv::Mat& mask, const std::string& savePath, int imageIndex) {
    cv::Mat result = image.clone();

    cv::Mat red_overlay = cv::Mat::zeros(image.size(), image.type());
    cv::Mat green_overlay = cv::Mat::zeros(image.size(), image.type());

    red_overlay.setTo(cv::Scalar(0, 0, 255), mask == 1);   
    green_overlay.setTo(cv::Scalar(0, 255, 0), mask == 2); 

    double alpha = 0.7; 
    cv::addWeighted(red_overlay, alpha, result, 1.0, 0, result);
    cv::addWeighted(green_overlay, alpha, result, 1.0, 0, result);

    
    std::string outputFilePath = savePath + "\\segmented_image" + std::to_string(imageIndex) + ".png";
    cv::imwrite(outputFilePath, result);
}

void applyThreshold(Mat& inputImage, double thresholdValue) {
    threshold(inputImage, inputImage, thresholdValue, 255, THRESH_BINARY);
}


vector<Mat> loadImagesFromDirectory(const string& folderPath) {
    vector<Mat> images;
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            string filePath = entry.path().string();
           
            cout << "Trying to load image: " << filePath << endl;

            Mat image = imread(filePath);
            if (image.empty()) {
                cerr << "Could not load image (empty image): " << filePath << endl;
            }
            else {
                cout << "Successfully loaded image: " << filePath << " with size: " << image.size() << " and type: " << image.type() << endl;
                images.push_back(image);
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error loading images from folder: " << folderPath << ". Exception: " << e.what() << endl;
    }
    return images;
}

void removeNoiseAndKeepCars(Mat& binaryImage) {
   
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(binaryImage, binaryImage, MORPH_OPEN, kernel);

   
    vector<vector<Point>> contours;
    findContours(binaryImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        
        if (area < 500) { 
            drawContours(binaryImage, contours, (int)i, Scalar(0), -1); 
        }
    }
}

void preprocessImages(const vector<Mat>& inputImages, vector<Mat>& outputImages, double alpha, int beta) {
    for (const Mat& image : inputImages) {
        
        Mat blurredImage;
        GaussianBlur(image, blurredImage, Size(5, 5), 0); 

        
        Mat contrastEnhancedImage;
        blurredImage.convertTo(contrastEnhancedImage, -1, alpha, beta);

        outputImages.push_back(contrastEnhancedImage);
    }
}


Mat createAverageBackground(const vector<Mat>& images) {
    Mat averageBackground = Mat::zeros(images[0].size(), CV_32FC1); 
    for (const Mat& img : images) {
        Mat floatImg;
        img.convertTo(floatImg, CV_32FC1); 
        averageBackground += floatImg;
    }
    averageBackground /= images.size(); 
    averageBackground.convertTo(averageBackground, CV_8UC1); 
    return averageBackground;
}

void drawBoundingBoxes(Mat& image, const vector<BoundingBox>& detectedRegions, const vector<int>& ids, const Scalar& color) {
    for (size_t i = 0; i < detectedRegions.size(); ++i) {
        
        Point2f vertices[4];
        detectedRegions[i].rect.points(vertices);
        for (int j = 0; j < 4; ++j) {
            line(image, vertices[j], vertices[(j + 1) % 4], color, 2);
        }
        putText(image, to_string(ids[i]), detectedRegions[i].rect.center, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
    }
}


vector<BoundingBox> detectParkingSpaces(const Mat& firstImage, const vector<Mat>& templates, double threshold, vector<int>& ids) {
    vector<BoundingBox> detectedRegions;
    
    Mat grayImage;
    cvtColor(firstImage, grayImage, COLOR_BGR2GRAY);

    for (size_t i = 0; i < templates.size(); ++i) {
        Mat result;
        matchTemplate(grayImage, templates[i], result, TM_CCOEFF_NORMED);

        double minVal, maxVal;
        Point minLoc, maxLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        
        if (maxVal >= threshold) {
            Rect matchRect(maxLoc, Size(templates[i].cols, templates[i].rows));
            
            BoundingBox box;
            box.rect = cv::RotatedRect(Point2f(matchRect.x + matchRect.width / 2.0f, matchRect.y + matchRect.height / 2.0f), matchRect.size(), 0); // angle = 0
            box.occupied = false; 
            detectedRegions.push_back(box);
            ids.push_back(i + 1); 
        }
    }

    return detectedRegions;
}


int extractIDFromFilename(const string& filename) {
    regex rgx("(\\d+)$");
    smatch match;
    if (regex_search(filename, match, rgx)) {
        return stoi(match[1]);
    }
    return -1; 
}

bool isBoundingBoxOccupied(const Mat& binaryImage, const RotatedRect& boundingBox, double whitePixelThreshold) {
   
    if (binaryImage.empty()) {
        cerr << "Error: binaryImage is empty." << endl;
        return false;
    }

    Mat mask = Mat::zeros(binaryImage.size(), CV_8UC1);
    Point2f vertices2f[4];
    boundingBox.points(vertices2f);

    Point vertices[4];
    for (int i = 0; i < 4; ++i) {
        vertices[i] = vertices2f[i];
    }

    fillConvexPoly(mask, vertices, 4, Scalar(255));

    if (binaryImage.size() != mask.size()) {
        cerr << "Error: binaryImage and mask sizes do not match." << endl;
        return false;
    }

    if (binaryImage.type() != CV_8UC1) {
        cerr << "Error: binaryImage is not of type CV_8UC1." << endl;
        return false;
    }

    int whitePixelCount = countNonZero(binaryImage & mask);
    return whitePixelCount >= whitePixelThreshold;
}


