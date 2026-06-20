//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//cv::Mat segmentCars(const std::string& imagePath, const std::string& maskPath, int dilationSize = 10) {
//   
//    cv::Mat image = cv::imread(imagePath);
//    if (image.empty()) {
//        std::cerr << "Orijinal görüntü yüklenemedi!" << std::endl;
//        return cv::Mat();
//    }
//
//    cv::Mat mask = cv::imread(maskPath, cv::IMREAD_GRAYSCALE);
//    if (mask.empty()) {
//        std::cerr << "Maske görüntüsü yüklenemedi!" << std::endl;
//        return cv::Mat();
//    }
//
//    cv::Mat dilatedMask;
//    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
//        cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1),
//        cv::Point(dilationSize, dilationSize));
//    cv::dilate(mask, dilatedMask, element);
//
//    
//    cv::Mat coloredMask;
//    cv::cvtColor(dilatedMask, coloredMask, cv::COLOR_GRAY2BGR); 
//    coloredMask.setTo(cv::Scalar(0, 0, 255), dilatedMask); 
//
//    cv::Mat result;
//    cv::addWeighted(image, 1.0, coloredMask, 0.5, 0, result); 
//
//    return result;
//}
