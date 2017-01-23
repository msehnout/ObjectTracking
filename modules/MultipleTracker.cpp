/*!
 * \file
 * \author Martin Sehnoutka
 */

#include "MultipleTracker.hpp"
#include "ModifiedKalmanFilter.hpp"
#include "MapBasedTracker.hpp"

MultipleTracker::MultipleTracker()
{
    ModifiedKalmanFilter *first = new ModifiedKalmanFilter(E_constant_velocity);
    MapBasedTracker *second = new MapBasedTracker();
    iTrackers.push_back(first);
    iTrackers.push_back(second);
}

MultipleTracker::MultipleTracker(const enum MultipleTrackerType aType)
{
    if(aType == E_doubleKalman_singleMap)
    {
        ModifiedKalmanFilter *first = new ModifiedKalmanFilter(E_constant_velocity);
        ModifiedKalmanFilter *second = new ModifiedKalmanFilter(E_constant_acceleration);
        MapBasedTracker *third = new MapBasedTracker();
        iTrackers.push_back(first);
        iTrackers.push_back(second);
        iTrackers.push_back(third);
    }else{
        ModifiedKalmanFilter *first = new ModifiedKalmanFilter(E_constant_velocity);
        MapBasedTracker *second = new MapBasedTracker();
        iTrackers.push_back(first);
        iTrackers.push_back(second);
    }
}

MultipleTracker::~MultipleTracker()
{
    for(auto& iterator : iTrackers)
        delete iterator;
}

void MultipleTracker::SetInitialPosition(cv::Point aCenter)
{
    for(auto& iterator : iTrackers)
        iterator->SetInitialPosition(aCenter);
}

void MultipleTracker::Predict(double aAngle)
{
    for(auto& iterator : iTrackers)
        iterator->Predict(aAngle);
}

void MultipleTracker::Correct(cv::Point aCenter)
{
    for(auto& iterator : iTrackers)
        iterator->Correct(aCenter);
}

cv::Point MultipleTracker::GetPredictedCenter() const
{
    cv::Point center;
    double coefficient = 1.0/(double)(iTrackers.size());
    for(auto& iterator : iTrackers)
    {
        center += coefficient*iterator->GetPredictedCenter();
    }

    return center;
}

cv::Point MultipleTracker::GetCorrectedCenter() const
{
    cv::Point center;
    double coefficient = 1.0/(double)(iTrackers.size());
    for(auto& iterator : iTrackers)
    {
        center += coefficient*iterator->GetPredictedCenter();
    }

    return center;
}
