/*!
 * \file
 * \author Martin Sehnoutka
 *
 * \brief Implementation of Object Tracker using classes VideoProcessor,  TrackerCore and Map
 *
 * In main function, all necessary objects are created and linked together using smart
 * pointers. Program can be started with argument defining name of a video file or
 * without any argument. In second case user will be asked to enter a file name.
 *
 */

#define __MIN_COMPILER_11 (__cplusplus >= 201103L) ///< C++11 or higher is needed for regular expressions

#include <iostream>
#include <memory>
#include <ctime>
#if __MIN_COMPILER_11
#include <regex>
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "modules/VideoProcessor.hpp"
#include "modules/TrackerCore.hpp"
#include "modules/TrackerFiles.hpp"

using namespace std;
using namespace cv;

/// Main function
int main(int argc, char **argv) {

    srand((unsigned int)(time(0)));

    cout << "Object tracker - bachelor project" << endl;
    cout << "Author: Martin Sehnoutka" << endl;

    //Get file name from user
    string file_str;

    if(argc > 1)
    {
        file_str = argv[1];
    }else{
        cout << "Enter file name: ";
        cin >> file_str;
    }

    //Vector for file name and suffix
    vector<string> file_array;

    #if __MIN_COMPILER_11
    //Match pattern file_name.suffix
    regex pieces_regex("(.+)\\.(\\w+)$");
    smatch pieces_match;

    if (regex_match(file_str, pieces_match, pieces_regex))
    {
        for (size_t i = 1; i < pieces_match.size(); ++i)
        {
            ssub_match sub_match = pieces_match[i];
            string piece = sub_match.str();
            file_array.push_back(piece);
        }
    }else{
        cout << "Unknown file name or suffix." << endl;
        return 0;
    }

    #else
        #pragma message "Compiling " __FILE__ " without C++11 features."

    size_t i = file_str.size();
    while(i > 0) //size_t is unsigned !!!
    {
        --i;
        if(file_str[i] == '.')
            break;
    }

    file_str[i] = '\0';
    file_array.push_back(file_str.c_str());
    file_array.push_back(&((file_str.c_str())[i+1]));

    #endif

    //Create file names
    TrackerFiles video;
    video.setFilePath("");
    video.setVideoName(file_array[0]);
    video.setVideoSuffix("."+file_array[1]);
    video.setOutputVideoSuffix("_output.avi");
    video.setImageSuffix(".jpg");
    video.setMapSuffix(".txt");
    video.setHiddenMaskSuffix(".mask.jpg");

    shared_ptr<VideoProcessor> video_processor = make_shared<VideoProcessor>();
    if(!video_processor->OpenFile(video.getVideoName()))
    {
        cerr << "Cannot open video file " << video.getVideoName() << "! Exiting..." << endl;
        return(1);
    }

    if(!video_processor->OpenBackgroundImage(video.getImageName()))
    {
        cerr << "Cannot open bg image! Exiting..." << endl;
        return(1);
    }

    if(!video_processor->OpenHiddenMask(video.getHiddenMaskName()))
    {
        cerr << "Cannot open hidden mask! Exiting..." << endl;
        return(1);
    }

    //video_processor->DisplayFgMask();
    //video_processor->DisplayPreProcess();
    if(!video_processor->SetOutputFile(video.getOutputVideoName()))
    {
        cerr << "Cannot open output video file!" << endl;
    }

    shared_ptr<Map> velocity_map = video_processor->GetMap();
    velocity_map->SetFileName(video.getMapName());
    velocity_map->LoadMap();
    TrackerBase::SetMapPointer(velocity_map);

    TrackerCore object_tracker;
    object_tracker.SetVideoProcessor(video_processor);
    object_tracker.SetHiddenMask(video_processor->GetHiddenMask());

    int keyboard = 0;
    while(video_processor->ReadNextFrame() && (char)keyboard != 'q' && (char)keyboard != 27)
    {
        object_tracker.TrackObjects();
        object_tracker.DrawObjects();
        video_processor->DisplayOutput();
        //velocity_map->PlotMap();
        keyboard = cv::waitKey(30);
        if((char)keyboard == 'p')
        {
            while((char)keyboard != 'c')
                keyboard = cv::waitKey( 30 );
        }
    }

    velocity_map->SaveMap();

    return 0;
}
