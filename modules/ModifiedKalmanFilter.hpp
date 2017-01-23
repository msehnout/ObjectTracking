/*!
\file ModifiedKalmanFilter.hpp
\brief ModifiedKalmanFilter class declaration
\author Martin Sehnoutka

*/

#ifndef __MODIFIEDKALMANFILTER_HPP__
#define __MODIFIEDKALMANFILTER_HPP__

#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

#include "TrackerBase.hpp"
#include "Map.hpp"

enum KalmanFilterType
{
    E_constant_velocity,
    E_constant_acceleration
};

/*!
 * \class ModifiedKalmanFilter
 * \brief Implements tracker with Kalman filter
 *
 * Tracking is based on object model and Kalman filter algorithm. There are two types of
 * model: object with constant velocity and object with constant acceleration.
 *
 */
class ModifiedKalmanFilter : public TrackerBase
{
private:
    //! Instance of Kalman filter implemented in OpenCV library
    cv::KalmanFilter iBaseFilter;

    //! Type of model
    const enum KalmanFilterType iModelType;

    //! Type of prediction
    void InitFilter();
public:
    ModifiedKalmanFilter();
    ModifiedKalmanFilter(const enum KalmanFilterType aType);
    ~ModifiedKalmanFilter();

    void SetInitialPosition(cv::Point aCenter);
    void Predict(double aAngle);
    void Correct(cv::Point aCenter);
    cv::Point GetPredictedCenter() const;
    cv::Point GetCorrectedCenter() const;
};

#endif //__MODIFIEDKALMANFILTER_HPP__