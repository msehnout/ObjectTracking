/*!
\file MapBasedTracker.hpp
\brief MapBasedTracker class declaration
\author Martin Sehnoutka

*/

#ifndef _OBJECTTRACKER_MAPBASEDTRACKER_HPP_
#define _OBJECTTRACKER_MAPBASEDTRACKER_HPP_

#include <opencv2/core/core.hpp>
#include "TrackerBase.hpp"

/*!
 * \class MapBasedTracker
 * \brief Implements tracker as a child of Tracker base
 *
 *  Core of this tracker is map. Map is used to predict next position of an object.
 *  Unlike Kalman filter, this tracker has no model of objects movement.
 *
 */
class MapBasedTracker : public TrackerBase
{
private:
    cv::Point iPosition, iPredictedPosition;
public:

    MapBasedTracker()
    {}

    ~MapBasedTracker()
    {}

    void SetInitialPosition(cv::Point aCenter)
    {
        iPredictedPosition = iPosition = aCenter;
    }

    void Predict(double aAngle);

    void Correct(cv::Point aCenter);

    cv::Point GetPredictedCenter() const
    {
        return iPredictedPosition;
    }

    cv::Point GetCorrectedCenter() const
    {
        return iPosition;
    }
};

#endif //_OBJECTTRACKER_MAPBASEDTRACKER_HPP_
