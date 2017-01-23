/*!
 * \file
 * \author Martin Sehnoutka
 */

#include <iostream>
#include <fstream>
#include <cmath>

#include <opencv2/highgui/highgui.hpp>

#include "Map.hpp"

#define __MIN_COMPILER_14 (__cplusplus >= 201402L) ///< C++14 is needed for generic lambdas

void Map::AllocateVelocityMatrix()
{
    // Create 3D velocity matrix
    iVelocityMatrix = new struct MapCell **[4];

    for (unsigned int i = 0; i < 4; ++i)
    {
        iVelocityMatrix[i] = new struct MapCell *[iHeight];
        for(unsigned int j=0; j<iHeight; ++j)
        {
            iVelocityMatrix[i][j] = new struct MapCell[iWidth];
            for(unsigned int k=0; k<iWidth; ++k)
            {
                iVelocityMatrix[i][j][k].Velocity.real(0);
                iVelocityMatrix[i][j][k].Velocity.imag(0);
                iVelocityMatrix[i][j][k].Counter = 0;
            }
        }
    }
    //dbg: std::cout << "w: " << iWidth << ", h: " << iHeight << std::endl;
}

void Map::DeleteVelocityMatrix()
{
    if(iVelocityMatrix == nullptr)
        return;

    for (unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j=0; j<iHeight; ++j)
        {
            delete(iVelocityMatrix[i][j]);
        }
        delete(iVelocityMatrix[i]);
    }
    delete(iVelocityMatrix);
}

//unsigned int aHeight=480, unsigned int aWidth=640, unsigned int aGrid=5
Map::Map(unsigned int aHeight, unsigned int aWidth, unsigned int aGrid) : iGrid(aGrid)
{
    iHeight = (unsigned int)(floor(double(aHeight)/aGrid + 0.5));
    iWidth = (unsigned int)(floor(double(aWidth)/aGrid + 0.5));
    AllocateVelocityMatrix();
}

Map::~Map()
{
    DeleteVelocityMatrix();
}

void Map::SetVelocityVector(unsigned int aXPosition, unsigned int aYPosition, double aXVelocity, double aYVelocity, double aDirection)
{
    std::complex<double> newVector(aXVelocity, aYVelocity);

    unsigned int x= (unsigned int)(floor(double(aXPosition)/iGrid ));
    unsigned int y= (unsigned int)(floor(double(aYPosition)/iGrid ));

    int direction = CalcDirection(aDirection);

    iVelocityMatrix[direction][y][x].Counter++;
    const unsigned int N = iVelocityMatrix[direction][y][x].Counter;
    const double A = double(N-1)/N;
    const double B = double(1)/N;
    double tc = (N > 1) ? 0.02 : 0;

    #if __MIN_COMPILER_14

    auto iir_filter = [A,B,tc](auto x_n_1, auto u_n)
    {
        /*
         * Implements IIR filter defined by equation:
         * x[n] = A*x[n-1] + B*u[n]
         */
        return (A-tc)*x_n_1 + (B+tc)*u_n;
    };

    //iVelocityMatrix[direction][y][x].Velocity = A*iVelocityMatrix[direction][y][x].Velocity + B*newVector;
    iVelocityMatrix[direction][y][x].Velocity = iir_filter(
            iVelocityMatrix[direction][y][x].Velocity,
            newVector);

    iVelocityMatrix[direction][y][x].mean_square_x = iir_filter(
            iVelocityMatrix[direction][y][x].mean_square_x,
            pow(newVector.real(), 2));

    iVelocityMatrix[direction][y][x].mean_square_y = iir_filter(
            iVelocityMatrix[direction][y][x].mean_square_y,
            pow(newVector.imag(), 2));

    #else
        #pragma message "Compiling Map without C++14 features."

        iVelocityMatrix[direction][y][x].Velocity = (A-tc)*iVelocityMatrix[direction][y][x].Velocity + (B+tc)*newVector;
        iVelocityMatrix[direction][y][x].mean_square_x =
            (A-tc)*iVelocityMatrix[direction][y][x].mean_square_x + (B+tc)*pow(newVector.real(), 2);
        iVelocityMatrix[direction][y][x].mean_square_y =
            (A-tc)*iVelocityMatrix[direction][y][x].mean_square_y + (B+tc)*pow(newVector.imag(), 2);

    #endif

}

