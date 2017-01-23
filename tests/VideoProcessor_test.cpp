#include <iostream>
#include <memory>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../modules/VideoProcessor.hpp"

int main(int argc, char **argv)
{
    std::srand((unsigned int)(std::time(0))); // use current time as seed for random generator

    //Set and open video file
    std::cout << "Test of video processor" << std::endl;
    std::string video_file_name = "../video/MVI_5510.MOV";
    std::string background_file_name = "../video/MVI_5510.jpg";

    VideoProcessor testProcessor;
    std::shared_ptr<Map> testMap;
    if(!testProcessor.OpenFile(video_file_name))
        return(1);
    if(!testProcessor.OpenBackgroundImage(background_file_name))
        return(1);
    testMap = testProcessor.GetMap();
    testMap->SetFileName("../video/MVI_5510.txt");
    testMap->LoadMap();

    //Play video
    int keyboard = 0;
    while(testProcessor.ReadNextFrame() && (char)keyboard != 'q' && (char)keyboard != 27)
    {
        testProcessor.DisplayOutput();
        testMap->PlotMap();
        keyboard = cv::waitKey(30);
        if((char)keyboard == 'p')
        {
            while((char)keyboard != 'c')
                keyboard = cv::waitKey( 30 );
        }
    }


    testMap->SaveMap();
    //std::cout << *testMap;
    testMap = nullptr;

    return(0);
}