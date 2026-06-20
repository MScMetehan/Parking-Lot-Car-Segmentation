cmake_minimum_required(VERSION 3.10)
project(ParkingLotAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(OpenCV_DIR "C:/workspace/source/opencv/build") # Adjust this path to your OpenCV build directory

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    "C:/workspace/source/opencv/build/include" # Add other include directories if needed
)

link_directories(
    "C:/workspace/source/opencv/build/x64/vc15/lib" # Adjust based on your configuration and platform
)

add_executable(ParkingLotAnalysis Main.cpp data_preparation.cpp tinyxml2-master/tinyxml2.cpp)
target_link_libraries(ParkingLotAnalysis ${OpenCV_LIBS})
cmake_minimum_required(VERSION 3.10)
project(ParkingLotDetection)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ParkingLotDetection main.cpp data_preparation.cpp)
target_link_libraries(ParkingLotDetection ${OpenCV_LIBS})
