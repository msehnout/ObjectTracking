#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(void)
{
    VideoCapture video_file("../video/knihovna_vetsi.MOV");
    Mat frame(640,480, CV_64FC3, 0), background(640,480, CV_64FC3, 0);
    int N = 1;
    double A=0,B=1;

    if(!video_file.isOpened())
    {
        std::cerr << "[ERROR] Cannot open the video file!" << std::endl;
        return(false);
    }

    namedWindow("Video", WINDOW_AUTOSIZE);
    namedWindow("Background", WINDOW_AUTOSIZE);

    video_file >> frame;
    background = frame.clone();
    int keyboard = 0;
    while(!frame.empty() && (char)keyboard != 'q' && (char)keyboard != 27)
    {
        //imshow("Video", frame);
        //imshow("Background", background);
        background = (A-0.05)*background + (B+0.05)*frame;
        ++N;
        A = double(N-1)/N;
        B = double(1)/N;
        //keyboard = waitKey(30);
        video_file >> frame;
    }

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);


    imwrite("../video/knihovna_vetsi.jpg", background, compression_params);


    imshow("Background", background);
    waitKey();

    video_file.release();
    return (0);
}