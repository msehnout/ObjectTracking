/*!
 * \file
 * \author Martin Sehnoutka
 */

#include <ctime>

#include "MovingObject.hpp"

MovingObject::MovingObject() :
        iHidden(false)
{
    InitFilter();
    InitIdentifier();
}

MovingObject::MovingObject(cv::Point aTopLeft, cv::Point aBottomRight) :
        iHidden(false)
{
    iTopLeft = aTopLeft;
    iBottomRight = aBottomRight;
    iWidth = iBottomRight.x - iTopLeft.x;
    iHeight = iBottomRight.y - iTopLeft.y;
    iCenter = iTopLeft;
    iCenter.x += iWidth/2;
    iCenter.y += iHeight/2;
    InitFilter();
    InitIdentifier();
}

void MovingObject::InitIdentifier()
{
    int random_number = std::rand()%100;
    iIdentifier = std::to_string(random_number);
}

void MovingObject::InitFilter()
{
    //iFilter = new ModifiedKalmanFilter(E_constant_velocity);
    //iFilter = new MapBasedTracker();
    iFilter = new MultipleTracker(E_doubleKalman_singleMap);
    iFilter->SetInitialPosition(iCenter);
}

MovingObject::~MovingObject()
{
    delete iFilter;
}

MovingMultiObject::~MovingMultiObject()
{
    for(auto object_iter : iObjects)
        if(object_iter != nullptr) delete object_iter;
}

void MovingObject::PredictPosition()
{
    size_t track = iObjectTrack.size();
    double angle = 0;
    if(track > 5)
    {
        angle = Map::CalcAngle(
                iObjectTrack[track-1].y - iObjectTrack[track-5].y,
                iObjectTrack[track-1].x - iObjectTrack[track-5].x
        );
    }else if(track > 1)
    {
        angle = Map::CalcAngle(
                iObjectTrack[track-1].y - iObjectTrack[track-2].y,
                iObjectTrack[track-1].x - iObjectTrack[track-2].x
        );
    }
    iFilter->Predict(angle);
    iPredictedCenter = iFilter->GetPredictedCenter();
}

void MovingObject::CorrectPosition(cv::Point aCenter)
{
    iFilter->Correct(aCenter);
    iObjectTrack.push_back(iFilter->GetCorrectedCenter());
}

void MovingObject::NoCorrection()
{
    iObjectTrack.push_back(iPredictedCenter);

    iTopLeft.x = iPredictedCenter.x - iWidth/2;
    iTopLeft.y = iPredictedCenter.y - iHeight/2;
    iBottomRight.x = iPredictedCenter.x + iWidth/2;
    iBottomRight.y = iPredictedCenter.y + iHeight/2;

    if(iHidden)
        ++iHiddenCounter;
}

void MovingObject::CopyParametersFrom(const MovingObject *aObject)
{
    iCenter = aObject->getCenter();
    iTopLeft = aObject->getTopLeft();
    iBottomRight = aObject->getBottomRight();
    iWidth = aObject->getWidth();
    iHeight = aObject->getHeight();
}

MovingMultiObject::MovingMultiObject(cv::Point aTopLeft, cv::Point aBottomRight)
{
    iTopLeft = aTopLeft;
    iBottomRight = aBottomRight;
    iWidth = iBottomRight.x - iTopLeft.x;
    iHeight = iBottomRight.y - iTopLeft.y;
    iCenter = iTopLeft;
    iCenter.x += iWidth/2;
    iCenter.y += iHeight/2;
}

void MovingMultiObject::PredictPosition()
{
    for(MovingObject* iterator : iObjects)
    {
        iterator->PredictPosition();
    }
}

void MovingMultiObject::NoCorrection()
{
    for(MovingObject* iterator : iObjects)
        iterator->NoCorrection();
}

void MovingMultiObject::AddNewObject(MovingObject *aObject)
{
    iObjects.push_back(aObject);
}

void MovingMultiObject::putTogetherIdentifiers()
{
    iIdentifier.clear();
    iIdentifier = "m";
    for(auto& object_iter : iObjects)
    {
        iIdentifier.append(object_iter->getIdentifier());
        iIdentifier.append(",");
    }
    iIdentifier.pop_back();
}
