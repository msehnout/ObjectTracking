/*!
 * \file
 * \author Martin Sehnoutka
 */

#include <complex>
#include "MapBasedTracker.hpp"

void MapBasedTracker::Predict(double aAngle)
{
    std::complex<double> velocity_vector = TrackerBase::iMap->GetVelocitVector
            ((unsigned int)iPosition.x, (unsigned int)iPosition.y, aAngle);
    iPredictedPosition.x += int(velocity_vector.real());
    iPredictedPosition.y += int(velocity_vector.imag());
}

void MapBasedTracker::Correct(cv::Point aCenter)
{
    iPredictedPosition = iPosition = aCenter;
}
