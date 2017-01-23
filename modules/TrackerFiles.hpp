/*!
 * \file
 * \author Martin Sehnoutka
 *
 * \brief Declaration of class TrackerFiles
 *
 */

#ifndef __TRACKERFILES_H__
#define __TRACKERFILES_H__

#include <string>

/*!
 * \class TrackerFiles
 * \brief Class manages names of all files needed with video file
 */
class TrackerFiles
{
private:
    std::string filePath, videoSuffix, outputVideoSuffix, imageSuffix, mapSuffix, maskSuffix, videoName;
public:
    TrackerFiles(){}
    ~TrackerFiles(){}
    void setFilePath(const std::string &filePath)
    {
        TrackerFiles::filePath = filePath;
    }

    void setVideoSuffix(const std::string &videoSuffix)
    {
        TrackerFiles::videoSuffix = videoSuffix;
    }

    void setOutputVideoSuffix(const std::string &videoSuffix)
    {
        TrackerFiles::outputVideoSuffix = videoSuffix;
    }

    void setImageSuffix(const std::string &imageSuffix)
    {
        TrackerFiles::imageSuffix = imageSuffix;
    }

    void setMapSuffix(const std::string &mapSuffix)
    {
        TrackerFiles::mapSuffix = mapSuffix;
    }

    void setHiddenMaskSuffix(const std::string &maskSuffix)
    {
        TrackerFiles::maskSuffix = maskSuffix;
    }

    void setVideoName(const std::string &videoName)
    {
        TrackerFiles::videoName = videoName;
    }

    std::string getVideoName(void) const
    {
        return filePath+videoName+videoSuffix;
    }

    std::string getOutputVideoName(void) const
    {
        return filePath+videoName+outputVideoSuffix;
    }

    std::string getImageName(void) const
    {
        return filePath+videoName+imageSuffix;
    }

    std::string getMapName(void) const
    {
        return filePath+videoName+mapSuffix;
    }

    std::string getHiddenMaskName(void) const
    {
        return filePath+videoName+maskSuffix;
    }

};

#endif //__TRACKERFILES_H__
