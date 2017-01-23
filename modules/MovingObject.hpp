/*!
 * \file
 * \author Martin Sehnoutka
 *
 * \brief Declaration of classes MovingObject and MovingMultiObject
 *
 * MovingMultiObject is a child of MovingObject so it can be used in the same vector of objects as MovingObject.
 *
 */

#ifndef __MOVINGOBJECT_HPP__
#define  __MOVINGOBJECT_HPP__

#include <opencv2/core/core.hpp>
#include <string>
#include <exception>

#include "TrackerBase.hpp"
#include "ModifiedKalmanFilter.hpp"
#include "MapBasedTracker.hpp"
#include "MultipleTracker.hpp"


/*!
 * \class MovingObject
 * \brief Implements single object in video
 */
class MovingObject
{
private:
    TrackerBase *iFilter;
    std::vector<cv::Point> iObjectTrack;

    void InitIdentifier();
    void InitFilter();

protected:
    cv::Point iCenter, iTopLeft, iBottomRight, iPredictedCenter;
    int iWidth, iHeight, iHiddenCounter;
    std::string iIdentifier;
    bool iHidden;

public:
    /// Constructor
    MovingObject();
    /// Constructor with geometric parameters of object
    MovingObject(cv::Point aTopLeft, cv::Point aBottomRight);
    /// Destructor
    virtual ~MovingObject();

    /// Return identifier (number in range from 0 to 99)
    const std::string& getIdentifier() const {return iIdentifier;}
    /// Return position of center
    const cv::Point& getCenter() const {return iCenter;}
    /// Return position of top left corner
    const cv::Point& getTopLeft() const {return iTopLeft;}
    /// Return position of bottom right corner
    const cv::Point& getBottomRight() const {return iBottomRight;}
    /// Return width of object
    const int getWidth() const {return iWidth;}
    /// Return height of object
    const int getHeight() const {return iHeight;}
    /// Copy geometric parameters (corners, center, width, height) from another object
    void CopyParametersFrom(const MovingObject* aObject);

    /// Return true to signalize that instance is not multi-object
    virtual bool IsSingleObject() {return true;}
    /// Predict position in the next frame
    virtual void PredictPosition();
    /// Correct position using measurement from current frame
    void CorrectPosition(cv::Point aCenter);
    /// If there is no measurement, there won'ŧ be any correction
    virtual void NoCorrection();

    /// Returns predicted center
    const cv::Point& getPredictedCenter() const {return iPredictedCenter;}
    /// Returns object track as vector of points in frame
    const std::vector<cv::Point>& getObjectTrack() const {return iObjectTrack;}

    /// Set object as hidden and reset hidden counter
    void SetAsHidden(void) {iHidden = true; iHiddenCounter=0;}
    /// Set object as visible
    void SetAsVisible(void) {iHidden = false;}
    /// Return true if object is hidden
    bool IsHidden(void) const {return iHidden;}
    /// Return value of hidden counter
    int GetHiddenCounter(void) const {return iHiddenCounter;}

    /// Throws an exception if this method is called
    virtual std::vector<MovingObject *>& GetObjectsInside()
    {
        throw std::logic_error("Invalid call of method GetObjectsInside.");
    }
};

/*!
 * \class MovingMultiObject
 * \brief More objects in a single segmented blob
 */
class MovingMultiObject : public MovingObject
{
private:
    std::vector<MovingObject *> iObjects;
public:
    MovingMultiObject(cv::Point aTopLeft, cv::Point aBottomRight);
    ~MovingMultiObject();
    virtual bool IsSingleObject() {return false;}

    void PredictPosition();
    void NoCorrection();

    /// Insert new object to vector of objects inside
    void AddNewObject(MovingObject *aObject);
    /// Return vector of objects
    virtual std::vector<MovingObject *>& GetObjectsInside() { return iObjects; }
    /// Create new identifier using identifiers of objects inside
    void putTogetherIdentifiers();
};

#endif // __MOVINGOBJECT_HPP__