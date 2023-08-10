#include "stl_io.h"
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
    EMSCRIPTEN_BINDINGS( my_point )
    {
        emscripten::value_array<seth_tl::Point>( "Point" )
            .element( &seth_tl::Point::x )
            .element( &seth_tl::Point::y )
            .element( &seth_tl::Point::z );
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

    float parse_float( std::istringstream& s )
    {
        char f_buf[ sizeof( float ) ];
        s.read( f_buf, 4 );
        float* fptr = (float*)f_buf;
        return *fptr;
    }

    Point parse_point( std::istringstream& s )
    {
        float x = parse_float( s );
        float y = parse_float( s );
        float z = parse_float( s );
        return Point( x, y, z );
    }

    void parse_stl( stl_data& info, const std::string& file_string )
    {
        std::istringstream stl_file( file_string, std::ios::in | std::ios::binary );
        if ( !stl_file )
        {
            std::cout << "ERROR: COULD NOT READ FILE" << std::endl;
            assert( false );
        }

        char header_info[ 80 ] = "";
        char n_triangles[ 4 ];
        stl_file.read( header_info, 80 );
        stl_file.read( n_triangles, 4 );
        std::string h( header_info );
        info.name = h;

        unsigned int* r = (unsigned int*)n_triangles;
        unsigned int num_triangles = *r;

        info.triangles.reserve( num_triangles );
        for ( unsigned int i = 0; i < num_triangles; i++ )
        {
            auto normal = parse_point( stl_file );
            auto v1 = parse_point( stl_file );
            auto v2 = parse_point( stl_file );
            auto v3 = parse_point( stl_file );
            info.triangles.emplace_back( std::move( normal ), std::move( v1 ), std::move( v2 ), std::move( v3 ) );
            char dummy[ 2 ];
            stl_file.read( dummy, 2 );
        }
    }

    void write_stl( std::string filename, std::vector<Triangle>& triangles )
    {

        // binary file
        std::string header_info = "solid " + filename + "-output";
        char head[ 80 ];
        std::strncpy( head, header_info.c_str(), sizeof( head ) - 1 );
        char attribute[ 2 ] = "0";
        unsigned long nTriLong = triangles.size();

        filename.append( ".stl" );

        std::ofstream myfile( filename.c_str(), std::ios::out | std::ios::binary );
        if ( !myfile.is_open() )
        {
            throw std::runtime_error( std::string( "Failed to open file: " ) + filename );
        }
        myfile.write( head, sizeof( head ) );
        myfile.write( (char*)&nTriLong, 4 );

        // write down every Triangle
        for ( std::vector<Triangle>::iterator it = triangles.begin(); it != triangles.end(); it++ )
        {
            // normal vector coordinates

            myfile.write( (char*)&it->normal.x, 4 );
            myfile.write( (char*)&it->normal.y, 4 );
            myfile.write( (char*)&it->normal.z, 4 );

            // p1 coordinates
            myfile.write( (char*)&it->v1.x, 4 );
            myfile.write( (char*)&it->v1.y, 4 );
            myfile.write( (char*)&it->v1.z, 4 );

            // p2 coordinates
            myfile.write( (char*)&it->v2.x, 4 );
            myfile.write( (char*)&it->v2.y, 4 );
            myfile.write( (char*)&it->v2.z, 4 );

            // p3 coordinates
            myfile.write( (char*)&it->v3.x, 4 );
            myfile.write( (char*)&it->v3.y, 4 );
            myfile.write( (char*)&it->v3.z, 4 );

            myfile.write( attribute, 2 );
        }

        myfile.close();
    }

    // https://stackoverflow.com/questions/2083771/a-method-to-calculate-the-centre-of-mass-from-a-stl-stereo-lithography-file
    seth_tl::Point getCenter( std::vector<seth_tl::Triangle> triangles )
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

        seth_tl::Point center( xCenter / totalVolume, yCenter / totalVolume, zCenter / totalVolume );
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
