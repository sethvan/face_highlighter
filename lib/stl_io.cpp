#include "stl_io.h"
#include "faces.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

namespace seth_tl
{

    EMSCRIPTEN_BINDINGS( my_bindings )
    {
        emscripten::value_array<std::array<float, 3>>( "array_float_3" )
            .element( emscripten::index<0>() )
            .element( emscripten::index<1>() )
            .element( emscripten::index<2>() );
    }

    bool Point::operator==( const Point& rhs ) const
    {
        return ( floating_point::equal( x, rhs.x ) && floating_point::equal( y, rhs.y ) && floating_point::equal( z, rhs.z ) );
    }

    bool Point::operator<( const Point& rhs ) const
    {
        if ( floating_point::equal( x, rhs.x ) )
        {
            if ( floating_point::equal( y, rhs.y ) )
            {
                return floating_point::less( z, rhs.z );
            }
            else
            {
                return floating_point::less( y, rhs.y );
            }
        }
        else
        {
            return floating_point::less( x, rhs.x );
        }
    }

    Triangle::Triangle( Point v1p, Point v2p, Point v3p )
        : v1( std::move( v1p ) )
        , v2( std::move( v2p ) )
        , v3( std::move( v3p ) )
        , groupId( 0 )
    {
        glmNormal = seth_tl::getNormal( glm::vec3( v1.x, v1.y, v1.z ), glm::vec3( v2.x, v2.y, v2.z ), glm::vec3( v3.x, v3.y, v3.z ) );
    }

    // https://stackoverflow.com/questions/2083771/a-method-to-calculate-the-centre-of-mass-from-a-stl-stereo-lithography-file
    std::array<float, 3> getCenter( std::vector<seth_tl::Triangle> triangles )
    {
        double totalVolume = 0, currentVolume;
        double xCenter = 0, yCenter = 0, zCenter = 0;
        int numTriangles = triangles.size();

        for ( int i = 0; i < numTriangles; i++ )
        {
            totalVolume += currentVolume = ( triangles[ i ].v1.x * triangles[ i ].v2.y * triangles[ i ].v3.z
                                               - triangles[ i ].v1.x * triangles[ i ].v3.y * triangles[ i ].v2.z
                                               - triangles[ i ].v2.x * triangles[ i ].v1.y * triangles[ i ].v3.z
                                               + triangles[ i ].v2.x * triangles[ i ].v3.y * triangles[ i ].v1.z
                                               + triangles[ i ].v3.x * triangles[ i ].v1.y * triangles[ i ].v2.z
                                               - triangles[ i ].v3.x * triangles[ i ].v2.y * triangles[ i ].v1.z )
                / 6;
            xCenter += ( ( triangles[ i ].v1.x + triangles[ i ].v2.x + triangles[ i ].v3.x ) / 4 ) * currentVolume;
            yCenter += ( ( triangles[ i ].v1.y + triangles[ i ].v2.y + triangles[ i ].v3.y ) / 4 ) * currentVolume;
            zCenter += ( ( triangles[ i ].v1.z + triangles[ i ].v2.z + triangles[ i ].v3.z ) / 4 ) * currentVolume;
        }

        std::array<float, 3> center = { static_cast<float>( xCenter / totalVolume ), static_cast<float>( yCenter / totalVolume ),
            static_cast<float>( zCenter / totalVolume ) };
        return center;
    }

    // https://en.wikibooks.org/wiki/OpenGL_Programming/Bounding_box
    float getScaleFactor( std::vector<seth_tl::Triangle> triangles )
    {

        float min_x, max_x, min_y, max_y, min_z, max_z;
        min_x = max_x = triangles.front().v1.x;
        min_y = max_y = triangles.front().v1.y;
        min_z = max_z = triangles.front().v1.z;

        const auto compareVertices = [ & ]( const auto& point ) -> void
        {
            if ( point.x < min_x )
                min_x = point.x;
            if ( point.x > max_x )
                max_x = point.x;
            if ( point.y < min_y )
                min_y = point.y;
            if ( point.y > max_y )
                max_y = point.y;
            if ( point.z < min_z )
                min_z = point.z;
            if ( point.z > max_z )
                max_z = point.z;
        };

        std::for_each( triangles.begin(), triangles.end(),
            [ & ]( const auto& triangle )
            {
                compareVertices( triangle.v1 );
                compareVertices( triangle.v2 );
                compareVertices( triangle.v3 );
            } );

        float length = std::max( std::max( max_x - min_x, max_y - min_y ), max_z - min_z );
        return 2.0f / length;
    }
}
