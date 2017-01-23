/*!
 * \file
 * \author Martin Sehnoutka
 */

#include "TrackerCore.hpp"

TrackerCore::TrackerCore()
{

}


TrackerCore::~TrackerCore()
{
    ClearObjects();
}

void TrackerCore::TrackObjects()
{
    InitObjects();
    PairObjects();
}

void TrackerCore::DrawObjects()
{
    cv::Mat frame = iVideoProcessor->GetFrameToDisplay();
    cv::Scalar red(0,0,255);
    cv::Scalar green(100,255,100);

    auto draw_single_object = [&frame, &red, &green](MovingObject *aSingle_object)->void
    {
        std::string identifier;
        if(!aSingle_object->IsHidden())
        {
            identifier = aSingle_object->getIdentifier();
        }else{
            identifier = aSingle_object->getIdentifier() + "h";
        }
        cv::rectangle(
                frame,
                aSingle_object->getTopLeft(),
                aSingle_object->getBottomRight(),
                red, 1, 8, 0);
        cv::putText(
                frame,
                identifier,
                aSingle_object->getTopLeft(),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, red, 2);
        const std::vector<cv::Point> track = aSingle_object->getObjectTrack();
        if(!track.empty())
        {
            for(size_t i=0; i < track.size()-1 ; ++i)
            {
                line(frame, track[i], track[i+1], green, 2, 0, 0);
            }
        }
    };

    for(MovingObject* object_iterator : iObjects)
    {
        if(object_iterator->IsSingleObject())
        {
            draw_single_object(object_iterator);
        }else{
            for(MovingObject* object_inside : object_iterator->GetObjectsInside())
            {
                if(object_inside->IsSingleObject())
                {
                    draw_single_object(object_inside);
                }else{
                    std::cerr << "[Runtime error] Multiobject inside another multiobject!" << std::endl;
                }
            }
        }
    }
}

void TrackerCore::InitObjects()
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    findContours(iVideoProcessor->GetFgMask(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    std::vector<cv::Rect> rectangle_boundaries(contours.size());

    //Find bounding boxes
    for(size_t i = 0; i < contours_poly.size(); ++i )
    {
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
        rectangle_boundaries[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
    }

    MovingObject *new_object = nullptr;

    for(cv::Rect& rectangle_iterator : rectangle_boundaries)
    {
        if(rectangle_iterator.area() > 800)
        {
            new_object = new MovingObject(rectangle_iterator.tl(), rectangle_iterator.br());
            iNewObjects.push_back(new_object);
        }
    }
}

void TrackerCore::PairObjects()
{
    for(auto object_iterator : iObjects)
        object_iterator->PredictPosition();

    float xmin, xmax, ymin, ymax;
    std::vector<MovingObject *> objects_list;

    for(MovingObject*& new_object_iter : iNewObjects)
    {
        objects_list.clear();
        xmin = new_object_iter->getTopLeft().x;
        xmax = new_object_iter->getBottomRight().x;
        ymin = new_object_iter->getTopLeft().y;
        ymax = new_object_iter->getBottomRight().y;

        for(auto& object_iter : iObjects)
        {
            if(object_iter == nullptr)
                continue;
            if(object_iter->IsSingleObject())
            {
                const cv::Point Center = object_iter->getPredictedCenter();
                if(xmin < Center.x && Center.x < xmax && ymin < Center.y && Center.y < ymax)
                {
                    objects_list.push_back(object_iter);
                    object_iter = nullptr;
                }
            }else{
                std::vector<MovingObject *> &objects_inside = object_iter->GetObjectsInside();
                for(auto& object_inside_iter : objects_inside)
                {
                    if(object_inside_iter != nullptr)
                    {
                        const cv::Point Center = object_inside_iter->getPredictedCenter();
                        if(xmin < Center.x && Center.x < xmax && ymin < Center.y && Center.y < ymax)
                        {
                            objects_list.push_back(object_inside_iter);
                            object_inside_iter = nullptr;
                        }
                    }
                } // objects inside
            } //is single object
        } // for all objects

        if(1 == objects_list.size())
        {
            if(objects_list[0]->IsHidden())
                objects_list[0]->SetAsVisible();

            objects_list[0]->CopyParametersFrom(new_object_iter);
            delete new_object_iter;
            new_object_iter = objects_list[0];
            new_object_iter->CorrectPosition(new_object_iter->getCenter());
        }else if(objects_list.size() > 1){

            if( (objects_list.size() == 2) && (objects_list[0]->IsHidden() || objects_list[1]->IsHidden()) )
            {
                int number_of_hidden_object = 0;
                if(objects_list[1]->IsHidden())
                    number_of_hidden_object = 1;

                delete objects_list[1-number_of_hidden_object];
                delete new_object_iter;

                objects_list[number_of_hidden_object]->SetAsVisible();
                new_object_iter = objects_list[number_of_hidden_object];
                new_object_iter->CorrectPosition(new_object_iter->getCenter());

            }else{
                MovingMultiObject* new_multiobject = new MovingMultiObject(
                        new_object_iter->getTopLeft(),
                        new_object_iter->getBottomRight());
                for(auto object_iter : objects_list)
                {
                    new_multiobject->AddNewObject(object_iter);
                }
                new_multiobject->NoCorrection();
                new_multiobject->putTogetherIdentifiers();
                delete new_object_iter;
                new_object_iter = new_multiobject;
            }
        }else{
            //new object
        }

    } //for all new objects

    //delete lost objects
    size_t length = iObjects.size();
    for(size_t i=0; i<length; ++i)
    {
        if(iObjects[i] != nullptr)
        {
            //std::cout << "Deleting object no:" << iObjects[i]->getIdentifier() << std::endl;
            //delete iObjects[i];
            if(iObjects[i]->IsSingleObject())
            {
                cv::Point center = iObjects[i]->getPredictedCenter();
                if(iHiddenMask.at<uchar>(center) > 200)
                {
                    if(!iObjects[i]->IsHidden())
                        iObjects[i]->SetAsHidden();

                    iObjects[i]->NoCorrection();

                    if(iObjects[i]->GetHiddenCounter() < 60)
                    {
                        iNewObjects.push_back(iObjects[i]);
                    }else{
                        delete iObjects[i];
                    }   //hidden counter
                }else{ //hidden mask
                    delete iObjects[i];
                }
            }else{ //single object
                delete iObjects[i];
            }
        }
    }

    iObjects.clear();
    iObjects = iNewObjects;
    iNewObjects.clear();
}

void TrackerCore::ClearObjects()
{
    size_t vector_length = iObjects.size();
    for(size_t i=0; i<vector_length; ++i)
    {
        delete(iObjects.back());
        iObjects.pop_back();
    }
}

void TrackerCore::SetVideoProcessor(std::shared_ptr<VideoProcessor> aVideoProcessor)
{
    iVideoProcessor = aVideoProcessor;
}

void TrackerCore::SetHiddenMask(cv::Mat aMask)
{
    if(!aMask.empty())
    {
        iHiddenMask = aMask;
    }else{
        if(iVideoProcessor != nullptr)
        {
            iHiddenMask = cv::Mat::ones(iVideoProcessor->GetVideoSize(), CV_8U);
            iHiddenMask = iHiddenMask*255;
        }else{
            throw std::runtime_error("Cannot open mask file or video file.");
        }
    }
}
