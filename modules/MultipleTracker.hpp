/*!
 * \file
 * \author Martin Sehnoutka
 *
 * \brief Declaration of class MultipleTracker
 *
 */

#ifndef OBJECTTRACKER_MULTIPLETRACKER_H
#define OBJECTTRACKER_MULTIPLETRACKER_H


#include "TrackerBase.hpp"
#include <opencv2/core/core.hpp>
#include <vector>

//! Defines two possible types of multiple tracker topology
enum MultipleTrackerType
{
    E_singleKalman_singleMap=0,
    E_doubleKalman_singleMap=1
};

/*!
 * \class MultipleTracker
 * \brief Tracker which includes one or two Kalman filters and one map based tracker
 */
class MultipleTracker : public TrackerBase
{
private:

    std::vector<TrackerBase *> iTrackers;

public:
    /// Constructor, implicit type is E_singleKalman_singleMap
    MultipleTracker();

    /// Constructor with type switch
    MultipleTracker(const enum MultipleTrackerType aType);

    /// Destructor
    ~MultipleTracker();

    void SetInitialPosition(cv::Point aCenter);

    void Predict(double aAngle);

    void Correct(cv::Point aCenter);

    cv::Point GetPredictedCenter() const;

    cv::Point GetCorrectedCenter() const;
};


#endif //OBJECTTRACKER_MULTIPLETRACKER_H
