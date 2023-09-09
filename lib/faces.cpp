#include "faces.h"
#include <algorithm>
#include <iomanip>

namespace seth_tl
{

    // this function solely exists for stl files with poor normals provided, should rarely be needed
    glm::vec3 getNormal( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c )
    {
        // Calculate normal by cross product of two edges of the triangle
        glm::vec3 edge1 = b - a;
        glm::vec3 edge2 = c - a;
        glm::vec3 normal = glm::cross( edge1, edge2 );
        normal = glm::normalize( normal );
        return normal;
    }

    // Function to calculate the angle between two normals in degrees
    float angleBetweenNormals( const glm::vec3& normal1, const glm::vec3& normal2 )
    {
        glm::vec3 normalizedNormal1 = glm::normalize( normal1 );
        glm::vec3 normalizedNormal2 = glm::normalize( normal2 );

        float dotProduct = glm::dot( normalizedNormal1, normalizedNormal2 );
        // Ensure the dot product is within the valid range [-1, 1]
        if ( dotProduct >= -1.0f && dotProduct <= 1.0f )
        {
            // Calculate the angle
            float angle = glm::acos( dotProduct );

            // Convert angle from radians to degrees
            angle = glm::degrees( angle );
            return angle;
        }
        else
        {
            return 0.0f;
        }
    }

    std::map<seth_tl::Point, std::set<unsigned int>> getFacesPerPoint( const std::vector<seth_tl::Triangle>& triangles )
    {
        std::map<seth_tl::Point, std::set<unsigned int>> facesPerPoint;

        for ( int i = 0; i < triangles.size(); ++i )
        {
            facesPerPoint[ triangles[ i ].v1 ].insert( i );
            facesPerPoint[ triangles[ i ].v2 ].insert( i );
            facesPerPoint[ triangles[ i ].v3 ].insert( i );
        }
        return facesPerPoint;
    }

    void getAdjacentFaces( std::vector<seth_tl::Triangle>& triangles )
    {
        auto facesPerPoint = getFacesPerPoint( triangles );

        std::vector<std::set<unsigned int>> adjacentFacesSets;
        adjacentFacesSets.resize( triangles.size() );

        auto insertFace = [ & ]( const Point& p, const unsigned int i )
        {
            std::for_each( facesPerPoint[ p ].begin(), facesPerPoint[ p ].end(),
                [ & ]( int face )
                {
                    if ( face != i )
                    {
                        adjacentFacesSets[ i ].insert( face );
                    }
                } );
        };

        for ( int i = 0; i < triangles.size(); ++i )
        {
            insertFace( triangles[ i ].v1, i );
            insertFace( triangles[ i ].v2, i );
            insertFace( triangles[ i ].v3, i );
            std::for_each( adjacentFacesSets[ i ].begin(), adjacentFacesSets[ i ].end(),
                [ & ]( const unsigned int n ) { triangles[ i ].adjacentFaces.push_back( n ); } );
        }
    }

    // getAdjacentFaces() must be ran before this is ran, groupId default 0 value means has no groupId assigned
    void getSelectionFaces( std::vector<seth_tl::Triangle>& triangles, std::vector<unsigned int>& selectedFaces, const unsigned int face,
        const unsigned int groupId, float tolerance )
    {

        std::vector<unsigned int> checkAdjacentsOf;
        triangles[ face ].groupId = groupId;
        selectedFaces.push_back( face );
        checkAdjacentsOf.push_back( face );
        size_t index = 0;

        while ( index < checkAdjacentsOf.size() )
        {
            auto currentFace = checkAdjacentsOf[ index ];
            std::for_each( triangles[ currentFace ].adjacentFaces.begin(), triangles[ currentFace ].adjacentFaces.end(),
                [ & ]( unsigned int adjacent )
                {
                    if ( !triangles[ adjacent ].groupId
                        && ( std::fabs( angleBetweenNormals( triangles[ currentFace ].glmNormal, triangles[ adjacent ].glmNormal ) )
                            < tolerance ) )
                    {
                        triangles[ adjacent ].groupId = groupId;
                        selectedFaces.push_back( adjacent );
                        checkAdjacentsOf.push_back( adjacent );
                    }
                } );
            ++index;
        }
    }

    const MultiDrawer getSelectionIndices( std::vector<unsigned int>& selectedFaces )
    {
        std::sort( selectedFaces.begin(), selectedFaces.end() );

        MultiDrawer selectedIndices;
        selectedIndices.startIndices.reserve( selectedFaces.size() );
        selectedIndices.counts.reserve( selectedFaces.size() );

        auto it = selectedFaces.begin();
        while ( it < selectedFaces.end() )
        {
            unsigned int index = ( *it ) * 3;
            unsigned int count = 3;
            while ( *( it + 1 ) == ( *it ) + 1 )
            {
                ++it;
                count += 3;
            }
            selectedIndices.startIndices.push_back( index );
            selectedIndices.counts.push_back( count );
            ++it;
        }
        return selectedIndices;
    }

}