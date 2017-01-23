/*!
\file VideoProcessor.hpp
\brief Declaration of class VideoProcessor
\author Martin Sehnoutka

*/

#ifndef __VIDEOPROCESSOR_HPP__
#define __VIDEOPROCESSOR_HPP__

#include <vector>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Map.hpp"

/*!
 * \class VideoProcessor
 * \brief Class handles the video file and do the basic image processing
 *
 * Class provide an interface for setting the video file name, the background image file name, reading next frame
 * and displaying frames. When the method for opening the video file is called also an instance of map is created.
 * Map is filled with data every time the VideoProcessor#ReadNextFrame method is called.
 */
class VideoProcessor
{
private:
    cv::VideoCapture iVideoFile;
    bool iOutputEnabled;
    cv::VideoWriter iOutputVideo;
    cv::Mat iActFrame, iActFrameGray, iPrevFrameGray, iPreProcessedFrame, iFgMask, iBgImage, iHiddenMask;
    cv::BackgroundSubtractorMOG2 iBgSubtractor;
    cv::Size iVideoSize;

    std::vector<cv::Point2f> iGoodFeatures;
    std::shared_ptr<Map> iVelocityMap;
    bool iUseBgImage, iDisplayFgMask, iDisplayPreProcessedFrame;

    void PreProcessFrame();

public:
    //! Constructor
    VideoProcessor();

    //! Destructor
    ~VideoProcessor();

    //! Open video file
    bool OpenFile(const std::string &aFileName);

    //! Set output video file name
    bool SetOutputFile(const std::string &aFileName);

    //! Open background image
    bool OpenBackgroundImage(const std::string &aFileName);

    //! Open mask where objects can be hidden
    bool OpenHiddenMask(const std::string &aFileName);

    //! Return hidden mask
    cv::Mat GetHiddenMask();

    //! Return size of video
    cv::Size GetVideoSize() { return iVideoSize; }

    //! Display video frames
    void DisplayOutput();

    //! Read next frame from video file and process it
    bool ReadNextFrame();

    //! Return foreground mask. This method is called from TrackerCore class
    const cv::Mat &GetFgMask();

    //! Return frame which is to be displayed.
    cv::Mat &GetFrameToDisplay();

    //! Return pointer to instance of Map class
    std::shared_ptr<Map> GetMap()
    {
        return iVelocityMap;
    }

    //! Turn on displaying of foreground mask
    void DisplayFgMask()
    {
        iDisplayFgMask = true;
        cv::namedWindow("fgMask", cv::WINDOW_AUTOSIZE);
    }

    //! Turn on displaying of preprocessed frame
    void DisplayPreProcess()
    {
        iDisplayPreProcessedFrame = true;
        cv::namedWindow("preProcFrame", cv::WINDOW_AUTOSIZE);
    }
};

#endif //__VIDEOPROCESSOR_HPP__