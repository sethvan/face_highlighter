#ifndef INCLUDED_FACES_H
#define INCLUDED_FACES_H

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/epsilon.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "multidrawer.h"
#include "stl_io.h"
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace seth_tl
{

    glm::vec3 getNormal( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c );

    float angleBetweenNormals( const glm::vec3& normal1, const glm::vec3& normal2 );

    std::map<seth_tl::Point, std::set<unsigned int>> getFacesPerPoint( const std::vector<seth_tl::Triangle>& triangles );

    void getAdjacentFaces( std::vector<seth_tl::Triangle>& triangles );

    void getSelectionFaces( std::vector<seth_tl::Triangle>& triangles, std::vector<unsigned int>& selectedFaces, const unsigned int face,
        const unsigned int groupId, float tolerance );

    const MultiDrawer getSelectionIndices( std::vector<unsigned int>& selectedFaces );

}

#endif // INCLUDED_FACES_H