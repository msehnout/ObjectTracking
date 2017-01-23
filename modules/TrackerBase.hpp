/*!
\file TrackerBase.hpp
\brief TrackerBase class declaration
\author Martin Sehnoutka

*/

#ifndef _OBJECTTRACKER_FILTERBASE_H_
#define _OBJECTTRACKER_FILTERBASE_H_

#include <memory>

#include <opencv2/core/core.hpp>

#include "Map.hpp"

/*!
 * \class TrackerBase
 * \brief Abstract class that defines tracking algorithm
 *
 * This class is used as an interface for modified Kalman filter
 * and map based tracking.
 *
 */
class TrackerBase
{
protected:
    static std::shared_ptr<Map> iMap;

public:
    //! This method sets pointer to the instance of map
    static void SetMapPointer(std::shared_ptr<Map> aMap)
    {
        iMap=aMap;
    }
    virtual ~TrackerBase(){};

    //! Set object's initial position
    virtual void SetInitialPosition(cv::Point aCenter) = 0;
    //! Predict next position
    virtual void Predict(double aAngle) = 0;
    //! Correct position after measurement
    virtual void Correct(cv::Point aCenter) = 0;
    //! Get predicted center of object
    virtual cv::Point GetPredictedCenter() const = 0;
    //! Get corrected center of object
    virtual cv::Point GetCorrectedCenter() const = 0;

};

#endif //_OBJECTTRACKER_FILTERBASE_H_
