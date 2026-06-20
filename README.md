# Parking-Lot-Car-Segmentation
C++ based Computer Vision pipeline for real-time parking lot occupancy detection using Hough Line Transform, adaptive car segmentation, and geometric feature extraction.

# Parking Lot Occupancy Analysis

A classical computer-vision pipeline (C++ / OpenCV) that detects parking spaces and
classifies their occupancy (occupied / free) from fixed surveillance-camera images.
Developed as a Computer Vision course project at the University of Padova.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green)

> **Scope note:** This project uses classical image-processing techniques only — **no deep learning**.
> It is built around a **single fixed camera viewpoint** and a specific dataset. See [Limitations](#limitations)
> before reusing it elsewhere.

---

## Table of Contents
- [Overview](#overview)
- [Pipeline](#pipeline)
- [Repository Structure](#repository-structure)
- [Expected Dataset Layout](#expected-dataset-layout)
- [Requirements](#requirements)
- [Build & Run](#build--run)
- [Results](#results)
- [Limitations](#limitations)
- [Possible Improvements](#possible-improvements)
- [Authors](#authors)
- [License](#license)

---

## Overview

The system performs three things:

1. **Parking-space localization** — locates each parking space on the empty-lot reference frame using
   template matching (`cv::matchTemplate`, normalized cross-correlation). Because the camera is fixed,
   the detected boxes are reused across all frames.
2. **Vehicle detection** — isolates vehicles using background subtraction against an averaged empty-lot
   background, followed by thresholding, morphological cleanup, and small-contour removal.
3. **Occupancy classification** — for each space, counts foreground pixels inside the box and applies a
   threshold to decide occupied vs. free. Results are drawn as colour-coded boxes
   (**red = occupied**, **blue = free**) and exported as per-frame XML.

The dataset is derived from the **PKLot** dataset (provided with the course). It is **not included** in
this repository — see [Expected Dataset Layout](#expected-dataset-layout).

---

## Pipeline

1. **Parking-space localization** — templates for each space are manually cropped from the empty-lot
   frame and matched with `cv::matchTemplate` (normalized cross-correlation). High-correlation locations
   become parking-space bounding boxes with unique IDs.
2. **Background model** — the average of the five empty-lot frames (`sequence0`), in grayscale.
3. **Vehicle detection (per frame):**
   `absdiff` vs. background → Gaussian blur + contrast/brightness stretch → binary threshold →
   subtract a static line mask → morphological opening + removal of small contours.
4. **Occupancy decision** — for each space, count non-zero pixels inside the (filled) box; if the count
   exceeds a threshold, the space is marked occupied.
5. **Output** — annotated images plus per-frame XML written via `cv::FileStorage`.

An earlier approach based on **Canny + Hough line detection**
(`Parking_Space_Detection_Hough_Line.cpp`) was abandoned: shadows and changing light made line detection
unreliable. The code is kept (commented out) for reference.

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

## Expected Dataset Layout

The dataset is **not part of this repo**. The code expects a `ParkingLot_dataset/` folder next to the
build directory with the following structure:

```
ParkingLot_dataset/
├── sequence0/{frames, bounding_boxes, masks}/   # empty lot — used as reference / background
├── sequence1..5/{frames, bounding_boxes, masks}/
├── template/                # manually cropped parking-space templates
├── Line/preprocessed6.png   # static line mask
├── XML/                     # OUTPUT: per-frame XML        (must exist before running)
└── Sonuclar/ Sonuclar2/ Sonuclar3/   # OUTPUT: annotated images (must exist before running)

---

## Requirements

- A C++17 compiler
- OpenCV 4.x
- CMake ≥ 3.10 **or** Visual Studio 2019+

---

## Build & Run

> The OpenCV path in `CMakeLists.txt` is currently hardcoded — set `OpenCV_DIR` to your own OpenCV build
> before building.

### CMake
```bash
git clone <repo-url>
cd ParkingLotAnalysis
mkdir build && cd build
cmake .. -DOpenCV_DIR=/path/to/opencv/build
cmake --build . --config Release
```

### Visual Studio
1. Open `ParkingLotAnalysis.sln`.
2. Set the OpenCV include and library paths in the project properties (x64).
3. Build in `Release | x64`.

### Run
Place `ParkingLot_dataset/` next to the executable (the code uses relative paths such as
`../ParkingLot_dataset/...`), create the output folders listed above, then run the produced binary.

---

## Results

See `CV_Project_Report.pdf` for the full set of figures and the detailed write-up.

**Evaluation status:** Mean Average Precision (mAP) for localization and Mean Intersection over Union
(mIoU) for segmentation were explored as evaluation metrics, but **quantitative scores were not
finalized** due to implementation issues. Current results are presented **qualitatively**.

---

## Limitations

- **Fixed-camera assumption** — templates and boxes are tied to one viewpoint and do not transfer to
  other cameras or layouts.
- **Manual templates** — each parking space is cropped by hand; there is no automatic space discovery.
- **Hand-tuned parameters** — pixel-count threshold, contrast/brightness, and binary threshold are
  dataset-specific magic numbers.
- **Sensitive to shadows and lighting changes**, which can cause occupancy misclassification.
- **No learned model** and **no finalized quantitative evaluation** (mAP / mIoU).

---

## Possible Improvements

- Replace template matching + background subtraction with a learned detector (e.g. a small CNN or YOLO)
  for more robust, transferable occupancy detection.
- Implement and **report** mAP and mIoU against the provided ground truth.
- Move paths and parameters into a config file; remove hardcoded values and absolute paths.
- Add the **2D top-view mini-map** described in the report as a user-facing output.
- Fit **rotated bounding boxes** to detected spaces/vehicles (currently axis-aligned in the algorithm).

---

## Authors

Metehan Kurtoğlu — University of Padova, Computer Vision course project.

---

## License

No license is currently specified. If you want others to reuse the code, add one (e.g. MIT). As a course
project, you may also keep it for portfolio / reference purposes only.
