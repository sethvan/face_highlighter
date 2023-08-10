#ifndef INCLUDED_STL_IO_H
#define INCLUDED_STL_IO_H

#include "floating_point.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>

namespace seth_tl
{
    // Point, Triangle, stl_data and parse_stl() modified from https://github.com/dillonhuff/stl_parser (MIT license)

    struct Point
    {
        float x;
        float y;
        float z;

        Point()
            : x( 0.0f )
            , y( 0.0f )
            , z( 0.0f )
        {
        }
        Point( float xp, float yp, float zp )
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
        Point normal;
        Point v1;
        Point v2;
        Point v3;
        unsigned int groupId;
        std::vector<int> adjacentFaces;
        glm::vec3 glmNormal;
        Triangle( Point normalp, Point v1p, Point v2p, Point v3p )
            : normal( normalp )
            , v1( v1p )
            , v2( v2p )
            , v3( v3p )
            , groupId( 0 )
            , glmNormal( glm::vec3( normalp.x, normalp.y, normalp.z ) ) // for faces.cpp
        {
        }
        ~Triangle() = default;
    };

    struct stl_data
    {
        std::string name;
        std::vector<Triangle> triangles;
    };

    void parse_stl( stl_data& info, const std::string& stl_path );

    void write_stl( std::string filename, std::vector<Triangle>& triangles );

    seth_tl::Point getCenter( std::vector<seth_tl::Triangle> triangles );

    float getScaleFactor( std::vector<seth_tl::Triangle> triangles );

}

#endif // INCLUDED_STL_IO_H
