cmake_minimum_required(VERSION 2.8.4)
project(ObjectTracker)

#Compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -g")
set(CMAKE_CXX_COMPILER "clang++")

#OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

#Use absolute path if this does not work
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

#Main program
set(SOURCE_FILES main.cpp
    modules/VideoProcessor.cpp
    modules/Map.cpp
    modules/TrackerCore.cpp
    modules/MovingObject.cpp
    modules/ModifiedKalmanFilter.cpp
    modules/MapBasedTracker.cpp
    modules/MultipleTracker.cpp)
add_executable(ObjectTracker ${SOURCE_FILES})
target_link_libraries( ObjectTracker ${OpenCV_LIBS} ${OPENCV_LIBRARIES} )

#Video processor test
set(SOURCE_FILES_VIDEO_PROCESSOR tests/VideoProcessor_test.cpp modules/VideoProcessor.cpp modules/Map.cpp)
add_executable(VideoProcessor ${SOURCE_FILES_VIDEO_PROCESSOR})
target_link_libraries( VideoProcessor ${OpenCV_LIBS} ${OPENCV_LIBRARIES} )

#Background image creater
set(SOURCE_FILES_BG_CREATOR tests/background_creator.cpp)
add_executable(BgCreator ${SOURCE_FILES_BG_CREATOR})
target_link_libraries( BgCreator ${OpenCV_LIBS} ${OPENCV_LIBRARIES} )

#Map test
set(SOURCE_FILES_VIDEO_PROCESSOR tests/Map_test.cpp modules/Map.cpp)
add_executable(Map ${SOURCE_FILES_VIDEO_PROCESSOR})
target_link_libraries( Map ${OpenCV_LIBS} ${OPENCV_LIBRARIES} )

#Tracker test
set(SOURCE_FILES_TRACKER tests/Tracker_test.cpp
    modules/VideoProcessor.cpp
    modules/Map.cpp
    modules/TrackerCore.cpp
    modules/MovingObject.cpp
    modules/ModifiedKalmanFilter.cpp
    modules/MapBasedTracker.cpp)
add_executable(TrackerCore ${SOURCE_FILES_TRACKER})
target_link_libraries( TrackerCore ${OpenCV_LIBS} ${OPENCV_LIBRARIES} )
