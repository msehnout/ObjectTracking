#include <iostream>
#include <memory>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../modules/VideoProcessor.hpp"
#include "../modules/TrackerCore.hpp"
#include "TrackerFiles.hpp"

int main(void)
{
    std::srand((unsigned int)(std::time(0))); // use current time as seed for random generator

    TrackerFiles video;
    video.setFilePath("../video/");
    video.setVideoName("knihovna_auta");
    video.setVideoSuffix(".MOV");
    video.setImageSuffix(".jpg");
    video.setMapSuffix(".txt");
    video.setHiddenMaskSuffix(".mask.jpg");

    //Set and open video file
    std::cout << "Test of tracker core" << std::endl;

    std::shared_ptr<VideoProcessor> test_processor = std::make_shared<VideoProcessor>();
    std::shared_ptr<Map> test_map;

    if(!test_processor->OpenFile(video.getVideoName()))
    {
        std::cerr << "Cannot open video file! Exiting..." << std::endl;
        return(1);
    }

    if(!test_processor->OpenBackgroundImage(video.getImageName()))
    {
        std::cerr << "Cannot open bg image! Exiting..." << std::endl;
        return(1);
    }

    if(!test_processor->OpenHiddenMask(video.getHiddenMaskName()))
    {
        std::cerr << "Cannot open hidden mask! Exiting..." << std::endl;
        return(1);
    }

    test_map = test_processor->GetMap();
    test_map->SetFileName(video.getMapName());
    test_map->LoadMap();
    TrackerBase::SetMapPointer(test_map);

    TrackerCore test_tracker;
    test_tracker.SetVideoProcessor(test_processor);
    test_tracker.SetHiddenMask(test_processor->GetHiddenMask());

    int keyboard = 0;
    while(test_processor->ReadNextFrame() && (char)keyboard != 'q' && (char)keyboard != 27)
    {
        test_tracker.TrackObjects();
        test_tracker.DrawObjects();
        test_processor->DisplayOutput();
        test_map->PlotMap();
        keyboard = cv::waitKey(30);
        if((char)keyboard == 'p')
        {
            while((char)keyboard != 'c')
                keyboard = cv::waitKey( 30 );
        }
    }

    test_map->SaveMap();
    return(0);
}