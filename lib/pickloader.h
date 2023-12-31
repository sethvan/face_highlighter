#ifndef INCLUDED_PICKLOADER_H_
#define INCLUDED_PICKLOADER_H_

#include "faces.h"
#include "multidrawer.h"
#include "stl_io.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>

struct MeshArrays
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class PickLoader
{
private:
    std::vector<seth_tl::Triangle> triangles;
    std::vector<MultiDrawer> selectedIndices;
    std::array<float, 3> model_center;
    float scaleFactor;
    MeshArrays mesh_arrays;
    void loadToAssimp( const std::string& stl_file_content );
    void loadNode( aiNode* node, const aiScene* scene );
    void loadMesh( aiMesh* mesh, const aiScene* scene );

public:
    PickLoader( const std::string& stl_file_content );
    PickLoader() = delete;
    PickLoader( const PickLoader& rhs ) = delete;
    PickLoader& operator=( const PickLoader& rhs ) = delete;
    PickLoader( const PickLoader&& rhs ) = delete;
    PickLoader& operator=( const PickLoader&& rhs ) = delete;
    ~PickLoader() = default;

    void populateTriangleVec();
    std::array<float, 3> getModelCenter() const;
    float getScaleFactor() const;
    const MultiDrawer calcCurrentFaces( int vertexId, float tolerance );
    bool on;
    unsigned int groupId;
    val getVertices() const;
    val getIndices() const;
    int getNumTriangles() const;
    void clearSelection();
};

#endif // INCLUDED_PICKLOADER_H_