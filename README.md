# Parking Lot Occupancy Analysis

A classical computer-vision pipeline (C++ / OpenCV) that detects parking spaces and
classifies their occupancy (occupied / free) from fixed surveillance-camera images.
Developed as a Computer Vision course project at the University of Padova.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green)

> **Scope note:** This project uses classical image-processing techniques only — **no deep learning**.
> It is built around a single fixed camera viewpoint and a specific dataset.

---

## Overview

The system:

- **Localizes** each parking space on an empty-lot reference frame using template matching.
- **Detects vehicles** via background subtraction against an averaged empty-lot background.
- **Classifies** each space as occupied or free, drawing colour-coded boxes
  (**red = occupied**, **blue = free**) and exporting per-frame XML.

The dataset is derived from the **PKLot** dataset (provided with the course) and is not included
in this repository.

---

## How It Works

1. **Parking-space localization** — templates for each space are manually cropped from the empty-lot
   frame and matched with `cv::matchTemplate` (normalized cross-correlation). Because the camera is
   fixed, the resulting boxes are reused across all frames, each with a unique ID.
2. **Background model** — the five empty-lot frames (`sequence0`) are averaged in grayscale to model
   the parking lot in its empty state.
3. **Vehicle detection (per frame)** —
   `absdiff` vs. background → Gaussian blur + contrast/brightness stretch → binary threshold →
   subtract a static line mask → morphological opening + removal of small contours.
4. **Occupancy decision** — for each space, the foreground (non-zero) pixels inside the box are counted;
   above a threshold the space is marked occupied.
5. **Output** — annotated images and per-frame XML (written via `cv::FileStorage`).

An earlier approach based on **Canny + Hough line detection**
(`Parking_Space_Detection_Hough_Line.cpp`) was abandoned because shadows and changing light made line
detection unreliable; the code is kept (commented out) for reference.

---

## Repository Structure

```
.
├── Main.cpp                                 # End-to-end pipeline (entry point)
├── data_preparation.h / data_preparation.cpp# I/O, preprocessing, detection, occupancy helpers
├── Parking_Space_Detection_Hough_Line.cpp   # Abandoned Hough-line experiment (commented out)
├── Car_Segmentation.cpp                     # Early segmentation experiment (commented out)
├── CMakeLists.txt                           # CMake build script
├── ParkingLotAnalysis.sln / *.vcxproj       # Visual Studio project files
└── CV_Project_Report.pdf                    # Full project report
```

---

## Results

Results are demonstrated qualitatively — see `CV_Project_Report.pdf` for the full set of figures and
the detailed write-up.

**Evaluation status:** Mean Average Precision (mAP) for localization and Mean Intersection over Union
(mIoU) for segmentation were explored as evaluation metrics, but **quantitative scores were not
finalized** due to implementation issues; results are presented qualitatively.

---

## Limitations

- **Fixed-camera assumption** — templates and boxes are tied to one viewpoint and do not transfer to
  other cameras or layouts.
- **Manual templates** — each parking space is cropped by hand; there is no automatic space discovery.
- **Hand-tuned parameters** — the pixel-count threshold, contrast/brightness, and binary threshold are
  dataset-specific.
- **Sensitive to shadows and lighting changes**, which can cause occupancy misclassification.
- **No learned model** and no finalized quantitative evaluation (mAP / mIoU).

---

## Possible Improvements

- Replace template matching + background subtraction with a learned detector (e.g. a small CNN or YOLO)
  for more robust, transferable occupancy detection.
- Compute and report mAP and mIoU against the provided ground truth.
- Add the 2D top-view mini-map described in the report as a user-facing output.
- Fit rotated bounding boxes to detected spaces/vehicles (currently axis-aligned in the algorithm).

---

## Author

- Metehan Kurtoğlu

University of Padova — Computer Vision course project.
