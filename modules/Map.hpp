/*!
\file
\brief Map class declaration
\author Martin Sehnoutka

*/
#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <iostream>
#include <complex>

/*!
 * \class Map
 * \brief Implements map of movement in video
 *
 * Map is implemented as 3D array of map cells.
 *
 */
class Map
{
private:
    struct MapCell ***iVelocityMatrix;
    unsigned int iGrid, iHeight, iWidth;
    std::string iFileName;
    static int CalcDirection(double aAngle);

    void AllocateVelocityMatrix();
    void DeleteVelocityMatrix();

public:
    //! Constructor takes video size and grid of velocity matrix
    Map(unsigned int aHeight, unsigned int aWidth, unsigned int aGrid);
    //! Destructor
    ~Map();
    //! Add new velocity vector
    void SetVelocityVector(unsigned int aXPosition, unsigned int aYPosition, double aXVelocity, double aYVelocity, double aDirection);
    //! Get velocity vector
    std::complex<double> GetVelocitVector(unsigned int aXPosition, unsigned int aYPosition, double aDirection) const;
    //! Display window with map
    void PlotMap();
    //! Debug output
    friend std::ostream& operator<< (std::ostream& aStream, const Map &aMap);
    //! Save map to file
    void SaveMap();
    //! Load map from file
    void LoadMap();
    //! Set name of file containing map
    void SetFileName(std::string aFileName);
    //! Calculate angle which object is coming from
    static double CalcAngle(double const aDeltaY, double const aDeltaX);

};

/*!
 * \struct MapCell
 * \brief Cell contains velocity vector and counter
 *
 * Velocity vector is implemented as complex number.
 * Counter is used for averaging vector value.
 *
 */
struct MapCell
{
    std::complex<double> Velocity;
    unsigned int Counter;
    double mean_square_x, mean_square_y;
    double VarianceX() { return mean_square_x - Velocity.real(); }
    double VarianceY() { return mean_square_y - Velocity.imag(); }
};

#endif //__MAP_HPP__