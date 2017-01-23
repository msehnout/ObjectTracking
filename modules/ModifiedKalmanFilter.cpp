/*!
 * \file
 * \author Martin Sehnoutka
 */

#include <iostream>

#include "ModifiedKalmanFilter.hpp"

std::shared_ptr<Map> TrackerBase::iMap = nullptr;

ModifiedKalmanFilter::ModifiedKalmanFilter() : iModelType(E_constant_velocity)
{
    InitFilter();
}

ModifiedKalmanFilter::ModifiedKalmanFilter(const enum KalmanFilterType aType) :
    iModelType(aType)
{
    InitFilter();
}

ModifiedKalmanFilter::~ModifiedKalmanFilter()
{

}

void ModifiedKalmanFilter::InitFilter()
{
    if(E_constant_acceleration == iModelType)
    {
        iBaseFilter.init(6, 2, 0);

        // Přenos rychlosti na polohu = čas
        float T = 10;
        // Matice přenosu stavu na stav
        iBaseFilter.transitionMatrix = (cv::Mat_<float>(6, 6) <<
                1,0,T,0,0       ,0,
                0,1,0,T,0       ,0,
                0,0,1,0,T       ,0,
                0,0,0,1,0       ,T,
                0,0,0,0,1       ,0,
                0,0,0,0,0       ,1);
        //Matice měření
        iBaseFilter.measurementMatrix = (cv::Mat_<float>(2,6) << 1,0,0,0,0,0,   0,1,0,0,0,0);

        //Inicializace stavů
        for(int i=0; i<6; ++i)
        {
            iBaseFilter.statePre.at<float>(i) = 0;
            iBaseFilter.statePost.at<float>(i) = 0;
        }

        //Inicializace matic měrení, procesního šumu, šumu měření a chybové kovariance
        setIdentity(iBaseFilter.processNoiseCov, cv::Scalar::all(1e-5)); //inicializace Q
        setIdentity(iBaseFilter.measurementNoiseCov, cv::Scalar::all(5e2)); //inicializace R
        setIdentity(iBaseFilter.errorCovPre, cv::Scalar::all(1e3)); //inicializace P
        setIdentity(iBaseFilter.errorCovPost, cv::Scalar::all(1e3)); //inicializace P
    }else{
        // 4 dynamické parametry, 2 měřené parametry a 0 vstupů
        iBaseFilter.init(4, 2, 0);

        // Přenos rychlosti na polohu = čas
        float T = 3;
        // Matice přenosu stavu na stav
        iBaseFilter.transitionMatrix = (cv::Mat_<float>(4, 4) << 1,0,T,0,   0,1,0,T,  0,0,1,0,  0,0,0,1);
        //Matice měření
        iBaseFilter.measurementMatrix = (cv::Mat_<float>(2,4) << 1,0,0,0,   0,1,0,0);

        //Inicializace stavů
        for(int i=0; i<4; ++i)
        {
            iBaseFilter.statePre.at<float>(i) = 0;
            iBaseFilter.statePost.at<float>(i) = 0;
        }

        //Inicializace matic měrení, procesního šumu, šumu měření a chybové kovariance
        setIdentity(iBaseFilter.processNoiseCov, cv::Scalar::all(1e-4)); //inicializace Q
        setIdentity(iBaseFilter.measurementNoiseCov, cv::Scalar::all(1e2)); //inicializace R
        setIdentity(iBaseFilter.errorCovPost, cv::Scalar::all(1)); //inicializace P
    }
}

void ModifiedKalmanFilter::SetInitialPosition(cv::Point aCenter)
{
    iBaseFilter.statePre.at<float>(0) = aCenter.x;
    iBaseFilter.statePost.at<float>(0) = aCenter.x;
    iBaseFilter.statePre.at<float>(1) = aCenter.y;
    iBaseFilter.statePost.at<float>(1) = aCenter.y;
}

void ModifiedKalmanFilter::Predict(double aAngle)
{
    iBaseFilter.predict();
}

void ModifiedKalmanFilter::Correct(cv::Point aCenter)
{
    cv::Mat_<float> measurement(2,1) ;
    measurement << aCenter.x, aCenter.y;
    iBaseFilter.correct(measurement);
}

cv::Point ModifiedKalmanFilter::GetPredictedCenter() const
{
    return cv::Point_<int>(
            int(iBaseFilter.statePre.at<float>(0)),
            int(iBaseFilter.statePre.at<float>(1))
    );
}

cv::Point ModifiedKalmanFilter::GetCorrectedCenter() const
{
    return cv::Point_<int>(
            int(iBaseFilter.statePost.at<float>(0)),
            int(iBaseFilter.statePost.at<float>(1))
    );
}

