#ifndef INCLUDED_STL_IO_H
#define INCLUDED_STL_IO_H

#include "floating_point.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include <array>

namespace seth_tl
{
    // Point, Triangle, stl_data and parse_stl() modified from https://github.com/dillonhuff/stl_parser (MIT license)

    struct Point
    {
        const float& x;
        const float& y;
        const float& z;

        Point( const float& xp, const float& yp, const float& zp )
            : x( xp )
            , y( yp )
            , z( zp )
        {
        }
        bool operator==( const Point& rhs ) const;

        bool operator<( const Point& rhs ) const;
    };

    struct Triangle
    {
        const Point v1;
        const Point v2;
        const Point v3;
        unsigned int groupId;
        glm::vec3 glmNormal;
        std::vector<int> adjacentFaces;
        Triangle( Point v1p, Point v2p, Point v3p );
        ~Triangle() = default;
    };

    std::array<float, 3> getCenter( std::vector<seth_tl::Triangle> triangles );

    float getScaleFactor( std::vector<seth_tl::Triangle> triangles );

}

#endif // INCLUDED_STL_IO_H
