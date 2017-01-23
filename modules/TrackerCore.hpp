/*!
 * \file
 * \author Martin Sehnoutka
 *
 * \brief Declaration of class TrackerCore
 *
 */

#ifndef __TRACKERCORE_HPP__
#define __TRACKERCORE_HPP__

#include <vector>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "VideoProcessor.hpp"
#include "MovingObject.hpp"

/*!
 * \class TrackerCore
 * \brief Core of tracking algorithm
 */
class TrackerCore
{
private:
    std::vector<MovingObject *> iNewObjects, iObjects;
    std::shared_ptr<VideoProcessor> iVideoProcessor;
    void InitObjects();
    void ClearObjects();
    void PairObjects();
    cv::Mat iHiddenMask;

public:
    /// Constructor
    TrackerCore();

    /// Destructor
    ~TrackerCore();

    /// Find objects tracks from previous frame to this frame
    void TrackObjects();

    /// Draw objects to current frame
    void DrawObjects();

    /// Set pointer to video processor
    void SetVideoProcessor(std::shared_ptr<VideoProcessor> aVideoProcessor);

    /// Set pointer to mask
    void SetHiddenMask(cv::Mat aMask);
};

#endif //__TRACKERCORE_HPP__