std::complex<double> Map::GetVelocitVector(unsigned int aXPosition, unsigned int aYPosition, double aDirection) const
{
    unsigned int x= (unsigned int)(floor(double(aXPosition)/iGrid ));
    unsigned int y= (unsigned int)(floor(double(aYPosition)/iGrid ));
    int direction = CalcDirection(aDirection);
    return iVelocityMatrix[direction][y][x].Velocity;
}

void Map::PlotMap()
{
    static bool first_loop = true;

    if(first_loop)
        cv::namedWindow("Map", cv::WINDOW_AUTOSIZE);

    first_loop = false;
    cv::Mat plot(iHeight*iGrid, iWidth*iGrid, CV_8UC3, cv::Scalar(255,255,255));

    cv::Point_<double> first, second;
    const cv::Scalar red(0, 0, 255); //BGR
    const cv::Scalar blue(255, 0, 0);
    const cv::Scalar green(0, 255, 0);
    const cv::Scalar black(0, 0, 0);

    const cv::Scalar colors[4] = {red, blue, green, black};

    for(unsigned int i=0; i < iHeight ; ++i )
    {
        for (unsigned int j = 0; j < iWidth ; ++j)
        {
            first.x = j*iGrid;
            first.y = i*iGrid;
            for(unsigned short k=0; k<4; ++k)
            {
                second.x = j*iGrid + iVelocityMatrix[k][i][j].Velocity.real();
                second.y = i*iGrid + iVelocityMatrix[k][i][j].Velocity.imag();
                cv::line(plot, first, second, colors[k], 1);
            }
        }
    }

    cv::imshow("Map", plot);
}


std::ostream &operator<<(std::ostream &aStream, const Map &aMap)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j=0; j<aMap.iHeight; ++j)
        {
            for(unsigned int k=0; k<aMap.iWidth; ++k)
            {
                aStream << aMap.iVelocityMatrix[i][j][k].Velocity << ", ";
            }
            aStream << std::endl;
        }
        aStream << std::endl;
    }
    return aStream;
}

void Map::SaveMap()
{
    std::ofstream output_file(iFileName, std::ios::out);
    if(output_file.is_open())
    {
        output_file << iHeight << ";" << iWidth << ";" << iGrid << ";" << std::endl;
        for (unsigned int i = 0; i < 4; ++i)
        {
            for(unsigned int j=0; j<iHeight; ++j)
            {
                for(unsigned int k=0; k<iWidth; ++k)
                {
                    output_file << iVelocityMatrix[i][j][k].Velocity << ";";
                }
                output_file << std::endl;
            }
            output_file << std::endl;
        }
        output_file.close();
    }

}

void Map::LoadMap()
{
    std::ifstream input_file(iFileName, std::ios::in);
    std::string temp_str;
    std::istringstream is;

    if(input_file.good())
    {
        DeleteVelocityMatrix();
        getline(input_file, temp_str, ';');
        iHeight = (unsigned int)std::stoul(temp_str);
        getline(input_file, temp_str, ';');
        iWidth = (unsigned int)std::stoul(temp_str);
        getline(input_file, temp_str, ';');
        iGrid = (unsigned int)std::stoul(temp_str);
        AllocateVelocityMatrix();
        for (unsigned int i = 0; i < 4; ++i)
        {
            for(unsigned int j=0; j<iHeight; ++j)
            {
                for(unsigned int k=0; k<iWidth; ++k)
                {
                    getline(input_file, temp_str, ';');
                    is.str(temp_str);
                    is >> iVelocityMatrix[i][j][k].Velocity;
                    iVelocityMatrix[i][j][k].Counter++;
                }
            }
        }
        input_file.close();
    }else{
        std::cerr << "[ERROR] Cannot open file with map! New map will be created." << std::endl;
    }
}

void Map::SetFileName(std::string aFileName)
{
    iFileName = aFileName;
}

double Map::CalcAngle(double const aDeltaY, double const aDeltaX)
{
    if(aDeltaX != 0)
    {
        double angle = atan(aDeltaY/aDeltaX);
        return angle + ((aDeltaX > 0) ? 0 : 180);
    }else{
        return (aDeltaY > 0) ? 90 : (-90);
    }
}

int Map::CalcDirection(double aAngle)
{
    int direction;

    if(aAngle >= -45 && aAngle <= 45)
        direction = 0;
    else if(aAngle > 45 && aAngle < 135)
        direction = 1;
    else if(aAngle >= 135 && aAngle <= 225)
        direction = 2;
    else
        direction = 3;

    return direction;
}