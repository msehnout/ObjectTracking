/*!
 * \file
 * \author Martin Sehnoutka
 */

#include <iostream>
#include <complex>

#include <opencv2/video/tracking.hpp>
#include "VideoProcessor.hpp"

VideoProcessor::VideoProcessor() :
        iOutputEnabled(false),
        iBgSubtractor(cv::BackgroundSubtractorMOG2(500,60,false)),
        iUseBgImage(false),
        iDisplayFgMask(false),
        iDisplayPreProcessedFrame(false)
{
    cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);
    iBgSubtractor.set("nmixtures", 5);
    //iBgSubtractor.set("fTau", 1);
}

VideoProcessor::~VideoProcessor()
{
    if(iVideoFile.isOpened())
        iVideoFile.release();
    if(iOutputVideo.isOpened())
        iOutputVideo.release();
    iVelocityMap = nullptr;
}

bool VideoProcessor::OpenFile(const std::string &aFileName)
{
    iVideoFile.open(aFileName);
    if(!iVideoFile.isOpened())
    {
        std::cerr << "[ERROR] Cannot open the video file!" << std::endl;
        return(false);
    }

    iVideoSize = cv::Size(int(iVideoFile.get(CV_CAP_PROP_FRAME_HEIGHT)), int(iVideoFile.get(CV_CAP_PROP_FRAME_WIDTH)));

    unsigned int width = (unsigned int)(iVideoFile.get(CV_CAP_PROP_FRAME_WIDTH));
    unsigned int height = (unsigned int)(iVideoFile.get(CV_CAP_PROP_FRAME_HEIGHT));
    unsigned int grid = 10;
    iVelocityMap = std::make_shared<Map>(height, width, grid);

    return(true);
}

bool VideoProcessor::SetOutputFile(const std::string &aFileName)
{
    int ex = static_cast<int>(iVideoFile.get(CV_CAP_PROP_FOURCC));
    cv::Size S = cv::Size((int) iVideoFile.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) iVideoFile.get(CV_CAP_PROP_FRAME_HEIGHT));

    iOutputVideo.open(aFileName, ex, 30.0, S, true);

    if (!iOutputVideo.isOpened())
    {
        std::cerr  << "Could not open the output video for write: " << aFileName << std::endl;
        return(false);
    }

    iOutputEnabled = true;

    return(true);
}

bool VideoProcessor::OpenBackgroundImage(const std::string &aFileName)
{
    iBgImage = cv::imread(aFileName, CV_LOAD_IMAGE_COLOR);
    iUseBgImage = !iBgImage.empty();
    return(iUseBgImage);
}

bool VideoProcessor::OpenHiddenMask(const std::string &aFileName)
{
    iHiddenMask = cv::imread(aFileName, CV_LOAD_IMAGE_GRAYSCALE);
    return !iHiddenMask.empty();
}

cv::Mat VideoProcessor::GetHiddenMask()
{
    return iHiddenMask;
}

void VideoProcessor::DisplayOutput()
{
    cv::imshow("Video", iActFrame);
    if(iDisplayFgMask)
        cv::imshow("fgMask", iFgMask);
    if(iDisplayPreProcessedFrame)
        cv::imshow("preProcFrame", iPreProcessedFrame);
    if(iOutputEnabled)
    {
        iOutputVideo << iActFrame;
    }
}

void VideoProcessor::PreProcessFrame()
{
    cv::GaussianBlur(iActFrame, iPreProcessedFrame, cv::Size(5,5), 2);
    iPreProcessedFrame.convertTo(iPreProcessedFrame, -1, 1.1, 0);
}

bool VideoProcessor::ReadNextFrame() {

    static bool firstLoop = true;

    iVideoFile >> iActFrame;
    if(iActFrame.empty())
        return(false);

    //Initialize background image
    if(firstLoop)
    {
        double bg_coef = 0.01;
        if(iUseBgImage)
        {
            iActFrame = iBgImage;
            PreProcessFrame();
            iBgSubtractor(iPreProcessedFrame, iFgMask, bg_coef);
        }
        for (int i = 0; i < 20; ++i) {
            iVideoFile >> iActFrame;
            if(iActFrame.empty())
                return(false);
            PreProcessFrame();
            iBgSubtractor(iPreProcessedFrame, iFgMask, bg_coef);
        }
    }


    //Pre-processing
    PreProcessFrame();
    cv::cvtColor(iActFrame, iActFrameGray, cv::COLOR_RGB2GRAY, CV_8U);

    //Background  subtraction
    iBgSubtractor(iPreProcessedFrame, iFgMask, 5e-4);

    //Mathematical morphology
    int morph_size = 2;
    cv::Mat element = getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );
    cv::morphologyEx(iFgMask, iFgMask, cv::MORPH_CLOSE, element);

    if(!firstLoop && !iGoodFeatures.empty())
    {
        std::vector<uchar> status;
        std::vector<float> err;
        std::vector<cv::Point2f> features_next;
        cv::calcOpticalFlowPyrLK(iPrevFrameGray, iActFrameGray, iGoodFeatures, features_next, status, err);
        for(size_t i=0; i<iGoodFeatures.size(); ++i)
        {
            double angle = Map::CalcAngle((features_next[i].y - iGoodFeatures[i].y), (features_next[i].x - iGoodFeatures[i].x));
            if(iVelocityMap != nullptr)
                iVelocityMap->SetVelocityVector(
                        (unsigned int)(iGoodFeatures[i].x),
                        (unsigned int)(iGoodFeatures[i].y),
                        features_next[i].x - iGoodFeatures[i].x,
                        features_next[i].y - iGoodFeatures[i].y,
                        angle);
        }
    }

    //Find good features to track
    //Detector's parameters
    int maxCorners = 10;
    double qualityLevel = 0.01;
    double minDistance = 5.0;
    int blockSize = 3;
    bool useHarrisDetector = false;
    double k = 0.04;
    cv::goodFeaturesToTrack( iActFrameGray, iGoodFeatures, maxCorners, qualityLevel, minDistance, iFgMask, blockSize, useHarrisDetector, k );

    iPrevFrameGray = iActFrameGray.clone();
    firstLoop = false;

    return(true);
}

const cv::Mat& VideoProcessor::GetFgMask()
{
    return iFgMask;
}

cv::Mat& VideoProcessor::GetFrameToDisplay()
{
    return iActFrame;
}


