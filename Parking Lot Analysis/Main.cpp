//main.cpp

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

int main() {


    std::string groundTruthParkingSpacePath = "..\\ParkingLot_dataset\\Ground_Truth_parcking_Space";
    std::string groundTruthCarSegmentationPath = "..\\ParkingLot_dataset\\Ground_Truth_car_segmentation";

    std::string images_dir_0 = "../ParkingLot_dataset/sequence0/frames";
    std::string bounding_boxes_dir_0 = "../ParkingLot_dataset/sequence0/bounding_boxes";
    std::string masks_dir_0 = "../ParkingLot_dataset/sequence0/masks";

    auto images_0 = load_images(images_dir_0);
    auto bounding_boxes_0 = load_bounding_boxes(bounding_boxes_dir_0);
    std::vector<cv::Mat> masks_0;

    if (std::filesystem::exists(masks_dir_0) && !std::filesystem::is_empty(masks_dir_0)) {
        masks_0 = load_masks(masks_dir_0);
    }
    else {
        std::cout << "Masks directory is missing or empty, proceeding without masks for sequence0." << std::endl;
    }

    if (images_0.empty() || bounding_boxes_0.empty()) {
        std::cout << "Failed to load images or bounding boxes for sequence0" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < images_0.size(); ++i) {
        draw_rotated_rects_with_ids(images_0[i], bounding_boxes_0[i], groundTruthParkingSpacePath, i + 1);
    }

    std::vector<std::string> image_dirs = {
        "../ParkingLot_dataset/sequence1/frames",
        "../ParkingLot_dataset/sequence2/frames",
        "../ParkingLot_dataset/sequence3/frames",
        "../ParkingLot_dataset/sequence4/frames",
        "../ParkingLot_dataset/sequence5/frames"
    };

    std::vector<std::string> bounding_boxes_dirs = {
        "../ParkingLot_dataset/sequence1/bounding_boxes",
        "../ParkingLot_dataset/sequence2/bounding_boxes",
        "../ParkingLot_dataset/sequence3/bounding_boxes",
        "../ParkingLot_dataset/sequence4/bounding_boxes",
        "../ParkingLot_dataset/sequence5/bounding_boxes"
    };

    std::vector<std::vector<cv::Mat>> all_images;
    std::vector<std::vector<std::vector<BoundingBox>>> all_bounding_boxes;
    load_sequences(image_dirs, bounding_boxes_dirs, all_images, all_bounding_boxes);

    for (size_t seq = 1; seq <= 5; ++seq) {
        for (size_t i = 0; i < all_images[seq - 1].size(); ++i) {
            draw_rotated_rects_with_ids(all_images[seq - 1][i], all_bounding_boxes[seq - 1][i], groundTruthParkingSpacePath, i + 1 + ((seq - 1) * 5));
        }
    }

    std::vector<std::string> masks_directories = {
        "../ParkingLot_dataset/sequence1/masks",
        "../ParkingLot_dataset/sequence2/masks",
        "../ParkingLot_dataset/sequence3/masks",
        "../ParkingLot_dataset/sequence4/masks",
        "../ParkingLot_dataset/sequence5/masks"
    };

    auto all_masks = load_all_masks(masks_directories);

    for (size_t seq = 1; seq <= 5; ++seq) {
        std::string sequence_dir = "../ParkingLot_dataset/sequence" + std::to_string(seq);
        auto images = load_images(sequence_dir + "/frames");

        if (seq - 1 < all_masks.size()) {
            auto masks = all_masks[seq - 1];

            for (size_t i = 0; i < images.size(); ++i) {
                apply_segmentation_mask(images[i], masks[i], groundTruthCarSegmentationPath, i + 1 + ((seq - 1) * 5));
            }
        }
    }


    string sequence0Path = "..\\ParkingLot_dataset\\sequence0\\frames";
    vector<string> sequencePaths = {
        "..\\ParkingLot_dataset\\sequence1\\frames",
        "..\\ParkingLot_dataset\\sequence2\\frames",
        "..\\ParkingLot_dataset\\sequence3\\frames",
        "..\\ParkingLot_dataset\\sequence4\\frames",
        "..\\ParkingLot_dataset\\sequence5\\frames"
    };

    string templateFolderPath = "..\\ParkingLot_dataset\\template";

    string lineMaskPath = "..\\ParkingLot_dataset\\Line\\preprocessed6.png";
    Mat lineMask = imread(lineMaskPath, IMREAD_GRAYSCALE);
    if (lineMask.empty()) {
        cerr << "Error loading line mask: " << lineMaskPath << endl;
        return -1;
    }

    vector<Mat> templates;
    vector<int> templateIDs;
    for (const auto& entry : fs::directory_iterator(templateFolderPath)) {
        Mat templ = imread(entry.path().string(), IMREAD_GRAYSCALE);
        if (!templ.empty()) {
            templates.push_back(templ);
            
            int id = extractIDFromFilename(entry.path().stem().string());
            templateIDs.push_back(id);
        }
    }

    string outputFolder1 = "..\\ParkingLot_dataset\\Sonuclar";
    string outputFolder2 = "..\\ParkingLot_dataset\\Sonuclar2";
    string outputFolder3 = "..\\ParkingLot_dataset\\Sonuclar3"; 

    vector<Mat> sequence0Images = loadImagesFromDirectory(sequence0Path);

    vector<Mat> sequence0Grayscale;
    for (const Mat& img : sequence0Images) {
        Mat gray;
        cvtColor(img, gray, COLOR_BGR2GRAY);
        sequence0Grayscale.push_back(gray);
    }

    Mat averageBackground = createAverageBackground(sequence0Grayscale);

    vector<BoundingBox> detectedRegions = detectParkingSpaces(sequence0Images[0], templates, 0.70, templateIDs);

    int imageCount = 1; 
    for (const string& folderPath : sequencePaths) {
        vector<Mat> images = loadImagesFromDirectory(folderPath);

        for (size_t i = 0; i < images.size(); ++i) {
            Mat frame = images[i].clone(); 
            drawBoundingBoxes(frame, detectedRegions, templateIDs, Scalar(255, 0, 0)); 

            string outputFilePath1 = outputFolder1 + "\\image" + to_string(imageCount) + ".png";
            imwrite(outputFilePath1, frame);
            cout << "Saved image with bounding boxes: " << outputFilePath1 << endl;

            imageCount++;
        }
    }

    
    double whitePixelThreshold = 1100.0;
    double thresholdValue = 200.0; 
    
    double alpha = 4.0; 
    int beta = 10; 

    
    imageCount = 1; 
    for (const string& folderPath : sequencePaths) {
        vector<Mat> images = loadImagesFromDirectory(folderPath);

        for (size_t i = 0; i < images.size(); ++i) {
            if (images[i].empty()) {
                cerr << "Warning: Image " << i << " in " << folderPath << " is empty. Skipping this image." << endl;
                continue;
            }

            Mat grayImage;
            try {
                cvtColor(images[i], grayImage, COLOR_BGR2GRAY);
            }
            catch (const cv::Exception& e) {
                cerr << "Error converting image to grayscale: " << e.what() << endl;
                continue;
            }

            if (grayImage.size() != averageBackground.size()) {
                cerr << "Error: Image size does not match the average background size. Skipping this image." << endl;
                continue;
            }

            Mat foreground;
            absdiff(grayImage, averageBackground, foreground);

            vector<Mat> preprocessedImages;
            preprocessImages(vector<Mat>{foreground}, preprocessedImages, alpha, beta);

            Mat binaryImage;
            if (!preprocessedImages.empty()) {
                applyThreshold(preprocessedImages[0], thresholdValue); 
                binaryImage = preprocessedImages[0].clone(); 
            }

            if (!binaryImage.empty()) {
                
                if (binaryImage.size() == lineMask.size()) {
                    binaryImage -= lineMask; 
                }
                else {
                    cerr << "Error: Line mask size does not match binary image size. Skipping this image." << endl;
                    continue;
                }

                removeNoiseAndKeepCars(binaryImage);

                string outputFilePath2 = outputFolder2 + "\\image" + to_string(imageCount) + ".png";
                imwrite(outputFilePath2, binaryImage);
                cout << "Saved binary preprocessed image after noise removal: " << outputFilePath2 << endl;
            }
            else {
                cerr << "Error: Preprocessing failed for image " << i << ". Skipping this image." << endl;
            }

            imageCount++;
        }
    }

    
    imageCount = 1; 
    for (const string& folderPath : sequencePaths) {
        for (size_t i = 0; i < 5; ++i) { 
            string preprocessedImagePath = outputFolder2 + "\\image" + to_string(imageCount) + ".png";
            Mat binaryPreprocessedImage = imread(preprocessedImagePath, IMREAD_GRAYSCALE);

            string originalImagePath = outputFolder1 + "\\image" + to_string(imageCount) + ".png";
            Mat originalImage = imread(originalImagePath);

            for (size_t j = 0; j < detectedRegions.size(); ++j) {
                if (isBoundingBoxOccupied(binaryPreprocessedImage, detectedRegions[j].rect, whitePixelThreshold)) {
                    
                    rectangle(originalImage, detectedRegions[j].rect.boundingRect(), Scalar(0, 0, 255), 2);
                    
                    detectedRegions[j].occupied = true; 
                }
                else {
                    detectedRegions[j].occupied = false; 
                }
            }

            std::string xmlFilePath = "..\\ParkingLot_dataset\\XML\\frame" + std::to_string(imageCount) + ".xml";
            saveBoundingBoxesToXML(xmlFilePath, detectedRegions);

            string outputFilePath3 = outputFolder3 + "\\final" + to_string(imageCount) + ".png";
            imwrite(outputFilePath3, originalImage);
            cout << "Saved final image with marked occupied spaces: " << outputFilePath3 << endl;

            imageCount++;
        }
    }


}
